#pragma once
#include <cstdint>
using namespace std;

class Edge {

private:
	uint64_t src;
	uint64_t dst;

public:
	
	Edge(uint64_t src_, uint64_t dst_):src(src_),dst(dst_) {};
	
	uint64_t get_src() const { return src; };
	uint64_t get_dst() const { return dst; };


	virtual ~Edge() = default;

};
