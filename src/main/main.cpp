#include "../models/Octree.hpp"
#include "../models/Parser.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>

namespace fs = std::filesystem;

void updateBounds(const point &p, point &minPoint, point &maxPoint) {
    if (p.x < minPoint.x) minPoint.x = p.x;
    if (p.y < minPoint.y) minPoint.y = p.y;
    if (p.z < minPoint.z) minPoint.z = p.z;

    if (p.x > maxPoint.x) maxPoint.x = p.x;
    if (p.y > maxPoint.y) maxPoint.y = p.y;
    if (p.z > maxPoint.z) maxPoint.z = p.z;
}

bool computeBounds(const std::vector<face> &faces, point &minPoint, point &maxPoint) {
    if (faces.empty()) {
        return false;
    }

    minPoint = faces.at(0).p1;
    maxPoint = faces.at(0).p1;

    for (int i = 0; i < static_cast<int>(faces.size()); i++) {
        updateBounds(faces.at(i).p1, minPoint, maxPoint);
        updateBounds(faces.at(i).p2, minPoint, maxPoint);
        updateBounds(faces.at(i).p3, minPoint, maxPoint);
    }

    return true;
}

bool readDepth(int &depth) {
    std::string line;
    std::getline(std::cin, line);

    std::stringstream ss(line);
    int parsedDepth;
    char extra;
    if (!(ss >> parsedDepth)) {
        return false;
    }
    if (ss >> extra) {
        return false;
    }
    if (parsedDepth < 0) {
        return false;
    }

    depth = parsedDepth;
    return true;
}

int main() {
    const fs::path testcaseDir("src/test/testcase");
    const fs::path resultDir("src/test/result");

    std::string fileName;
    std::cout << "Input file name from src/test/testcase: ";
    std::getline(std::cin, fileName);

    if (fileName.empty()) {
        std::cout << "Denied: file name cannot be empty." << std::endl;
        return 1;
    }

    fs::path inputPath = testcaseDir / fileName;
    if (!fs::exists(inputPath) || !fs::is_regular_file(inputPath)) {
        std::cout << "Denied: file does not exist at " << inputPath.string() << std::endl;
        return 1;
    }

    int depth = 0;
    std::cout << "Input octree depth (>= 0): ";
    while (!readDepth(depth)) {
        std::cout << "Invalid depth. Input octree depth (>= 0): ";
    }

    Parser parser;
    std::string inputPathString = inputPath.string();
    std::vector<face> faces = parser.loadfile(inputPathString);
    if (faces.empty()) {
        std::cout << "Parsing failed or file contains no faces." << std::endl;
        return 1;
    }

    point minPoint;
    point maxPoint;
    if (!computeBounds(faces, minPoint, maxPoint)) {
        std::cout << "Failed to compute bounds from parsed faces." << std::endl;
        return 1;
    }
    auto start = std::chrono::steady_clock::now();

    Octree octree(maxPoint, minPoint, depth, faces);
    octree.subdivide();

    VoxelObject voxelObject = octree.buildVoxelObject();

    fs::create_directories(resultDir);
    fs::path outputPath = resultDir / (inputPath.stem().string() + "_voxel_d" + std::to_string(depth) + ".obj");

    std::ofstream outputFile(outputPath.string());
    if (!outputFile.is_open()) {
        std::cout << "Failed to write output file: " << outputPath.string() << std::endl;
        return 1;
    }

    outputFile << voxelObject.toOBJString();
    outputFile.close();
    auto end = std::chrono::steady_clock::now();

    std::cout << "Voxelization complete." << std::endl;
    std::cout << "Voxels: " << voxelObject.voxelCount << std::endl;
    std::cout << "Vertices: " << voxelObject.vertices.size() << std::endl;
    std::cout << "Faces: " << voxelObject.faces.size() << std::endl;
    std::cout << "Output: " << outputPath.string() << std::endl;
    octree.getStatistics(depth);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time: " << duration.count() << "ms" << std::endl;
    std::cout << "Path: src/test/result";

    return 0;
}
