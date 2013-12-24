//
// compiler_support.hpp
//
// (c) 2013 helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS3_COMPILER_SUPPORT_HPP
#define BOOST_SIGNALS3_COMPILER_SUPPORT_HPP

#include <boost/move/move.hpp>
#include <boost/smart_ptr/detail/spinlock_pool.hpp>

// testing defines
//#define BOOST_NO_CXX11_ATOMIC
//#define BOOST_NO_CXX11_HDR_FUNCTIONAL
//#define BOOST_NO_CXX11_HDR_TUPLE

#include <memory>
#include <atomic>
#include <mutex>
#include <functional>
#include <numeric>
#include <forward_list>
#include <tuple>
#include <memory>

namespace boost
{
    namespace signals3
    {
        namespace detail
        {
            // shared_ptr and atomic shared_ptr
            using ::std::shared_ptr;
            using ::std::weak_ptr;
            using ::std::static_pointer_cast;
            using ::std::atomic_compare_exchange_strong;

            /**
             * FIXME! Compilers don't yet support atomic_load/store from C++11, so this needs
             * deleting when they do! DIRTY DIRTY UNSAFE HACK!
             */
            template<class T> std::shared_ptr<T> atomic_load(std::shared_ptr<T> const * p)
            {
                boost::detail::spinlock_pool<2>::scoped_lock lock( p );
                return *p;
            }

            template<class T> void atomic_store(std::shared_ptr<T> * p, std::shared_ptr<T> r)
            {
                boost::detail::spinlock_pool<2>::scoped_lock lock( p );
                p->swap(r);
            }

            using ::std::make_shared;

            // atomic int
            using ::std::atomic;

            // thread mutex and unique locking
            using ::std::mutex;
            using ::std::lock_guard;

            // tuple
            using ::std::tuple;
            using ::std::get;
            using ::std::function;

            // forward list
            using ::std::forward_list;
        }
    }
}

#endif // BOOST_SIGNALS3_COMPILER_SUPPORT_HPP
