#pragma once
#include <array>
#include <string>
using namespace std;

namespace ChronOS::Utils {

	array<uint8_t, 16> normalizeIP(const string& ip);

	// inline for multiple of same functions, TODO:Make it more efficient
	inline bool compareIP(const array<uint8_t,16>& a, const array<uint8_t,16>&b) {

		for (int i = 0; i < 16;i++) {
			if (a[i] != b[i]) return false;
		}
		return true;
	}

}

