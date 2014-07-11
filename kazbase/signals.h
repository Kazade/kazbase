#ifndef SIGNALS2_H
#define SIGNALS2_H

#include <functional>
#include <memory>
#include <deque>
#include <atomic>
#include <algorithm>

#include "threadsafe/ring_buffer.h"

namespace sig {

class ConnectionImpl;

class Disconnector {
public:
    virtual bool disconnect(const ConnectionImpl& conn) = 0;
    virtual bool connection_exists(const ConnectionImpl& conn) const = 0;
};

class Connection;

class ConnectionImpl {
public:
    ConnectionImpl(Disconnector* parent, size_t id):
        id_(id),
        parent_(parent) {}

    ConnectionImpl(const ConnectionImpl& rhs):
        id_(rhs.id_),
        parent_(rhs.parent_) {}

    bool operator!=(const ConnectionImpl& rhs) const { return !(*this == rhs); }
    bool operator==(const ConnectionImpl& rhs) const { return this->id_ == rhs.id_; }

    ConnectionImpl& operator=(const ConnectionImpl& rhs) {
        if(*this == rhs) {
            return *this;
        }

        this->id_ = rhs.id_;
        this->parent_ = rhs.parent_;

        return *this;
    }

private:
    size_t id_;
    Disconnector* parent_;
    friend class Connection;
};

class Connection {
public:
    Connection() {}

    Connection(std::shared_ptr<ConnectionImpl> impl):
        impl_(impl) {}

    bool disconnect() {
        auto p = impl_.lock();
        return p && p->parent_->disconnect(*p);
    }

    bool is_connected() const {
        auto p = impl_.lock();
        return p && p->parent_->connection_exists(*p);
    }
private:
    std::weak_ptr<ConnectionImpl> impl_;
};

template<typename> class ProtoSignal;

template<typename R, typename... Args>
class ProtoSignal<R (Args...)> : public Disconnector {
public:
    typedef R result;
    typedef std::function<R (Args...)> callback;

    Connection connect(const callback& func) {
        connection_counter_++;
        size_t id = connection_counter_.load();
        auto conn_impl = std::make_shared<ConnectionImpl>(this, id);
        links_.push_back({func, conn_impl});
        return Connection(conn_impl);
    }

    void operator()(Args... args) {
        for(auto& link: links_) {
            link.func(args...);
        }
    }

    bool connection_exists(const ConnectionImpl& conn) const {
        for(auto link: links_) {
            if(*link.conn_impl == conn) {
                return true;
            }
        }
        return false;
    }

    bool disconnect(const ConnectionImpl& conn_impl) {
        auto size = links_.size();                

        for(auto it = links_.begin(); it != links_.end();) {
            if((*it).conn_impl.get() == &conn_impl) {
                it = links_.erase(it);
            } else {
                ++it;
            }
        }

        return size != links_.size();
    }

private:
    std::atomic<size_t> connection_counter_;

    struct Link {
        callback func;
        std::shared_ptr<ConnectionImpl> conn_impl;
    };

    threadsafe::ring_buffer<Link> links_;
};

template<typename Signature>
class signal : public ProtoSignal<Signature> {

};

typedef Connection connection;

}

#endif // SIGNALS2_H
