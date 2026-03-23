#include "Octree.hpp"
#include <cmath>

BoundingBox::BoundingBox() : max(point()), min(point()) {}
BoundingBox::BoundingBox(point pmax, point pmin) : max(pmax), min(pmin){}

point BoundingBox::getCentre(){
    return (min+max)*0.5f;
}

void Octree::setFaceList(std::vector<face> f){
    listF = f;
}

void Octree::addFacetoList(face f){
    listF.push_back(f);
}

Octree::Octree() : isLeaf(false), child({nullptr}), B(BoundingBox()), m_depth(0){
    for(int i = 0; i < 8; ++i) child[i] = nullptr;
}

Octree::Octree(point pmax, point pmin, int depth, std::vector<face> l) : isLeaf(false), child({nullptr}), B(BoundingBox(pmax, pmin)), m_depth(depth), listF(l){
    for(int i = 0; i < 8; ++i) child[i] = nullptr;
}

void Octree::subdivide(){
    if(m_depth <= 0 || listF.empty()){
        isLeaf = true;
        return;
    }

    isLeaf = false;
    point c = B.getCentre();
    point min = B.min;
    point max = B.max;

    for(int i = 0; i < 8; i++){
        point cMin, cMax;

        cMin.x = (i & 1) ? c.x : min.x;
        cMax.x = (i & 1) ? max.x : c.x;
        
        cMin.y = (i & 2) ? c.y : min.y;
        cMax.y = (i & 2) ? max.y : c.y;
        
        cMin.z = (i & 4) ? c.z : min.z;
        cMax.z = (i & 4) ? max.z : c.z;


        BoundingBox cBox(cMax, cMin);
        std::vector<face> listC;

        for(const auto& f : listF){
            if(cBox.collisionCheck(f)){
                listC.push_back(f);
            }
        }

        if(!listC.empty()){
            child[i] = new Octree(cMax, cMin, m_depth - 1, listC);
            child[i]->subdivide();
        }
        else{
            child[i] = nullptr;
        }
    }

    listF.clear();
}

int Octree::getVoxelsCount(){
    if(isLeaf && !listF.empty()){
        return 1;
    }
    else{
        int result = 0;
        for(int i = 0; i < 8; i++){
            if(child[i] != nullptr){
                result += child[i]->getVoxelsCount();
            }
        }
        return result;
    }
}


bool axisReject(const point& axis, const point& v0, const point& v1, const point& v2, const point& h){
    float p0 = axis.DOT(v0);
    float p1 = axis.DOT(v1);
    float p2 = axis.DOT(v2);

    float triMin = std::fmin(p0, std::fmin(p1, p2));
    float triMax = std::fmax(p0, std::fmax(p1, p2));
    float r = h.x * std::fabs(axis.x) + h.y * std::fabs(axis.y) + h.z * std::fabs(axis.z);

    return triMin > r || triMax < -r;
}

bool testEdgeAxes(const point& e, const point& v0, const point& v1, const point& v2, const point& h){
    point axisX(0.0f, -e.z, e.y);   
    point axisY(e.z, 0.0f, -e.x);   
    point axisZ(-e.y, e.x, 0.0f);   

    if(axisReject(axisX, v0, v1, v2, h)) return false;
    if(axisReject(axisY, v0, v1, v2, h)) return false;
    if(axisReject(axisZ, v0, v1, v2, h)) return false;
    return true;
}

bool BoundingBox::collisionCheck(face f){
    point c = getCentre();
    point h = (max - min) * 0.5f;
    point v0 = f.p1 - c;
    point v1 = f.p2 - c;
    point v2 = f.p3 - c;
    point e0 = v1 - v0;
    point e1 = v2 - v1;
    point e2 = v0 - v2;

    // Bullet 3 from the paper: 9 separating axes from edge x axis tests.
    if(!testEdgeAxes(e0, v0, v1, v2, h)) return false;
    if(!testEdgeAxes(e1, v0, v1, v2, h)) return false;
    if(!testEdgeAxes(e2, v0, v1, v2, h)) return false;

    // Bullet 1: overlap in x/y/z directions.
    float minX = std::fmin(v0.x, std::fmin(v1.x, v2.x));
    float maxX = std::fmax(v0.x, std::fmax(v1.x, v2.x));
    if(minX > h.x || maxX < -h.x) return false;

    float minY = std::fmin(v0.y, std::fmin(v1.y, v2.y));
    float maxY = std::fmax(v0.y, std::fmax(v1.y, v2.y));
    if(minY > h.y || maxY < -h.y) return false;

    float minZ = std::fmin(v0.z, std::fmin(v1.z, v2.z));
    float maxZ = std::fmax(v0.z, std::fmax(v1.z, v2.z));
    if(minZ > h.z || maxZ < -h.z) return false;

    // Bullet 2: box-plane overlap using triangle normal.
    point normal = e0.CROSS(e1);
    float d = normal.DOT(v0);
    point vmin, vmax;

    if(normal.x > 0.0f){
        vmin.x = -h.x;
        vmax.x = h.x;
    } else {
        vmin.x = h.x;
        vmax.x = -h.x;
    }

    if(normal.y > 0.0f){
        vmin.y = -h.y;
        vmax.y = h.y;
    } else {
        vmin.y = h.y;
        vmax.y = -h.y;
    }

    if(normal.z > 0.0f){
        vmin.z = -h.z;
        vmax.z = h.z;
    } else {
        vmin.z = h.z;
        vmax.z = -h.z;
    }

    if(normal.DOT(vmin) > d) return false;
    if(normal.DOT(vmax) < d) return false;

    return true;
}

