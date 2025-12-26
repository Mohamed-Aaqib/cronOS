#pragma once
#include "events/Event.h"
#include <cstdint>
#include <cstdio>
using namespace std;

class NetworkEvent : public Event {

public:

	uint8_t src_ip[16];
	uint8_t dst_ip[16];
	
	uint16_t src_port;
	uint16_t dst_port;

	uint8_t protocol;
	uint64_t bytes;
	uint32_t packets;

	uint8_t flags;

	Timepoint start_time;  // First packet time (also stored in base class timestamp)
	Timepoint end_time;    // Last packet time




	NetworkEvent(

		Timepoint start,
		Timepoint end,
		uint8_t src[16],
		uint8_t dst[16],
		uint16_t sport,
		uint16_t dport,
		uint8_t prot,
		uint64_t byteCount,
		uint32_t packetCount,
		uint8_t flagBits

		) : Event(start), src_port(sport), dst_port(dport), protocol(prot), bytes(byteCount), packets(packetCount), flags(flagBits), start_time(start), end_time(end) {
	
		for (int i = 0; i < 16; i++) {
			src_ip[i] = src[i];
			dst_ip[i] = dst[i];
		}
		
	};
		

	const char* getType() const noexcept override { 
		return "NetworkEvent"; 
	};

	void serialize(char* buffer, size_t buffer_size) const noexcept override {

		// could use string stream but it has type errors and not very clean unlike this formatting style
		snprintf(
			buffer,
			buffer_size,
			"NET|%u|%u|%u|%u|%u|%llu|%u|%u",
			protocol,
			src_port,
			dst_port,
			flags,
			packets,
			static_cast<unsigned long long>(bytes),
			src_ip[15],
			dst_ip[15]
		);

	};


};