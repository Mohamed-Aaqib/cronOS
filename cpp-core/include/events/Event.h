#pragma once
#include <string>
#include <chrono>
using namespace std;

class Event {

	
protected:
	using Timepoint = chrono::steady_clock::time_point;
	Timepoint timestamp;

public:

	explicit Event(Timepoint ts) noexcept : timestamp(ts) {};

	Timepoint getTimeStamp() const noexcept { return timestamp; };

	virtual const char* getType() const noexcept = 0;
	virtual void serialize(char* buffer, size_t buffer_size) const noexcept = 0;

	virtual ~Event() = default;

};