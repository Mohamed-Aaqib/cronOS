#pragma once
#include <chrono>
#include <cstdint>
#include "core/Edge.h"
#include <string>
using namespace std;


class TemporalEdge : public Edge {

public:
	
	using TimePoint = chrono::steady_clock::time_point;

private:

	TimePoint window_start;
	TimePoint window_end;

	uint64_t packet_count = 0;
	uint64_t byte_count = 0;

	uint8_t protocol;
	
	// Critical fields for GNN attack detection
	uint16_t src_port = 0;
	uint16_t dst_port = 0;
	uint8_t flags = 0;
	uint64_t duration_ms = 0;


public:

	TemporalEdge(uint64_t src_, uint64_t dst_, TimePoint start_, TimePoint end_, uint8_t prot,
	             uint16_t sport, uint16_t dport, uint8_t flagBits) 
	:Edge(src_,dst_),window_start(start_),window_end(end_),protocol(prot),
	 src_port(sport),dst_port(dport),flags(flagBits) {
		// Calculate duration in milliseconds
		auto duration = end_ - start_;
		duration_ms = chrono::duration_cast<chrono::milliseconds>(duration).count();
	}

	void add_packets(uint64_t packets, uint64_t bytes) {
		packet_count += packets;
		byte_count += bytes;
	}

	const TimePoint& get_start() const { return window_start; }
	const TimePoint& get_end() const { return window_end; }

	uint64_t get_packetCount() const { return packet_count; }
	uint64_t get_byteCount() const { return byte_count; }
	uint8_t get_protocol() const { return protocol; }
	
	// New getters for GNN features
	uint16_t get_src_port() const { return src_port; }
	uint16_t get_dst_port() const { return dst_port; }
	uint8_t get_flags() const { return flags; }
	uint64_t get_duration_ms() const { return duration_ms; }
	
	// Calculate average packet size (useful feature)
	float get_avg_packet_size() const {
		return packet_count > 0 ? static_cast<float>(byte_count) / static_cast<float>(packet_count) : 0.0f;
	}



};