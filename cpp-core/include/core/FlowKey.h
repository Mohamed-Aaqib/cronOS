#pragma once
#include <utils/HashUtils.h>
#include <functional>
#include <cstdint>
using namespace std;

enum class L4Proto :uint8_t {
	TCP = 6,
	UDP = 17,
	UNKNOWN = 0,
};

struct FlowKey {

	array<uint8_t, 16> src_ip;
	array<uint8_t, 16> dst_ip;
	uint16_t src_port;
	uint16_t dst_port;
	L4Proto protocol;

	bool operator==(const FlowKey& other) const noexcept {
		return(
			src_ip == other.src_ip &&
			dst_ip == other.dst_ip &&
			src_port == other.src_port &&
			dst_port == other.dst_port &&
			protocol == other.protocol
		);
	}

};

namespace std {
	template<>
	struct hash<FlowKey> {
		size_t operator()(const FlowKey& k) const noexcept {
			size_t h = 0;
			h ^= ChronOS::Utils::hashBytes(k.src_ip.data(), 16);
			h ^= ChronOS::Utils::hashBytes(k.dst_ip.data(), 16) << 1;
			// no performance gain overall on this, even if we creted temp class once
			h ^= std::hash<uint16_t>{}(k.src_port) << 2;
			h ^= std::hash<uint16_t>{}(k.dst_port) << 3;
			h ^= std::hash<uint8_t>{}(static_cast<uint8_t>(k.protocol)) << 4;
			return h;
		}
	};
}
