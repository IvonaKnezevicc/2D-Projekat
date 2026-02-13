#include "../Header/Renderer.h"
#include "../Header/Cinema.h"
#include "../Header/Seat.h"
#include "../Header/Person.h"
#include "../Header/Camera.h"
#include "../Header/Util.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdio>
#include <limits>
#include <fstream>

Renderer::Renderer(int windowWidth, int windowHeight)
    : windowWidth(windowWidth), windowHeight(windowHeight), studentInfoTexture(0), seatTextureUpper(0), seatTextureBase(0)
{
    setupShader();
    setupBuffers();
    
    studentInfoTexture = loadImageToTexture("Resources/student_info.png");
    seatTextureUpper = loadImageToTexture("Resources/seat1.jpeg");
    seatTextureBase = loadImageToTexture("Resources/seat2.png");
    std::cout << "Seat teksture ucitane (upper/base): "
              << (seatTextureUpper != 0 ? "DA" : "NE")
              << "/"
              << (seatTextureBase != 0 ? "DA" : "NE")
              << std::endl;
    
    for (int i = 1; i <= 20; i++) {
        char path[256];
        unsigned int texture = 0;
        auto tryLoadIfExists = [&](const char* candidatePath) {
            if (texture != 0) return;
            std::ifstream f(candidatePath, std::ios::binary);
            if (!f.good()) return;
            f.close();
            texture = loadImageToTexture(candidatePath);
        };

        sprintf_s(path, "Resources/film_frame_%02d.jpg", i);
        tryLoadIfExists(path);
        sprintf_s(path, "Resources/film_frame_%02d.jpeg", i);
        tryLoadIfExists(path);
        sprintf_s(path, "Resources/film_frame_%02d.png", i);
        tryLoadIfExists(path);
        sprintf_s(path, "Resources/film_frame_%d.jpg", i);
        tryLoadIfExists(path);
        sprintf_s(path, "Resources/film_frame_%d.jpeg", i);
        tryLoadIfExists(path);
        sprintf_s(path, "Resources/film_frame_%d.png", i);
        tryLoadIfExists(path);

        if (texture != 0) {
            filmTextures.push_back(texture);
        }
    }
    std::cout << "Ucitano film tekstura: " << filmTextures.size() << "/20" << std::endl;
    
    for (int i = 1; i <= 15; i++) {
        char objPath[256];
        sprintf_s(objPath, "Resources/models/human_%02d.obj", i);
        
        ModelData modelData;
        if (loadOBJModel(objPath, modelData)) {
            Model model;
            
            if (!modelData.vertices.empty() && !modelData.indices.empty()) {
                glGenVertexArrays(1, &model.VAO);
                glGenBuffers(1, &model.VBO);
                glGenBuffers(1, &model.EBO);
                
                glBindVertexArray(model.VAO);
                
                glBindBuffer(GL_ARRAY_BUFFER, model.VBO);
                glBufferData(GL_ARRAY_BUFFER, modelData.vertices.size() * sizeof(float), 
                            modelData.vertices.data(), GL_STATIC_DRAW);
                
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelData.indices.size() * sizeof(unsigned int),
                            modelData.indices.data(), GL_STATIC_DRAW);
                
                int stride = 8 * sizeof(float);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
                glEnableVertexAttribArray(0);
                
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);
                
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
                glEnableVertexAttribArray(2);
                
                glBindVertexArray(0);
                
                model.vertices = modelData.vertices;
                model.indices = modelData.indices;

                float minX = std::numeric_limits<float>::max();
                float minY = std::numeric_limits<float>::max();
                float minZ = std::numeric_limits<float>::max();
                float maxX = std::numeric_limits<float>::lowest();
                float maxY = std::numeric_limits<float>::lowest();
                float maxZ = std::numeric_limits<float>::lowest();
                for (size_t v = 0; v + 2 < modelData.vertices.size(); v += 8) {
                    float x = modelData.vertices[v + 0];
                    float y = modelData.vertices[v + 1];
                    float z = modelData.vertices[v + 2];
                    if (x < minX) minX = x;
                    if (y < minY) minY = y;
                    if (z < minZ) minZ = z;
                    if (x > maxX) maxX = x;
                    if (y > maxY) maxY = y;
                    if (z > maxZ) maxZ = z;
                }
                float height = maxY - minY;
                if (height > 0.0001f) {
                    model.normalizeScale = 1.0f / height;
                } else {
                    model.normalizeScale = 1.0f;
                }
                model.localOffset = glm::vec3(-(minX + maxX) * 0.5f, -minY, -(minZ + maxZ) * 0.5f);
                
                if (!modelData.texturePath.empty()) {
                    model.texture = loadImageToTexture(modelData.texturePath.c_str());
                    model.hasTexture = (model.texture != 0);
                }
                
                humanoidModels.push_back(model);
                std::cout << "Ucitan humanoidni model " << i << std::endl;
            }
        } else {
            sprintf_s(objPath, "Resources/models/human_%d.obj", i);
            ModelData modelData2;
            if (loadOBJModel(objPath, modelData2)) {
                Model model;
                
                if (!modelData2.vertices.empty() && !modelData2.indices.empty()) {
                    glGenVertexArrays(1, &model.VAO);
                    glGenBuffers(1, &model.VBO);
                    glGenBuffers(1, &model.EBO);
                    
                    glBindVertexArray(model.VAO);
                    
                    glBindBuffer(GL_ARRAY_BUFFER, model.VBO);
                    glBufferData(GL_ARRAY_BUFFER, modelData2.vertices.size() * sizeof(float), 
                                modelData2.vertices.data(), GL_STATIC_DRAW);
                    
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.EBO);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelData2.indices.size() * sizeof(unsigned int),
                                modelData2.indices.data(), GL_STATIC_DRAW);
                    
                    int stride = 8 * sizeof(float);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
                    glEnableVertexAttribArray(0);
                    
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
                    glEnableVertexAttribArray(1);
                    
                    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
                    glEnableVertexAttribArray(2);
                    
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                    
                    model.vertices = modelData2.vertices;
                    model.indices = modelData2.indices;

                    float minX = std::numeric_limits<float>::max();
                    float minY = std::numeric_limits<float>::max();
                    float minZ = std::numeric_limits<float>::max();
                    float maxX = std::numeric_limits<float>::lowest();
                    float maxY = std::numeric_limits<float>::lowest();
                    float maxZ = std::numeric_limits<float>::lowest();
                    for (size_t v = 0; v + 2 < modelData2.vertices.size(); v += 8) {
                        float x = modelData2.vertices[v + 0];
                        float y = modelData2.vertices[v + 1];
                        float z = modelData2.vertices[v + 2];
                        if (x < minX) minX = x;
                        if (y < minY) minY = y;
                        if (z < minZ) minZ = z;
                        if (x > maxX) maxX = x;
                        if (y > maxY) maxY = y;
                        if (z > maxZ) maxZ = z;
                    }
                    float height = maxY - minY;
                    if (height > 0.0001f) {
                        model.normalizeScale = 1.0f / height;
                    } else {
                        model.normalizeScale = 1.0f;
                    }
                    model.localOffset = glm::vec3(-(minX + maxX) * 0.5f, -minY, -(minZ + maxZ) * 0.5f);
                    
                    if (!modelData2.texturePath.empty()) {
                        model.texture = loadImageToTexture(modelData2.texturePath.c_str());
                        model.hasTexture = (model.texture != 0);
                    }
                    
                    humanoidModels.push_back(model);
                    std::cout << "Ucitan humanoidni model " << i << std::endl;
                }
            }
        }
    }

    if (humanoidModels.empty()) {
        std::cout << "Upozorenje: Nisu ucitani humanoidni modeli. Koristice se kocke umesto modela." << std::endl;
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
    if (seatTextureUpper != 0) {
        glDeleteTextures(1, &seatTextureUpper);
    }
    if (seatTextureBase != 0) {
        glDeleteTextures(1, &seatTextureBase);
    }
    
    for (auto& model : humanoidModels) {
        if (model.VAO != 0) {
            glDeleteVertexArrays(1, &model.VAO);
            glDeleteBuffers(1, &model.VBO);
            glDeleteBuffers(1, &model.EBO);
        }
        if (model.texture != 0) {
            glDeleteTextures(1, &model.texture);
        }
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
        4,  7,  6,   6,  5,  4,
        8,  9,  10,  10, 11, 8,
        12, 15, 14,  14, 13, 12,
        16, 17, 18,  18, 19, 16,
        20, 22, 21,  22, 20, 23
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
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;
    if (windowWidth <= 0 || windowHeight <= 0) {
        return;
    }

    glViewport(0, 0, windowWidth, windowHeight);
    glUseProgram(shaderProgram);
    
    float aspect = (float)windowWidth / (float)windowHeight;
    glm::mat4 projection = glm::perspective(glm::radians(camera.getFOV()), aspect, 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();

    auto setMat4 = [&](const char* name, const glm::mat4& value) {
        GLint loc = glGetUniformLocation(shaderProgram, name);
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    };
    auto setVec3 = [&](const char* name, const glm::vec3& value) {
        GLint loc = glGetUniformLocation(shaderProgram, name);
        glUniform3fv(loc, 1, glm::value_ptr(value));
    };
    auto setFloat = [&](const char* name, float value) {
        GLint loc = glGetUniformLocation(shaderProgram, name);
        glUniform1f(loc, value);
    };
    auto setBool = [&](const char* name, bool value) {
        GLint loc = glGetUniformLocation(shaderProgram, name);
        glUniform1i(loc, value ? 1 : 0);
    };

    setMat4("uProjection", projection);
    setMat4("uView", view);
    
    CinemaState state = cinema.getState();
    bool isProjection = cinema.isFilmPlaying();
    bool isHallClosed = (state == CinemaState::RESERVATION || state == CinemaState::RESETTING);
    bool hallLightEnabled = !isHallClosed && !isProjection;
    bool screenLightEnabled = isProjection;
    bool useSceneLighting = hallLightEnabled || screenLightEnabled;

    glm::vec3 hallLightPos(
        0.0f,
        cinema.getHallMaxY() - 1.35f,
        (cinema.getHallMinZ() + cinema.getHallMaxZ()) * 0.5f
    );
    glm::vec3 hallLightColor(1.0f, 0.95f, 0.86f);
    glm::vec3 screenLightPos(
        cinema.getScreenX(),
        cinema.getScreenY(),
        cinema.getScreenZ() - 0.25f
    );
    glm::vec3 screenLightColor(0.42f, 0.42f, 0.40f);
    float ambientStrength = 0.34f;
    float specularStrength = 0.16f;
    float shininess = 18.0f;

    if (isProjection) {
        ambientStrength = 0.02f;
        specularStrength = 0.02f;
        shininess = 10.0f;
    }

    setBool("uUseLighting", useSceneLighting);
    setBool("uHallLightEnabled", hallLightEnabled);
    setBool("uScreenLightEnabled", screenLightEnabled);
    setVec3("uHallLightPos", hallLightPos);
    setVec3("uHallLightColor", hallLightColor);
    setVec3("uScreenLightPos", screenLightPos);
    setVec3("uScreenLightColor", screenLightColor);
    setVec3("uViewPos", camera.getPosition());
    setFloat("uAmbientStrength", ambientStrength);
    setFloat("uSpecularStrength", specularStrength);
    setFloat("uShininess", shininess);
    
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
        renderSeat(seat, view, projection, isProjection);
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

void Renderer::renderSeat(const Seat& seat, const glm::mat4& view, const glm::mat4& projection, bool dimForProjection) {
    float r, g, b;
    
    switch (seat.status) {
        case SeatStatus::AVAILABLE:
            r = 0.10f; g = 0.20f; b = 0.42f;
            break;
        case SeatStatus::RESERVED:
            r = 0.40f; g = 0.36f; b = 0.10f;
            break;
        case SeatStatus::BOUGHT:
            r = 0.40f; g = 0.11f; b = 0.11f;
            break;
    }
    const bool useSolidStatusColor = (seat.status == SeatStatus::RESERVED || seat.status == SeatStatus::BOUGHT);
    const glm::vec3 statusColorBase = (seat.status == SeatStatus::RESERVED)
        ? glm::vec3(0.93f, 0.82f, 0.26f)
        : glm::vec3(0.88f, 0.22f, 0.22f);
    const glm::vec3 statusColorUpper = (seat.status == SeatStatus::RESERVED)
        ? glm::vec3(0.78f, 0.62f, 0.14f)
        : glm::vec3(0.70f, 0.10f, 0.10f);
    const float seatDim = dimForProjection ? 0.72f : 1.0f;
    
    const float seatWidth = 0.6f;
    const float seatDepth = 0.6f;
    const float seatHeight = 0.4f;
    const float backHeight = 0.8f;
    const float armrestWidth = 0.08f;
    const float armrestHeight = 0.28f;
    const float armrestDepth = 0.52f;
    
    glm::vec3 basePos(seat.x, seat.y, seat.z);
    glm::vec3 baseSize(seatWidth, seatHeight, seatDepth);
    if (useSolidStatusColor) {
        drawCube(basePos, baseSize, statusColorBase * seatDim, view, projection);
    } else if (seatTextureBase != 0) {
        drawCubeWithTexture(basePos, baseSize, seatTextureBase, glm::vec3(seatDim), view, projection);
    } else {
        drawCube(basePos, baseSize, glm::vec3(r, g, b) * seatDim, view, projection);
    }
    
    glm::vec3 backPos(seat.x, seat.y + seatHeight * 0.5f + backHeight * 0.5f, seat.z - seatDepth * 0.45f);
    glm::vec3 backSize(seatWidth * 0.9f, backHeight, 0.1f);
    if (useSolidStatusColor) {
        drawCube(backPos, backSize, statusColorUpper * seatDim, view, projection);
    } else if (seatTextureUpper != 0) {
        drawCubeWithTexture(backPos, backSize, seatTextureUpper, glm::vec3(seatDim), view, projection);
    } else {
        drawCube(backPos, backSize, glm::vec3(r * 0.7f, g * 0.7f, b * 0.7f) * seatDim, view, projection);
    }

    glm::vec3 leftArmPos(
        seat.x - seatWidth * 0.5f - armrestWidth * 0.5f,
        seat.y + armrestHeight * 0.5f,
        seat.z
    );
    glm::vec3 rightArmPos(
        seat.x + seatWidth * 0.5f + armrestWidth * 0.5f,
        seat.y + armrestHeight * 0.5f,
        seat.z
    );
    glm::vec3 armSize(armrestWidth, armrestHeight, armrestDepth);
    glm::vec3 armColor(r * 0.6f, g * 0.6f, b * 0.6f);
    if (useSolidStatusColor) {
        drawCube(leftArmPos, armSize, statusColorUpper * seatDim, view, projection);
        drawCube(rightArmPos, armSize, statusColorUpper * seatDim, view, projection);
    } else if (seatTextureUpper != 0) {
        drawCubeWithTexture(leftArmPos, armSize, seatTextureUpper, glm::vec3(seatDim), view, projection);
        drawCubeWithTexture(rightArmPos, armSize, seatTextureUpper, glm::vec3(seatDim), view, projection);
    } else {
        drawCube(leftArmPos, armSize, armColor * seatDim, view, projection);
        drawCube(rightArmPos, armSize, armColor * seatDim, view, projection);
    }
}

void Renderer::renderPerson(const Person& person, const glm::mat4& view, const glm::mat4& projection) {
    if (!humanoidModels.empty()) {
        int safeIndex = person.modelIndex % (int)humanoidModels.size();
        if (safeIndex < 0) safeIndex += (int)humanoidModels.size();

        float yawDegrees = 0.0f;
        if (person.isSeated) {
            yawDegrees = 0.0f;
        } else {
            float targetX = person.reachedIntermediate ? person.targetX : person.intermediateX;
            float targetZ = person.reachedIntermediate ? person.targetZ : person.intermediateZ;
            float dirX = targetX - person.x;
            float dirZ = targetZ - person.z;
            float dirLenSq = dirX * dirX + dirZ * dirZ;

            if (dirLenSq > 0.0001f) {
                yawDegrees = glm::degrees(std::atan2(dirX, dirZ));
            } else {
                yawDegrees = person.isExiting ? 0.0f : 180.0f;
            }
        }

        const Model& selectedModel = humanoidModels[safeIndex];
        const float targetHeight = 1.0f;
        float uniformScale = targetHeight * selectedModel.normalizeScale;

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(person.x, person.y, person.z));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(yawDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(uniformScale, uniformScale, uniformScale));
        modelMatrix = glm::translate(modelMatrix, selectedModel.localOffset);
        renderModel(selectedModel, modelMatrix, view, projection);
    } else {
        glm::vec3 personPos(person.x, person.y + 0.5f, person.z);
        glm::vec3 personSize(0.5f, 1.0f, 0.5f);
        drawCube(personPos, personSize, glm::vec3(0.85f, 0.55f, 0.35f), view, projection);
    }
}

void Renderer::renderModel(const Model& model, const glm::mat4& modelMatrix, const glm::mat4& view, const glm::mat4& projection) {
    if (model.VAO == 0 || model.indices.empty()) return;
    
    glUseProgram(shaderProgram);
    
    GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    
    GLint viewLoc = glGetUniformLocation(shaderProgram, "uView");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    
    GLint projLoc = glGetUniformLocation(shaderProgram, "uProjection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    GLint useTexLoc = glGetUniformLocation(shaderProgram, "uUseTexture");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
    if (model.hasTexture && model.texture != 0) {
        glUniform1i(useTexLoc, 1);
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, model.texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else {
        glUniform1i(useTexLoc, 0);
        glUniform4f(colorLoc, 0.75f, 0.75f, 0.80f, 1.0f);
    }
    
    GLint alphaLoc = glGetUniformLocation(shaderProgram, "uAlpha");
    glUniform1f(alphaLoc, 1.0f);
    
    glDepthMask(GL_TRUE);
    GLboolean wasCullEnabled = glIsEnabled(GL_CULL_FACE);
    if (wasCullEnabled) {
        glDisable(GL_CULL_FACE);
    }
    
    GLboolean wasBlendEnabled = glIsEnabled(GL_BLEND);
    glDisable(GL_BLEND);
    
    glBindVertexArray(model.VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)model.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    if (wasBlendEnabled) {
        glEnable(GL_BLEND);
    }
    if (wasCullEnabled) {
        glEnable(GL_CULL_FACE);
    }

    if (model.hasTexture && model.texture != 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
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

    float doorWidth = 1.5f;
    float doorHeight = 2.5f;
    float doorDepth = 0.06f;
    
    float doorZ = z - 0.08f;

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
    glUniform4f(colorLoc, 0.10f, 0.28f, 0.12f, 1.0f);

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
    if (windowWidth <= 0 || windowHeight <= 0) {
        return;
    }

    float aspect = (float)windowWidth / (float)windowHeight;
    float overlayWidth = 2.0f * aspect;
    float overlayHeight = 2.0f;
    drawRectangle(0.0f, 0.0f, overlayWidth, overlayHeight, 0.2f, 0.2f, 0.2f, 0.5f);
}

void Renderer::renderStudentInfo() {
    if (windowWidth <= 0 || windowHeight <= 0) {
        return;
    }

    glUseProgram(shaderProgram);

    GLboolean wasDepthEnabled = glIsEnabled(GL_DEPTH_TEST);

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

    if (wasDepthEnabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
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
    if (windowWidth <= 0 || windowHeight <= 0) {
        return;
    }

    GLboolean wasDepthEnabled = glIsEnabled(GL_DEPTH_TEST);

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
    if (wasDepthEnabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
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

void Renderer::drawCubeWithTexture(glm::vec3 position, glm::vec3 size, unsigned int texture, const glm::vec3& tint,
                                   const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, size);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
    glUniform4f(colorLoc, tint.r, tint.g, tint.b, 1.0f);

    GLint useTexLoc = glGetUniformLocation(shaderProgram, "uUseTexture");
    glUniform1i(useTexLoc, 1);

    GLint alphaLoc = glGetUniformLocation(shaderProgram, "uAlpha");
    glUniform1f(alphaLoc, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLboolean wasBlendEnabled = glIsEnabled(GL_BLEND);
    glDisable(GL_BLEND);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (wasBlendEnabled) {
        glEnable(GL_BLEND);
    }
}

void Renderer::renderHall(const Cinema& cinema, const glm::mat4& view, const glm::mat4& projection) {
    const glm::vec3 wallColor(0.34f, 0.34f, 0.34f);
    const glm::vec3 ceilingColor(0.02f, 0.02f, 0.02f);

    float seatStartZ = cinema.getHallMaxZ() - 2.0f;
    float seatEndZ = seatStartZ - 9.0f * 1.0f;
    
    float seatHeight = 0.4f;
    float stepHeight = 0.22f;
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
    drawCube(ceilingPos, ceilingSize, ceilingColor, view, projection);

    {
        float hallWidth = cinema.getHallMaxX() - cinema.getHallMinX();
        float hallDepth = cinema.getHallMaxZ() - cinema.getHallMinZ();
        float yGrid = cinema.getHallMaxY() - 0.045f;
        float lineThickness = 0.02f;
        int xDiv = 8;
        int zDiv = 8;
        glm::vec3 gridColor(0.16f, 0.16f, 0.16f);

        for (int i = 1; i < xDiv; i++) {
            float x = cinema.getHallMinX() + (hallWidth * i) / (float)xDiv;
            drawCube(
                glm::vec3(x, yGrid, (cinema.getHallMinZ() + cinema.getHallMaxZ()) * 0.5f),
                glm::vec3(lineThickness, 0.012f, hallDepth),
                gridColor,
                view,
                projection
            );
        }

        for (int i = 1; i < zDiv; i++) {
            float z = cinema.getHallMinZ() + (hallDepth * i) / (float)zDiv;
            drawCube(
                glm::vec3((cinema.getHallMinX() + cinema.getHallMaxX()) * 0.5f, yGrid, z),
                glm::vec3(hallWidth, 0.012f, lineThickness),
                gridColor,
                view,
                projection
            );
        }
    }

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
    drawCube(leftWallPos, leftWallSize, wallColor, view, projection);

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
    drawCube(rightWallPos, rightWallSize, wallColor, view, projection);

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
        drawCube(backWallTopPos, backWallTopSize, wallColor, view, projection);
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
        drawCube(backWallBottomPos, backWallBottomSize, wallColor, view, projection);
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
        drawCube(backWallLeftPos, backWallLeftSize, wallColor, view, projection);
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
        drawCube(backWallRightPos, backWallRightSize, wallColor, view, projection);
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
    drawCube(frontWallPos, frontWallSize, wallColor, view, projection);
    
    renderStairs(cinema, view, projection);
}

void Renderer::renderStairs(const Cinema& cinema, const glm::mat4& view, const glm::mat4& projection) {
    int numRows = 10;
    float seatHeight = 0.4f;
    float stepHeight = 0.22f;
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
