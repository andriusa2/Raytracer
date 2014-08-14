#include "./Triangle.h"

Triangle::Triangle() {
    vertices[0] = 0;
    vertices[1] = 0;
    vertices[2] = 0;
}

Triangle::Triangle(Vertex* v1, Vertex* v2, Vertex* v3, bool calcNorms) {
    vertices[0] = v1;
    vertices[1] = v2;
    vertices[2] = v3;
    if (calcNorms)
        recalcNorms();
    precompute();
}

Triangle::~Triangle() {

}

int Triangle::intersect(Ray& ray, float& dist, float& u, float& v) const {
    const unsigned int& ku=MOD3_ACCEL[k + 1];
	const unsigned int& kv=MOD3_ACCEL[k + 2];
	const Vector3D& O=ray.getO();
	const Vector3D& D=ray.getD();
	const Vector3D& A=vertices[0]->getPos();
	const float t=(nd-O.get(k)-nu*O.get(ku)-nv*O.get(kv))/(D.get(k)+nu*D.get(ku)+nv*D.get(kv));
	if(dist+EPS<t) return MISS;
	if(t<EPS) return MISS;
	const float hu=O.get(ku)+t*D.get(ku)-A.get(ku);
	const float hv=O.get(kv)+t*D.get(kv)-A.get(kv);
	const float alpha=hv*bnu+hu*bnv;
	if(alpha<0) return MISS;
	const float beta=hu*cnu+hv*cnv;
	if(beta<0) return MISS;
	if((alpha+beta)>1)return MISS;
	/*u = vertices[0]->getU() * (1 - alpha - beta) +
        vertices[1]->getU() * alpha +
        vertices[2]->getU() * beta;
	v = vertices[0]->getV() * (1 - alpha - beta) +
        vertices[1]->getV() * alpha +
        vertices[2]->getV() * beta;*/
    u = alpha;
    v = beta;
	dist=t;
	return HIT;
}

int Triangle::intersect(Ray& ray, float& dist) const {
    const unsigned int& ku=MOD3_ACCEL[k + 1];
	const unsigned int& kv=MOD3_ACCEL[k + 2];
	const Vector3D& O=ray.getO();
	const Vector3D& D=ray.getD();
	const Vector3D& A=vertices[0]->getPos();
	const float t=(nd-O.get(k)-nu*O.get(ku)-nv*O.get(kv))/(D.get(k)+nu*D.get(ku)+nv*D.get(kv));
	if(dist+EPS<t) return MISS;
	if(t<EPS) return MISS;
	const float hu=O.get(ku)+t*D.get(ku)-A.get(ku);
	const float hv=O.get(kv)+t*D.get(kv)-A.get(kv);
	const float alpha=hv*bnu+hu*bnv;
	if(alpha<0) return MISS;
	const float beta=hu*cnu+hv*cnv;
	if(beta<0) return MISS;
	if((alpha+beta)>1)return MISS;
	dist=t;
	return HIT;
}


__m128 Triangle::intersectPack(__m128 orig[], __m128 dir[], __m128 & dist, __m128 & tnear) const {
    const unsigned int& ku = MOD3_ACCEL[k + 1];
    const unsigned int& kv = MOD3_ACCEL[k + 2];
    const Vector3D& A = vertices[0]->getPos();
    const __m128 verts[] = { _mm_set_ps1(A.get(ku)), _mm_set_ps1(A.get(kv)) };
    //(nd - O.get(k) - nu*O.get(ku) - nv*O.get(kv)) / 
    const __m128 nd_ = _mm_set_ps1(nd);
    const __m128 nu_ = _mm_set_ps1(nu);
    const __m128 nv_ = _mm_set_ps1(nv);
    const __m128 bnu_ = _mm_set_ps1(bnu);
    const __m128 bnv_ = _mm_set_ps1(bnv);
    const __m128 cnu_ = _mm_set_ps1(cnu);
    const __m128 cnv_ = _mm_set_ps1(cnv);
    const __m128 up =
        _mm_sub_ps(
        nd_,
        _mm_add_ps(
            orig[k],
            _mm_add_ps(
                _mm_mul_ps(nu_,orig[ku]),
                _mm_mul_ps(nv_, orig[kv])
            )
        ));
    //(D.get(k) + nu*D.get(ku) + nv*D.get(kv));
    const __m128 bot = _mm_add_ps(dir[k], _mm_add_ps(_mm_mul_ps(nu_, dir[ku]), _mm_mul_ps(nv_, dir[kv])));
    const __m128 t = _mm_div_ps(up, bot);
    
    __m128 retval = _mm_cmpge_ps(dist, t);
    retval = _mm_and_ps(retval, _mm_cmpge_ps(t, tnear));
    //const float hu = O.get(ku) + t*D.get(ku) - A.get(ku);
    //const float hv = O.get(kv) + t*D.get(kv) - A.get(kv);
    if (_mm_movemask_ps(retval) == 0)
        return retval;
    const __m128 hu = _mm_add_ps(orig[ku], _mm_sub_ps(_mm_mul_ps(t, dir[ku]), verts[0]));
    const __m128 hv = _mm_add_ps(orig[kv], _mm_sub_ps(_mm_mul_ps(t, dir[kv]), verts[1]));
    //const float alpha = hv*bnu + hu*bnv;
    
    const __m128 alpha = _mm_add_ps(_mm_mul_ps(hv, bnu_), _mm_mul_ps(hu, bnv_));

    retval = _mm_and_ps(retval, _mm_cmpge_ps(alpha, _mm_setzero_ps()));
    
    //const float beta = hu*cnu + hv*cnv;
    
    const __m128 beta = _mm_add_ps(_mm_mul_ps(hu, cnu_), _mm_mul_ps(hv, cnv_));
    retval = _mm_and_ps(retval, _mm_cmpge_ps(beta, _mm_setzero_ps()));

    retval = _mm_and_ps(retval, _mm_cmple_ps(_mm_add_ps(alpha, beta), _mm_set_ps1(1.0f)));
    dist = _mm_or_ps(_mm_and_ps(t, retval), _mm_andnot_ps(retval, dist));
    return retval;
}

