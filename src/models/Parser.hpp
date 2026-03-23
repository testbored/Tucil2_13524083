#pragma once
#include "Base.hpp"
#include <fstream>
#include <sstream>
#include <string>


class Parser{
    public:
        std::vector<face> loadfile(std::string& path);
};

