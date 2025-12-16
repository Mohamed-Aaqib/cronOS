#pragma once
#include "events/Event.h"
#include <string>
using namespace std;

class LogEvent : public Event {

public:
	string source;
	string message;

	string getType() const override;

};