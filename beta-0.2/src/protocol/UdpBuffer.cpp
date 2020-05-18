//
// Created by apple on 2019/11/8.
//

#include "protocol/UdpBuffer.h"

UdpBuffer::UdpBuffer() : buffer_{}, size_(0), read_(0), write_(0) {}

UdpBuffer::UdpBuffer(const UdpBuffer &rhs)
    : buffer_{}
    , size_(rhs.size_)
    , read_(rhs.read_)
    , write_(rhs.write_) {
    memcpy(buffer_, rhs.buffer_, size_);
}

UdpBuffer::UdpBuffer(const char *c, size_t size)
    : buffer_{}
    , size_(size)
    , read_(0)
    , write_(size_) {
    memcpy(buffer_, c, size);
}

char *UdpBuffer::buffer() {
    return buffer_;
}

size_t UdpBuffer::size() const {
    return size_;
}

void UdpBuffer::size(size_t new_size) {
    size_ = new_size;
    read_ = 0;
    write_ = size_;
}

void UdpBuffer::clear() {
    size_ = 0;
    read_ = 0;
    write_ = 0;
}

UdpBuffer &UdpBuffer::operator<<(const bool &val) {
    return *this << (char)val;
}

UdpBuffer &UdpBuffer::operator<<(const char &val) {
    buffer_[write_++] = val;
    size_ = write_ > size_ ? write_ : size_;
    return *this;
}

UdpBuffer &UdpBuffer::operator<<(const uint8_t &val) {
    return *this << (char)val;
}

UdpBuffer &UdpBuffer::operator<<(const uint16_t &val) {
    size_t bytes = 2;
    for (size_t i = 0; i < bytes; ++i) {
        *this << (char)(val >> (8 * (bytes - 1 - i)) & 0xFF);
    }
    return *this;
}

UdpBuffer &UdpBuffer::operator<<(const uint32_t &val) {
    size_t bytes = 4;
    uint32_t temp = 0;
    for (size_t i = 0; i < bytes; ++i) {
        *this << (char)(val >> (8 * (bytes - 1 - i)) & 0xFF);
    }
    return *this;
}

UdpBuffer &UdpBuffer::operator<<(const uint64_t &val) {
    size_t bytes = 8;
    for (size_t i = 0; i < bytes; ++i) {
        *this << (char)(val >> (8 * (bytes - 1 - i)) & 0xFF);
    }
    return *this;
}

UdpBuffer &UdpBuffer::operator<<(const std::string &val) {
    uint32_t length = val.length();
    *this << length;
    memcpy(buffer_ + write_, val.c_str(), length);
    write_ += length;
    size_ = write_ > size_ ? write_ : size_;
    return *this;
}

UdpBuffer &UdpBuffer::operator<<(const char *val) {
    return *this << std::string(val);
}

UdpBuffer &UdpBuffer::operator<<(const boost::asio::ip::udp::endpoint &endpoint) {
    return *this << (uint32_t)endpoint.address().to_v4().to_ulong() << (uint16_t)endpoint.port();
}

UdpBuffer &UdpBuffer::operator>>(bool &val) {
    val = buffer_[read_++];
    return *this;
}

UdpBuffer &UdpBuffer::operator>>(char &val) {
    val = buffer_[read_++];
    return *this;
}

UdpBuffer &UdpBuffer::operator>>(uint8_t &val) {
    val = buffer_[read_++];
    return *this;
}

UdpBuffer &UdpBuffer::operator>>(uint16_t &val) {
    uint16_t result = 0;
    size_t bytes = 2;
    for (size_t i = 0; i < bytes; ++i) {
        result += (uint8_t)buffer_[read_++] << 8 * (bytes - 1 - i);
    }
    val = result;
    return *this;
}

UdpBuffer &UdpBuffer::operator>>(uint32_t &val) {
    uint32_t result = 0;
    size_t bytes = 4;
    for (size_t i = 0; i < bytes; ++i) {
        result += (uint8_t)buffer_[read_++] << 8 * (bytes - 1 - i);
    }
    val = result;
    return *this;
}

UdpBuffer &UdpBuffer::operator>>(uint64_t &val) {
    uint64_t result = 0;
    size_t bytes = 8;
    for (size_t i = 0; i < bytes; ++i) {
        result += (uint8_t)buffer_[read_++] << 8 * (bytes - 1 - i);
    }
    val = result;
    return *this;
}

UdpBuffer &UdpBuffer::operator>>(std::string &val) {
    uint32_t length;
    *this >> length;
    val = std::string(buffer_ + read_, length);
    read_ += length;
    return *this;
}

UdpBuffer &UdpBuffer::operator>>(boost::asio::ip::udp::endpoint &endpoint) {
    uint32_t address;
    uint16_t port;
    *this >> address >> port;
    endpoint.address(boost::asio::ip::address_v4(address));
    endpoint.port(port);
    return *this;
}
