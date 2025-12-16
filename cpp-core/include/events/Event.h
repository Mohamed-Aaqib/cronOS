#pragma once
#include <string>
#include <chrono>
using namespace std;

class Event {

public:
	using Timepoint = chrono::system_clock::time_point;

	explicit Event(Timepoint ts);

	Timepoint getTimeStamp() const;
	virtual string getType() const = 0;
	virtual string serialize() const = 0;

	virtual ~Event() = default;

protected:
	Timepoint timestamp;
};