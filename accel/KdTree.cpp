#include "./KdTree.h"

KdTree::KdTree() : root(0) {   
    mStack = new KdStack*[8];
    for (int i = 0; i < 8; i++)
        mStack[i] = NEW_ALIGNED(KdStack,256,16);
    sortByAxis[0] = lessAxis<0>;
    sortByAxis[1] = lessAxis<1>;
    sortByAxis[2] = lessAxis<2>;
}

KdTree :: KdTree(vector<Triangle *> & tris) {
    makeTree(tris);
    mStack = new KdStack*[8];
    for (int i = 0; i < 8; i++)
        mStack[i] = NEW_ALIGNED(KdStack,256,16);
}

void KdTree::Load(vector<Triangle *> & tris) {
    makeTree(tris);
    //DumpTree();
    scene_bound.min_v -= Vector3D(EPS, EPS, EPS);
    scene_bound.max_v += Vector3D(EPS, EPS, EPS);
    LogDefault->flush();
}

void KdTree::resetAll() {
    //mManager.reset();
    root = 0;
}

struct HelperStruct{
    KdHelperList * pt;
    float data;
};

bool HelperisLess(const HelperStruct & a,
            const HelperStruct & b) {
    return a.data < b.data;
}

void KdTree::makeTree(vector<Triangle *> & tris) {
    // first of all fix some triangles (kdTree has some problems
    // with axis-aligned triangles)

    //todo

    // make an axis-based sorted list of primitive
    // start and end-points

    KdHelperList * help = mManager.getKdHelperNodes(tris.size() * 2);
    KdHelperList * heads[3] = {};
    int helpID = 0;
    AABB scene(Vector3D(+INF,+INF,+INF), Vector3D(-INF,-INF,-INF));
    MAX_KD_DEPTH = 8.0f + 1.3f * logf(float(tris.size())) + 0.9f;
    LogDefault->outValue("MAX_KD_DEPTH",MAX_KD_DEPTH);
    avg_depth = avg_prims = 0.0f;
    cnt_depth = cnt_prims = min_prims = max_prims = 0;
    min_prims = 100;
    for (vector<Triangle *>::iterator it = tris.begin();
        it != tris.end(); it++) {
        for (int i = 0; i < 3; i++) {
            help[helpID].data[i] = (*it)->getMinAxis(i);
            help[helpID+1].data[i] = (*it)->getMaxAxis(i);
            if (help[helpID].data[i] < scene.min_v[i])
                scene.min_v[i] = help[helpID].data[i];
            if (help[helpID + 1].data[i] > scene.max_v[i])
                scene.max_v[i] = help[helpID + 1].data[i];
        }
        help[helpID].left = true;
        help[helpID].otherSide = help+(helpID + 1);
        help[helpID].tri = *it;
        help[helpID+1].left = false;
        help[helpID+1].otherSide = help+helpID;
        help[helpID+1].tri = *it;
        helpID += 2;
    }
    vector<HelperStruct> tmp_v(tris.size() * 2);
    int sz = tmp_v.size();
    for (int axis = 0; axis < 3; axis++) {
        for (int i = 0; i < sz; i++) {
            tmp_v[i].pt = help + i;
            tmp_v[i].data = help[i].data[axis];
        }
        sort(tmp_v.begin(), tmp_v.end(), HelperisLess);
        heads[axis] = tmp_v.front().pt;
        for (vector<HelperStruct>::iterator it = tmp_v.begin();
            it != tmp_v.end(); it++) {
            if ((it+1) != tmp_v.end())
                it->pt->next[axis] = (it+1)->pt;
            else it->pt->next[axis] = nullptr;
            if (it != tmp_v.begin())
                it->pt->prev[axis] = (it - 1)->pt;
            else
                it->pt->prev[axis] = nullptr;
        }
    }
    // now we have a nice sorted view
    root = new KdTreeNode();
    scene_bound = scene;
    subdivide(heads, root, scene, 0, tris.size(), 0);
    avg_depth *= 1.0f / float(cnt_depth);
    avg_prims *= 1.0f / float(cnt_prims);
    LogDefault->criticalOutValue("Average depth", avg_depth);
    LogDefault->criticalOutValue("Count leafs", cnt_depth);
    LogDefault->criticalOutValue("Average prims", avg_prims);
    LogDefault->criticalOutValue("Leaves with prims", cnt_prims);
    LogDefault->criticalOutValue("Max prims", max_prims);
    LogDefault->criticalOutValue("Min prims", min_prims);
    LogDefault->line();
    mManager.releaseKdHelperNodes();
}

