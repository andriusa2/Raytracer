#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>
#include <string>
#include <map>
#include "./util_pack.h"

using namespace std;

class Config {
public:
    Config();
    Config(const char filename[]);
    void resetDefault();
    void parse(const char filename[]);
    // not the best solution ever...
    int getIntByName(const string& name);
    string getStringByName(const string& name);
    float getFloatByName(const string& name);
    friend ostream& operator<< (ostream& out, const Config& data);
private:
    map<string, int> ints;
    map<string, float> floats;
    map<string, string> strings;
};


#endif