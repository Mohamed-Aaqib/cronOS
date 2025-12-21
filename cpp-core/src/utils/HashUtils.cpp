#include "utils/HashUtils.h"
#include <array>
#include <string>
using namespace std;

namespace ChronOS::Utils{

	// use FNV hashing algorithm
	uint64_t hashBytes(const uint8_t* data, size_t len) {
		uint64_t hash = 14695981039346656037ULL;
		for (size_t i = 0; i < len; i++) {
			hash ^= data[i];
			hash *= 1099511628211ULL;
		}
		return hash;
	}

	uint64_t hashIP(const array<uint8_t,16>& ip) {
		return hashBytes(ip.data(), ip.size());
	}



}