unsigned int KdTree::calcTriangles(KdHelperList * head, int axis,
                                   float spos, fpKHListTest test) {
    unsigned int retval = 0;
    for (KdHelperList * tmp = head;
        tmp != 0;
        tmp = tmp->next[axis])
    {
        if (!test(tmp->data[axis], spos)) continue;
        // __ALL__ object starts should be on the head!
        if (!tmp->read) {
            retval++;
            tmp->otherSide->read = true;
        }
    }
    for (KdHelperList * tmp = head;
        tmp != 0;
        tmp = tmp->next[axis])
    {
        tmp->read = false;
        tmp->otherSide->read=false;
    }
    return retval;
}
void KdTree::addTriangles(KdHelperList * head, int axis,
                          ObjList * objs, float spos, fpKHListTest test) {
    int i = 0;
    for (KdHelperList * tmp = head;
        tmp != 0;
        tmp = tmp->next[axis])
    {
        if (!test(tmp->data[axis], spos)) continue;
        // __ALL__ object starts should be on the head!
        if (!tmp->read) {
            (objs+i)->setTriangle(tmp->tri);
            (objs+i)->setNext(0);
            if (i) (objs+i-1)->setNext(objs+i);
            i++;
            tmp->otherSide->read = true;
        }
    }
    for (KdHelperList * tmp = head;
        tmp != 0;
        tmp = tmp->next[axis])
    {
        tmp->read = false;
        tmp->otherSide->read=false;
    }
}

KdHelperList * KdTree::removeFromList(KdHelperList * node, KdHelperList * heads[], int axis)
{
    for (int i = 0; i < 3; i++)
    {
        if (node->prev[i] == nullptr)
            heads[i] = node->next[i];
        else
            node->prev[i]->next[i] = node->next[i];

        if (node->next[i] != nullptr)
            node->next[i]->prev[i] = node->prev[i];
    }
    if (node->prev[axis] == nullptr)
        return nullptr;
    else
        return node->prev[axis];
}

