#include "../Header/Application.h"
#include "../Header/Cinema.h"
#include "../Header/Renderer.h"
#include "../Header/Camera.h"
#include "../Header/Util.h"
#include <iostream>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Application::Application()
    : window(nullptr), cursor(nullptr), depthTestEnabled(true), faceCullingEnabled(false),
      firstMouse(true), lastX(0.0f), lastY(0.0f)
{
}

Application::~Application() {
    if (cursor) {
        glfwDestroyCursor(cursor);
    }
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

bool Application::initialize() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    
    int windowWidth = mode->width;
    int windowHeight = mode->height;
    
    window = glfwCreateWindow(windowWidth, windowHeight, "Bioskop - Upravljanje sedistima", monitor, NULL);
    if (window == NULL) {
        std::cout << "Prozor nije uspeo da se kreira." << std::endl;
        return false;
    }
    
    glfwMakeContextCurrent(window);
    
    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW nije uspeo da se inicijalizuje." << std::endl;
        return false;
    }
    
    cursor = createCameraCursor();
    if (cursor) {
        glfwSetCursor(window, cursor);
    }
    
    setupOpenGL();
    setupCallbacks();
    
    cinema = std::make_unique<Cinema>(windowWidth, windowHeight);
    renderer = std::make_unique<Renderer>(windowWidth, windowHeight);
    
    glm::vec3 startCameraPos(
        cinema->getHallMaxX() - 1.0f,
        cinema->getHallMaxY() - 0.8f,
        cinema->getHallMaxZ() - 1.0f
    );
    camera = std::make_unique<Camera>(
        startCameraPos,
        glm::vec3(0.0f, 1.0f, 0.0f),
        145.0f,
        -25.0f
    );
    
    camera->setBounds(
        glm::vec3(cinema->getHallMinX() + 0.5f, cinema->getHallMinY() + 0.5f, cinema->getHallMinZ() + 0.5f),
        glm::vec3(cinema->getHallMaxX() - 0.5f, cinema->getHallMaxY() - 0.5f, cinema->getHallMaxZ() - 0.5f)
    );
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    std::cout << "Bioskop - Upravljanje sedistima (3D)" << std::endl;
    std::cout << "Kontrole:" << std::endl;
    std::cout << "  - Strelice: Kretanje kamere" << std::endl;
    std::cout << "  - Mis: Rotacija kamere" << std::endl;
    std::cout << "  - Levi klik: Rezervacija/otkazivanje sedista" << std::endl;
    std::cout << "  - Tasteri 1-9: Kupovina karata" << std::endl;
    std::cout << "  - Enter: Pocetak projekcije" << std::endl;
    std::cout << "  - Taster D: Ukljuci/iskljuci depth testing" << std::endl;
    std::cout << "  - Taster C: Ukljuci/iskljuci face culling" << std::endl;
    std::cout << "  - Escape: Izlaz" << std::endl;
    
    return true;
}

void Application::setupOpenGL() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Application::setupCallbacks() {
    glfwSetWindowUserPointer(window, this);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);
}

Application* Application::getInstance(GLFWwindow* window) {
    return static_cast<Application*>(glfwGetWindowUserPointer(window));
}

void Application::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Application* app = getInstance(window);
    if (!app || !app->cinema || !app->camera || !app->renderer) return;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        glm::mat4 view = app->camera->getViewMatrix();
        float aspect = (float)app->renderer->getWindowWidth() / (float)app->renderer->getWindowHeight();
        glm::mat4 projection = glm::perspective(glm::radians(app->camera->getFOV()), aspect, 0.1f, 100.0f);
        
        app->cinema->handleMouseClick(xpos, ypos, view, projection, app->camera->getPosition());
    }
}

void Application::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    Application* app = getInstance(window);
    if (!app || !app->camera) return;
    
    if (app->firstMouse) {
        app->lastX = xpos;
        app->lastY = ypos;
        app->firstMouse = false;
    }
    
    float xoffset = xpos - app->lastX;
    float yoffset = app->lastY - ypos;
    
    app->lastX = xpos;
    app->lastY = ypos;
    
    app->camera->processMouseMovement(xoffset, yoffset);
}

void Application::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Application* app = getInstance(window);
    if (!app || !app->camera) return;
    
    app->camera->processMouseScroll(yoffset);
}

void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application* app = getInstance(window);
    if (!app) return;
    
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        } else if (key == GLFW_KEY_D) {
            app->depthTestEnabled = !app->depthTestEnabled;
            if (app->depthTestEnabled) {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
                std::cout << "Depth testing: ON" << std::endl;
            } else {
                glDisable(GL_DEPTH_TEST);
                std::cout << "Depth testing: OFF" << std::endl;
            }
        } else if (key == GLFW_KEY_C) {
            app->faceCullingEnabled = !app->faceCullingEnabled;
            if (app->faceCullingEnabled) {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                glFrontFace(GL_CCW);
                std::cout << "Face culling: ON" << std::endl;
            } else {
                glDisable(GL_CULL_FACE);
                std::cout << "Face culling: OFF" << std::endl;
            }
        } else if (app->cinema) {
            app->cinema->handleKeyPress(key);
        }
    }
}

void Application::run() {
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        if (camera) {
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                camera->processKeyboard(0, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                camera->processKeyboard(1, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                camera->processKeyboard(2, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                camera->processKeyboard(3, deltaTime);
        }
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(currentTime - lastTime).count();
        
        if (elapsed >= FRAME_TIME) {
            lastTime = currentTime;
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            cinema->update();
            
            glfwGetWindowSize(window, &windowWidth, &windowHeight);
            renderer->render(*cinema, *camera, windowWidth, windowHeight);
            
            glfwSwapBuffers(window);
        }
        
        glfwPollEvents();
    }
}
