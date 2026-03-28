#include "Parser.hpp"

struct edge{
    int a, b, c;
};

int parseVertexIndexToken(const std::string &token, int vertexCount) {
    std::string value = token;
    std::size_t slashPos = value.find('/');
    if (slashPos != std::string::npos) {
        value = value.substr(0, slashPos);
    }

    if (value.empty()) {
        return -1;
    }

    int idx = std::stoi(value);

    // OBJ positive indices are 1-based, negative indices are relative to the end.
    if (idx > 0) {
        idx = idx - 1;
    } else if (idx < 0) {
        idx = vertexCount + idx;
    } else {
        return -1;
    }

    if (idx < 0 || idx >= vertexCount) {
        return -1;
    }

    return idx;
}

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
            std::string t1, t2, t3;
            if (!(iss >> t1 >> t2 >> t3)) {
                continue;
            }

            edge e;
            e.a = parseVertexIndexToken(t1, static_cast<int>(pVer.size()));
            e.b = parseVertexIndexToken(t2, static_cast<int>(pVer.size()));
            e.c = parseVertexIndexToken(t3, static_cast<int>(pVer.size()));

            if (e.a >= 0 && e.b >= 0 && e.c >= 0) {
                eVer.push_back(e);
            }
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