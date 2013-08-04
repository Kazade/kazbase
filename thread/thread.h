#ifndef KAZBASE_THREAD_H
#define KAZBASE_THREAD_H

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <memory>

class Thread {
public:
	Thread():
		stop_requested_(false) {}
	
	virtual ~Thread() {}
	
	void start() {
		assert(!worker_);
		worker_.reset(new boost::thread(boost::bind(&Thread::run, this)));
	}
	
	void stop() {
		assert(worker_);
		stop_requested_ = true;
		worker_->join();
	}

	virtual void run() = 0;

protected:
	bool stop_requested() const { return stop_requested_; } 

	void sleep(int seconds) {
		boost::posix_time::seconds to_sleep(seconds);
		boost::this_thread::sleep(to_sleep);
	}
			
private:
	volatile bool stop_requested_;
	std::shared_ptr<boost::thread> worker_;
};

#endif
