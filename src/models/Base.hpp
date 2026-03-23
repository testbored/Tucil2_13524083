#pragma once
#include <iostream>
#include <vector>

using namespace std;


struct point{
    float x;
    float y;
    float z;
    point();
    point(float a, float b, float c);
    point operator+(const point& other) const;
    point operator*(const int& other) const;
    point operator*(const float& other) const;
    point operator-(const point& other) const;
    float DOT(const point& other) const;
    point CROSS(const point& other) const;
};

struct face{
    point p1, p2, p3;
};






