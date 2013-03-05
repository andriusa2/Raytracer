#include "./utils/util_pack.h"
int main() {
    Logger log;
    Vector3D tmpV(0.0f,1.0f,-1.0f);
    log.outValue("tmpVal", 5.0f);
    log.outValue("tmpV", tmpV);
    return 0;
}