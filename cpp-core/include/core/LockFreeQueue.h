#pragma once
#include <thread>
#include <atomic>
#include <chrono>
#include <memory>
#include <vector>
using namespace std;


template<typename T, size_t Capacity>
class LockFreeQueue {
	
	static_assert((Capacity& (Capacity - 1)) == 0, "Capacity must be power of 2");
		
private:
	struct Node {
		alignas(64) atomic<size_t> sequence;
		T data;
	};

	alignas(64) vector<Node> buffer_;
	const size_t mask_;
	alignas(64) atomic<size_t> head_;
	alignas(64) atomic<size_t> tail_;

public:
	
	LockFreeQueue() : buffer_(Capacity), mask_(Capacity - 1), head_(0), tail_(0) {
		for (size_t i = 0; i < Capacity; i++) {
			buffer_[i].sequence.store(i, memory_order_relaxed);
		}
	}

	bool push(const T& Item) {

		Node* node;
		size_t pos = head_.load(memory_order_relaxed);
		for (;;) {

			node = &buffer_[pos & mask_];
			size_t seq = node->sequence.load(memory_order_acquire);
			intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);

			if (diff == 0) {
				if (head_.compare_exchange_weak(pos, pos + 1, memory_order_relaxed)) {
					break;
				}
			}
			else if (diff < 0) {
				//full
				return false;
			}
			else {
				pos = head_.load(memory_order_relaxed);
			}

		}

		node->data = item;
		node->sequence.store(pos + 1, memory_order_release);
		return true;

	}

	bool pop(T& item) {

		Node* node;
		size_t pos = tail_.load(memory_order_relaxed);
		for (;;) {

			node = &buffer_[pos & mask_];
			size_t seq = node->sequence.load(memory_order_acquire);
			intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);

			if (diff == 0) {
				if (tail_.compare_exchange_weak(pos, pos + 1, memory_order_relaxed)) {
					break;
				}
			}
			else if (diff < 0) {
				//empty
				return false;
			}
			else {
				// only occurs when diff > 0 where one thread gets beaten multiple times
				pos = tail_.load(memory_order_relaxed);
			}

		}

		item = node->data;
		node->sequence.store(pos + Capacity, memory_order_release);
		return true;

	}

	size_t size() const {

		size_t h = head_.load(memory_order_acquire);
		size_t t = tail_.load(memory_order_acquire);
		return h >= t ? h - t : Capacity - t + h;

	}

	bool empty() const { return size() == 0; };
	bool full() const { return size() == Capacity; };

};