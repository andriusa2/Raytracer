#include "./Config.h"

Config::Config() {
    resetDefault();
    parse(DEFAULT_CONFIG);
}
Config::Config(const char filename[]) {
    resetDefault();
    parse(filename);
}
int Config::getIntByName(const string& name) {
    if (ints.count(name) == 1)
        return ints[name];
    else return 0;
}
float Config::getFloatByName(const string& name) {
    if (floats.count(name) == 1)
        return floats[name];
    else return -INF;
}
string Config::getStringByName(const string& name) {
    if (strings.count(name) == 1)
        return strings[name];
    else return "";
}
ostream& operator<< (ostream& out, const Config& data) {
    out << "Dumping config data:\n";
    for (map<string, int> :: const_iterator it = data.ints.begin();
        it != data.ints.end(); it++) {
        out << '\t' << it->first << "=" << it->second << '\n';
    }
    for (map<string, float> :: const_iterator it = data.floats.begin();
        it != data.floats.end(); it++) {
        out << '\t' << it->first << "=" << it->second << '\n';
    }
    for (map<string, string> :: const_iterator it = data.strings.begin();
        it != data.strings.end(); it++) {
        out << '\t' << it->first << "=\"" << it->second << '"' << '\n';
    }
    return out;
}
void Config::resetDefault() {
    ints["max_depth"] = MAX_DEPTH;
    ints["width"] = DEFAULT_WIDTH;
    ints["height"] = DEFAULT_HEIGHT;
    ints["threads"] = max(omp_get_max_threads() - 1, 1);
    floats["epsilon"] = EPS;
    strings["scene"] = DEFAULT_SCENE;
}
void Config::parse(const char filename[]) {
    ifstream in(filename);
    string key;
    string val;
    int tmpInt = 0;
    float tmpFloat = 0.0f;
    float dim = 0.1f;
    char tmp[256];
    while (in >> key) {
        in.ignore();
        in.getline(tmp,256);
        val = tmp;
        tmpInt = 0;
        tmpFloat = 0.0f;
        dim = 0.1f;
        int len = strlen(tmp);
        int type = 0;
        for (int i = 0; i < len && type < 2; i++) {
            switch (type) {
            case 0:
                if (tmp[i] >= '0' && tmp[i] <= '9') {
                    tmpInt *= 10;
                    tmpInt += tmp[i] - '0';
                }
                else if (tmp[i] == '.') {
                    type = 1;
                    tmpFloat = (float)tmpInt;
                }
                else type = 2;
                break;
            case 1:
                if (tmp[i] >= '0' && tmp[i] <= '9') {
                    tmpFloat += dim * float(tmp[i] - '0');
                    dim *= 0.1f;
                }
                else {
                    type = 3;
                }
                break;
            }
        }
        switch (type) {
        case 0: ints[key] = tmpInt;
            break;
        case 1:
        case 3: floats[key] = tmpFloat;
            break;
        case 2: strings[key] = val;
        }
    }
}
