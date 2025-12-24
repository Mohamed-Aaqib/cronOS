#include <cstdint>
#include <chrono>
#include "core/FlowAggregator.h"
using namespace std;


FlowAggregator::FlowAggregator(chrono::seconds timeout) :flow_timeout(timeout) {};

void FlowAggregator::ingestPacket(const FlowKey& key, uint32_t bytes, bool syn, bool fin, bool rst, bool ack) {

	auto now = chrono::steady_clock::now();
	auto state = flows[key];

	if (state.packet_count == 0) {
		state.first_seen = now;
	}

	state.last_seen = now;
	state.packet_count++;
	state.byte_count += bytes;

	// can optimize this better
	if (syn) state.syn_count++;
	if (fin) state.fin_count++;
	if (rst) state.rst_count++;
	if (ack) state.ack_count++;

}


template<typename Sink>
void FlowAggregator::flushExpired(Sink&& emit) {

	auto now = chrono::steady_clock::now();

	for (auto it = flows.begin(); it != flows.end()) {

		if (now - it->second.first_seen > flow_timeout) {

			const FlowKey& key = it->first;
			const FlowState& st = it->second;

			NetworkEvent evt(
				st.last_seen,        
				key.src_ip.data(),  
				key.dst_ip.data(),
				key.src_port,       
				key.dst_port, 
				static_cast<uint8_t>(key.protocol),
				st.byte_count,
				st.packet_count,
				(st.syn_count ? 0x01 : 0) |
				(st.fin_count ? 0x02 : 0) |
				(st.rst_count ? 0x04 : 0) |
				(st.ack_count ? 0x08 : 0)
			);

			emit(evt);
			// after erasing it points to next element in the map.
			it = flows.erase(it);
		}
		else {
			++it
		}

	}


}