void KdTree::subdivide(KdHelperList * heads[], KdTreeNode * node,
                       AABB box, int depth, int triangles, int openL)
{
    // depth is tested somewhere else, should always fail
    if (depth > MAX_KD_DEPTH)
        LogDefault->outStringN("KdTree went past max_depth");
    Vector3D sz = box.max_v - box.min_v;
    // surface area/volume
    const float SAV = 1.0f / (sz[0] * sz[1] + sz[0] * sz[2] + sz[1] * sz[2]);
    int minAxis = -1;
	float minCost[3] = { INF, INF, INF };
	float bestPosA[3] = { -INF, -INF, -INF };
//#pragma omp parallel for
    for (int axis = 0; axis < 3; axis++) {
        KdHelperList * tmp = heads[axis];
        
        int n1 = 0, // triangles on the left
            n2 = triangles, // triangles on the right
            n3 = 0, // amount of triangles who will _start_ on the left
                    // after pushing splitpos to the right
            n4 = 0; // amount of triangles who will fully reside on the left
                    // after pushing splitpos to the right
        Vector3D lsz = sz;
        Vector3D rsz = sz;
        while ( tmp ) {
            float spos = tmp->data[axis];
            while(tmp && spos == tmp->data[axis]) {
                if (!tmp->left) { // endpoint
                    if (tmp->otherSide->data[axis] == tmp->data[axis])
                        n4++;
                    else n2--; // if it ends on the spos, assume that it's fully on the LHS
                }
                else { // start point
                    if (tmp->otherSide->data[axis] != tmp->data[axis])
                        n3++;
                }
                tmp = tmp->next[axis];
            }
            if (spos > box.min_v[axis] + EPS && spos < box.max_v[axis] - EPS) {
                // as we don't want to make a split so close to the edge of AABB
                lsz[axis] = spos - box.min_v[axis];
                rsz[axis] = sz[axis] - lsz[axis];
                float SA1 = lsz[0] * lsz[1] + lsz[0] * lsz[2] + lsz[1] * lsz[2];
                float SA2 = rsz[0] * rsz[1] + rsz[0] * rsz[2] + rsz[1] * rsz[2];
                float splitcost =
                    KDCTRAV + (SA1 * (n1+n4+n3) + SA2 * (n2 - n4)) * SAV * KDCINTR;
                if (splitcost < minCost[axis]) {
					minCost[axis] = splitcost;
					bestPosA[axis] = spos;
                    //minAxis = axis;
                }
            }
            n1 += n3 + n4;
            n2 -= n4;
            n3 = 0;
            n4 = 0;
        }
    }
    float bestPos;
    // determine minAxis
    if (minCost[0] < minCost[1])
        if (minCost[0] < minCost[2])
            minAxis = 0;
        else minAxis = 2;
    else if (minCost[1] < minCost[2])
            minAxis = 1;
        else minAxis = 2;
    bestPos = bestPosA[minAxis];
    if (minCost[minAxis] == INF)
        minAxis = -1;

    if (minAxis == -1 || depth >= MAX_KD_DEPTH) {
        // make a child out of this node
        node->setLeaf(true);
        ObjList * head = mManager.getObjListNodes(calcTriangles(heads[0], 0, 0, KdTree::testAll));
        addTriangles(heads[0], 0, head, 0, KdTree::testAll);
        //LogDefault->criticalOutValue("MadeIntLeaf, objects", calcTriangles(heads[0],0,0,KdTree::testAll));
        node->setObjList(head);
        max_prims = max(max_prims, calcTriangles(heads[0], 0, 0, KdTree::testAll));
        min_prims = min(min_prims, calcTriangles(heads[0], 0, 0, KdTree::testAll));
        avg_prims += calcTriangles(heads[0], 0, 0, KdTree::testAll);
        cnt_prims++;
        avg_depth += depth;
        cnt_depth++;
        return ;
    }
    // so we found some axis and split position...
    node->setSplitPos(bestPos);
    node->setAxis(minAxis);
    node->setLeaf(false);
    node->setLeft(mManager.getKdNode());
    /* go through all start/end points
     * if some are divided by splitpos
     * create some "midpoints" for them
     * also calculate how many primitives are on
     * one side or another
     */

    AABB box_l(box);
    AABB box_r(box);
    box_l.max_v[minAxis] = bestPos;
    box_r.min_v[minAxis] = bestPos;
    KdHelperList * tmp = heads[minAxis];
    list<KdHelperList*> newnodes;
    list<KdHelperList*> newnodes_alloc;
    while (tmp) {
        if (tmp->left) {
            if (tmp->data[minAxis] < bestPos) {
                if (tmp->otherSide->data[minAxis] > bestPos) {
                    // this one goes over the spos, so drop both ends from the list
                    // and create few new ones
                    // perfect clipping!
                    //*
                    KdHelperList * start_left = tmp;
                    KdHelperList * end_right = tmp->otherSide;
                    tmp = removeFromList(start_left, heads, minAxis);
                    removeFromList(end_right, heads, minAxis);

                    KdHelperList * end_left = mManager.getKdHelperNode();
                    KdHelperList * start_right = mManager.getKdHelperNode();

                    newnodes_alloc.push_back(end_left);
                    newnodes_alloc.push_back(start_right);

                    start_right->otherSide = end_right;
                    end_right->otherSide = start_right;

                    start_left->otherSide = end_left;
                    end_left->otherSide = start_left;

                    start_left->left = true;
                    start_right->left = true;
                    start_left->goes_left = true;
                    end_left->goes_left = true;
                    start_right->goes_left = false;
                    end_right->goes_left = false;
                    start_right->tri = start_left->tri;

                    end_left->tri = start_left->tri;

                    AABB l = start_left->tri->getClippedAABB(box_l);
                    AABB r = start_left->tri->getClippedAABB(box_r);
                    for (int i = 0; i < 3; i++)
                    {
                        
                        start_left->data[i] = l.min_v.get(i);
                        end_left->data[i] = l.max_v.get(i);
                        
                        end_left->next[i] = nullptr;
                        start_right->next[i] = nullptr;
                        end_right->next[i] = nullptr;
                        
                        start_right->data[i] = r.min_v.get(i);
                        end_right->data[i] = r.max_v.get(i);
                    }

                    if (l.valid() && l.surfaceArea() > EPS)
                    {
                        newnodes.push_back(start_left);
                        newnodes.push_back(end_left);

                    }
                    if (r.valid() && r.surfaceArea() > EPS)
                    {
                        newnodes.push_back(start_right);
                        newnodes.push_back(end_right);

                    }
                    /*/
                    KdHelperList * end_left = mManager.getKdHelperNode();
                    KdHelperList * start_right = mManager.getKdHelperNode();
                    start_right->otherSide = tmp;
                    end_left->goes_left = false;
                    tmp->goes_left = true;
                    tmp->otherSide->goes_left = false;
                    start_right->goes_left = true;
                    end_left->otherSide = tmp->otherSide;
                    end_left->left = true;
                    start_right->tri = tmp->tri;
                    end_left->tri = tmp->tri;
                    for (int i = 0; i < 3; i++){
                        start_right->data[i] = tmp->otherSide->data[i];
                        end_left->data[i] = tmp->data[i];
                    }
                    start_right->otherSide->otherSide = start_right;
                    end_left->otherSide->otherSide = end_left;
                    start_right->data[minAxis] = bestPos;
                    end_left->data[minAxis] = bestPos + 2 * EPS;
                    newnodes_alloc.push_back(end_left);
                    newnodes_alloc.push_back(start_right);
                    newnodes.push_back(end_left);
                    newnodes.push_back(start_right);//*/
                }
                else
                {
                    tmp->goes_left = true;
                    tmp->otherSide->goes_left = true;
                }
            }
            else
            {
                tmp->goes_left = false;
                tmp->otherSide->goes_left = false;
            }
        }
        if (tmp)
            tmp = tmp->next[minAxis];
        else
            tmp = heads[minAxis];
    }
    for (int axis = 0; axis < 3; axis++) {
        newnodes.sort(sortByAxis[axis]);
        KdHelperList * tmpHead = heads[axis];
        for (auto it = newnodes.begin(); it != newnodes.end(); it++) {
            if (tmpHead->data[axis] > (*it)->data[axis]) { // well this means that the current node should come before another
                (*it)->next[axis] = tmpHead;
                (*it)->prev[axis] = tmpHead->prev[axis];
                tmpHead->prev[axis] = *it;
                tmpHead = *it;
                if (tmpHead->prev[axis] == nullptr)
                    heads[axis] = tmpHead;
                continue;
            }
            while (tmpHead->next[axis] && tmpHead->next[axis]->data[axis] <= (*it)->data[axis]) {
                tmpHead = tmpHead->next[axis];
            }
            (*it)->next[axis] = tmpHead->next[axis];
            (*it)->prev[axis] = tmpHead;
            if ((*it)->next[axis] != nullptr)
                (*it)->next[axis]->prev[axis] = *it;
            tmpHead->next[axis] = *it;
            tmpHead = *it;
        }
    }
    int n_left_start = 0;
    int n_right_start = 0;
    int n_right_end = 0;
    int n_left_end = 0;
    // now we should have lists with intermediate values and so on
    // we _have_ to reconstruct them (as some triangles move to the rhs, etc)
    KdHelperList * l_heads[3] = {};
    KdHelperList * r_heads[3] = {};
    KdHelperList * curr_r, * curr_l;
    for (int i = 0; i < 3; i++) {
        curr_l = nullptr;
        curr_r = nullptr;
        KdHelperList * tmp = heads[i];
        while (tmp) {
            /*if (tmp->data[minAxis] <= (bestPos)  // left side is on the left
                && tmp->otherSide->data[minAxis] <= (bestPos) // right side on the left
                )*/
            if (tmp->goes_left)
            { // to left!
                if (l_heads[i]) { // if we have left head for it, just append to the list
                    curr_l->next[i] = tmp;
                    tmp->prev[i] = curr_l;
                    curr_l = tmp;
                }
                else { // or create new head
                    tmp->prev[i] = nullptr;
                    l_heads[i] = tmp;
                    curr_l = tmp;
                }
                if (tmp->left) n_left_start++;
                else n_left_end++;
            }
            else {
                if (r_heads[i]) {
                    curr_r->next[i] = tmp;
                    tmp->prev[i] = curr_r;
                    curr_r = tmp;
                }
                else {
                    r_heads[i] = tmp;
                    tmp->prev[i] = nullptr;
                    curr_r = tmp;
                }
                if (tmp->left) n_right_start++;
                else n_right_end++;
            }
            tmp = tmp->next[i];
        }
        if (curr_l) curr_l->next[i] = 0;
        if (curr_r) curr_r->next[i] = 0;

    }
    n_left_end/=3;
    n_left_start/=3;
    n_right_start/=3;
    n_right_end/=3;
    int n_left = n_left_start+openL;
    int n_right = n_right_end;
    int n_left_s = openL + n_left_start - n_left_end;
    unsigned int n_left_in = calcTriangles(l_heads[minAxis],minAxis,bestPos, KdTree::testLeft);
    if (n_left_in > PRIMITIVE_CUTOFF)
        subdivide(l_heads, node->getLeft(),box_l,depth+1, n_left,openL);
    else {
        node->getLeft()->setLeaf(true);

        avg_depth += depth;
        cnt_depth++;
        if (n_left_in == 0)
            node->getLeft()->setObjList(0);
        else {
            ObjList * head = mManager.getObjListNodes(n_left_in);
            addTriangles(l_heads[minAxis], minAxis, head, bestPos, KdTree::testLeft);
            //LogDefault->criticalOutValue("MadeLeftLeaf, objects", n_left_in);
            node->getLeft()->setObjList(head);
            max_prims = max(max_prims, n_left_in);
            min_prims = min(min_prims, n_left_in);
            avg_prims += n_left_in;
            cnt_prims++;
        }
    }
    unsigned int n_right_in = calcTriangles(r_heads[minAxis], minAxis, bestPos, KdTree::testRight);

    if (n_right_in > PRIMITIVE_CUTOFF)
        subdivide(r_heads, node->getRight(), box_r, depth+1, n_right, n_left_s);
    else {
        node->getRight()->setLeaf(true);
        avg_depth += depth;
        cnt_depth++;
        if (n_right_in == 0)
            node->getRight()->setObjList(0);
        else {
            ObjList * head = mManager.getObjListNodes(n_right_in);
            addTriangles(r_heads[minAxis], minAxis, head, bestPos, KdTree::testRight);
            //LogDefault->criticalOutValue("MadeRightLeaf, objects", n_right_in);
            node->getRight()->setObjList(head);
            max_prims = max(max_prims, n_right_in);
            min_prims = min(min_prims, n_right_in);
            avg_prims += n_right_in;
            cnt_prims++;
        }
    }
    mManager.releaseKdHelperNodes(newnodes_alloc);
}

