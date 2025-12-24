#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <unordered_map>
#include "core/Node.h"
#include "core/TemporalEdge.h"
using namespace std;

class Graph {

private:

	unordered_map<string, Node::NodeId> ip_to_node;
	unordered_map<Node::NodeId, shared_ptr<Node>> nodes;
	vector<shared_ptr<TemporalEdge>> edges;

	// TODO: horrible way of adding node, should reconsider this.
	Node::NodeId next_node_id = 1;

	mutable mutex nodes_mutex;
	mutable mutex edges_mutex;

public:

	Graph() = default;

	shared_ptr<Node> get_create_node(string& ip) {

		// get 
		//TODO we may access nodes created too early in rare cases, fix that soon
		//TODO after more research shared_lock makes sense as its a reader writer problem
		auto it = ip_to_node.find(ip);
		if (it != ip_to_node.end()) return nodes[it->second];

		// create
		shared_ptr<Node> node;
		{
			lock_guard<mutex> lock(nodes_mutex);

			Node::NodeId id = next_node_id++;
			node = make_shared<Node>(id, ip);

			ip_to_node[ip] = id;
			nodes[id] = node;
		}
		return node;
	
	}

	shared_ptr<TemporalEdge> add_temporal_edge(string& src_ip, string& dst_ip, TemporalEdge::TimePoint start, TemporalEdge::TimePoint end, uint8_t protocol) {


		auto src = get_create_node(src_ip);
		auto dst = get_create_node(dst_ip);

		auto edge = make_shared<TemporalEdge>(
			src->getId(),
			dst->getId(),
			start,
			end,
			protocol
		);

		{
			lock_guard<mutex> lock(edges_mutex);
			edges.push_back(edge);
		}

		return edge;
	}

	const unordered_map<Node::NodeId, shared_ptr<Node>>& get_nodes() const {
		return nodes;
	}

	const vector<shared_ptr<TemporalEdge>>& get_edges() const {
		return edges;
	}



};