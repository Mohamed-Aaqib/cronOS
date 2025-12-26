#include "core/test_path_reconstructor.h"
#include "core/PCAPIngestor.h"
#include "core/ThreadPool.h"
#include "core/Graph.h"
#include "core/GraphBuilder.h"
#include "core/PathReconstructor.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <memory>
using namespace std;

void test_path_reconstructor() {

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
            this_thread::sleep_for(chrono::milliseconds(50));
            
            size_t flushed_this_round = 0;
            aggregator.flushExpiredIncremental([&](const NetworkEvent& evt) {
                lock_guard<mutex> lock(events_mutex);
                events.push_back(evt);
                flushed_this_round++;
            }, chrono::seconds(1), chrono::milliseconds(50));
            
            if (flushed_this_round > 0) {
                flush_count++;
                cout << "[path_reconstructor] Incremental flush #" << flush_count
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
            cout << "[path_reconstructor] Final incremental flush: "
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
        cout << "[path_reconstructor] Final flushAll: "
             << final_flush_count << " events flushed\n";
    }

    auto flush_end = chrono::steady_clock::now();

    auto processing_duration = chrono::duration_cast<chrono::milliseconds>(processing_end - start_time);
    auto flush_duration = chrono::duration_cast<chrono::milliseconds>(flush_end - processing_end);
    auto total_duration = chrono::duration_cast<chrono::milliseconds>(flush_end - start_time);

    cout << "[path_reconstructor] Events generated: " << events.size() << "\n";
    cout << "[path_reconstructor] Processing time: " << processing_duration.count() << " ms\n";
    cout << "[path_reconstructor] Flush time: " << flush_duration.count() << " ms\n";
    cout << "[path_reconstructor] Total time: " << total_duration.count() << " ms\n\n";

    // Build graph from events
    auto graph = make_shared<Graph>();
    GraphBuilder graphBuilder(graph);
    
    cout << "[path_reconstructor] Building graph from " << events.size() << " events...\n";
    graphBuilder.ingestEvents(events);
    
    const auto& nodes = graph->get_nodes();
    const auto& edges = graph->get_edges();
    
    cout << "\n[path_reconstructor] === Graph Statistics ===\n";
    cout << "[path_reconstructor] Nodes: " << nodes.size() << "\n";
    cout << "[path_reconstructor] Edges: " << edges.size() << "\n\n";

    // Use PathReconstructor to build adjacency and confirm something printable
    PathReconstructor pathReconstructor(*graph);
    pathReconstructor.build_adjacency();
    if (!nodes.empty()) {
        auto first_node_it = nodes.begin();
        cout << "[path_reconstructor] Adjacency built for at least node ID: "
             << first_node_it->first << "\n\n";
    } else {
        cout << "[path_reconstructor] No nodes in graph, adjacency is empty.\n\n";
    }
}


