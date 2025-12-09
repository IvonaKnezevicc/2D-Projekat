#pragma once
#include <GL/glew.h>
#include "Cinema.h"
#include "Seat.h"
#include "Person.h"

class Renderer {
public:
    Renderer(int windowWidth, int windowHeight);
    ~Renderer();
    
    void render(const Cinema& cinema);
    void renderSeat(const Seat& seat);
    void renderPerson(const Person& person);
    void renderScreen(float x, float y, float width, float height, float r, float g, float b);
    void renderDoor(float x, float y, bool isOpen, float angle);
    void renderOverlay();
    void renderStudentInfo();
    
private:
    int windowWidth, windowHeight;
    unsigned int shaderProgram;
    unsigned int VAO, VBO, EBO;
    unsigned int studentInfoTexture;
    
    void setupBuffers();
    void setupShader();
    void drawRectangle(float x, float y, float width, float height, float r, float g, float b, float a = 1.0f);
    void drawRectangleWithTexture(float x, float y, float width, float height, unsigned int texture, float alpha = 1.0f);
    void drawCircle(float x, float y, float radius, float r, float g, float b, float a = 1.0f);
    void drawPerson(float x, float y);
    unsigned int createTextTexture(const char* text, int& width, int& height);
};

