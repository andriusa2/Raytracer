#include "./Logger.h"
Logger::Logger() {
    out = ofstream(DEFAULT_LOG);
}
Logger::Logger(const char filename[]) {
    out = ofstream(filename);
}
void Logger::flush() {
    out.flush();
}
void Logger::line() {
    for (int i = 0; i < 40; i++)
        out << '#';
    out << '\n';
}
void Logger::outTime() {
    char buf[12];
    const char pattern[] = "[%H:%M:%S] ";
    const time_t t = time(NULL);
    strftime(buf,12,pattern, localtime(&t));
    out << buf;
}
void Logger::outString(const char str[]){
    out << str;
}
void Logger::outStringN(const char str[]) {
    out << str << '\n';
}

Logger * LogDefault;