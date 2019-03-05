#pragma once

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <atomic>

namespace netlistDB {
namespace utils {

class TaskGroup;
class ThreadPool {
public:
	boost::asio::io_service ios;
protected:
	boost::thread_group thread_grp;
	// without the work the threads will end immediately
	boost::asio::io_service::work work;

public:

	ThreadPool(const ThreadPool &) = delete;
	// construct thread pool with the optimal number of threads
	ThreadPool();
	// construct thread pool with the specified number of threads
	ThreadPool(size_t pool_size);

	size_t size();

	~ThreadPool();
private:
	void wrap_job(boost::function<void()> job);
};

class TaskGroup {
protected:
	ThreadPool & thread_pool;
	std::atomic<size_t> dispatched;
	std::atomic<size_t> completed;

	void wrap_job(boost::function<void()> job);
public:

	TaskGroup(const TaskGroup &) = delete;
	TaskGroup(ThreadPool & thread_pool);

	template<typename Job>
	auto run(Job job) {
		dispatched++;
		return thread_pool.ios.post(
				boost::bind(&TaskGroup::wrap_job, this,
						boost::function<void()>(job)));
	}

	void parallel_for(int first, int last, const std::function<void(int)>& f);

	void wait();
	~TaskGroup();
};

}
}
