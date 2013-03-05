#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <ctime>
#include "./constants.h"

using namespace std;

class Logger {
public:
    Logger();
    Logger(const char filename[]);
    void outString(const char str[]);
    template<class T>
    void outValue(const char name[], T value);
    void flush();
private:
    void outTime();
    ofstream out;

};

// as long as T has ostream operator<<, everything is fine
template<class T>
void Logger::outValue(const char name[], T value) {
    outTime();
    out << name << "=" << value << "\n";
}
#endif