#pragma once
#include <iostream>
#include <string>

class Timestamp
{
private:
    time_t secsinceepoch_;
public:
    Timestamp();
    Timestamp(int64_t secsinceepoch);

    static Timestamp now();

    time_t toint() const;
    std::string tostring() const;
};