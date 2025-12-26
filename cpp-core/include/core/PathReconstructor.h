#pragma once
#include <chrono>
#include <unordered_map>
#include "core/Node.h"
#include <unordered_set>
#include "core/TemporalEdge.h"
#include <core/Graph.h>
#include "core/Path.h"
using namespace std;


class PathReconstructor {

private:
	Graph& graph;
	unordered_map<Node::NodeId, vector<shared_ptr<TemporalEdge>>> adjacency;

public:

	// Build adjacency list from graph edges (public so callers can refresh after graph changes)
	void build_adjacency();

	explicit PathReconstructor(Graph& g) : graph(g) { build_adjacency(); };

};