Triangle * KdTree::intersect(Ray & ray, float & dist, float & u, float & v) {
    Triangle * retval = 0;
    float tnear = 0;
    float tfar = dist;
    float t;
    Vector3D p1 = scene_bound.min_v;
    Vector3D p2 = scene_bound.max_v;
    Vector3D &D = ray.getD();
    Vector3D &O = ray.getO();

    for (int i = 0; i < 3; i++) {
        if (D.get(i) < 0) {
            if (O.get(i) < p1.get(i))
                return retval;
        }
        else if (D.get(i) > 0) {
            if (O.get(i) > p2.get(i))
                return retval;
        }
    }
    /*
    for (int i = 0; i < 3; i++) {
        float pos =tfar * D.get(i);
        if (D.get(i) < 0) {
            if (pos < p1.get(i))
                tfar = tnear - (tfar - tnear) * (O.get(i) - p1.get(i))/pos;
            if (O.get(i) > p2.get(i))
                tnear += (tfar - tnear) * (O.get(i) - p2.get(i))/pos;
        }
        else {
            if (pos > p2.get(i))
                tfar = tnear + (tfar - tnear) * (p2.get(i) - O.get(i)) / pos;
            if (O.get(i) < p1.get(i))
                tnear += (tfar - tnear) * (p1.get(i) - O.get(i)) / pos;
        }
        if(tnear > tfar) return retval;
    }*/
    for (int i = 0; i < 3; i++) {
        float t1 = (p1.get(i) - O.get(i))/D.get(i);
        float t2 = (p2.get(i) - O.get(i))/D.get(i);
        if (t1 > t2) {
            float c = t2;
            t2 = t1;
            t1 = c;
        }
        if (t1 > tnear) tnear = t1;
        if (t2 < tfar) tfar = t2;
        if (tnear > tfar) return false;
    }
    int threadID = omp_get_thread_num();
    int entry = 0;
    int exit = 1;
    KdTreeNode *farchild;
    KdTreeNode *currnode;
    currnode = root;
    KdStack * stack = mStack[threadID];
    stack[entry].t = tnear;
    if (tnear > 0)
        stack[entry].pb = O + D * tnear;
    else
        stack[entry].pb = O;
    stack[exit].t = tfar;
    stack[exit].pb = O + D * tfar;
    stack[exit].node = 0;
    float pr[3] = {1.0f/D.get(0), 1.0f/D.get(1), 1.0f/D.get(2)};
    while (currnode) {
        while (!currnode->isLeaf()) {
            const float & splitPos = currnode->getSplitPos();
            const int & axis = currnode->getAxis();
            if (stack[entry].pb.get(axis) < splitPos) {
                currnode = currnode->getLeft();
                farchild = currnode+1;
                if (stack[exit].pb.get(axis) < splitPos) continue;
            }
            else {
                farchild = currnode->getLeft();
                currnode = farchild +1;
                if (stack[exit].pb.get(axis) > splitPos) continue;
            }
            t = (splitPos - O.get(axis)) * pr[axis];
            int tmp = exit++;
            if (exit == entry) ++exit;
            stack[exit].prev = tmp;
            stack[exit].t = t;
            stack[exit].node = farchild;
            stack[exit].pb = O + t * D;
            stack[exit].pb[axis] = splitPos;
        }
        ObjList * head = currnode->getObjList();
        float d = stack[exit].t+EPS;
        while (head) {
            Triangle * tri = head->getTriangle();
            int result;
            float mu;
            float mv;
            if (result = tri->intersect(ray, d, mu, mv)) {
                retval = tri;
                dist = d;
                u = mu;
                v = mv;
            }
            head = head->getNext();
        }
        if (retval) return retval;
        entry = exit;
        currnode = stack[exit].node;
        exit = stack[entry].prev;
    }
    return retval;
}


