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
    scene_bound.left -= Vector3D(EPS,EPS,EPS);
    scene_bound.right += Vector3D(EPS,EPS,EPS);
    LogDefault->flush();
}

void KdTree::resetAll() {
    mManager.reset();
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
    for (vector<Triangle *>::iterator it = tris.begin();
        it != tris.end(); it++) {
        for (int i = 0; i < 3; i++) {
            help[helpID].data[i] = (*it)->getMinAxis(i);
            help[helpID+1].data[i] = (*it)->getMaxAxis(i);
            if (help[helpID].data[i] < scene.left[i])
                scene.left[i] = help[helpID].data[i];
            if (help[helpID+1].data[i] > scene.right[i])
                scene.right[i] = help[helpID+1].data[i];
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
            else it->pt->next[axis] = 0;
        }
    }
    // now we have a nice sorted view
    root = new KdTreeNode();
    scene_bound = scene;
    subdivide(heads, root, scene, 0, tris.size(), 0);
    LogDefault->line();
    mManager.releaseHelperNodes();
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
void KdTree::subdivide(KdHelperList * heads[], KdTreeNode * node,
                       AABB box, int depth, int triangles, int openL)
{
    // depth is tested somewhere else, should always fail
    if (depth > MAX_KD_DEPTH)
        LogDefault->outStringN("KdTree went past max_depth");
    Vector3D sz = box.right - box.left;
    // surface area/volume
    float SAV = sz[0] * sz[1] + sz[0] * sz[2] + sz[1] * sz[2];
    SAV = 1.0f / SAV;
    int minAxis = -1;
    float minCost = INF;
    float bestPos = -INF;
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
            if (spos > box.left[axis]+EPS && spos < box.right[axis]-EPS) {
                // as we don't want to make a split so close to the edge of AABB
                lsz[axis] = spos - box.left[axis];
                rsz[axis] = sz[axis] - lsz[axis];
                float SA1 = lsz[0] * lsz[1] + lsz[0] * lsz[2] + lsz[1] * lsz[2];
                float SA2 = rsz[0] * rsz[1] + rsz[0] * rsz[2] + rsz[1] * rsz[2];
                float splitcost =
                    KDCTRAV + (SA1 * (n1+n4+n3) + SA2 * (n2 - n4)) * SAV * KDCINTR;
                if (splitcost < minCost) {
                    minCost = splitcost;
                    bestPos = spos;
                    minAxis = axis;
                }
            }
            n1 += n3 + n4;
            n2 -= n4;
            n3 = 0;
            n4 = 0;
        }
    }
    if (minAxis == -1 || depth >= MAX_KD_DEPTH) {
        // make a child out of this node
        node->setLeaf(true);
        ObjList * head = mManager.getObjListNodes(calcTriangles(heads[0], 0, 0, KdTree::testAll));
        addTriangles(heads[0], 0, head, 0, KdTree::testAll);
        //LogDefault->criticalOutValue("MadeIntLeaf, objects", calcTriangles(heads[0],0,0,KdTree::testAll));
        node->setObjList(head);
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
    
    KdHelperList * tmp = heads[minAxis];
    list<KdHelperList*> newnodes;
    while (tmp) {
        if (tmp->left) {
            if (tmp->data[minAxis] < bestPos) {
                if (tmp->otherSide->data[minAxis] > bestPos) {
                    KdHelperList * left = mManager.getKdHelperNode();
                    KdHelperList * right = mManager.getKdHelperNode();
                    left->otherSide = tmp;
                    right->otherSide = tmp->otherSide;
                    right->left = true;
                    left->tri = tmp->tri;
                    right->tri = tmp->tri;
                    for (int i = 0; i < 3; i++){
                        left->data[i] = tmp->otherSide->data[i];
                        right->data[i] = tmp->data[i];
                    }
                    left->otherSide->otherSide = left;
                    right->otherSide->otherSide = right;
                    left->data[minAxis] = bestPos;
                    right->data[minAxis] = bestPos + 2 * EPS;
                    newnodes.push_back(left);
                    newnodes.push_back(right);
                }
            }
        }
        tmp = tmp->next[minAxis];
    }
    for (int axis = 0; axis < 3; axis++) {
        newnodes.sort(sortByAxis[axis]);
        KdHelperList * tmpHead = heads[axis];
        for (list<KdHelperList *>::iterator it = newnodes.begin();
            it != newnodes.end(); it++) {
            if (tmpHead->data[axis] > (*it)->data[axis]) {
                (*it)->next[axis] = tmpHead;
                tmpHead = *it;
                heads[axis] = tmpHead;
                continue;
            }
            while (tmpHead->next[axis] && tmpHead->next[axis]->data[axis] <= (*it)->data[axis]) {
                tmpHead = tmpHead->next[axis];
            }
            (*it)->next[axis] = tmpHead->next[axis];
            tmpHead->next[axis] = (*it);
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
        curr_l = 0;
        curr_r = 0;
        KdHelperList * tmp = heads[i];
        while (tmp) {
            if (tmp->data[minAxis] <= (bestPos) && tmp->otherSide->data[minAxis] <= (bestPos)) { // to left!
                if (l_heads[i]) {
                    curr_l->next[i] = tmp;
                    curr_l = tmp;
                }
                else {
                    l_heads[i] = tmp;
                    curr_l = tmp;
                }
                if (tmp->left) n_left_start++;
                else n_left_end++;
            }
            else {
                if (r_heads[i]) {
                    curr_r->next[i] = tmp;
                    curr_r = tmp;
                }
                else {
                    r_heads[i] = tmp;
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
    AABB box_l(box);
    AABB box_r(box);
    box_l.right[minAxis]=bestPos;
    box_r.left[minAxis]=bestPos;
    int n_left_in = calcTriangles(l_heads[minAxis],minAxis,bestPos, KdTree::testLeft);
    if (n_left_in > 2) 
        subdivide(l_heads, node->getLeft(),box_l,depth+1, n_left,openL);
    else {
        node->getLeft()->setLeaf(true);
        if (n_left_in == 0)
            node->getLeft()->setObjList(0);
        else {
            ObjList * head = mManager.getObjListNodes(n_left_in);
            addTriangles(l_heads[minAxis], minAxis, head, bestPos, KdTree::testLeft);
            //LogDefault->criticalOutValue("MadeLeftLeaf, objects", n_left_in);
            node->getLeft()->setObjList(head);
        }
    }
    int n_right_in = calcTriangles(r_heads[minAxis], minAxis, bestPos, KdTree::testRight);

    if (n_right_in > 2)
        subdivide(r_heads, node->getRight(), box_r, depth+1, n_right, n_left_s);
    else {
        node->getRight()->setLeaf(true);
        if (n_right_in == 0)
            node->getRight()->setObjList(0);
        else {
            ObjList * head = mManager.getObjListNodes(n_right_in);
            addTriangles(r_heads[minAxis], minAxis, head, bestPos, KdTree::testRight);
            //LogDefault->criticalOutValue("MadeRightLeaf, objects", n_right_in);
            node->getRight()->setObjList(head);
        }
    }
}

Triangle * KdTree::intersect(Ray & ray, float & dist, float & u, float & v) {
    Triangle * retval = 0;
    float tnear = 0;
    float tfar = dist;
    float t;
    Vector3D p1 = scene_bound.left;
    Vector3D p2 = scene_bound.right;
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
    Vector3D p1 = scene_bound.left;
    Vector3D p2 = scene_bound.right;
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
        left.right[node->getAxis()] = node->getSplitPos();
        right.left[node->getAxis()] = node->getSplitPos();
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