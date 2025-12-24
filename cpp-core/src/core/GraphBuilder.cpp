#pragma once
#include "core/FlowAggregator.h"
#include "core/Graph.h"
#include "core/GraphBuilder.h"
#include "events/NetworkEvent.h"
#include <string>
using namespace std;


explicit GraphBuilder::GraphBuilder(shared_ptr<Graph> graph_):graph(move(graph_)) {};

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
        evt.getTimeStamp(),
        evt.getTimeStamp(),
        evt.protocol
    );

    edge->add_packets(
        evt.packets,
        evt.bytes
    );

};



template<typename Iterable>
void GraphBuilder::ingestEvents(const Iterable& events) {
    for (const auto& evt : events) {
        ingestEvent(evt);
    }
}
