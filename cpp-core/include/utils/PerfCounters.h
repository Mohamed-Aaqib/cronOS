#pragma once
#include <atomic>
#include <chrono>
using namespace std;

//minimal functions not needed to be passed through 


namespace ChronOS::Utils {

	struct PerfCounters {

		atomic<uint64_t> events_processed{ 0 };
		atomic<uint64_t> bytes_processed{ 0 };
		atomic<uint64_t> current_queue_depth{ 0 };

		//its monotomic
		chrono::steady_clock::time_point start_time = chrono::steady_clock::now();

		void incrementEvents(uint64_t n = 0) {
			events_processed += n;
		}

		void incrementBytes(uint64_t n = 0) {
			bytes_processed += n;
		}

		double getEventsThroughput() const {
			auto elapsed = chrono::duration<double>(chrono::steady_clock::now() - start_time).count();
			return elapsed > 0 ? (events_processed.load()) / elapsed : 0.0;
		}

		double getBytesThroughput() const {
			auto elapsed = chrono::duration<double>(chrono::steady_clock::now() - start_time).count();
			return elapsed > 0 ? (bytes_processed.load()) / elapsed : 0.0;
		}

	};



}
