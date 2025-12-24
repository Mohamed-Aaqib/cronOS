#include "core/PCAPIngestor.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring> 
#include <chrono>
using namespace std;

// packs padding so we can reinterpret cast properly
#pragma pack(push, 1)
struct ip_header {
    uint8_t ihl : 4;
    uint8_t version : 4;
    uint8_t tos;
    uint16_t tot_len;
    uint16_t id;
    uint16_t frag_off;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t check;
    uint32_t saddr;
    uint32_t daddr;
};

struct tcp_header {
    uint16_t source;
    uint16_t dest;
    uint32_t seq;
    uint32_t ack_seq;
    uint8_t doff : 4;
    uint8_t flags;
    uint16_t window;
    uint16_t check;
    uint16_t urg_ptr;
};

struct udp_header {
    uint16_t sport;
    uint16_t dport;
    uint16_t len;
    uint16_t check;
};
#pragma pack(pop)

// TCP flag constants
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#define TH_ECE  0x40
#define TH_CWR  0x80



PCAPIngestor::PCAPIngestor(const string& path): pcap_path(path) {};

void PCAPIngestor::run(FlowAggregator& aggregator) {

	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t* handle = pcap_open_offline(pcap_path.c_str(), errbuf);
	if (!handle) return;

	const u_char* packet;
	struct pcap_pkthdr* header;

	while (pcap_next_ex(handle,&header,&packet)>0) {

        if (!header || !packet) continue;

        // non ipv4 ethernet packets need to be skipped
        const uint16_t eth_type = (packet[12] << 8) | packet[13];
        if (eth_type != 0x0800) continue; 

		// ignore the 14 bytes of the ethernet II frame to get IP Header
		const struct ip_header* iphdr = reinterpret_cast<const struct ip_header*>(packet + 14);


		FlowKey key{};

        memset(key.src_ip.data(), 0, 12);
        memcpy(key.src_ip.data()+12, &iphdr->saddr,4);

        memset(key.dst_ip.data(), 0, 12);
        memcpy(key.dst_ip.data() + 12, &iphdr->daddr, 4);

        if (iphdr->protocol == IPPROTO_TCP) {
            const tcp_header* tcp = reinterpret_cast<const tcp_header*>(
                //reinterpret_cast<const u_char*>(iphdr) + ihl * 4 also works suprisigly
                packet + 14 + iphdr->ihl*4
            );

            key.src_port = ntohs(tcp->source);
            key.dst_port = ntohs(tcp->dest);

            aggregator.ingestPacket(
                key,
                header->len,
                tcp->flags & TH_SYN,
                tcp->flags & TH_FIN,
                tcp->flags & TH_RST,
                tcp->flags & TH_ACK
            );

        }
        else if (iphdr->protocol == IPPROTO_UDP) {
            
            const udp_header* udp = reinterpret_cast<const udp_header*>(packet + 14 + iphdr->ihl * 4);
            key.src_port = ntohs(udp->sport);
            key.dst_port = ntohs(udp->dport);
        
            aggregator.ingestPacket(key, header->len, false, false, false, false);

        }

	}

    pcap_close(handle);


}