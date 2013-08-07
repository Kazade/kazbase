#ifndef FUTURE_H_INCLUDED
#define FUTURE_H_INCLUDED

#include <thread>
#include <future>
#include <memory>
#include <type_traits>

namespace thread {

template<typename Func>
std::shared_future<typename std::result_of<Func()>::type> submit_task(Func f) {
    typedef typename std::result_of<Func()>::type ResultType;

    std::packaged_task<ResultType ()> task(f);
    std::shared_future<ResultType> res = task.get_future();
    std::thread(std::move(task)).detach();
    return res;
}

}

#endif // FUTURE_H_INCLUDED
