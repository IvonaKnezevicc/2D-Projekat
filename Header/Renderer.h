#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Model.h"

class Cinema;
class Seat;
class Person;
class Camera;

class Renderer {
public:
    Renderer(int windowWidth, int windowHeight);
    ~Renderer();
    
    void render(const Cinema& cinema, const Camera& camera, int windowWidth, int windowHeight);
    void renderSeat(const Seat& seat, const glm::mat4& view, const glm::mat4& projection, bool dimForProjection = false);
    void renderPerson(const Person& person, const glm::mat4& view, const glm::mat4& projection);
    void renderModel(const Model& model, const glm::mat4& modelMatrix, const glm::mat4& view, const glm::mat4& projection);
    void renderScreen(float x, float y, float z, float width, float height, float depth, float r, float g, float b, const glm::mat4& view, const glm::mat4& projection);
    void renderScreenWithTexture(float x, float y, float z, float width, float height, float depth, unsigned int texture, const glm::mat4& view, const glm::mat4& projection);
    void renderDoor(float x, float y, float z, bool isOpen, float angle, const glm::mat4& view, const glm::mat4& projection);
    void renderPortal(float x, float y, float z, const glm::mat4& view, const glm::mat4& projection);
    void renderHall(const Cinema& cinema, const glm::mat4& view, const glm::mat4& projection);
    void renderStairs(const Cinema& cinema, const glm::mat4& view, const glm::mat4& projection);
    void renderOverlay();
    void renderStudentInfo();
    void renderCrosshair();
    
    int getWindowWidth() const { return windowWidth; }
    int getWindowHeight() const { return windowHeight; }
    
private:
    int windowWidth, windowHeight;
    unsigned int shaderProgram;
    unsigned int VAO, VBO, EBO;
    unsigned int studentInfoTexture;
    unsigned int seatTextureUpper;
    unsigned int seatTextureBase;
    std::vector<unsigned int> filmTextures;
    std::vector<Model> humanoidModels;
    
    void setupBuffers();
    void setupShader();
    void drawCube(glm::vec3 position, glm::vec3 size, glm::vec3 color, const glm::mat4& view, const glm::mat4& projection);
    void drawCubeWithTexture(glm::vec3 position, glm::vec3 size, unsigned int texture, const glm::vec3& tint, const glm::mat4& view, const glm::mat4& projection);
    void drawPlane(glm::vec3 position, glm::vec3 normal, glm::vec2 size, glm::vec3 color, const glm::mat4& view, const glm::mat4& projection);
    void drawRectangle(float x, float y, float width, float height, float r, float g, float b, float a = 1.0f);
    void drawRectangleWithTexture(float x, float y, float width, float height, unsigned int texture, float alpha = 1.0f);
    void drawPerson(float x, float y);
};
