#pragma once
#include "Base.hpp"

#define TopLeftFront 0
#define TopRightFront 1
#define BottomRightFront 2
#define BottomLeftFront 3
#define TopLeftBottom 4
#define TopRightBottom 5
#define BottomRightBack 6
#define BottomLeftBack 7

struct BoundingBox{
    point min;
    point max;
    
    BoundingBox();
    BoundingBox(point pmax, point pmin);
    point getCentre();
    bool collisionCheck(face f);
};


class Octree{
    private:
        bool isLeaf;
        Octree *child[8];
        BoundingBox B;
        int m_depth;
        std::vector<face> listF;
    public:
        void setFaceList(std::vector<face> f);
        void addFacetoList(face f);
        Octree();
        Octree(point pmax, point pmin, int depth, std::vector<face> l);
        void subdivide();
        int getVoxelsCount();
};




