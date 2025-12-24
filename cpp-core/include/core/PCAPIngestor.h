#pragma once
#include <string>
#include <pcap.h>
#include "core/FlowAggregator.h"
using namespace std;

class PCAPIngestor {
public:
	explicit PCAPIngestor(const string& path);

	void run(FlowAggregator& aggregator);

private:
	string pcap_path;


};