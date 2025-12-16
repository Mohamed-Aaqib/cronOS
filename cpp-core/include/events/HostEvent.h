#pragma once
#include "events/Event.h"
#include <string>
using namespace std;

class HostEvent :public Event {

public:
	string hostname;
	string process;

	string getType() const override;

};