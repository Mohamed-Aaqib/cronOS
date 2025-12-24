#pragma once
#include <thread>
#include <iostream>
#include "core/ThreadPool.h"
using namespace std;



void ThreadPool::workerLoop() {
	Task task;
	while (running_) {
		if (taskQueue_.pop(task)) {
			try{
				task();
			}
			catch (const std::exception&){
				cerr << "task cannot be called \n";
			}
		}else {
			this_thread::yield();
		}
	}
}


ThreadPool::ThreadPool(size_t numThreads) {
	for (size_t i = 0; i < numThreads; i++) {
		workers_.emplace_back(&ThreadPool::workerLoop, this);
	}
}

bool ThreadPool::submit(Task&& task) {
	return taskQueue_.push(move(task));
}

size_t ThreadPool::queueSize() const { return taskQueue_.size(); }

ThreadPool::~ThreadPool() {
	running_ = false;
	for (auto& w : workers_) {
		if (w.joinable()) {
			w.join();
		}
	}
}
