#pragma once
#include "events/Event.h"
#include "core/RingBuffer.h"
#include <string>
using namespace std;

class PCAPParser {

private:
	string filepath;

public:
	PCAPParser(const string& file);
	void parse(RingBuffer<Event,8192>& buffer);

};