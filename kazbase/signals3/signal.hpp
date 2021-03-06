//
// signals.hpp
//
// (c) 2013 helloworld922
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SIGNALS3_SIGNAL_HPP
#define BOOST_SIGNALS3_SIGNAL_HPP

#include "detail/extended_signature.hpp"
//#include <detail/node_base.hpp>
//#include <detail/signal_base.hpp>
#include "detail/compiler_support.hpp"
#include "slots.hpp"
#include "connection.hpp"
#include "optional_last_value.hpp"
#include <map>

namespace boost
{
    namespace signals3
    {
        template<typename Signature, typename Combiner = optional_last_value<
                typename ::boost::signals3::detail::function< Signature >::result_type >,
                typename Group = int, typename GroupCompare = std::less< Group >,
                typename FunctionType = ::boost::signals3::detail::function< Signature >,
                typename ExtendedFunctionType = typename ::boost::signals3::detail::extended_signature<
                        Signature >::type, typename Mutex = ::boost::signals3::detail::mutex>
            class signal;

        namespace detail
        {
            class signal_base;

            class node_base;
        }

        template<typename ResultType, typename ... Args, typename Combiner, typename Group,
                typename GroupCompare, typename FunctionType, typename ExtendedFunctionType,
                typename Mutex>
            class signal< ResultType
            (Args...), Combiner, Group, GroupCompare, FunctionType, ExtendedFunctionType, Mutex > : public ::boost::signals3::detail::signal_base
            {
            public:
                // typedefs
                typedef Mutex mutex_type;
                typedef boost::signals3::detail::lock_guard< mutex_type > lock_guard_type;
                typedef ResultType result_type;
                typedef Combiner combiner_type;
                typedef Group group_type;
                typedef GroupCompare group_compare_type;
                typedef boost::signals3::slot< ResultType
                (Args...), FunctionType > slot_type;
                typedef boost::signals3::slot< ResultType
                (const connection&, Args...), ExtendedFunctionType > extended_slot_type;
                typedef ::boost::signals3::detail::atomic< int > atomic_int_type;
            private:
                // used for unpacking tuple to function call
                template<int...>
                struct seq
                {};
                template<int N, int... S>
                struct gens : gens<N - 1, N - 1, S...>
                {};
                template<int... S>
                struct gens<0, S...>
                {
                    typedef seq<S...> type;
                };

                struct t_node_base;

                // private typedefs
                typedef std::multimap<group_type, ::boost::signals3::detail::weak_ptr<t_node_base>, group_compare_type > group_storage_type;

                struct t_node_base : public ::boost::signals3::detail::node_base
                {
                    ::boost::signals3::detail::shared_ptr< t_node_base > next;
                    ::boost::signals3::detail::weak_ptr< t_node_base > prev;

                    virtual bool
                    try_lock(
                            ::boost::signals3::detail::forward_list<
                            ::boost::signals3::detail::shared_ptr< void > >& list) const = 0;

                    virtual typename group_storage_type::iterator* const pos(void)
                    {
                        return nullptr;
                    }

                    virtual bool
                    grouped(void) const
                    {
                        return false;
                    }

                    virtual ResultType
                    operator()(boost::signals3::detail::tuple< Args... > params) const = 0;
                };

                struct node : public t_node_base
                {
                    slot_type callback;
                    node(const slot_type& callback) :
                    callback(callback)
                    {
                    }

                    node(slot_type&& callback) :
                    callback(std::move(callback))
                    {
                    }

                    virtual bool
                    try_lock(
                            ::boost::signals3::detail::forward_list<
                            ::boost::signals3::detail::shared_ptr< void > >& list) const override
                    {
                        return callback.try_lock(list);
                    }

                    template<int... S>
                    inline ResultType call_func(seq<S...>, ::boost::signals3::detail::tuple<Args...>& params) const
                    {
                        return callback.slot_function()(std::forward<Args>(::boost::signals3::detail::get<S>(params))...);
                    }

                    virtual ResultType
                    operator()(::boost::signals3::detail::tuple<Args...> params) const override
                    {
                        return call_func(typename gens<sizeof...(Args)>::type(), params);
                    }
                };

                struct extended_node : public t_node_base
                {
                    extended_slot_type callback;
                    connection conn;

                    extended_node(const extended_slot_type& callback) :
                    callback(callback)
                    {
                    }

                    extended_node(extended_slot_type&& callback) :
                    callback(std::move(callback))
                    {
                    }

                    virtual bool
                    try_lock(
                            ::boost::signals3::detail::forward_list<
                            ::boost::signals3::detail::shared_ptr< void > >& list) const override
                    {
                        return callback.try_lock(list);
                    }

                    template<int... S>
                    inline ResultType call_func(seq<S...>, ::boost::signals3::detail::tuple<Args...>& params) const
                    {
                        return callback.slot_function()(conn, std::forward<Args>(::boost::signals3::detail::get<S>(params))...);
                    }

                    virtual ResultType
                    operator()(::boost::signals3::detail::tuple<Args...> params) const override
                    {
                        return call_func(typename gens<sizeof...(Args)>::type(), params);
                    }
                };

                template<typename B>
                struct grouped_node : public B
                {
                    typename group_storage_type::iterator iter;

                    template<typename SlotType>
                    grouped_node(const SlotType& callback) : B(callback)
                    {}

                    template<typename SlotType>
                    grouped_node(SlotType&& callback) : B(std::forward<SlotType>(callback))
                    {}

                    grouped_node(const grouped_node& rhs) : B(rhs), iter(rhs.iter)
                    {
                    }

                    grouped_node(grouped_node& rhs) : B(rhs), iter(rhs.iter)
                    {
                    }

                    grouped_node(grouped_node&& rhs) : B(std::move(rhs)), iter(std::move(rhs.iter))
                    {
                    }

                    typename group_storage_type::iterator * const pos(void)
                    {
                        return &iter;
                    }

                    bool grouped(void) const
                    {
                        return true;
                    }
                };

                class iterator;
                //friend class signal::iterator;

                class unsafe_iterator;
                //friend class signal::unsafe_iterator;

                // fields
                ::boost::signals3::detail::shared_ptr< t_node_base > head;
                ::boost::signals3::detail::shared_ptr< t_node_base > tail;
                ::boost::signals3::detail::shared_ptr< t_node_base > group_head;

                group_storage_type group_storage;

                mutex_type _mutex;

                Combiner combiner;

                void
                push_back_impl(::boost::signals3::detail::shared_ptr< t_node_base >&& node)
                {
                    lock_guard_type _lock(_mutex);
                    if (tail == nullptr)
                    {
                        // only one node
                        tail = node;
                        ::boost::signals3::detail::atomic_store(&head, std::move(node));
                    }
                    else
                    {
                        // more than one node
                        node->prev = tail;
                        ::boost::signals3::detail::atomic_store(&(tail->next), node);
                        tail = std::move(node);
                    }
                }

                void
                push_front_impl(::boost::signals3::detail::shared_ptr< t_node_base >&& node)
                {
                    lock_guard_type _lock(_mutex);
                    if (head == nullptr)
                    {
                        // only one node
                        tail = node;
                        group_head = node;
                    }
                    else
                    {
                        // more than one node
                        if (group_head == nullptr)
                        {
                            group_head = node;
                        }
                        node->next = head;
                    }
                    ::boost::signals3::detail::atomic_store(&head, std::move(node));
                }

                void
                push_back_impl_unsafe(::boost::signals3::detail::shared_ptr< t_node_base >&& node)
                {
                    if (tail == nullptr)
                    {
                        // only one node
                        tail = node;
                        head = std::move(node);
                    }
                    else
                    {
                        // more than one node
                        node->prev = tail;
                        tail->next = node;
                        tail = std::move(node);
                    }
                }

                void
                push_front_impl_unsafe(::boost::signals3::detail::shared_ptr< t_node_base >&& node)
                {
                    if (head == nullptr)
                    {
                        // only one node
                        tail = node;
                        group_head = node;
                    }
                    else
                    {
                        // more than one node
                        if (group_head == nullptr)
                        {
                            group_head = node;
                        }
                        node->next = head;
                    }
                    head = std::move(node);
                }

                /**
                 * Assumes caller already has a unique lock
                 */
                void pop_back_impl(void)
                {
                    if (tail != nullptr)
                    {
                        // actually have a node to remove
                        tail->mark_disconnected();
                        if(tail->grouped())
                        {
                            // handle group slots
                            group_storage.erase(*(tail->pos()));
                        }
                        ::boost::signals3::detail::shared_ptr< t_node_base > prev = tail->prev.lock();
                        if (prev != nullptr)
                        {
                            // more than 1 node
                            tail->prev.reset();
                            if (group_head == tail)
                            {
                                group_head = prev;
                            }
                            ::boost::signals3::detail::atomic_store(&(prev->next), ::boost::signals3::detail::shared_ptr< t_node_base >());

                            tail = std::move(prev);
                        }
                        else
                        {
                            // only one node
                            tail.reset();
                            group_head.reset();
                            ::boost::signals3::detail::atomic_store(&head, tail);
                        }
                    }
                }

                /**
                 * Assumes caller already has a unique lock
                 */
                void pop_front_impl(void)
                {
                    if (head != nullptr)
                    {
                        // actually have a node to remove
                        head->mark_disconnected();
                        if(head->grouped())
                        {
                            // handle group slots
                            group_storage.erase(*(head->pos()));
                        }
                        if (head == tail)
                        {
                            // only have one node
                            tail.reset();
                            group_head.reset();
                            ::boost::signals3::detail::atomic_store(&head, tail);
                        }
                        else
                        {
                            // more than one node
                            if (group_head == head)
                            {
                                group_head.reset();
                            }
                            ::boost::signals3::detail::atomic_store(&head, head->next);
                        }
                    }
                }

                void disconnect(::boost::signals3::detail::shared_ptr< t_node_base >& node)
                {
                    lock_guard_type _lock(_mutex);
                    if(node->mark_disconnected())
                    {
                        if(node == head)
                        {
                            pop_front_impl();
                        }
                        else if(node == tail)
                        {
                            pop_back_impl();
                        }
                        else
                        {
                            if(node->grouped())
                            {
                                // handle group slots
                                group_storage.erase(*(node->pos()));
                            }

                            ::boost::signals3::detail::shared_ptr< t_node_base > prev = node->prev.lock();

                            if(node == group_head)
                            {
                                group_head = prev;
                            }
                            ::boost::signals3::detail::atomic_store(&(prev->next), node->next);
                            node->next->prev = std::move(prev);
                        }
                    }
                }

                virtual void disconnect(::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base >&& n) override
                {
                    ::boost::signals3::detail::shared_ptr<t_node_base> node = ::boost::signals3::detail::static_pointer_cast<t_node_base>(std::move(n));
                    disconnect(node);
                }

                virtual void disconnect_unsafe(::boost::signals3::detail::shared_ptr< ::boost::signals3::detail::node_base >&& n) override
                {
                    ::boost::signals3::detail::shared_ptr<t_node_base> node = ::boost::signals3::detail::static_pointer_cast<t_node_base>(std::move(n));
                    disconnect_unsafe(node);
                }

                void disconnect_unsafe(::boost::signals3::detail::shared_ptr< t_node_base >& node)
                {
                    if(node->mark_disconnected())
                    {
                        if(node == head)
                        {
                            pop_front_unsafe();
                        }
                        else if(node == tail)
                        {
                            pop_back_unsafe();
                        }
                        else
                        {
                            if(node->grouped())
                            {
                                // handle group slots
                                group_storage.erase(*(node->pos()));
                            }

                            ::boost::signals3::detail::shared_ptr< t_node_base > prev = node->prev.lock();
                            if(node == group_head)
                            {
                                group_head = prev;
                            }
                            prev->next = node->next;
                            node->next->prev = std::move(prev);
                        }
                    }
                }

                template<typename B>
                void insert_impl(const group_type& group, ::boost::signals3::detail::shared_ptr< grouped_node<B> >&& n)
                {
                    typename group_storage_type::value_type val(group, n);
                    lock_guard_type _lock(_mutex);
                    typename group_storage_type::iterator iter = group_storage.insert(std::move(val));
                    n->iter = iter;
                    if(iter != group_storage.begin())
                    {
                        // n is not going to be the new group_head
                        typename group_storage_type::iterator prev_iter = iter;
                        --prev_iter;
                        ::boost::signals3::detail::shared_ptr<t_node_base> prev = prev_iter->second.lock();
                        n->prev = prev_iter->second;
                        n->next = prev->next;
                        if(prev->next == nullptr)
                        {
                            // push_back case, update tail
                            tail = n;
                        }
                        ::boost::signals3::detail::atomic_store(&(prev->next), ::boost::signals3::detail::static_pointer_cast<t_node_base>(std::move(n)));
                    }
                    else
                    {
                        // n is first grouped node, hook to group_head
                        if(group_head != nullptr)
                        {
                            n->prev = group_head;
                            n->next = group_head->next;
                            ::boost::signals3::detail::atomic_store(&(group_head->next), ::boost::signals3::detail::static_pointer_cast<t_node_base>(std::move(n)));
                        }
                        else
                        {
                            // push_front case, update head
                            if(head != nullptr)
                            {
                                n->next = head;
                            }
                            else
                            {
                                // only node
                                tail = n;
                            }
                            ::boost::signals3::detail::atomic_store(&head, ::boost::signals3::detail::static_pointer_cast<t_node_base>(std::move(n)));
                        }
                    }
                }

                template<typename B>
                void insert_impl_unsafe(const group_type& group, ::boost::signals3::detail::shared_ptr< grouped_node<B> >&& n)
                {
                    typename group_storage_type::value_type val(group, n);
                    typename group_storage_type::iterator iter = group_storage.insert(std::move(val));
                    n->iter = iter;
                    if(iter != group_storage.begin())
                    {
                        // n is not going to be the new group_head
                        typename group_storage_type::iterator prev_iter = iter;
                        --prev_iter;
                        ::boost::signals3::detail::shared_ptr<t_node_base> prev = prev_iter->second.lock();
                        n->prev = prev_iter->second;
                        n->next = prev->next;
                        if(prev->next == nullptr)
                        {
                            // push_back case, update tail
                            tail = n;
                        }
                        prev->next = std::move(n);
                    }
                    else
                    {
                        // n is first grouped node, hook to group_head
                        if(group_head != nullptr)
                        {
                            n->prev = group_head;
                            n->next = group_head->next;
                            group_head->next = std::move(n);
                        }
                        else
                        {
                            // push_front case, update head
                            if(head != nullptr)
                            {
                                n->next = head;
                            }
                            else
                            {
                                // only node
                                tail = n;
                            }
                            head = std::move(n);
                        }
                    }
                }

            public:
                signal(void) : head(), tail(), group_head(), _mutex(), combiner()
                {
                }

                connection insert(const group_type& group, const slot_type& callback)
                {
                    ::boost::signals3::detail::shared_ptr< grouped_node<node> > n = ::boost::signals3::detail::make_shared< grouped_node<node> >(callback);
                    connection conn(this, n);
                    insert_impl(group, std::move(n));
                    return conn;
                }

                connection insert(const group_type& group, slot_type&& callback)
                {
                    ::boost::signals3::detail::shared_ptr< grouped_node<node> > n = ::boost::signals3::detail::make_shared< grouped_node<node> >(std::move(callback));
                    connection conn(this, n);
                    insert_impl(group, std::move(n));
                    return conn;
                }

                connection insert_unsafe(const group_type& group, const slot_type& callback)
                {
                    ::boost::signals3::detail::shared_ptr< grouped_node<node> > n = ::boost::signals3::detail::make_shared< grouped_node<node> >(callback);
                    connection conn(this, n);
                    insert_impl_unsafe(group, std::move(n));
                    return conn;
                }

                connection insert_unsafe(const group_type& group, slot_type&& callback)
                {
                    ::boost::signals3::detail::shared_ptr< grouped_node<node> > n = ::boost::signals3::detail::make_shared< grouped_node<node> >(std::move(callback));
                    connection conn(this, n);
                    insert_impl_unsafe(group, std::move(n));
                    return conn;
                }

                connection insert_extended(const group_type& group, const extended_slot_type& callback)
                {
                    ::boost::signals3::detail::shared_ptr< grouped_node<extended_node> > n = ::boost::signals3::detail::make_shared< grouped_node<extended_node> >(callback);
                    connection conn(this, n);
                    insert_impl(group, std::move(n));
                    return conn;
                }

                connection insert_extended(const group_type& group, extended_slot_type&& callback)
                {
                    ::boost::signals3::detail::shared_ptr< grouped_node<extended_node> > n = ::boost::signals3::detail::make_shared< grouped_node<extended_node> >(std::move(callback));
                    connection conn(this, n);
                    insert_impl(group, std::move(n));
                    return conn;
                }

                connection insert_extended_unsafe(const group_type& group, const extended_slot_type& callback)
                {
                    ::boost::signals3::detail::shared_ptr< grouped_node<extended_node> > n = ::boost::signals3::detail::make_shared< grouped_node<extended_node> >(callback);
                    connection conn(this, n);
                    insert_impl_unsafe(group, std::move(n));
                    return conn;
                }

                connection insert_extended_unsafe(const group_type& group, extended_slot_type&& callback)
                {
                    ::boost::signals3::detail::shared_ptr< grouped_node<extended_node> > n = ::boost::signals3::detail::make_shared< grouped_node<extended_node> >(std::move(callback));
                    connection conn(this, n);
                    insert_impl_unsafe(group, std::move(n));
                    return conn;
                }

                connection
                push_back(const slot_type& callback)
                {
                    ::boost::signals3::detail::shared_ptr < node > n = ::boost::signals3::detail::make_shared<node>(callback);
                    ::boost::signals3::connection conn(this, n);
                    push_back_impl(std::move(n));
                    return conn;
                }

                connection
                push_back(slot_type&& callback)
                {
                    ::boost::signals3::detail::shared_ptr<node> n = ::boost::signals3::detail::make_shared<node>(std::move(callback));
                    ::boost::signals3::connection conn(this, n);
                    push_back_impl(std::move(n));
                    return conn;
                }

                connection
                connect(slot_type&& callback) {
                    ::boost::signals3::detail::shared_ptr<node> n = ::boost::signals3::detail::make_shared<node>(std::move(callback));
                    assert(n);
                    ::boost::signals3::connection conn(this, n);
                    push_back_impl(std::move(n));
                    return conn;
                }

                connection
                push_back_unsafe(const slot_type& callback)
                {
                    ::boost::signals3::detail::shared_ptr<node> n = ::boost::signals3::detail::make_shared<node>(callback);
                    ::boost::signals3::connection conn(this, n);
                    push_back_impl_unsafe(n);
                    return conn;
                }

                connection
                push_back_unsafe(slot_type&& callback)
                {
                    ::boost::signals3::detail::shared_ptr<node> n = ::boost::signals3::detail::make_shared<node>(std::move(callback));
                    ::boost::signals3::connection conn(this, n);
                    push_back_impl_unsafe(std::move(n));
                    return conn;
                }

                connection
                push_back_extended(const extended_slot_type& callback)
                {
                    ::boost::signals3::detail::shared_ptr<extended_node> n = ::boost::signals3::detail::make_shared<extended_node>(callback);
                    ::boost::signals3::connection conn(this, n);
                    n->conn = conn;
                    push_back_impl(std::move(n));
                    return conn;
                }

                connection
                push_back_extended(extended_slot_type&& callback)
                {
                    ::boost::signals3::detail::shared_ptr<extended_node> n = ::boost::signals3::detail::make_shared<extended_node>(std::move(callback));
                    ::boost::signals3::connection conn(this, n);
                    n->conn = conn;
                    push_back_impl(std::move(n));
                    return conn;
                }

                connection
                push_back_extended_unsafe(const extended_slot_type& callback)
                {
                    ::boost::signals3::detail::shared_ptr<extended_node> n = ::boost::signals3::detail::make_shared<extended_node>(callback);
                    ::boost::signals3::connection conn(this, n);
                    n->conn = conn;
                    push_back_impl_unsafe(std::move(n));
                    return conn;
                }

                connection
                push_back_extended_unsafe(extended_slot_type&& callback)
                {
                    ::boost::signals3::detail::shared_ptr<extended_node> n = ::boost::signals3::detail::make_shared<extended_node>(std::move(callback));
                    ::boost::signals3::connection conn(this, n);
                    n->conn = conn;
                    push_back_impl_unsafe(std::move(n));
                    return conn;
                }

                connection
                push_front(const slot_type& callback)
                {
                    ::boost::signals3::detail::shared_ptr<node> n = ::boost::signals3::detail::make_shared<node>(callback);
                    ::boost::signals3::connection conn(this, n);
                    push_front_impl(std::move(n));
                    return conn;
                }

                connection
                push_front(slot_type&& callback)
                {
                    ::boost::signals3::detail::shared_ptr<node> n = ::boost::signals3::detail::make_shared<node>(std::move(callback));
                    ::boost::signals3::connection conn(this, n);
                    push_front_impl(std::move(n));
                    return conn;
                }

                connection
                push_front_unsafe(const slot_type& callback)
                {
                    ::boost::signals3::detail::shared_ptr<node> n = ::boost::signals3::detail::make_shared<node>(callback);
                    ::boost::signals3::connection conn(this, n);
                    push_front_impl_unsafe(std::move(n));
                    return conn;
                }

                connection
                push_front_unsafe(slot_type&& callback)
                {
                    ::boost::signals3::detail::shared_ptr<node> n = ::boost::signals3::detail::make_shared<node>(std::move(callback));
                    ::boost::signals3::connection conn(this, n);
                    push_front_impl_unsafe(std::move(n));
                    return conn;
                }

                connection
                push_front_extended(const extended_slot_type& callback)
                {
                    ::boost::signals3::detail::shared_ptr<extended_node> n = ::boost::signals3::detail::make_shared<extended_node>(callback);
                    ::boost::signals3::connection conn(this, n);
                    n->conn = conn;
                    push_front_impl(std::move(n));
                    return conn;
                }

                connection
                push_front_extended(extended_slot_type&& callback)
                {
                    ::boost::signals3::detail::shared_ptr<extended_node> n = ::boost::signals3::detail::make_shared<extended_node>(std::move(callback));
                    ::boost::signals3::connection conn(this, n);
                    n->conn = conn;
                    push_front_impl(std::move(n));
                    return conn;
                }

                connection
                push_front_extended_unsafe(const extended_slot_type& callback)
                {
                    ::boost::signals3::detail::shared_ptr<extended_node> n = ::boost::signals3::detail::make_shared<extended_node>(callback);
                    ::boost::signals3::connection conn(this, n);
                    n->conn = conn;
                    push_front_impl_unsafe(std::move(n));
                    return conn;
                }

                connection
                push_front_extended_unsafe(extended_slot_type&& callback)
                {
                    ::boost::signals3::detail::shared_ptr<extended_node> n = ::boost::signals3::detail::make_shared<extended_node>(std::move(callback));
                    ::boost::signals3::connection conn(this, n);
                    n->conn = conn;
                    push_front_impl_unsafe(std::move(n));
                    return conn;
                }

                void
                pop_back(void)
                {
                    lock_guard_type _lock(_mutex);
                    pop_back_impl();
                }

                void
                pop_front(void)
                {
                    lock_guard_type _lock(_mutex);
                    pop_front_impl();
                }

                void
                pop_back_unsafe(void)
                {
                    if (tail != nullptr)
                    {
                        // actually have a node to remove
                        tail->mark_disconnected();
                        if(tail->grouped())
                        {
                            // handle group slots
                            group_storage.erase(*(tail->pos()));
                        }
                        ::boost::signals3::detail::shared_ptr< t_node_base > prev = tail->prev.lock();
                        if (prev != nullptr)
                        {
                            // more than 1 node
                            tail->prev.reset();
                            if (group_head == tail)
                            {
                                group_head = prev;
                            }
                            prev->next.reset();

                            tail = std::move(prev);
                        }
                        else
                        {
                            // only one node
                            tail.reset();
                            group_head.reset();
                            head.reset();
                        }
                    }
                }

                void
                pop_front_unsafe(void)
                {
                    if (head != nullptr)
                    {
                        // actually have a node to remove
                        head->mark_disconnected();
                        if(head->grouped())
                        {
                            // handle group slots
                            group_storage.erase(*(head->pos()));
                        }
                        if (head == tail)
                        {
                            // only have one node
                            tail.reset();
                            group_head.reset();
                            head.reset();
                        }
                        else
                        {
                            // more than one node
                            if (group_head == head)
                            {
                                group_head.reset();
                            }
                            head = head->next;
                        }
                    }
                }

                void clear(void)
                {
                    lock_guard_type _lock(_mutex);
                    if(head != nullptr)
                    {
                        ::boost::signals3::detail::shared_ptr<t_node_base> iter = head;
                        while(iter != nullptr)
                        {
                            iter->mark_disconnected();
                            iter = iter->next;
                        }
                        tail.reset();
                        group_head.reset();
                        ::boost::signals3::detail::atomic_store(&head, tail);
                        group_storage.clear();
                    }
                }

                void clear_unsafe(void)
                {
                    if(head != nullptr)
                    {
                        ::boost::signals3::detail::shared_ptr<t_node_base> iter = head;
                        while(iter != nullptr)
                        {
                            iter->mark_disconnected();
                            iter = iter->next;
                        }
                        tail.reset();
                        group_head.reset();
                        head.reset();
                        group_storage.clear();
                    }
                }

                void erase(const group_type& group)
                {
                    lock_guard_type _lock(_mutex);
                    std::pair<typename group_storage_type::iterator, typename group_storage_type::iterator> bounds = group_storage.equal_range(group);
                    if(bounds.first != group_storage.end())
                    {
                        // at least one slot to disconnect
                        ::boost::signals3::detail::shared_ptr<t_node_base> iter = bounds.first->second.lock();
                        // assume we're removing all nodes bounds.first to end
                        ::boost::signals3::detail::shared_ptr<t_node_base> end_pos;
                        if(bounds.second != group_storage.end())
                        {
                            // segment being removed is somewhere in the middle (not removing tail)
                            end_pos = bounds.second->second.lock();
                        }
                        else
                        {
                            tail = iter->prev.lock();
                        }
                        if(iter == head)
                        {
                            // move head to end_pos
                            ::boost::signals3::detail::atomic_store(&head, end_pos);
                        }
                        do
                        {
                            iter->mark_disconnected();
                            ::boost::signals3::detail::shared_ptr<t_node_base> next = iter->next;
                            ::boost::signals3::detail::atomic_store(&(iter->next), end_pos);

                            iter = std::move(next);
                            ++bounds.first;
                        }
                        while(bounds.first != bounds.second);
                        group_storage.erase(group);
                    }
                }

                void erase_unsafe(const group_type& group)
                {
                    std::pair<typename group_storage_type::iterator, typename group_storage_type::iterator> bounds = group_storage.equal_range(group);
                    if(bounds.first != group_storage.end())
                    {
                        // at least one slot to disconnect
                        ::boost::signals3::detail::shared_ptr<t_node_base> iter = bounds.first->second.lock();
                        // assume we're removing all nodes bounds.first to end
                        ::boost::signals3::detail::shared_ptr<t_node_base> end_pos;
                        if(bounds.second != group_storage.end())
                        {
                            // segment being removed is somewhere in the middle (not removing tail)
                            end_pos = bounds.second->second.lock();
                        }
                        else
                        {
                            tail = iter->prev.lock();
                        }
                        if(iter == head)
                        {
                            // move head to end_pos
                            head = end_pos;
                        }
                        do
                        {
                            iter->mark_disconnected();
                            ::boost::signals3::detail::shared_ptr<t_node_base> next = iter->next;
                            iter->next = end_pos;

                            iter = std::move(next);
                            ++bounds.first;
                        }
                        while(bounds.first != bounds.second);
                        group_storage.erase(group);
                    }
                }

                typename Combiner::result_type
                emit(Args ... args)
                {
                    ::boost::signals3::detail::forward_list
                    < ::boost::signals3::detail::shared_ptr< void > > tracking_list;
                    ::boost::signals3::detail::tuple< Args
                    ... > params(std::forward<Args>(args)...);
                    ::boost::signals3::detail::shared_ptr< t_node_base > begin_ptr =
                    ::boost::signals3::detail::atomic_load(&head);
                    while (begin_ptr != nullptr)
                    {
                        if (!begin_ptr->usable())
                        {
                            begin_ptr = ::boost::signals3::detail::atomic_load(&(begin_ptr->next));
                        }
                        else if (!begin_ptr->try_lock(tracking_list))
                        {
                            // automatic disconnect
                            tracking_list.clear();
                            disconnect(begin_ptr);
                            begin_ptr = ::boost::signals3::detail::atomic_load(&(begin_ptr->next));
                        }
                        else
                        {
                            break;
                        }
                    }
                    iterator begin(std::move(begin_ptr), params, tracking_list, *this);
                    iterator end(nullptr, params, tracking_list, *this);
                    return combiner(std::move(begin), std::move(end));
                }

                typename Combiner::result_type
                emit_unsafe(Args ... args)
                {
                    ::boost::signals3::detail::forward_list
                    < ::boost::signals3::detail::shared_ptr< void > > tracking_list;
                    ::boost::signals3::detail::tuple< Args... > params(std::forward<Args>(args)...);
                    ::boost::signals3::detail::shared_ptr< t_node_base > begin_ptr = head;
                    while (begin_ptr != nullptr)
                    {
                        if (!begin_ptr->usable())
                        {
                            begin_ptr = begin_ptr->next;
                        }
                        else if (!begin_ptr->try_lock(tracking_list))
                        {
                            // automatic disconnect
                            tracking_list.clear();
                            disconnect_unsafe(begin_ptr);
                            begin_ptr = begin_ptr->next;
                        }
                        else
                        {
                            break;
                        }
                    }
                    unsafe_iterator begin(std::move(begin_ptr), params, tracking_list, *this);
                    unsafe_iterator end(nullptr, params, tracking_list, *this);
                    return combiner(std::move(begin), std::move(end));
                }
            };

