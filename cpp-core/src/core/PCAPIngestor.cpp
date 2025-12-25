#include "core/PCAPIngestor.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring> 
#include <chrono>
#include <thread>
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



PCAPIngestor::PCAPIngestor(const string& path, ThreadPool& pool): 
	pcap_path(path), 
	threadPool(pool),
	first_packet_set(false),
	stop_workers(false) {
}

chrono::steady_clock::time_point PCAPIngestor::convertPcapTime(const struct timeval& ts) {
	auto pcap_time = chrono::system_clock::time_point(
		chrono::seconds(ts.tv_sec) +
		chrono::microseconds(ts.tv_usec)
	);
	
	static chrono::steady_clock::time_point base_time = chrono::steady_clock::now();
	
	if (!first_packet_set) {
		first_pcap_time = pcap_time;
		first_packet_set = true;
		base_time = chrono::steady_clock::now();
	}
	
	auto pcap_duration = chrono::duration_cast<chrono::microseconds>(pcap_time - first_pcap_time);
	return base_time + pcap_duration;
}


void PCAPIngestor::run(FlowAggregator& aggregator) {

	first_packet_set = false;
	stop_workers = false;

	// Submit worker tasks to ThreadPool that continuously consume from packetQueue
	const size_t numWorkers = thread::hardware_concurrency();
	for (size_t i = 0; i < numWorkers; i++) {
		threadPool.submit([&aggregator, this]() {
			PacketTask task;
			while (!stop_workers || !packetQueue.empty()) {
				if (packetQueue.pop(task)) {
					aggregator.ingestPacket(
						task.key,
						task.bytes,
						task.syn,
						task.fin,
						task.rst,
						task.ack,
						task.packet_time
					);
				} else {
					this_thread::yield();
				}
			}
		});
	}

	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t* handle = pcap_open_offline(pcap_path.c_str(), errbuf);
	if (!handle) {
		stop_workers = true;
		// Wait for submitted tasks to complete
		while (threadPool.queueSize() > 0 || !packetQueue.empty()) {
			this_thread::sleep_for(chrono::milliseconds(10));
		}
		return;
	}

	const u_char* packet;
	struct pcap_pkthdr* header;

	// Main thread: Read PCAP and push to queue (workers process concurrently)
	while (pcap_next_ex(handle, &header, &packet) > 0) {

        if (!header || !packet) continue;

        // non ipv4 ethernet packets need to be skipped
        const uint16_t eth_type = (packet[12] << 8) | packet[13];
        if (eth_type != 0x0800) continue; 

		// ignore the 14 bytes of the ethernet II frame to get IP Header
		const struct ip_header* iphdr = reinterpret_cast<const struct ip_header*>(packet + 14);

		PacketTask task{};
		task.bytes = header->len;
		task.packet_time = convertPcapTime(header->ts);

        memset(task.key.src_ip.data(), 0, 12);
        memcpy(task.key.src_ip.data()+12, &iphdr->saddr, 4);

        memset(task.key.dst_ip.data(), 0, 12);
        memcpy(task.key.dst_ip.data() + 12, &iphdr->daddr, 4);

        if (iphdr->protocol == IPPROTO_TCP) {
            const tcp_header* tcp = reinterpret_cast<const tcp_header*>(
                packet + 14 + iphdr->ihl * 4
            );

            task.key.src_port = ntohs(tcp->source);
            task.key.dst_port = ntohs(tcp->dest);
            task.key.protocol = L4Proto::TCP;
            task.syn = (tcp->flags & TH_SYN) != 0;
            task.fin = (tcp->flags & TH_FIN) != 0;
            task.rst = (tcp->flags & TH_RST) != 0;
            task.ack = (tcp->flags & TH_ACK) != 0;

        }
        else if (iphdr->protocol == IPPROTO_UDP) {
            const udp_header* udp = reinterpret_cast<const udp_header*>(packet + 14 + iphdr->ihl * 4);
            task.key.src_port = ntohs(udp->sport);
            task.key.dst_port = ntohs(udp->dport);
            task.key.protocol = L4Proto::UDP;
            task.syn = false;
            task.fin = false;
            task.rst = false;
            task.ack = false;
        }
		else {
			continue;
		}

		// Push to queue (workers automatically process)
		while (!packetQueue.push(task)) {
			this_thread::yield();
		}
	}

	pcap_close(handle);

	// Signal workers to finish when queue is empty
	stop_workers = true;

	// Wait for all submitted tasks to complete and queue to drain
	while (threadPool.queueSize() > 0 || !packetQueue.empty()) {
		this_thread::sleep_for(chrono::milliseconds(10));
	}

}