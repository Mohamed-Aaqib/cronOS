#pragma once
#include <vector>
#include "core/TemporalEdge.h"
#include "core/Graph.h"
using namespace std;

class Path {

public:
	using EdgePtr = shared_ptr<TemporalEdge>;

private:
	vector<EdgePtr> edges;

public:
	Path() = default;

	void add_edge(EdgePtr edge) {
		edges.push_back(edge);
	}

	const vector<EdgePtr>& getEdges() const {
		return edges;
	}

	void removeLastEdge() {
		if (!edges.empty()) edges.pop_back();
	}

	uint64_t getTotalPackets() const {
		uint64_t sum = 0;
		for (auto& e : edges) {
			sum += e->get_packetCount();
		}
		return sum;
	}

	uint64_t getTotalBytes() const {
		uint64_t sum = 0;
		for (auto& e : edges) {
			sum += e->get_byteCount();
		}
		return sum;
	}

	bool empty() const {
		return edges.empty();
	}

	void clear() {
		edges.clear();
	}

};