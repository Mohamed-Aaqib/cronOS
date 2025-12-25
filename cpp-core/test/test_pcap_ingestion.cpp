#include "utils/test_pcap_ingestion.h"
#include "core/PCAPIngestor.h"
#include "core/ThreadPool.h"
#include <iostream>
using namespace std;

void test_pcap_ingestion() {

    FlowAggregator aggregator(std::chrono::seconds(3));
    ThreadPool pool;

    PCAPIngestor ingestor("data/http-chunked-gzip.pcap", pool);

    ingestor.run(aggregator);

    std::vector<NetworkEvent> events;

    aggregator.flushAll([&](const NetworkEvent& evt) {
        events.push_back(evt);
    });

    cout << "Events generated: " << events.size() << "\n\n";

    for (const auto& e : events) {
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
            << "\n";
    }

};