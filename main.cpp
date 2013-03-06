#include "./utils/util_pack.h"

#include "./utils/Config.h"

int main () {
    Logger log;
    Config cfg;

    log.criticalOutValue("cfg", cfg);
    Config cf("config.txt");
    log.criticalOutValue("cf", cf);
    return 0;
}