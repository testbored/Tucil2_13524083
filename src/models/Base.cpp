#include "Base.hpp"


point::point() : x(0), y(0), z(0){}

point::point(float a, float b, float c) :  x(a), y(b), z(c){}

point point::operator+(const point& other) const{
    point p;
    p.x = this->x + other.x;
    p.y = this->y + other.y;
    p.z = this->z + other.z;
    return p;
}

point point::operator*(const int& other) const{
    point p;
    p.x = this->x*other;
    p.y = this->y*other;
    p.z = this->z*other;
    return p;
}

point point::operator*(const float& other) const{
    point p;
    p.x = this->x*other;
    p.y = this->y*other;
    p.z = this->z*other;
    return p;
}

point point::operator-(const point& other) const{
    point p;
    p.x = this->x - other.x;
    p.y = this->y - other.y;
    p.z = this->z - other.z;
    return p;
}

float point::DOT(const point& other) const{
    return this->x*other.x + this->y*other.y + this->z*other.z;
}

point point::CROSS(const point& other) const{
    point p;
    p.x = y*other.z - z*other.y;
    p.y = z*other.x - x*other.z;
    p.z = x*other.y - y*other.x;
    return p;
}