Triangle * KdTree::intersect(Ray & ray, float & dist) {
    Triangle * retval = 0;
    float tnear = 0;
    float tfar = dist;
    float t;
    Vector3D p1 = scene_bound.min_v;
    Vector3D p2 = scene_bound.max_v;
    Vector3D &D = ray.getD();
    Vector3D &O = ray.getO();

    for (int i = 0; i < 3; i++) {
        if (D.get(i) < 0) {
            if (O.get(i) < p1.get(i))
                return retval;
        }
        else if (D.get(i) > 0) {
            if (O.get(i) > p2.get(i))
                return retval;
        }
    }
    /*
    for (int i = 0; i < 3; i++) {
        float pos =tfar * D.get(i);
        if (D.get(i) < 0) {
            if (pos < p1.get(i))
                tfar = tnear - (tfar - tnear) * (O.get(i) - p1.get(i))/pos;
            if (O.get(i) > p2.get(i))
                tnear += (tfar - tnear) * (O.get(i) - p2.get(i))/pos;
        }
        else {
            if (pos > p2.get(i))
                tfar = tnear + (tfar - tnear) * (p2.get(i) - O.get(i)) / pos;
            if (O.get(i) < p1.get(i))
                tnear += (tfar - tnear) * (p1.get(i) - O.get(i)) / pos;
        }
        if(tnear > tfar) return retval;
    }*/
    for (int i = 0; i < 3; i++) {
        float t1 = (p1.get(i) - O.get(i))/D.get(i);
        float t2 = (p2.get(i) - O.get(i))/D.get(i);
        if (t1 > t2) {
            float c = t2;
            t2 = t1;
            t1 = c;
        }
        if (t1 > tnear) tnear = t1;
        if (t2 < tfar) tfar = t2;
        if (tnear > tfar) return false;
    }
    int threadID = omp_get_thread_num();
    int entry = 0;
    int exit = 1;
    KdTreeNode *farchild;
    KdTreeNode *currnode;
    currnode = root;
    KdStack * stack = mStack[threadID];
    stack[entry].t = tnear;
    if (tnear > 0)
        stack[entry].pb = O + D * tnear;
    else
        stack[entry].pb = O;
    stack[exit].t = tfar;
    stack[exit].pb = O + D * tfar;
    stack[exit].node = 0;
    float pr[3] = {1.0f/D.get(0), 1.0f/D.get(1), 1.0f/D.get(2)};
    while (currnode) {
        while (!currnode->isLeaf()) {
            const float & splitPos = currnode->getSplitPos();
            const int & axis = currnode->getAxis();
            if (stack[entry].pb.get(axis) < splitPos) {
                currnode = currnode->getLeft();
                farchild = currnode+1;
                if (stack[exit].pb.get(axis) < splitPos) continue;
            }
            else {
                farchild = currnode->getLeft();
                currnode = farchild +1;
                if (stack[exit].pb.get(axis) > splitPos) continue;
            }
            t = (splitPos - O.get(axis)) * pr[axis];
            int tmp = exit++;
            if (exit == entry) ++exit;
            stack[exit].prev = tmp;
            stack[exit].t = t;
            stack[exit].node = farchild;
            stack[exit].pb = O + t * D;
            stack[exit].pb[axis] = splitPos;
        }
        ObjList * head = currnode->getObjList();
        float d = stack[exit].t+EPS;
        while (head) {
            Triangle * tri = head->getTriangle();
            int result;
            if (result = tri->intersect(ray, d)) {
                retval = tri;
                dist = d;
            }
            head = head->getNext();
        }
        if (retval) return retval;
        entry = exit;
        currnode = stack[exit].node;
        exit = stack[entry].prev;
    }
    return retval;
}

