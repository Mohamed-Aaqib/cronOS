#include "utils/test_pcap_ingestion.h"
#include "core/PCAPIngestor.h"
#include "core/ThreadPool.h"
#include "core/Graph.h"
#include "core/GraphBuilder.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <thread>
#include <atomic>
#include <mutex>
#include <memory>

using namespace std;

void test_pcap_ingestion() {

    auto start_time = chrono::steady_clock::now();

    FlowAggregator aggregator(std::chrono::seconds(3));
    ThreadPool pool;

    PCAPIngestor ingestor("data/http-chunked-gzip.pcap", pool);

    std::vector<NetworkEvent> events;
    atomic<bool> processing_done(false);
    mutex events_mutex;  // Protect events vector from concurrent access

    // Start periodic incremental flushing in a background thread
    size_t flush_count = 0;
    thread flushing_thread([&]() {
        while (!processing_done) {
            // Call incremental flushing every 50ms
            this_thread::sleep_for(chrono::milliseconds(50));
            
            size_t flushed_this_round = 0;
            aggregator.flushExpiredIncremental([&](const NetworkEvent& evt) {
                //TODO: dont think we need mutex here since we are only pushing to the vector
                lock_guard<mutex> lock(events_mutex);
                events.push_back(evt);
                flushed_this_round++;
            }, chrono::seconds(1), chrono::milliseconds(50));
            
            if (flushed_this_round > 0) {
                flush_count++;
                cout << "Incremental flush #" << flush_count << ": " << flushed_this_round << " events flushed\n";
            }
        }
        
        // One final flush after processing is done
        size_t final_flush_count = 0;
        aggregator.flushExpiredIncremental([&](const NetworkEvent& evt) {
            lock_guard<mutex> lock(events_mutex);
            events.push_back(evt);
            final_flush_count++;
        }, chrono::seconds(1), chrono::milliseconds(50));
        
        if (final_flush_count > 0) {
            cout << "Final incremental flush: " << final_flush_count << " events flushed\n";
        }
    });

    ingestor.run(aggregator);
    processing_done = true;

    flushing_thread.join();

    auto processing_end = chrono::steady_clock::now();

    size_t final_flush_count = 0;
    aggregator.flushAll([&](const NetworkEvent& evt) {
        lock_guard<mutex> lock(events_mutex);
        events.push_back(evt);
        final_flush_count++;
    });
    
    if (final_flush_count > 0) {
        cout << "Final flushAll: " << final_flush_count << " events flushed\n";
    }

    auto flush_end = chrono::steady_clock::now();

    auto processing_duration = chrono::duration_cast<chrono::milliseconds>(processing_end - start_time);
    auto flush_duration = chrono::duration_cast<chrono::milliseconds>(flush_end - processing_end);
    auto total_duration = chrono::duration_cast<chrono::milliseconds>(flush_end - start_time);

    cout << "Events generated: " << events.size() << "\n";
    cout << "Processing time: " << processing_duration.count() << " ms\n";
    cout << "Flush time: " << flush_duration.count() << " ms\n";
    cout << "Total time: " << total_duration.count() << " ms\n\n";

    // Build graph from events (basic verification)
    auto graph = make_shared<Graph>();
    GraphBuilder graphBuilder(graph);
    
    cout << "Building graph from " << events.size() << " events...\n";
    graphBuilder.ingestEvents(events);
    
    const auto& nodes = graph->get_nodes();
    const auto& edges = graph->get_edges();
    
    cout << "\n=== Graph Statistics ===\n";
    cout << "Nodes: " << nodes.size() << "\n";
    cout << "Edges: " << edges.size() << "\n\n";

    // Output all events (this test focuses on event output)
    if (!events.empty()) {
        // Use first event's start_time as reference for relative timestamps
        auto reference_time = events[0].start_time;

        for (const auto& e : events) {
            // Calculate relative times from reference
            auto start_duration = chrono::duration_cast<chrono::microseconds>(e.start_time - reference_time);
            auto end_duration = chrono::duration_cast<chrono::microseconds>(e.end_time - reference_time);
            auto flow_duration = chrono::duration_cast<chrono::microseconds>(e.end_time - e.start_time);

            auto start_seconds = chrono::duration_cast<chrono::seconds>(start_duration);
            auto start_us = (start_duration - start_seconds).count();
            auto end_seconds = chrono::duration_cast<chrono::seconds>(end_duration);
            auto end_us = (end_duration - end_seconds).count();
            auto flow_seconds = chrono::duration_cast<chrono::seconds>(flow_duration);
            auto flow_us = (flow_duration - flow_seconds).count();

            cout
                << "PROTO=" << (int)e.protocol
                << " SRC=" << (int)e.src_ip[12] << "."
                << (int)e.src_ip[13] << "."
                << (int)e.src_ip[14] << "."
                << (int)e.src_ip[15]
                << ":" << e.src_port
                << " DST=" << (int)e.dst_ip[12] << "."
                << (int)e.dst_ip[13] << "."
                << (int)e.dst_ip[14] << "."
                << (int)e.dst_ip[15]
                << ":" << e.dst_port
                << " PKTS=" << e.packets
                << " BYTES=" << e.bytes
                << " FLAGS=" << (int)e.flags
                << " START=+" << start_seconds.count() << "." << setfill('0') << setw(6) << start_us
                << " END=+" << end_seconds.count() << "." << setfill('0') << setw(6) << end_us
                << " DUR=" << flow_seconds.count() << "." << setfill('0') << setw(6) << flow_us
                << "\n";
        }
    }

};