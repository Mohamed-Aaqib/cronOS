#pragma once
#include "core/FlowAggregator.h"
#include "core/Graph.h"
using namespace std;

class GraphBuilder {

private:
	shared_ptr<Graph> graph;

public:

	explicit GraphBuilder(shared_ptr<Graph> graph_);

	void ingestEvent(const NetworkEvent& evt);

	template<typename Iterable>
	void ingestEvents(const Iterable& events) {
		for (const auto& evt : events) {
			ingestEvent(evt);
		}
	}


};