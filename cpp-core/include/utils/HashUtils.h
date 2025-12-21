#pragma once
#include <string>
#include <array>
using namespace std;

namespace ChronOS::Utils {

	uint64_t hashBytes(const uint8_t* data, size_t len);

	uint64_t hashIP(const array<uint8_t,16>& ip);

	inline uint64_t combineHash(uint64_t h1, uint64_t h2) {
		//super effective way of hashing two using XOR mashed
		return h1 ^ (h2+ 0x9e3779b97f4a7c15+(h1 << 12)+(h2 >> 4));
	}

}