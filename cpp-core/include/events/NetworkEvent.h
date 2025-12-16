#pragma once
#include "events/Event.h"
#include <string>
using namespace std;

class NetworkEvent : public Event {

public:
	
	string srcIp;
	string dstIp;
	int port;

	string getType() const override;

};