        template<typename ResultType, typename ... Args, typename Combiner, typename Group,
                typename GroupCompare, typename FunctionType, typename ExtendedFunctionType,
                typename Mutex>
            class signal< ResultType
            (Args...), Combiner, Group, GroupCompare, FunctionType, ExtendedFunctionType, Mutex >::iterator
            {
                ::boost::signals3::detail::shared_ptr< t_node_base > curr;
                ::boost::signals3::detail::tuple< Args... >& params;
                ::boost::signals3::detail::forward_list<
                        ::boost::signals3::detail::shared_ptr< void > >& tracking;
                signal< ResultType
                (Args...), Combiner, Group, GroupCompare, FunctionType, ExtendedFunctionType >& sig;

            public:
                iterator(::boost::signals3::detail::shared_ptr< t_node_base >&& start_node,
                        ::boost::signals3::detail::tuple< Args... >& params,
                        ::boost::signals3::detail::forward_list<
                                ::boost::signals3::detail::shared_ptr< void > >& tracking,
                        signal< ResultType
                        (Args...), Combiner, Group, GroupCompare, FunctionType, ExtendedFunctionType >& sig) :
                        curr(std::move(start_node)), params(params), tracking(tracking), sig(sig)
                {
                }

                iterator(iterator&& rhs) :
                        curr(std::move(rhs.curr)), params(rhs.params), tracking(rhs.tracking), sig(
                                rhs.sig)
                {
                }

                ResultType
                operator*(void) const
                {
                    return (*curr)(params);
                }

                iterator&
                operator++(void)
                {
                    // release any locks we might have
                    tracking.clear();
                    if (curr != nullptr)
                    {
                        while (true)
                        {
                            curr = ::boost::signals3::detail::atomic_load(&(curr->next));
                            if (curr == nullptr)
                            {
                                return *this;
                            }
                            else if (curr->usable())
                            {
                                if (curr->try_lock(tracking))
                                {
                                    return *this;
                                }
                                else
                                {
                                    // automatic disconnect
                                    sig.disconnect(curr);
                                    tracking.clear();
                                }
                            }
                        }
                    }
                    return *this;
                }

                bool
                operator ==(const iterator& rhs) const
                {
                    return curr == rhs.curr;
                }

                bool
                operator !=(const iterator& rhs) const
                {
                    return curr != rhs.curr;
                }
            };

