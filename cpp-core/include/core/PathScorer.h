#pragma once
#include "core/Path.h"
using namespace std;

class PathScorer {

public:

	static double score(const Path& path) {

		if (path.empty()) return 0.0;

		double score = 0.0;
		for (auto& e: path.getEdges()) {
			score += e->get_packetCount() * 1.0 + e->get_byteCount() * 0.001;
		}

		return score;

	}

};