Vector3D Triangle::getNormal() const {
    return vertices[0]->getNormal();
}

Vector3D Triangle::getNormal(const float& u, const float& v) const {
    Vector3D retval = vertices[0]->getNormal() * (1.0f - u - v) + vertices[1]->getNormal()*u + vertices[2]->getNormal()*v;
    retval.normalize();
    return retval;
}

Vertex* Triangle::getVertex(int ID) const {
    if (ID >= 0 && ID < 3)
        return vertices[ID];
}

void Triangle::setVertex(int ID, Vertex* v) {
    if (ID >= 0 && ID < 3)
        vertices[ID] = v;
}

void Triangle::normalize() {
    for (int i = 0; i < 3; i++)
        vertices[i]->normalize();
}

void Triangle::precompute() {
    
	Vector3D A=vertices[0]->getPos();
	Vector3D B=vertices[1]->getPos();
	Vector3D C=vertices[2]->getPos();
	Vector3D c=B-A;
	Vector3D b=C-A;

	Vector3D mN=b.cross(c);
    /* or maybe?
    Vector3D mN = V3D_BLANK;
    for (int i = 0; i < 3; i++)
        mN += vertices[i]->getNormal();
    mN.normalize();
    */
	int u, v;
    // find the max axis
	if(fabs(mN[0])>fabs(mN[1]))
	{
		if(fabs(mN[0])>fabs(mN[2])) k=0; else k=2;
	}
	else{
		if(fabs(mN[1])>fabs(mN[2])) k=1; else k=2;
	}
	u=(k+1)%3;//projection axis
	v=(k+2)%3;
	float koef=1.0f/(mN[k]);
	nu=mN[u]*koef;
	nv=mN[v]*koef;
	nd=mN.dot(A)*koef;
	float kofe=1.0f/(b[u]*c[v]-b[v]*c[u]);
	bnu=b[u]*kofe;
	bnv=b[v]*(-kofe);
	cnu=c[v]*kofe;
	cnv=c[u]*(-kofe);

    nd_ = _mm_set_ps1(nd);
    nu_ = _mm_set_ps1(nu);
    nv_ = _mm_set_ps1(nv);
    bnu_ = _mm_set_ps1(bnu);
    bnv_ = _mm_set_ps1(bnv);
    cnu_ = _mm_set_ps1(cnu);
    cnv_ = _mm_set_ps1(cnv);
}
void Triangle::recalcNorms() {
   	Vector3D A=vertices[0]->getPos();
	Vector3D B=vertices[1]->getPos();
	Vector3D C=vertices[2]->getPos();
	Vector3D c=B-A;
	Vector3D b=C-A;

	Vector3D mN=b.cross(c);
    mN.normalize();
	for(int i=0; i<3;++i)
		vertices[i]->accNormal(mN);
}
ostream& operator<< (ostream& out, const Triangle& tri) {
    out << "Triangle: \n";
    for (int i = 0; i < 3; i++)
        out << "\tvertex"<<i<<"="<<(*tri.vertices[i])<<"\n";
    out << "\t(nu,nv,nd)=(" << tri.nu << ";" << tri.nv << ";" << tri.nd <<")\n" <<
        "\t(bnu,bnv)=(" << tri.bnu << ";" << tri.bnv << ")\n" <<
        "\t(cnu,cnv)=(" << tri.cnu << ";" << tri.cnv << ")\n";
    return out;
}

