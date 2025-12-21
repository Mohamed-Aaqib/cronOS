#include "core/PCAPParser.h"
#include "events/NetworkEvent.h"
#include <pcap.h>
#include <iostream>
using namespace std;

void PCAPParser::parse(RingBuffer<Event, 8192>& buffer) {

	char errBuff[PCAP_ERRBUF_SIZE];
	pcap_t* handle = pcap_open_offline(filepath.c_str(), errBuff);
	if (!handle) {
		cerr << "unable to use PCAP file: " << errBuff << endl;
		return;
	}

	struct pcap_pkthdr* header;
	const u_char* data;
	int res;
	while ((res = pcap_next_ex(handle,&header, &data)) >= 0) {
		if (res == 0) continue;
		auto evt = make_shared<NetworkEvent>(chrono::system_clock::from_time_t(header->ts.tv_sec));
		//TODO: we would need to error handle from this buffer point
		buffer.push(evt);
	}
	pcap_close(handle);
}