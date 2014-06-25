#ifndef KAZBASE_THREAD_H
#define KAZBASE_THREAD_H

#include <memory>
#include <chrono>
#include <thread>

class Thread {
public:
	Thread():
		stop_requested_(false) {}
	
	virtual ~Thread() {}
	
	void start() {
		assert(!worker_);
        worker_ = std::make_shared<std::thread>(std::bind(&Thread::run, this));
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
        std::chrono::seconds delay(seconds);
        std::this_thread::sleep_for(delay);
	}
			
private:
	volatile bool stop_requested_;
    std::shared_ptr<std::thread> worker_;
};

#endif
