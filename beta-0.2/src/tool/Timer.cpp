//
// Created by apple on 2020/5/10.
//

#include "tool/Timer.h"

Timer::Timer(const Timer &rhs)
    : io_service_(rhs.io_service_)
    , deadline_timer_(rhs.io_service_)
    , expire_time_(rhs.expire_time_)
    , handler_(rhs.handler_)
    , auto_reset_times_(rhs.auto_reset_times_)
    , expired_times_(0) {

}

Timer::Timer(boost::asio::io_service &io_service, size_t expire_time,
        HandlerType handler, int auto_reset_times)
    : io_service_(io_service)
    , deadline_timer_(io_service)
    , expire_time_(expire_time)
    , handler_(std::move(handler))
    , auto_reset_times_(auto_reset_times)
    , expired_times_(0) {}

Timer::~Timer() {
    Stop();
}

void Timer::Start() {
    deadline_timer_.cancel();
    deadline_timer_.expires_from_now(boost::posix_time::milliseconds(expire_time_));
    deadline_timer_.async_wait(
            [this](const boost::system::error_code& ec) { HandleWait(ec); });
}

void Timer::Stop() {
    deadline_timer_.cancel();
}

void Timer::Restart() {
    Stop();
    Start();
}

void Timer::SetExpireTime(size_t expire_time) {
    expire_time_ = expire_time;
}

size_t Timer::GetExpiredTimes() const {
    return expired_times_;
}

void Timer::SetAutoReset(int auto_reset_times) {
    auto_reset_times_ = auto_reset_times;
}

void Timer::HandleWait(const boost::system::error_code &ec) {
    if (!ec && handler_ != nullptr) {
        ++expired_times_;
        if (auto_reset_times_ < 0) {
            Start();
        } else if (auto_reset_times_ > 0) {
            Start();
            --auto_reset_times_;
        }
        handler_();
    }
}