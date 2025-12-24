#include <vector>
#include <unordered_set>
#include "core/PathReconstructor.h"
#include "core/TemporalEdge.h"
using namespace std;

void PathReconstructor::build_adjacency() {
	adjacency.clear();
	for (auto& edge : graph.get_edges()) {
		adjacency[edge->get_src()].push_back(edge);
	}
}

// This is used to test
void PathReconstructor::dfs(
	Node::NodeId current,
	Path& currentPath,
	std::vector<Path>& allPaths,
	size_t maxDepth,
	unordered_set<Node::NodeId>& visited
) {

	if (currentPath.getEdges().size() >= maxDepth) {
		allPaths.push_back(currentPath);
		return;
	}

	visited.insert(current);

	auto it = adjacency.find(current);
	if (it == adjacency.end()) return;

	for (auto& edge : it->second) {
		auto dst = edge->get_dst();
		if (visited.find(dst) != visited.end()) continue;

		currentPath.add_edge(edge);
		dfs(dst, currentPath, allPaths, maxDepth, visited);
		currentPath.remove_lastEdge();
	}

	visited.erase(current);

	if (!currentPath.empty()) allPaths.push_back(currentPath); 

}


vector<Path> PathReconstructor::recostructPath(size_t maxDepth) {
	std::vector<Path> allPaths;

	for (auto& [nodeId, nodePtr] : graph.get_nodes()) {
		Path path;
		std::unordered_set<Node::NodeId> visited;
		dfs(nodeId, path, allPaths, maxDepth, visited);
	}

	return allPaths;
}


