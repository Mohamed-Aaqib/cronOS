#pragma once
#include <string>
#include <pcap.h>
#include "core/FlowAggregator.h"
#include "core/ThreadPool.h"
#include "core/LockFreeQueue.h"
#include "core/FlowKey.h"
#include <chrono>
#include <thread>
#include <atomic>
using namespace std;

struct PacketTask {
	FlowKey key;
	uint32_t bytes;
	bool syn;
	bool fin;
	bool rst;
	bool ack;
	chrono::steady_clock::time_point packet_time;
};

class PCAPIngestor {
public:
	explicit PCAPIngestor(const string& path, ThreadPool& pool);

	void run(FlowAggregator& aggregator);

private:
	string pcap_path;
	ThreadPool& threadPool;
	LockFreeQueue<PacketTask, 1024> packetQueue;
	chrono::system_clock::time_point first_pcap_time;
	bool first_packet_set;
	atomic<bool> stop_workers;

	chrono::steady_clock::time_point convertPcapTime(const struct timeval& ts);
};