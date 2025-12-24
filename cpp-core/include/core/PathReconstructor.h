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

	void build_adjacency();

	void dfs(
		Node::NodeId current,
		Path& currentPath,
		std::vector<Path>& allPaths,
		size_t maxDepth,
		unordered_set<Node::NodeId>& visited 
	);
		
public:

	explicit PathReconstructor(Graph& g) : graph(g) { build_adjacency(); };

	vector<Path> recostructPath(size_t maxDepth = 5);


};