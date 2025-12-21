#pragma once
#include "events/Event.h"
#include <string>
#include <sstream>
using namespace std;

class NetworkEvent : public Event {

public:

	string srcIp;
	string dstIp;
	int port;

	NetworkEvent(Timepoint ts, const string& src = " ", const string& dst = " ", int p = 0) : Event(ts), srcIp(src), dstIp(dst), port(p) {};

	string getType() const override { return "NetworkEvent"; };

	string serialize() const override {

		stringstream ss;
		ss << getType() << "|" << srcIp << "|" << dstIp << "|" << port;

		return ss.str();
	}


};