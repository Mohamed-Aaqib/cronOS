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

public:

	Node(NodeId _id, string& _ip) :id(_id), ip(_ip) {};

	NodeId getId() const { return id; };
	const string& getIp() const { return ip; };

	bool operator==(const Node& other) const {
		return id == other.id;
	}

};