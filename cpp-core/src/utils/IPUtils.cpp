#include <string>
#include "utils/IPUtils.h"
#include <sstream>
using namespace std;



array<uint8_t, 16> normalizeIP(const string& ip) {

	array<uint8_t, 16> out{};
	out[10] = 0xFF;
	out[11] = 0xFF;

	stringstream ss(ip);
	int byte;
	char dot;
	
	for (int i = 0; i < 4; i++) {
		ss >> byte;
		out[12 + i] = static_cast<uint8_t>(byte);

		if (i == 3) break;
		ss >> dot;
	}

	return out;

};