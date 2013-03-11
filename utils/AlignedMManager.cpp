#include "./AlignedMManager.h"

AlignedMManager::AlignedMManager(int preload) :PRELOAD(preload) {
    kdnodes.push_back(NEW_ALIGNED(KdTreeNode, PRELOAD, 16));
    objnodes.push_back(NEW_ALIGNED(ObjList, PRELOAD, 16));
    currKd = kdnodes.begin();
    currObj = objnodes.begin();
    kdID = 0;
    objID = 0;
}

KdTreeNode * AlignedMManager::getKdNode() {
    if(kdID < PRELOAD) {
        kdID += 2;
        return (*currKd)+(kdID-2);
    }
    list<KdTreeNode*>::iterator it = currKd++;
    if (currKd == kdnodes.end()) {
        kdnodes.push_back(NEW_ALIGNED(KdTreeNode, PRELOAD, 16));
        currKd = it++;
    }
    kdID = 0;
    return getKdNode();
}

ObjList * AlignedMManager::getObjListNode() {
    if(objID < PRELOAD) {
        objID += 1;
        return (*currObj)+(objID-1);
    }
    list<ObjList*>::iterator it = currObj++;
    if (currObj == objnodes.end()){
        objnodes.push_back(NEW_ALIGNED(ObjList, PRELOAD, 16));
        currObj = it++;
    }
    objID = 0;
    return getObjListNode();
}

ObjList * AlignedMManager::getObjListNodes(int amount) {
    if(objID < PRELOAD && (PRELOAD - objID >= amount)) {
        objID += amount;
        return (*currObj)+(objID-amount);
    }
    if (PRELOAD < amount) {
        LogDefault->outStringN("Critical Error: not enough space for objnodes");
        LogDefault->criticalOutValue("Asked for nodes", amount);
        return 0;
    }
    list<ObjList*>::iterator it = currObj++;
    if (currObj == objnodes.end()){
        objnodes.push_back(NEW_ALIGNED(ObjList, PRELOAD, 16));
        currObj = it++;
    }
    objID = 0;
    return getObjListNode();
}
// SHOULD ONLY BE USED WHEN ALL REFERENCES ARE DEAD
void AlignedMManager::reset() {
    currObj = objnodes.begin();
    objID = 0;
    currKd = kdnodes.begin();
    kdID = 0;
}

AlignedMManager mManager;