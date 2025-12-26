#include "core/PCAPIngestor.h"
#include "core/ThreadPool.h"
#include "core/Graph.h"
#include "core/GraphBuilder.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <memory>
#include "core/test_nodes_edges.h"
using namespace std;

void test_nodes_edges() {

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
                lock_guard<mutex> lock(events_mutex);
                events.push_back(evt);
                flushed_this_round++;
            }, chrono::seconds(1), chrono::milliseconds(50));
            
            if (flushed_this_round > 0) {
                flush_count++;
                cout << "[nodes_edges] Incremental flush #" << flush_count
                     << ": " << flushed_this_round << " events flushed\n";
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
            cout << "[nodes_edges] Final incremental flush: "
                 << final_flush_count << " events flushed\n";
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
        cout << "[nodes_edges] Final flushAll: "
             << final_flush_count << " events flushed\n";
    }

    auto flush_end = chrono::steady_clock::now();

    auto processing_duration = chrono::duration_cast<chrono::milliseconds>(processing_end - start_time);
    auto flush_duration = chrono::duration_cast<chrono::milliseconds>(flush_end - processing_end);
    auto total_duration = chrono::duration_cast<chrono::milliseconds>(flush_end - start_time);

    cout << "[nodes_edges] Events generated: " << events.size() << "\n";
    cout << "[nodes_edges] Processing time: " << processing_duration.count() << " ms\n";
    cout << "[nodes_edges] Flush time: " << flush_duration.count() << " ms\n";
    cout << "[nodes_edges] Total time: " << total_duration.count() << " ms\n\n";

    // Build graph from events
    auto graph = make_shared<Graph>();
    GraphBuilder graphBuilder(graph);
    
    cout << "[nodes_edges] Building graph from " << events.size() << " events...\n";
    graphBuilder.ingestEvents(events);
    
    // Log graph statistics
    const auto& nodes = graph->get_nodes();
    const auto& edges = graph->get_edges();
    
    cout << "\n[nodes_edges] === Graph Statistics ===\n";
    cout << "[nodes_edges] Nodes: " << nodes.size() << "\n";
    cout << "[nodes_edges] Edges: " << edges.size() << "\n\n";
    
    // Log first node properties
    if (!nodes.empty()) {
        auto first_node_it = nodes.begin();
        auto first_node = first_node_it->second;
        cout << "[nodes_edges] === First Node Properties ===\n";
        cout << "[nodes_edges] Node ID: " << first_node->getId() << "\n";
        cout << "[nodes_edges] IP: " << first_node->getIp() << "\n";
        cout << "[nodes_edges] Degree: " << first_node->get_degree() << "\n";
        cout << "[nodes_edges] In-degree: " << first_node->get_in_degree() << "\n";
        cout << "[nodes_edges] Out-degree: " << first_node->get_out_degree() << "\n";
        cout << "[nodes_edges] Unique ports: " << first_node->get_unique_ports() << "\n";
        cout << "[nodes_edges] Bytes sent: " << first_node->get_total_bytes_sent() << "\n";
        cout << "[nodes_edges] Bytes received: " << first_node->get_total_bytes_recv() << "\n";
        cout << "[nodes_edges] Is internal: " << (first_node->get_is_internal() ? "true" : "false") << "\n\n";
    }
    
    // Log first edge properties
    if (!edges.empty()) {
        auto first_edge = edges[0];
        cout << "[nodes_edges] === First Edge Properties ===\n";
        cout << "[nodes_edges] Source node ID: " << first_edge->get_src() << "\n";
        cout << "[nodes_edges] Destination node ID: " << first_edge->get_dst() << "\n";
        cout << "[nodes_edges] Protocol: " << (int)first_edge->get_protocol() << " (6=TCP, 17=UDP)\n";
        cout << "[nodes_edges] Source port: " << first_edge->get_src_port() << "\n";
        cout << "[nodes_edges] Destination port: " << first_edge->get_dst_port() << "\n";
        cout << "[nodes_edges] Flags: " << (int)first_edge->get_flags() << "\n";
        cout << "[nodes_edges] Duration: " << first_edge->get_duration_ms() << " ms\n";
        cout << "[nodes_edges] Packets: " << first_edge->get_packetCount() << "\n";
        cout << "[nodes_edges] Bytes: " << first_edge->get_byteCount() << "\n";
        cout << "[nodes_edges] Avg packet size: " << fixed << setprecision(2) << first_edge->get_avg_packet_size() << " bytes\n";
        
        // Calculate and display time window
        auto start = first_edge->get_start();
        auto end = first_edge->get_end();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        auto duration_sec = chrono::duration_cast<chrono::seconds>(duration);
        auto duration_us = (duration - duration_sec).count();
        cout << "[nodes_edges] Time window: " << duration_sec.count() << "."
             << setfill('0') << setw(6) << duration_us << " seconds\n\n";
    }
}


