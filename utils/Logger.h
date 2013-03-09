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
    void criticalOutValue(const char name[], const T & value);
    template<class T>
    void outValue(const char name[], const T & value);
    void flush();
    void line();
    void outStringN(const char str[]);
private:
    void outTime();
    ofstream out;

};
// same as outValue, but flushes the stream to file
template<class T>
void Logger::criticalOutValue(const char name[], const T & value){
    outValue(name, value);
    flush();
}
// as long as T has ostream operator<<, everything is fine
template<class T>
void Logger::outValue(const char name[], const T & value) {
    outTime();
    out << name << "=" << value << "\n";
}
#endif