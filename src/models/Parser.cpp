#include "Parser.hpp"

struct edge{
    int a, b, c;
};

std::vector<face> Parser::loadfile(std::string& path){
    std::vector<point> pVer;
    std::vector<edge> eVer;
    std::ifstream file(path);
    std::string line;

    while(std::getline(file, line)){
        std::istringstream iss(line);
        std::string keyword;

        iss >> keyword;
        if(keyword == "v"){
            point p;
            iss >> p.x >> p.y >> p.z;
            pVer.push_back(p);
        }
        if(keyword == "f"){
            edge e;
            iss >> e.a >> e.b >> e.c;
            eVer.push_back(e);
        }
        else continue;
    }

    std::vector<face> fVer;
    for(int i = 0; i < eVer.size(); i++){
        face f;
        edge e = eVer.at(i);
        f.p1 = pVer.at(e.a);
        f.p2 = pVer.at(e.b);
        f.p3 = pVer.at(e.c);
        fVer.push_back(f);
    }

    return fVer;
}