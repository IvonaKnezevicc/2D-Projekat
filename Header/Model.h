#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Model {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;
    unsigned int texture;
    bool hasTexture;
    float normalizeScale;
    glm::vec3 localOffset;
    
    Model() : VAO(0), VBO(0), EBO(0), texture(0), hasTexture(false), normalizeScale(1.0f), localOffset(0.0f) {}
};
