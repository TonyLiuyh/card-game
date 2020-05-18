//
// Created by apple on 2020/5/10.
//

#ifndef TOOL_TIMER_H
#define TOOL_TIMER_H

#include <boost/asio.hpp>
#include <memory>
#include <functional>

class Timer {
public:
    typedef std::shared_ptr<Timer> ptr;
    typedef std::function<void()> HandlerType;

    explicit Timer(const Timer& rhs);

    // expire_time is in milliseconds
    // reset_times: 0 - no reset; -1 - infinite times;
    Timer(boost::asio::io_service& io_service, size_t expire_time,
            HandlerType handler, int auto_reset_times = 0);

    ~Timer();

    void Start();

    void Stop();

    void Restart();

    void SetExpireTime(size_t expire_time);

    size_t GetExpiredTimes() const;

    void SetAutoReset(int auto_reset_times);

private:
    void HandleWait(const boost::system::error_code& ec);

private:
    boost::asio::io_service& io_service_;

    boost::asio::deadline_timer deadline_timer_;

    size_t expire_time_;

    HandlerType handler_;

    int auto_reset_times_;

    size_t expired_times_;
};


#endif // TOOL_TIMER_H
