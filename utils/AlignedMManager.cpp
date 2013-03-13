#include "./AlignedMManager.h"

AlignedMManager::AlignedMManager(int preload) :PRELOAD(preload) {
    kdnodes.push_back(NEW_ALIGNED(KdTreeNode, PRELOAD, 16));
    objnodes.push_back(NEW_ALIGNED(ObjList, PRELOAD, 16));
    kdhelpnodes.push_back(new countRefs<KdHelperList>(PRELOAD));
    currkdh = kdhelpnodes.back()->pointer;
    currKd = kdnodes.back();
    currObj = objnodes.back();
    kdID = 0;
    objID = 0;
    kdhID = 0;
    releasedKdHelpers.push_back(0);
}

KdTreeNode * AlignedMManager::getKdNode() {
    if(kdID < PRELOAD) {
        kdID += 2;
        return currKd+(kdID-2);
    }
    kdnodes.push_back(NEW_ALIGNED(KdTreeNode, PRELOAD, 16));
    currKd = kdnodes.back();
    kdID = 2;

    return currKd;
}

ObjList * AlignedMManager::getObjListNode() {
    if(objID < PRELOAD) {
        objID += 1;
        return currObj+(objID-1);
    }
    objnodes.push_back(NEW_ALIGNED(ObjList, PRELOAD, 16));
    currObj = objnodes.back();
    objID = 1;
    return currObj;
}

ObjList * AlignedMManager::getObjListNodes(int amount) {
    if (amount <= 0)
        return 0;
    if(objID < PRELOAD && (PRELOAD - objID >= amount)) {
        objID += amount;
        return currObj+(objID-amount);
    }
    if (PRELOAD < amount) {
        LogDefault->outStringN("Not enough space for objnodes, allocating it customly");
        LogDefault->criticalOutValue("Asked for nodes", amount);
        objnodes.push_front(NEW_ALIGNED(ObjList, amount+1, 16));
        return *(objnodes.begin());
    }
    objnodes.push_back(NEW_ALIGNED(ObjList, PRELOAD, 16));
    currObj = objnodes.back();
    objID = amount;
    return currObj;
}


KdHelperList * AlignedMManager::getKdHelperNode() {
    if(kdhID < PRELOAD) {
        kdhID += 1;
        return currkdh+(kdhID-1);
    }
    kdhelpnodes.push_back(new countRefs<KdHelperList>(PRELOAD));
    currkdh = kdhelpnodes.back()->pointer;
    kdhID = 1;
    return currkdh;
}
KdHelperList * AlignedMManager::getKdHelperNodes(int amount) {
    if (amount <= 0)
        return 0;
    if(kdhID < PRELOAD && (PRELOAD - kdhID >= amount)) {
        kdhID += amount;
        return currkdh+(kdhID-amount);
    }
    if (PRELOAD < amount) {
        LogDefault->outStringN("Not enough space for kdhelpernodes, allocating it customly");
        LogDefault->criticalOutValue("Asked for nodes", amount);
        //kdhelpnodes.push_front(NEW_ALIGNED(KdHelperList, amount+2, 16));
        kdhelpnodes.push_front(new countRefs<KdHelperList>(amount));
        return kdhelpnodes.front()->pointer;
    }
    kdhelpnodes.push_back(new countRefs<KdHelperList>(PRELOAD));
    currkdh = kdhelpnodes.back()->pointer;
    kdhID = amount;
    return currkdh;
}
void AlignedMManager::releaseKdHelperNodes() {
    for (list<countRefs<KdHelperList> *>::iterator it = kdhelpnodes.begin();
        it!=kdhelpnodes.end(); it++)
        delete [] (*it)->pointer;
    kdhelpnodes.clear();
    //kdhelpnodes.push_front(0);
    kdhID = PRELOAD;
    currkdh = 0;
}


AlignedMManager mManager;