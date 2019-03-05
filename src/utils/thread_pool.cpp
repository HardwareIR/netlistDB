#include <netlistDB/utils/thread_pool.h>
#include <thread>

namespace netlistDB {
namespace utils {

ThreadPool::ThreadPool() :
		ThreadPool(std::thread::hardware_concurrency()) {
}

ThreadPool::ThreadPool(size_t pool_size) :
		work(ios) {

	assert(pool_size > 0);
	for (size_t i = 0; i < pool_size; ++i) {
		std::function<void()> fn = boost::bind(&boost::asio::io_service::run,
				&ios);
		thread_grp.create_thread(fn);
	}
}

ThreadPool::~ThreadPool() {
	ios.stop();
	try {
		thread_grp.join_all();
	} catch (const std::exception&) {
	}
}

size_t ThreadPool::size() {
	return thread_grp.size();
}

void ThreadPool::wrap_job(boost::function<void()> job) {
	job();
}

void TaskGroup::parallel_for(int first, int last,
		const std::function<void(int)>& f) {
	int step = 0;
	if (first == last)
		return;
	assert(first < last);
	step = last - first / thread_pool.size();
	if (step == 0) {
		step = 1;
	}
	// dispatch batches
	for (int start = first; start < last; start += step) {
		run([f, start, step, last]() {
			int end = std::min(last, start + step);
			// run tasks in the batch
			for (int i = start; i < end; i++) {
				f(i);
			}
		});
	}
}
void TaskGroup::wrap_job(boost::function<void()> job) {
	job();
	completed++;

}

TaskGroup::TaskGroup(ThreadPool & thread_pool) :
		thread_pool(thread_pool), dispatched(0), completed(0) {
}

void TaskGroup::wait() {
	while (completed != dispatched) {
		thread_pool.ios.poll();
	}
	completed = 0;
	dispatched = 0;
}

TaskGroup::~TaskGroup() {
	wait();
}

}
}
