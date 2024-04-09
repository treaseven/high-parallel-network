#pragma once
#include <string>
#include <iostream>

class Buffer
{
private:
    std::string buf_;
    const uint16_t sep_;
public:
    Buffer(uint16_t sep=0);
    ~Buffer();

    void append(const char *data, size_t size);
    void appendwithsep(const char *data, size_t size);
    void erase(size_t pos, size_t nn);
    size_t size();
    const char *data();
    void clear(); 
    bool pickmessage(std::string &ss);
};