#pragma once
#include <ctime>
#include <list>
#include <string>

class NetworkTimeRequester
{
public:
    explicit NetworkTimeRequester(bool logging);
    std::time_t getTimestamp() const;

private:
    std::list<std::string> getServerPool() const;
    //
    bool mLogging = false;
};
