#ifndef FUTURE_H_INCLUDED
#define FUTURE_H_INCLUDED

#include <boost/thread.hpp>
#include <memory>
#include <type_traits>

namespace boost {
   using std::move;
}

namespace thread {

template<typename Func>
boost::unique_future<typename std::result_of<Func()>::type> submit_task(Func f) {
    typedef typename std::result_of<Func()>::type ResultType;

    boost::packaged_task<ResultType> task(f);
    boost::unique_future<ResultType> res = task.get_future();
    boost::thread(boost::move(task)).detach();
    return res;
}

}

#endif // FUTURE_H_INCLUDED
