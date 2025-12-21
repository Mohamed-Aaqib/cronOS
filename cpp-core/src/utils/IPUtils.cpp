#include <string>
#include "utils/IPUtils.h"
#include <sstream>
using namespace std;


namespace ChronOS::Utils {
	
	// allow parsing for IPv6 as well method overloading
	array<uint8_t, 16> normalizeIP(const string& ip) {

		array<uint8_t, 16> out{};
		out.fill(0);
		out[10] = 0xFF;
		out[11] = 0xFF;

		stringstream ss(ip);
		int byte;
		char dot;
	
		for (int i = 0; i < 4; i++) {
			if (!(ss >> byte)) throw runtime_error("Invalid IPv4: " + ip);
			
			out[12 + i] = static_cast<uint8_t>(byte);

			if (i < 3 && !(ss >> dot) && dot != '.') throw runtime_error("Invalid IPv4: " + ip);
		}

		return out;

	};

}