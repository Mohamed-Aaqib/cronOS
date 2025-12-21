#pragma once
#include <string>
#include <chrono>
using namespace std;
// learnt that this populates namespace with std
// anything unknown is gonna automatically be called with std
// TODO: Change it later on.


namespace ChronOS::Utils {

	chrono::system_clock::time_point normalizeToUTC(const string& timestamp, const string& timezone);
	
	inline chrono::system_clock::time_point now() {
		return chrono::system_clock::now();
	}

	string timePointToString(const chrono::system_clock::time_point& tp);


}
