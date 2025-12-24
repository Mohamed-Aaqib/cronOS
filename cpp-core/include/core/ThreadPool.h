#pragma once
#include <future>
#include <memory>
#include <atomic>
#include <thread>
#include <vector>
#include <iostream>
#include "LockFreeQueue.h"
using namespace std;



class ThreadPool {

public:
	using Task = function<void()>;

private:
	
	LockFreeQueue<Task, 1024> taskQueue_;
	vector<thread> workers_;
	atomic<bool> running_{ true };

	void workerLoop();

public:

	explicit ThreadPool(size_t numThreads = thread::hardware_concurrency());

	bool submit(Task&& task);

	size_t queueSize() const;

	~ThreadPool();



};