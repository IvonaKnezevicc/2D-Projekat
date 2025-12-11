#include "../Header/Renderer.h"
#include "../Header/Util.h"
#include <cmath>
#include <iostream>
#include <vector>

Renderer::Renderer(int windowWidth, int windowHeight)
    : windowWidth(windowWidth), windowHeight(windowHeight), studentInfoTexture(0)
{
    setupShader();
    setupBuffers();
    
    studentInfoTexture = loadImageToTexture("Resources/student_info.png");
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    if (studentInfoTexture != 0) {
        glDeleteTextures(1, &studentInfoTexture);
    }
}

void Renderer::setupShader() {
    shaderProgram = createShader(
        "Shaders/vertex_shader.vert",
        "Shaders/fragment_shader.frag"
    );
    
    if (shaderProgram == 0) {
        std::cout << "Greska pri kreiranju sejdera!" << std::endl;
    }
}

void Renderer::setupBuffers() {
    float vertices[] = {
        0.5f,  0.5f,    1.0f, 1.0f,
        0.5f, -0.5f,    1.0f, 0.0f,
        -0.5f, -0.5f,   0.0f, 0.0f,
        -0.5f,  0.5f,   0.0f, 1.0f
    };
    
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void Renderer::render(const Cinema& cinema) {
    glUseProgram(shaderProgram);
    
    float aspect = (float)windowWidth / (float)windowHeight;
    float proj[16] = {
        1.0f/aspect, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    GLint projLoc = glGetUniformLocation(shaderProgram, "uProjection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, proj);
    
    renderScreen(cinema.getScreenX(), cinema.getScreenY(), 
                 cinema.getScreenWidth(), cinema.getScreenHeight(),
                 cinema.getScreenColorR(), cinema.getScreenColorG(), cinema.getScreenColorB());
    
    renderDoor(cinema.getDoorX(), cinema.getDoorY(), cinema.isDoorOpen(), cinema.getDoorAngle());
    
    for (const auto& seat : cinema.getSeats()) {
        renderSeat(seat);
    }
    
    for (const auto& person : cinema.getPeople()) {
        renderPerson(person);
    }
    
    if (cinema.showOverlay()) {
        renderOverlay();
    }
    
    renderStudentInfo();
}

void Renderer::renderSeat(const Seat& seat) {
    float r, g, b;
    
    switch (seat.status) {
        case SeatStatus::AVAILABLE:
            r = 0.2f; g = 0.4f; b = 1.0f;
            break;
        case SeatStatus::RESERVED:
            r = 1.0f; g = 1.0f; b = 0.0f;
            break;
        case SeatStatus::BOUGHT:
            r = 1.0f; g = 0.0f; b = 0.0f;
            break;
    }
    
    float seatWidth = 0.05f;
    float seatDepth = 0.042f;
    float backHeight = 0.024f;
    float armrestWidth = 0.009f;
    float armrestDepth = 0.038f;
    
    drawRectangle(seat.x, seat.y, seatWidth, seatDepth, r, g, b, 1.0f);
    
    float backY = seat.y - seatDepth/2.0f - backHeight/2.0f;
    drawRectangle(seat.x, backY, seatWidth * 0.94f, backHeight, r * 0.87f, g * 0.87f, b * 0.87f, 1.0f);
    
    float leftArmrestX = seat.x - seatWidth/2.0f - armrestWidth/2.0f;
    drawRectangle(leftArmrestX, seat.y, armrestWidth, armrestDepth, r * 0.73f, g * 0.73f, b * 0.73f, 1.0f);
    
    float rightArmrestX = seat.x + seatWidth/2.0f + armrestWidth/2.0f;
    drawRectangle(rightArmrestX, seat.y, armrestWidth, armrestDepth, r * 0.73f, g * 0.73f, b * 0.73f, 1.0f);
    
    float armrestVerticalWidth = armrestWidth * 1.3f;
    float armrestVerticalHeight = backHeight * 0.65f;
    float armrestVerticalY = backY + backHeight/3.0f;
    
    drawRectangle(leftArmrestX, armrestVerticalY, armrestVerticalWidth, armrestVerticalHeight, r * 0.71f, g * 0.71f, b * 0.71f, 1.0f);
    drawRectangle(rightArmrestX, armrestVerticalY, armrestVerticalWidth, armrestVerticalHeight, r * 0.71f, g * 0.71f, b * 0.71f, 1.0f);
}

void Renderer::renderPerson(const Person& person) {
    drawPerson(person.x, person.y);
}

void Renderer::renderScreen(float x, float y, float width, float height, float r, float g, float b) {
    drawRectangle(x + width/2.0f, y - height/2.0f, width, height, r, g, b, 1.0f);
}

void Renderer::renderDoor(float x, float y, bool isOpen, float angle) {
    float doorWidth = 0.1f;
    float doorHeight = 0.14f;
    
    float frameWidth = 0.12f;
    float frameHeight = 0.16f;
    drawRectangle(x, y, frameWidth, frameHeight, 0.3f, 0.3f, 0.3f, 1.0f);
    
    float angleRad = angle * 3.14159265359f / 180.0f;
    float greenIntensity = 0.3f + (angle / 90.0f) * 0.7f;
    float doorX = x - (frameWidth/2.0f - doorWidth/2.0f) * (1.0f - cos(angleRad));
    float doorY = y;
    
    drawRectangle(doorX, doorY, doorWidth, doorHeight, 0.0f, greenIntensity, 0.0f, 1.0f);
    
    float panelHeight = doorHeight / 4.0f;
    for (int i = 0; i < 3; i++) {
        float panelY = doorY - doorHeight/2.0f + panelHeight * (i + 0.5f);
        drawRectangle(doorX, panelY, doorWidth * 0.9f, 0.003f, 0.0f, greenIntensity * 0.7f, 0.0f, 1.0f);
    }
    
    float handleX = doorX + doorWidth/2.0f - 0.015f;
    float handleY = doorY;
    drawRectangle(handleX, handleY, 0.008f, 0.015f, 0.8f, 0.8f, 0.8f, 1.0f);
}

void Renderer::renderOverlay() {
    float aspect = (float)windowWidth / (float)windowHeight;
    float overlayWidth = 2.0f * aspect;
    float overlayHeight = 2.0f;
    drawRectangle(0.0f, 0.0f, overlayWidth, overlayHeight, 0.2f, 0.2f, 0.2f, 0.5f);
}

void Renderer::renderStudentInfo() {
    float aspect = (float)windowWidth / (float)windowHeight;
    float infoX = -0.98f * aspect;
    float infoY = -0.95f;
    float infoWidth = 0.6f;
    float infoHeight = 0.15f;
    
    drawRectangle(infoX + infoWidth/2.0f, infoY + infoHeight/2.0f, 
                  infoWidth, infoHeight, 0.1f, 0.1f, 0.1f, 0.7f);
    
    if (studentInfoTexture != 0) {
        drawRectangleWithTexture(infoX + infoWidth/2.0f, infoY + infoHeight/2.0f, 
                                 infoWidth * 0.95f, infoHeight * 0.95f,
                                 studentInfoTexture, 0.95f);
    } else {
        float textY = infoY + infoHeight - 0.03f;
        float lineHeight = 0.025f;
        float startX = infoX + 0.02f;
        
        float lineWidth1 = 0.1f;
        drawRectangle(startX + lineWidth1/2.0f, textY, lineWidth1, lineHeight * 0.5f, 1.0f, 1.0f, 1.0f, 0.95f);
        
        float textY2 = textY - lineHeight - 0.003f;
        float lineWidth2 = 0.16f;
        drawRectangle(startX + lineWidth2/2.0f, textY2, lineWidth2, lineHeight * 0.5f, 1.0f, 1.0f, 1.0f, 0.95f);
        
        float textY3 = textY2 - lineHeight - 0.003f;
        float lineWidth3 = 0.14f;
        drawRectangle(startX + lineWidth3/2.0f, textY3, lineWidth3, lineHeight * 0.5f, 1.0f, 1.0f, 1.0f, 0.95f);
    }
}

void Renderer::drawRectangle(float x, float y, float width, float height, float r, float g, float b, float a) {
    glUseProgram(shaderProgram);
    
    float model[16] = {
        width, 0.0f, 0.0f, 0.0f,
        0.0f, height, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        x, y, 0.0f, 1.0f
    };
    
    GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
    
    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
    glUniform4f(colorLoc, r, g, b, a);
    
    GLint useTexLoc = glGetUniformLocation(shaderProgram, "uUseTexture");
    glUniform1i(useTexLoc, 0);
    
    GLint alphaLoc = glGetUniformLocation(shaderProgram, "uAlpha");
    glUniform1f(alphaLoc, 1.0f);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::drawRectangleWithTexture(float x, float y, float width, float height, unsigned int texture, float alpha) {
    glUseProgram(shaderProgram);
    
    float model[16] = {
        width, 0.0f, 0.0f, 0.0f,
        0.0f, height, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        x, y, 0.0f, 1.0f
    };
    
    GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
    
    GLint useTexLoc = glGetUniformLocation(shaderProgram, "uUseTexture");
    glUniform1i(useTexLoc, 1);
    
    GLint alphaLoc = glGetUniformLocation(shaderProgram, "uAlpha");
    glUniform1f(alphaLoc, alpha);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::drawPerson(float x, float y) {
    drawRectangle(x, y + 0.025f, 0.02f, 0.02f, 1.0f, 0.8f, 0.6f, 1.0f);
    
    drawRectangle(x, y, 0.015f, 0.05f, 0.2f, 0.4f, 0.8f, 1.0f);
    
    drawRectangle(x, y + 0.01f, 0.04f, 0.01f, 0.2f, 0.4f, 0.8f, 1.0f);
    
    drawRectangle(x - 0.008f, y - 0.025f, 0.008f, 0.02f, 0.3f, 0.3f, 0.3f, 1.0f);
    drawRectangle(x + 0.008f, y - 0.025f, 0.008f, 0.02f, 0.3f, 0.3f, 0.3f, 1.0f);
}

