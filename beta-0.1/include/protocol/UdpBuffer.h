//
// Created by apple on 2019/11/8.
//

#ifndef PROTOCOL_UDPBUFFER_H
#define PROTOCOL_UDPBUFFER_H

#include <boost/asio.hpp>
#include <cstdint>
#include <string>
#include <vector>

class UdpBuffer {
public:
    const static int BUFFER_SIZE = 1440;

    UdpBuffer();

    UdpBuffer(const UdpBuffer& rhs);

    UdpBuffer(const char *c, size_t size);

    char *buffer();

    size_t size() const;

    void size(size_t new_size);

    void clear();

    UdpBuffer &operator<<(const bool &val);

    UdpBuffer &operator<<(const char &val);

    UdpBuffer &operator<<(const std::uint8_t &val);

    UdpBuffer &operator<<(const std::uint16_t &val);

    UdpBuffer &operator<<(const std::uint32_t &val);

    UdpBuffer &operator<<(const std::uint64_t &val);

    UdpBuffer &operator<<(const size_t &val);

    UdpBuffer &operator<<(const std::string &val);

    UdpBuffer &operator<<(const char *val);

    UdpBuffer &operator<<(const boost::asio::ip::tcp::endpoint &endpoint);

    UdpBuffer &operator>>(bool &val);

    UdpBuffer &operator>>(char &val);

    UdpBuffer &operator>>(std::uint8_t &val);

    UdpBuffer &operator>>(std::uint16_t &val);

    UdpBuffer &operator>>(std::uint32_t &val);

    UdpBuffer &operator>>(std::uint64_t &val);

    UdpBuffer &operator>>(size_t &val);

    UdpBuffer &operator>>(std::string &val);

    UdpBuffer &operator>>(boost::asio::ip::tcp::endpoint &endpoint);

    template<typename T>
    UdpBuffer &operator<<(const std::vector<T> &vec) {
        *this << (uint32_t) vec.size();
        for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
            *this << *iter;
        }
        return *this;
    }

    template<typename T>
    UdpBuffer &operator<<(std::vector<T> &vec) {
        std::vector<T> temp;
        uint32_t size;
        *this >> size;
        for (size_t i = 0; i < size; ++i) {
            T value;
            *this >> value;
            temp.push_back(value);
        }
        for (size_t i = 0; i < size; ++i) {
            vec.push_back(temp[i]);
        }
        return *this;
    }

private:
    char buffer_[BUFFER_SIZE];
    size_t size_;
    size_t read_;
    size_t write_;
};

#endif // PROTOCOL_UDPBUFFER_H
