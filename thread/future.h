#ifndef FUTURE_H_INCLUDED
#define FUTURE_H_INCLUDED

#include <boost/thread/thread.hpp>
#include <boost/thread/future.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility/result_of.hpp>

namespace thread {

template<typename Func>
boost::shared_future<typename boost::result_of<Func()>::type> submit_task(Func f) {
    typedef typename boost::result_of<Func()>::type ResultType;
    typedef boost::packaged_task<ResultType> PackagedTaskType;

    PackagedTaskType task(f);
    boost::shared_future<ResultType> res(task.get_future());
    boost::thread task_thread(boost::move(task));
    return res;
}

}

#endif // FUTURE_H_INCLUDED
