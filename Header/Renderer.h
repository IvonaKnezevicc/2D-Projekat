#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Cinema.h"
#include "Seat.h"
#include "Person.h"
#include "Camera.h"

class Renderer {
public:
    Renderer(int windowWidth, int windowHeight);
    ~Renderer();
    
    void render(const Cinema& cinema, const Camera& camera, int windowWidth, int windowHeight);
    void renderSeat(const Seat& seat, const glm::mat4& view, const glm::mat4& projection);
    void renderPerson(const Person& person, const glm::mat4& view, const glm::mat4& projection);
    void renderScreen(float x, float y, float z, float width, float height, float depth, float r, float g, float b, const glm::mat4& view, const glm::mat4& projection);
    void renderDoor(float x, float y, float z, bool isOpen, float angle, const glm::mat4& view, const glm::mat4& projection);
    void renderHall(const Cinema& cinema, const glm::mat4& view, const glm::mat4& projection);
    void renderOverlay();
    void renderStudentInfo();
    
private:
    int windowWidth, windowHeight;
    unsigned int shaderProgram;
    unsigned int VAO, VBO, EBO;
    unsigned int studentInfoTexture;
    
    void setupBuffers();
    void setupShader();
    void drawCube(glm::vec3 position, glm::vec3 size, glm::vec3 color, const glm::mat4& view, const glm::mat4& projection);
    void drawPlane(glm::vec3 position, glm::vec3 normal, glm::vec2 size, glm::vec3 color, const glm::mat4& view, const glm::mat4& projection);
    void drawRectangle(float x, float y, float width, float height, float r, float g, float b, float a = 1.0f);
    void drawRectangleWithTexture(float x, float y, float width, float height, unsigned int texture, float alpha = 1.0f);
    void drawPerson(float x, float y);
};

