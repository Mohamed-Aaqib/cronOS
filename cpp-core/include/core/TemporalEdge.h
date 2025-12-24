#pragma once
#include <chrono>
#include <cstdint>
#include "core/Edge.h"
#include <string>
using namespace std;


class TemporalEdge : public Edge {

public:
	
	using TimePoint = chrono::system_clock::time_point;

private:

	TimePoint window_start;
	TimePoint window_end;

	uint64_t packet_count = 0;
	uint64_t byte_count = 0;

	uint8_t protocol;


public:

	TemporalEdge(uint64_t src_, uint64_t dst_,TimePoint start_,TimePoint end_, uint8_t prot) 
	:Edge(src_,dst_),window_start(start_),window_end(end_),protocol(prot){};

	void add_packets(uint64_t packets, uint64_t bytes) {
		packet_count += packets;
		byte_count += bytes;
	}

	const TimePoint& get_start() const { return window_start; }
	const TimePoint& get_end() const { return window_end; }

	uint64_t get_packetCount() const { return packet_count; }
	uint64_t get_byteCount() const { return byte_count; }
	uint8_t get_protocol() const { return protocol; }



};