#pragma once
#include <cstdint>
#include <events/NetworkEvent.h>
#include <core/FlowKey.h>
#include <chrono>
using namespace std;


struct FlowState {

	uint64_t packet_count = 0;
	uint64_t byte_count = 0;

	uint32_t syn_count = 0;
	uint32_t fin_count = 0;
	uint32_t rst_count = 0;
	uint32_t ack_count = 0;

	chrono::steady_clock::time_point first_seen;
	chrono::steady_clock::time_point last_seen;

};


class FlowAggregator {

public:

	explicit FlowAggregator(chrono::seconds timeout);

	void ingestPacket(const FlowKey& key, uint32_t bytes, bool syn, bool fin, bool rst, bool ack);

	template<typename Sink>
	void flushExpired(Sink&& emit);


private:
	chrono::seconds flow_timeout;
	// tho we could use pointers, we would have to manage it and cache is worse cache locality , since everything is scatterd
	//TODO: think about hashing would work
	unordered_map<FlowKey,FlowState>flows;

};