        template<typename ResultType, typename ... Args, typename Combiner, typename Group,
                typename GroupCompare, typename FunctionType, typename ExtendedFunctionType,
                typename Mutex>
            class signal< ResultType
            (Args...), Combiner, Group, GroupCompare, FunctionType, ExtendedFunctionType, Mutex >::unsafe_iterator
            {
                ::boost::signals3::detail::shared_ptr< t_node_base > curr;
                ::boost::signals3::detail::tuple< Args... >& params;
                ::boost::signals3::detail::forward_list<
                        ::boost::signals3::detail::shared_ptr< void > >& tracking;
                signal< ResultType
                (Args...), Combiner, Group, GroupCompare, FunctionType, ExtendedFunctionType >& sig;

            public:
                unsafe_iterator(::boost::signals3::detail::shared_ptr< t_node_base >&& start_node,
                        ::boost::signals3::detail::tuple< Args... >& params,
                        ::boost::signals3::detail::forward_list<
                                ::boost::signals3::detail::shared_ptr< void > >& tracking,
                        signal< ResultType
                        (Args...), Combiner, Group, GroupCompare, FunctionType, ExtendedFunctionType >&sig) :
                        curr(std::move(start_node)), params(params), tracking(tracking), sig(sig)
                {
                }

                unsafe_iterator(unsafe_iterator&& rhs) :
                        curr(std::move(rhs.curr)), params(rhs.params), tracking(rhs.tracking), sig(
                                rhs.sig)
                {
                }

                ResultType
                operator*(void) const
                {
                    return (*curr)(params);
                }

                unsafe_iterator&
                operator++(void)
                {
                    // release any locks we might have
                    tracking.clear();
                    if (curr != nullptr)
                    {
                        while (true)
                        {
                            curr = curr->next;
                            if (curr == nullptr)
                            {
                                return *this;
                            }
                            if (curr->usable())
                            {
                                if (curr->try_lock(tracking))
                                {
                                    return *this;
                                }
                                else
                                {
                                    // automatic disconnect
                                    tracking.clear();
                                    sig.disconnect_unsafe(curr);
                                }
                            }
                        }
                    }
                    return *this;
                }

                bool
                operator ==(const unsafe_iterator& rhs) const
                {
                    return curr == rhs.curr;
                }

                bool
                operator !=(const unsafe_iterator& rhs) const
                {
                    return curr != rhs.curr;
                }
            };
    }
}

#endif // BOOST_SIGNALS3_SIGNAL_HPP
