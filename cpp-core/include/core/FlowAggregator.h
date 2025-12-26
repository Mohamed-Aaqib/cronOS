#pragma once
#include <cstdint>
#include <cstring>
#include <mutex>
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


private:
	chrono::seconds flow_timeout;
	// tho we could use pointers, we would have to manage it and cache is worse cache locality , since everything is scatterd
	//TODO: think about hashing would work
	unordered_map<FlowKey, FlowState>flows;
	mutable mutex flows_mutex;


public:

	explicit FlowAggregator(chrono::seconds timeout);

	void ingestPacket(const FlowKey& key, uint32_t bytes, bool syn, bool fin, bool rst, bool ack, chrono::steady_clock::time_point packet_time);

	template<typename Sink>
	void flushExpired(Sink&& emit) {
		lock_guard<mutex> lock(flows_mutex);
		auto now = chrono::steady_clock::now();

		for (auto it = flows.begin(); it != flows.end();) {

			if (now - it->second.first_seen > flow_timeout) {

				const FlowKey& key = it->first;
				const FlowState& st = it->second;

				uint8_t src_ip[16];
				uint8_t dst_ip[16];
				memcpy(src_ip, key.src_ip.data(), 16);
				memcpy(dst_ip, key.dst_ip.data(), 16);

				NetworkEvent evt(
					st.first_seen,
					st.last_seen,
					src_ip,
					dst_ip,
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
				++it;
			}

		}
	}

	// flush everything
	template<typename Sink>
	void flushAll(Sink&& emit) {
		lock_guard<mutex> lock(flows_mutex);
		for (auto it = flows.begin(); it != flows.end();) {
			const FlowKey& key = it->first;
			const FlowState& st = it->second;

			uint8_t src_ip[16];
			uint8_t dst_ip[16];
			memcpy(src_ip, key.src_ip.data(), 16);
			memcpy(dst_ip, key.dst_ip.data(), 16);

			NetworkEvent evt(
				st.first_seen,
				st.last_seen,
				src_ip,
				dst_ip,
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
			it = flows.erase(it);
		}
	}

	// Incremental flushing: flush flows that exceeded window OR are inactive
	// This is more efficient for GNN temporal windows and reduces memory usage
	template<typename Sink, typename Duration>
	void flushExpiredIncremental(Sink&& emit, chrono::seconds window_size, Duration inactivity_timeout) {
		lock_guard<mutex> lock(flows_mutex);
		auto now = chrono::steady_clock::now();

		for (auto it = flows.begin(); it != flows.end();) {
			const FlowKey& key = it->first;
			const FlowState& st = it->second;

			// Flush if flow exceeded window OR is inactive
			bool exceeded_window = (now - st.first_seen) > window_size;
			auto inactive_duration = chrono::duration_cast<chrono::milliseconds>(now - st.last_seen);
			auto timeout_duration = chrono::duration_cast<chrono::milliseconds>(inactivity_timeout);
			bool inactive = inactive_duration > timeout_duration;

			if (exceeded_window || inactive) {
				uint8_t src_ip[16];
				uint8_t dst_ip[16];
				memcpy(src_ip, key.src_ip.data(), 16);
				memcpy(dst_ip, key.dst_ip.data(), 16);

				NetworkEvent evt(
					st.first_seen,
					st.last_seen,
					src_ip,
					dst_ip,
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
				it = flows.erase(it);
			} else {
				++it;
			}
		}
	}


};