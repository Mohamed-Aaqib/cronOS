#pragma once
#include <atomic>
#include <memory>
#include <vector>
using namespace std;

template<typename T, size_t size>
class RingBuffer {

private:
	vector<shared_ptr<T>> buffer;
	atomic<size_t> head{0};
	atomic<size_t> tail{0};
public:

	RingBuffer() :buffer(size) {};

	bool push(shared_ptr<T> item) {

		size_t curr_head = head.load(memory_order_relaxed);
		size_t next_head = (curr_head + 1) % size;

		if (next_head == tail.load(memory_order_acquire)) {
			return false;
		}

		buffer[curr_head] = item;
		head.store(next_head, memory_order_release);
		return true;
	}

	bool pop(shared_ptr<T>& item) {

		size_t curr_tail = tail.load(memory_order_relaxed);

		if (curr_tail == head.load(memory_order_acquire)) {
			return false;
		}

		item = buffer[curr_tail];
		tail.store(((curr_tail+1) % size),memory_order_release)

		return true;
	}


};
