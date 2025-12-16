#pragma once
#include <string>
#include <chrono>
using namespace std;

chrono::system_clock::time_point normalizeToUTC(const string& timestamp, const string& timezone);