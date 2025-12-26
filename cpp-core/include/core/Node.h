#pragma once
#include <cstdint>
#include <string>
using namespace std;

class Node {

public:
	using NodeId = uint64_t;

private:
	NodeId id;
	string ip;
	
	// Behavioral features for GNN attack detection
	uint32_t degree = 0;
	uint32_t in_degree = 0;
	uint32_t out_degree = 0;
	uint32_t unique_ports = 0;
	uint64_t total_bytes_sent = 0;
	uint64_t total_bytes_recv = 0;
	bool is_internal = false;  // Internal vs external IP

public:

	Node(NodeId _id, string& _ip) :id(_id), ip(_ip) {
		// Determine if internal IP (simplified: 127.x.x.x, 10.x.x.x, 192.168.x.x, 172.16-31.x.x)
		is_internal = (ip.find("127.") == 0) || 
		              (ip.find("10.") == 0) ||
		              (ip.find("192.168.") == 0) ||
		              (ip.find("172.") == 0);
	};

	NodeId getId() const { return id; };
	const string& getIp() const { return ip; };
	
	// Getters for behavioral features
	uint32_t get_degree() const { return degree; }
	uint32_t get_in_degree() const { return in_degree; }
	uint32_t get_out_degree() const { return out_degree; }
	uint32_t get_unique_ports() const { return unique_ports; }
	uint64_t get_total_bytes_sent() const { return total_bytes_sent; }
	uint64_t get_total_bytes_recv() const { return total_bytes_recv; }
	bool get_is_internal() const { return is_internal; }
	
	// Methods to update features (called by GraphBuilder)
	void increment_in_degree() { in_degree++; degree = in_degree + out_degree; }
	void increment_out_degree() { out_degree++; degree = in_degree + out_degree; }
	void add_bytes_sent(uint64_t bytes) { total_bytes_sent += bytes; }
	void add_bytes_recv(uint64_t bytes) { total_bytes_recv += bytes; }
	void increment_unique_port() { unique_ports++; }

	bool operator==(const Node& other) const {
		return id == other.id;
	}

};