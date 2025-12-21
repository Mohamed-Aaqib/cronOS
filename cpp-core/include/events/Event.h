#pragma once
#include <string>
#include <chrono>
using namespace std;

class Event {

protected:
	using Timepoint = chrono::system_clock::time_point;
	Timepoint timestamp;

public:

	explicit Event(Timepoint ts) : timestamp(ts) {};

	Timepoint getTimeStamp() const { return timestamp; };

	virtual string getType() const = 0;
	virtual string serialize() const = 0;

	virtual ~Event() = default;

};