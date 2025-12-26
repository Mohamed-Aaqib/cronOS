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