#include "Octree.hpp"
#include <algorithm>
#include <cmath>
#include <map>
#include <sstream>
#include <tuple>

BoundingBox::BoundingBox() : max(point()), min(point()) {}
BoundingBox::BoundingBox(point pmax, point pmin) : max(pmax), min(pmin){}

VoxelObject::VoxelObject() : voxelCount(0) {}

std::string VoxelObject::toOBJString() const {
    std::ostringstream out;
    out << "# voxel_count " << voxelCount << "\n";

    for (const point &v : vertices) {
        out << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }

    for (const std::array<int, 4> &f : faces) {
        out << "f " << f[0] << " " << f[1] << " " << f[2] << " " << f[3] << "\n";
    }

    return out.str();
}

point BoundingBox::getCentre(){
    return (min+max)*0.5f;
}

void Octree::setFaceList(std::vector<face> f){
    listF = f;
}

void Octree::addFacetoList(face f){
    listF.push_back(f);
}

Octree::Octree() : isLeaf(false), B(BoundingBox()), m_depth(0){
    for(int i = 0; i < 8; ++i) child[i] = nullptr;
}

Octree::Octree(point pmax, point pmin, int depth, std::vector<face> l) : isLeaf(false), B(BoundingBox(pmax, pmin)), m_depth(depth), listF(l){
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
    if(isLeaf){
        return listF.empty() ? 0 : 1;
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

void Octree::getLeafBoxes(std::vector<BoundingBox> &box){
    if(isLeaf){
        if(!listF.empty()){
            box.push_back(this->B);
        }
    }
    else{
        for(int i = 0; i < 8; i++){
            if(child[i]){
                child[i]->getLeafBoxes(box);
            }
        }
    }
}

VoxelObject Octree::buildVoxelObject() {
    std::vector<BoundingBox> boxes;
    getLeafBoxes(boxes);

    VoxelObject object;
    object.voxelCount = static_cast<int>(boxes.size());
    object.vertices.reserve(boxes.size() * 8);
    object.faces.reserve(boxes.size() * 6);

    std::map<std::tuple<float, float, float>, int> vertexIndex;
    std::map<std::array<int, 4>, std::pair<std::array<int, 4>, int>> faceCounter;

    auto getOrAddVertex = [&object, &vertexIndex](const point &p) -> int {
        std::tuple<float, float, float> key(p.x, p.y, p.z);
        std::map<std::tuple<float, float, float>, int>::iterator it = vertexIndex.find(key);
        if (it != vertexIndex.end()) {
            return it->second;
        }

        object.vertices.push_back(p);
        int index = static_cast<int>(object.vertices.size());
        vertexIndex[key] = index;
        return index;
    };

    auto addFace = [&faceCounter](const std::array<int, 4> &face) {
        std::array<int, 4> canonical = face;
        std::sort(canonical.begin(), canonical.end());

        std::map<std::array<int, 4>, std::pair<std::array<int, 4>, int>>::iterator it = faceCounter.find(canonical);
        if (it == faceCounter.end()) {
            faceCounter[canonical] = std::make_pair(face, 1);
        } else {
            it->second.second += 1;
        }
    };

    for (const BoundingBox &box : boxes) {
        point v1(box.min.x, box.min.y, box.min.z);
        point v2(box.max.x, box.min.y, box.min.z);
        point v3(box.max.x, box.max.y, box.min.z);
        point v4(box.min.x, box.max.y, box.min.z);
        point v5(box.min.x, box.min.y, box.max.z);
        point v6(box.max.x, box.min.y, box.max.z);
        point v7(box.max.x, box.max.y, box.max.z);
        point v8(box.min.x, box.max.y, box.max.z);

        int i1 = getOrAddVertex(v1);
        int i2 = getOrAddVertex(v2);
        int i3 = getOrAddVertex(v3);
        int i4 = getOrAddVertex(v4);
        int i5 = getOrAddVertex(v5);
        int i6 = getOrAddVertex(v6);
        int i7 = getOrAddVertex(v7);
        int i8 = getOrAddVertex(v8);

        addFace({i1, i2, i3, i4});
        addFace({i5, i6, i7, i8});
        addFace({i1, i2, i6, i5});
        addFace({i4, i3, i7, i8});
        addFace({i1, i5, i8, i4});
        addFace({i2, i3, i7, i6});
    }

    for (const std::pair<const std::array<int, 4>, std::pair<std::array<int, 4>, int>> &entry : faceCounter) {
        if (entry.second.second == 1) {
            object.faces.push_back(entry.second.first);
        }
    }

    return object;
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

void Octree::getStatistics(int n){
    std::vector<int> count;
    std::cout<<"Statistics Searched Node/Depth" << std::endl;
    for(int i = 1; i <= n; i++){
            int cur = getNodeCount(i);
            count.push_back(cur);
            std::cout << "Depth " << i << ": " << cur << std::endl;  
    }
    std::cout<<"Statisctic Unsearched Node/Depth" << std::endl;
    for(int i = 1; i <= n; i++){
        std::cout<<"Depth " << i << ": " << std::pow(8, i) - count.at(i - 1) << std::endl;  
    }
    std::cout << "Octree depth: " << n << endl;
}



int Octree::getNodeCount(int n){
    if(n == 0) return 1;
    else{
        int count = 0;
        for(int i = 0; i < 8; i++){
            if(child[i] != nullptr){
                count += child[i]->getNodeCount(n - 1);
            }
        }
        return count;
    }
}