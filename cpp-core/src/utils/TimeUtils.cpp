#include <sstream>
#include "utils/TimeUtils.h"
#include <chrono>
#include <iomanip>
#include <ctime>
using namespace std;

namespace ChronOS::Utils{
	
	chrono::system_clock::time_point normalizeToUTC(const string& timestamp, const string& timezone) {

		tm t = {};
		stringstream ss(timestamp);

		ss >> get_time(&t,"%Y-%m-%d %H:%M:%S");

		// creates local time into time_point 
		auto tp = chrono::system_clock::from_time_t(mktime(&t));

		int sign = timezone[0] == '-' ? -1 : 1;
		int hours = stoi(timezone.substr(1, 2));
		int minutes = stoi(timezone.substr(3, 2));

		tp -= (chrono::hours(sign * hours) + chrono::minutes(sign * minutes));
		return tp;
	}

	string timePointToString(const chrono::system_clock::time_point& tp) {
		time_t t = chrono::system_clock::to_time_t(tp);
		tm tmm = {};
		#ifdef _WIN32
				gmtime_s(&tmm, &t);   // Windows (thread-safe)
		#else
				gmtime_r(&t, &tmm);   // Linux / macOS (thread-safe)
		#endif
		char buffer[32];
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tmm);
		return string(buffer);
	}


}

