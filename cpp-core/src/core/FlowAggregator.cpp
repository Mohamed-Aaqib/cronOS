#include <cstdint>
#include <chrono>
#include "core/FlowAggregator.h"
using namespace std;


FlowAggregator::FlowAggregator(chrono::seconds timeout) :flow_timeout(timeout) {};

void FlowAggregator::ingestPacket(const FlowKey& key, uint32_t bytes, bool syn, bool fin, bool rst, bool ack, chrono::steady_clock::time_point packet_time) {

	lock_guard<mutex> lock(flows_mutex);
	auto& state = flows[key];

	if (state.packet_count == 0) {
		state.first_seen = packet_time;
	}

	state.last_seen = packet_time;
	state.packet_count++;
	state.byte_count += bytes;

	// can optimize this better
	if (syn) state.syn_count++;
	if (fin) state.fin_count++;
	if (rst) state.rst_count++;
	if (ack) state.ack_count++;

}