void KdTree::DumpTree() {
    DumpNode(root);
}

void KdTree::DumpNode(KdTreeNode * node) {
    LogDefault->outValue("NodeType[leaf]",node->isLeaf());
    LogDefault->outValue("NodeType[data]",(unsigned int)node->getObjList() + node->getAxis());
    LogDefault->outValue("NodeType[axis]",node->getAxis());
    LogDefault->outValue("NodeType[spli]", node->getSplitPos());
    if (node->isLeaf()) LogDefault->outValue("NodeObjList", node->getObjList());
    else LogDefault->outValue("NextLeftNode", node->getLeft());
    LogDefault->line();
    if (!node->isLeaf()){
        LogDefault->line();DumpNode(node->getLeft());LogDefault->line();
    DumpNode(node->getRight());LogDefault->line();}
}
Triangle* KdTree::debugIntersect(Ray & ray, float & dist) {
    return intersect(ray, dist);

    return recursiveIntersection(ray, dist, root, scene_bound);
}

Triangle * KdTree::recursiveIntersection(Ray ray, float & dist, KdTreeNode * node, AABB box) {
    Triangle * retval = 0;
    if (node->isLeaf()){
        ObjList * head = node->getObjList();
        while (head) {
            Triangle * tri = head->getTriangle();
            int result;
            float d=dist;;
            if (result = tri->intersect(ray, d)) {
                retval = tri;
                dist = d;
            }
            head = head->getNext();
        }
        return retval;
    }
    else {
        AABB left = box;
        AABB right = box;
        left.max_v[node->getAxis()] = node->getSplitPos();
        right.min_v[node->getAxis()] = node->getSplitPos();
        if (ray.direction[node->getAxis()] == 0) {
            if (ray.origin[node->getAxis()] <= node->getSplitPos())
                return recursiveIntersection(ray, dist, node->getLeft(), left);
            else return recursiveIntersection(ray, dist, node->getRight(), right);
        }
        float distToSpos = node->getSplitPos() - ray.origin[node->getAxis()];
        distToSpos /= ray.direction[node->getAxis()];
        Vector3D poi = ray.origin + distToSpos * ray.direction;
        if (box.Contains(poi) && distToSpos >= 0) {
            if (ray.origin[node->getAxis()] <= node->getSplitPos()) {
                retval = recursiveIntersection(ray, dist, node->getLeft(), left);
                if (retval) return retval;
                return recursiveIntersection(ray, dist, node->getRight(), right);
            }
            else {
                
                retval = recursiveIntersection(ray, dist, node->getRight(), right);
                if (retval) return retval;
                return recursiveIntersection(ray, dist, node->getLeft(), left);
            }
        }
        else {
            if (ray.origin[node->getAxis()] <= node->getSplitPos())
                return recursiveIntersection(ray, dist, node->getLeft(), left);
            else return recursiveIntersection(ray, dist, node->getRight(), right);
        }
    }
}