Vector3D Triangle::sampleSurface() const {
    // try to sample the surface uniformally
    const Vector3D& a = vertices[0]->getPos();
    const Vector3D& b = vertices[1]->getPos();
    const Vector3D& c = vertices[2]->getPos();
    const int & id = omp_get_thread_num();
    const float e1 = GET_RND(id);
    const float e2 = e1 * GET_RND(id);

    return a * (1.0f - e1 - e2) + b * e1 + c * e2;
}

Material * Triangle::getMat() const {
    return mat;
}
void Triangle::setMat(Material * a) {
    mat = a;
}

float Triangle::getMinAxis(unsigned int axis) {
    float minAxis = INF;
    for (int i = 0; i < 3; i++)
        if (minAxis > vertices[i]->getPos().get(axis))
            minAxis = vertices[i]->getPos().get(axis);
    return minAxis;
}
float Triangle::getMaxAxis(unsigned int axis) {
    float maxAxis = -INF;
    for (int i = 0; i < 3; i++) 
        if (maxAxis < vertices[i]->getPos().get(axis))
            maxAxis = vertices[i]->getPos().get(axis);
    return maxAxis;
}
void Triangle::recalc(bool recalcNormals) {
    if (recalcNormals)
        recalcNorms();
    precompute();
}
static int sutherlandHodgman(Vector3D input[], int inCount, Vector3D *output, int axis,
    double splitPos, bool isMinimum) {
    if (inCount < 3)
        return 0;

    Vector3D cur = input[0];
    double sign = isMinimum ? 1.0f : -1.0f;
    double distance = sign * (cur.get(axis) - splitPos);
    bool  curIsInside = (distance >= 0);
    int   outCount = 0;

    for (int i = 0; i < inCount; ++i) {
        int nextIdx = i + 1;
        if (nextIdx == inCount)
            nextIdx = 0;
        Vector3D next = input[nextIdx];
        distance = sign * (next.get(axis) - splitPos);
        bool nextIsInside = (distance >= 0);

        if (curIsInside && nextIsInside) {
            /* Both this and the next vertex are inside, add to the list */
            output[outCount++] = next;
        }
        else if (curIsInside && !nextIsInside) {
            /* Going outside -- add the intersection */
            double t = (splitPos - cur.get(axis)) / (next.get(axis) - cur.get(axis));
            Vector3D p = cur + (next - cur) * t;
            p[axis] = splitPos; // Avoid roundoff errors
            output[outCount++] = p;
        }
        else if (!curIsInside && nextIsInside) {
            /* Coming back inside -- add the intersection + next vertex */
            double t = (splitPos - cur[axis]) / (next[axis] - cur[axis]);
            Vector3D p = cur + (next - cur) * t;
            p[axis] = splitPos; // Avoid roundoff errors
            output[outCount++] = p;
            output[outCount++] = next;
        }
        else {
            /* Entirely outside - do not add anything */
        }
        cur = next;
        curIsInside = nextIsInside;
    }
    return outCount;
}


AABB Triangle::getClippedAABB(const AABB &aabb) const {
    /* Reserve room for some additional vertices */
    // todo track how many vertices are used really
    Vector3D vertices1[30], vertices2[30];
    int nVertices = 3;

    for (int i = 0; i<3; ++i)
        vertices1[i] = getVertex(i)->getPos();

    for (int axis = 0; axis<3; ++axis) {
        nVertices = sutherlandHodgman(vertices1, nVertices, vertices2, axis, aabb.min_v.get(axis), true);
        nVertices = sutherlandHodgman(vertices2, nVertices, vertices1, axis, aabb.max_v.get(axis), false);
    }

    AABB result;
    for (int i = 0; i<nVertices; ++i) {
        for (int j = 0; j<3; ++j) {
            result.min_v[j] = std::min(result.min_v[j], vertices1[i][j]);
            result.max_v[j] = std::max(result.max_v[j], vertices1[i][j]);
        }
    }
    for (int i = 0; i<3; ++i) {
        result.min_v[i] = std::max(result.min_v[i], aabb.min_v.get(i));
        result.max_v[i] = std::min(result.max_v[i], aabb.max_v.get(i));
    }

    return result;
}