#include "../Header/Renderer.h"
#include "../Header/Cinema.h"
#include "../Header/Util.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdio>

Renderer::Renderer(int windowWidth, int windowHeight)
    : windowWidth(windowWidth), windowHeight(windowHeight), studentInfoTexture(0)
{
    setupShader();
    setupBuffers();
    
    studentInfoTexture = loadImageToTexture("Resources/student_info.png");
    
    for (int i = 1; i <= 20; i++) {
        char path[256];
        sprintf_s(path, "Resources/film_frame_%02d.jpg", i);
        unsigned int texture = loadImageToTexture(path);
        if (texture == 0) {
            sprintf_s(path, "Resources/film_frame_%02d.png", i);
            texture = loadImageToTexture(path);
        }
        if (texture == 0) {
            sprintf_s(path, "Resources/film_frame_%d.jpg", i);
            texture = loadImageToTexture(path);
        }
        if (texture == 0) {
            sprintf_s(path, "Resources/film_frame_%d.png", i);
            texture = loadImageToTexture(path);
        }
        if (texture != 0) {
            filmTextures.push_back(texture);
        }
    }
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
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f
    };
    
    unsigned int indices[] = {
        0,  1,  2,   2,  3,  0,
        4,  5,  6,   6,  7,  4,
        8,  9,  10,  10, 11, 8,
        12, 13, 14,  14, 15, 12,
        16, 17, 18,  18, 19, 16,
        20, 21, 22,  22, 23, 20
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void Renderer::render(const Cinema& cinema, const Camera& camera, int windowWidth, int windowHeight) {
    glUseProgram(shaderProgram);
    
    float aspect = (float)windowWidth / (float)windowHeight;
    glm::mat4 projection = glm::perspective(glm::radians(camera.getFOV()), aspect, 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();
    
    GLint projLoc = glGetUniformLocation(shaderProgram, "uProjection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    GLint viewLoc = glGetUniformLocation(shaderProgram, "uView");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    
    GLint useLightingLoc = glGetUniformLocation(shaderProgram, "uUseLighting");
    glUniform1i(useLightingLoc, false);
    
    renderHall(cinema, view, projection);
    
    if (cinema.isFilmPlaying() && !filmTextures.empty()) {
        int textureIndex = cinema.getCurrentFilmTextureIndex() % filmTextures.size();
        renderScreenWithTexture(cinema.getScreenX(), cinema.getScreenY(), cinema.getScreenZ(),
                                cinema.getScreenWidth(), cinema.getScreenHeight(), cinema.getScreenDepth(),
                                filmTextures[textureIndex], view, projection);
    } else {
        renderScreen(cinema.getScreenX(), cinema.getScreenY(), cinema.getScreenZ(),
                     cinema.getScreenWidth(), cinema.getScreenHeight(), cinema.getScreenDepth(),
                     cinema.getScreenColorR(), cinema.getScreenColorG(), cinema.getScreenColorB(),
                     view, projection);
    }
    
    for (const auto& seat : cinema.getSeats()) {
        renderSeat(seat, view, projection);
    }
    
    for (const auto& person : cinema.getPeople()) {
        renderPerson(person, view, projection);
    }
    
    renderDoor(cinema.getDoorX(), cinema.getDoorY(), cinema.getDoorZ(), 
               cinema.isDoorOpen(), cinema.getDoorAngle(), view, projection);
    
    if (cinema.isDoorOpen()) {
        renderPortal(cinema.getPortalX(), cinema.getPortalY(), cinema.getPortalZ(), view, projection);
    }
    
    if (cinema.showOverlay()) {
        renderOverlay();
    }
    
    renderStudentInfo();
    renderCrosshair();
}

void Renderer::renderSeat(const Seat& seat, const glm::mat4& view, const glm::mat4& projection) {
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
    
    const float seatWidth = 0.6f;
    const float seatDepth = 0.6f;
    const float seatHeight = 0.4f;
    const float backHeight = 0.8f;
    
    glm::vec3 basePos(seat.x, seat.y, seat.z);
    glm::vec3 baseSize(seatWidth, seatHeight, seatDepth);
    drawCube(basePos, baseSize, glm::vec3(r, g, b), view, projection);
    
    glm::vec3 backPos(seat.x, seat.y + seatHeight * 0.5f + backHeight * 0.5f, seat.z - seatDepth * 0.45f);
    glm::vec3 backSize(seatWidth * 0.9f, backHeight, 0.1f);
    drawCube(backPos, backSize, glm::vec3(r * 0.7f, g * 0.7f, b * 0.7f), view, projection);
}

void Renderer::renderPerson(const Person& person, const glm::mat4& view, const glm::mat4& projection) {
    glm::vec3 personPos(person.x, person.y + 0.5f, person.z);
    glm::vec3 personSize(0.5f, 1.0f, 0.5f);
    drawCube(personPos, personSize, glm::vec3(0.85f, 0.55f, 0.35f), view, projection);
}

void Renderer::renderScreen(float x, float y, float z, float width, float height, float depth, float r, float g, float b,
                            const glm::mat4& view, const glm::mat4& projection) {
    glm::vec3 screenPos(x, y, z);
    glm::vec3 screenSize(width, height, depth);
    drawCube(screenPos, screenSize, glm::vec3(r, g, b), view, projection);
}

void Renderer::renderScreenWithTexture(float x, float y, float z, float width, float height, float depth, unsigned int texture,
                                       const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, z));
    model = glm::scale(model, glm::vec3(width, height, depth));
    
    GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
    GLint useLightingLoc = glGetUniformLocation(shaderProgram, "uUseLighting");
    glUniform1i(useLightingLoc, 0);
    
    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
    glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
    
    GLint useTexLoc = glGetUniformLocation(shaderProgram, "uUseTexture");
    glUniform1i(useTexLoc, 1);
    
    GLint alphaLoc = glGetUniformLocation(shaderProgram, "uAlpha");
    glUniform1f(alphaLoc, 1.0f);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::renderDoor(float x, float y, float z, bool isOpen, float angle, const glm::mat4& view, const glm::mat4& projection) {
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    glUseProgram(shaderProgram);
    GLint useLightingLoc = glGetUniformLocation(shaderProgram, "uUseLighting");
    glUniform1i(useLightingLoc, 0);

    float doorWidth = 1.5f;
    float doorHeight = 2.5f;
    float doorDepth = 0.1f;
    
    float doorZ = z - 0.01f;

    glm::vec3 framePos(x, y, doorZ);
    glm::vec3 frameSize(0.2f, doorHeight + 0.2f, doorDepth);
    drawCube(framePos, frameSize, glm::vec3(0.3f, 0.3f, 0.3f), view, projection);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, doorZ));
    if (isOpen) {
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    model = glm::translate(model, glm::vec3(doorWidth / 2.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(doorWidth, doorHeight, doorDepth));

    GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
    glUniform4f(colorLoc, 0.0f, 1.0f, 0.0f, 1.0f);

    GLint useTexLoc = glGetUniformLocation(shaderProgram, "uUseTexture");
    glUniform1i(useTexLoc, 0);

    GLint alphaLoc = glGetUniformLocation(shaderProgram, "uAlpha");
    glUniform1f(alphaLoc, 1.0f);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glEnable(GL_BLEND);
}

void Renderer::renderPortal(float x, float y, float z, const glm::mat4& view, const glm::mat4& projection) {
    float portalWidth = 1.5f;
    float portalHeight = 2.5f;
    float portalDepth = 0.1f;
    
    glm::vec3 portalPos(x, y, z);
    glm::vec3 portalSize(portalWidth, portalHeight, portalDepth);
    drawCube(portalPos, portalSize, glm::vec3(0.0f, 0.0f, 0.0f), view, projection);
}

void Renderer::renderOverlay() {
    float aspect = (float)windowWidth / (float)windowHeight;
    float overlayWidth = 2.0f * aspect;
    float overlayHeight = 2.0f;
    drawRectangle(0.0f, 0.0f, overlayWidth, overlayHeight, 0.2f, 0.2f, 0.2f, 0.5f);
}

void Renderer::renderStudentInfo() {
    glUseProgram(shaderProgram);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float aspect = (float)windowWidth / (float)windowHeight;
    glm::mat4 ortho = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);

    GLint projLoc = glGetUniformLocation(shaderProgram, "uProjection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(ortho));

    GLint viewLoc = glGetUniformLocation(shaderProgram, "uView");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    GLint useLightingLoc = glGetUniformLocation(shaderProgram, "uUseLighting");
    glUniform1i(useLightingLoc, false);

    const float marginX = 0.06f;
    const float marginY = 0.06f;
    const float infoW = 0.75f;
    const float infoH = 0.22f;

    const float cx = (-aspect + marginX) + infoW * 0.5f;
    const float cy = (-1.0f + marginY) + infoH * 0.5f;

    drawRectangle(cx, cy, infoW, infoH, 0.05f, 0.05f, 0.05f, 0.35f);
    if (studentInfoTexture != 0) {
        drawRectangleWithTexture(cx, cy, infoW, infoH, studentInfoTexture, 0.85f);
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
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

void Renderer::renderCrosshair() {
    glDisable(GL_DEPTH_TEST);
    glUseProgram(shaderProgram);
    
    float aspect = (float)windowWidth / (float)windowHeight;
    glm::mat4 ortho = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    
    GLint projLoc = glGetUniformLocation(shaderProgram, "uProjection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(ortho));
    
    GLint viewLoc = glGetUniformLocation(shaderProgram, "uView");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    
    GLint useLightingLoc = glGetUniformLocation(shaderProgram, "uUseLighting");
    glUniform1i(useLightingLoc, false);
    
    float crosshairSize = 0.02f;
    float crosshairThickness = 0.003f;
    
    glm::mat4 model = glm::mat4(1.0f);
    GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
    GLint useTexLoc = glGetUniformLocation(shaderProgram, "uUseTexture");
    GLint alphaLoc = glGetUniformLocation(shaderProgram, "uAlpha");
    
    glUniform1i(useTexLoc, 0);
    glUniform1f(alphaLoc, 1.0f);
    glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 0.8f);
    
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(crosshairThickness, crosshairSize, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(crosshairSize, crosshairThickness, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::drawPerson(float x, float y) {
    drawRectangle(x, y + 0.025f, 0.02f, 0.02f, 1.0f, 0.8f, 0.6f, 1.0f);
    
    drawRectangle(x, y, 0.015f, 0.05f, 0.2f, 0.4f, 0.8f, 1.0f);
    
    drawRectangle(x, y + 0.01f, 0.04f, 0.01f, 0.2f, 0.4f, 0.8f, 1.0f);
    
    drawRectangle(x - 0.008f, y - 0.025f, 0.008f, 0.02f, 0.3f, 0.3f, 0.3f, 1.0f);
    drawRectangle(x + 0.008f, y - 0.025f, 0.008f, 0.02f, 0.3f, 0.3f, 0.3f, 1.0f);
}

void Renderer::drawCube(glm::vec3 position, glm::vec3 size, glm::vec3 color, 
                        const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, size);
    
    GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
    glUniform4f(colorLoc, color.r, color.g, color.b, 1.0f);
    
    GLint useTexLoc = glGetUniformLocation(shaderProgram, "uUseTexture");
    glUniform1i(useTexLoc, 0);
    
    GLint alphaLoc = glGetUniformLocation(shaderProgram, "uAlpha");
    glUniform1f(alphaLoc, 1.0f);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::renderHall(const Cinema& cinema, const glm::mat4& view, const glm::mat4& projection) {
    float seatStartZ = cinema.getHallMaxZ() - 2.0f;
    float seatEndZ = seatStartZ - 9.0f * 1.0f;
    
    float seatHeight = 0.4f;
    float stepHeight = 0.15f;
    float startY = seatHeight / 2.0f + 0.05f;
    int numRows = 10;
    float lastStairY = startY + (numRows - 1) * stepHeight;
    
    if (seatEndZ > cinema.getHallMinZ()) {
        glm::vec3 floorFrontPos(
            0.0f,
            0.0f,
            (cinema.getHallMaxZ() + seatStartZ) / 2.0f
        );
        glm::vec3 floorFrontSize(
            cinema.getHallMaxX() - cinema.getHallMinX(),
            0.1f,
            cinema.getHallMaxZ() - seatStartZ
        );
        drawCube(floorFrontPos, floorFrontSize, glm::vec3(0.2f, 0.2f, 0.2f), view, projection);
        
        glm::vec3 floorBackPos(
            0.0f,
            lastStairY,
            (seatEndZ + cinema.getHallMinZ()) / 2.0f
        );
        glm::vec3 floorBackSize(
            cinema.getHallMaxX() - cinema.getHallMinX(),
            0.1f,
            seatEndZ - cinema.getHallMinZ()
        );
        drawCube(floorBackPos, floorBackSize, glm::vec3(0.25f, 0.25f, 0.25f), view, projection);
    } else {
        glm::vec3 floorPos(
            0.0f,
            0.0f,
            (cinema.getHallMinZ() + cinema.getHallMaxZ()) / 2.0f
        );
        glm::vec3 floorSize(
            cinema.getHallMaxX() - cinema.getHallMinX(),
            0.1f,
            cinema.getHallMaxZ() - cinema.getHallMinZ()
        );
        drawCube(floorPos, floorSize, glm::vec3(0.2f, 0.2f, 0.2f), view, projection);
    }

    glm::vec3 ceilingPos(
        0.0f,
        cinema.getHallMaxY(),
        (cinema.getHallMinZ() + cinema.getHallMaxZ()) / 2.0f
    );
    glm::vec3 ceilingSize(
        cinema.getHallMaxX() - cinema.getHallMinX(),
        0.1f,
        cinema.getHallMaxZ() - cinema.getHallMinZ()
    );
    drawCube(ceilingPos, ceilingSize, glm::vec3(0.15f, 0.15f, 0.15f), view, projection);

    glm::vec3 leftWallPos(
        cinema.getHallMinX(),
        cinema.getHallMaxY() / 2.0f,
        (cinema.getHallMinZ() + cinema.getHallMaxZ()) / 2.0f
    );
    glm::vec3 leftWallSize(
        0.1f,
        cinema.getHallMaxY(),
        cinema.getHallMaxZ() - cinema.getHallMinZ()
    );
    drawCube(leftWallPos, leftWallSize, glm::vec3(0.25f, 0.25f, 0.25f), view, projection);

    glm::vec3 rightWallPos(
        cinema.getHallMaxX(),
        cinema.getHallMaxY() / 2.0f,
        (cinema.getHallMinZ() + cinema.getHallMaxZ()) / 2.0f
    );
    glm::vec3 rightWallSize(
        0.1f,
        cinema.getHallMaxY(),
        cinema.getHallMaxZ() - cinema.getHallMinZ()
    );
    drawCube(rightWallPos, rightWallSize, glm::vec3(0.25f, 0.25f, 0.25f), view, projection);

    float wallTopHeight =
        cinema.getHallMaxY() - (cinema.getScreenY() + cinema.getScreenHeight() / 2.0f);
    if (wallTopHeight > 0.1f) {
        glm::vec3 backWallTopPos(
            0.0f,
            cinema.getScreenY() + cinema.getScreenHeight() / 2.0f + wallTopHeight / 2.0f,
            cinema.getHallMaxZ()
        );
        glm::vec3 backWallTopSize(
            cinema.getHallMaxX() - cinema.getHallMinX(),
            wallTopHeight,
            0.1f
        );
        drawCube(backWallTopPos, backWallTopSize, glm::vec3(0.25f, 0.25f, 0.25f), view, projection);
    }

    float wallBottomHeight =
        cinema.getScreenY() - cinema.getScreenHeight() / 2.0f;
    if (wallBottomHeight > 0.1f) {
        glm::vec3 backWallBottomPos(
            0.0f,
            wallBottomHeight / 2.0f,
            cinema.getHallMaxZ()
        );
        glm::vec3 backWallBottomSize(
            cinema.getHallMaxX() - cinema.getHallMinX(),
            wallBottomHeight,
            0.1f
        );
        drawCube(backWallBottomPos, backWallBottomSize, glm::vec3(0.25f, 0.25f, 0.25f), view, projection);
    }

    if (cinema.getScreenX() - cinema.getScreenWidth() / 2.0f > cinema.getHallMinX()) {
        glm::vec3 backWallLeftPos(
            (cinema.getHallMinX() + cinema.getScreenX() - cinema.getScreenWidth() / 2.0f) / 2.0f,
            cinema.getScreenY(),
            cinema.getHallMaxZ()
        );
        glm::vec3 backWallLeftSize(
            cinema.getScreenX() - cinema.getScreenWidth() / 2.0f - cinema.getHallMinX(),
            cinema.getScreenHeight(),
            0.1f
        );
        drawCube(backWallLeftPos, backWallLeftSize, glm::vec3(0.25f, 0.25f, 0.25f), view, projection);
    }

    if (cinema.getScreenX() + cinema.getScreenWidth() / 2.0f < cinema.getHallMaxX()) {
        glm::vec3 backWallRightPos(
            (cinema.getHallMaxX() + cinema.getScreenX() + cinema.getScreenWidth() / 2.0f) / 2.0f,
            cinema.getScreenY(),
            cinema.getHallMaxZ()
        );
        glm::vec3 backWallRightSize(
            cinema.getHallMaxX() - (cinema.getScreenX() + cinema.getScreenWidth() / 2.0f),
            cinema.getScreenHeight(),
            0.1f
        );
        drawCube(backWallRightPos, backWallRightSize, glm::vec3(0.25f, 0.25f, 0.25f), view, projection);
    }
    
    glm::vec3 frontWallPos(
        0.0f,
        cinema.getHallMaxY() / 2.0f,
        cinema.getHallMinZ()
    );
    glm::vec3 frontWallSize(
        cinema.getHallMaxX() - cinema.getHallMinX(),
        cinema.getHallMaxY(),
        0.1f
    );
    drawCube(frontWallPos, frontWallSize, glm::vec3(0.0f, 0.0f, 0.0f), view, projection);
    
    renderStairs(cinema, view, projection);
}

void Renderer::renderStairs(const Cinema& cinema, const glm::mat4& view, const glm::mat4& projection) {
    int numRows = 10;
    float seatHeight = 0.4f;
    float stepHeight = 0.15f;
    float startY = seatHeight / 2.0f + 0.05f;
    float startZ = cinema.getHallMaxZ() - 2.0f;
    float rowSpacing = 1.0f;
    
    float stairDepth = 1.0f;
    float stairHeight = stepHeight;
    float stairWidth = cinema.getHallMaxX() - cinema.getHallMinX();
    
    for (int row = 0; row < numRows; row++) {
        float y = startY + row * stepHeight;
        float z = startZ - row * rowSpacing;
        
        glm::vec3 stairPos(0.0f, y, z);
        glm::vec3 stairSize(stairWidth, stairHeight, stairDepth);
        drawCube(stairPos, stairSize, glm::vec3(0.3f, 0.3f, 0.3f), view, projection);
    }
}