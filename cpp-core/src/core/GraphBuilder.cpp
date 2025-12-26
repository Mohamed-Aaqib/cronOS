#pragma once
#include "core/FlowAggregator.h"
#include "core/Graph.h"
#include "core/GraphBuilder.h"
#include "events/NetworkEvent.h"
#include <string>
#include <chrono>
using namespace std;

GraphBuilder::GraphBuilder(shared_ptr<Graph> graph_):graph(graph_) {};

void GraphBuilder::ingestEvent(const NetworkEvent& evt) {

    auto ipToString = [](const uint8_t ip[16]) {

        ostringstream oss;
        oss << static_cast<int>(ip[12]) << "."
            << static_cast<int>(ip[13]) << "."
            << static_cast<int>(ip[14]) << "."
            << static_cast<int>(ip[15]);
        return oss.str();

     };

    string src_ip = ipToString(evt.src_ip);
    string dst_ip = ipToString(evt.dst_ip);

    auto edge = graph->add_temporal_edge(
        src_ip,
        dst_ip,
        evt.start_time,
        evt.end_time,
        evt.protocol,
        evt.src_port,
        evt.dst_port,
        evt.flags
    );

    edge->add_packets(
        evt.packets,
        evt.bytes
    );
    
    // Update node bytes after packets are added to edge
    auto src_node = graph->get_create_node(src_ip);
    auto dst_node = graph->get_create_node(dst_ip);
    
    src_node->add_bytes_sent(evt.bytes);
    dst_node->add_bytes_recv(evt.bytes);

};
