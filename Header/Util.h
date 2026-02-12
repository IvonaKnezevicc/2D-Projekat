#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

struct ModelData {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::string texturePath;
};

int endProgram(std::string message);
unsigned int createShader(const char* vsSource, const char* fsSource);
unsigned loadImageToTexture(const char* filePath);
GLFWcursor* createCameraCursor();
bool loadOBJModel(const char* filePath, ModelData& outModel);