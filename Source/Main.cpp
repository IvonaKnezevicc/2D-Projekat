#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Header/Util.h"
#include "../Header/Cinema.h"
#include "../Header/Renderer.h"
#include "../Header/Camera.h"

Cinema* g_cinema = nullptr;
Renderer* g_renderer = nullptr;
GLFWwindow* g_window = nullptr;
Camera* g_camera = nullptr;

const double TARGET_FPS = 75.0;
const double FRAME_TIME = 1.0 / TARGET_FPS;

bool depthTestEnabled = true;
bool faceCullingEnabled = false;

bool firstMouse = true;
float lastX = 0.0f;
float lastY = 0.0f;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && g_cinema && g_camera && g_renderer) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        glm::mat4 view = g_camera->getViewMatrix();
        float aspect = (float)g_renderer->getWindowWidth() / (float)g_renderer->getWindowHeight();
        glm::mat4 projection = glm::perspective(glm::radians(g_camera->getFOV()), aspect, 0.1f, 100.0f);
        
        g_cinema->handleMouseClick(xpos, ypos, view, projection, g_camera->getPosition());
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;
    
    if (g_camera) {
        g_camera->processMouseMovement(xoffset, yoffset);
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (g_camera) {
        g_camera->processMouseScroll(yoffset);
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        } else if (key == GLFW_KEY_D) {
            depthTestEnabled = !depthTestEnabled;
            if (depthTestEnabled) {
                glEnable(GL_DEPTH_TEST);
                if (faceCullingEnabled) {
                    glDisable(GL_CULL_FACE);
                    faceCullingEnabled = false;
                }
                std::cout << "Depth testing: ON (face culling automatski isključen)" << std::endl;
            } else {
                glDisable(GL_DEPTH_TEST);
                std::cout << "Depth testing: OFF" << std::endl;
            }
        } else if (key == GLFW_KEY_C) {
            if (depthTestEnabled) {
                std::cout << "Face culling ne može biti uključen dok je depth testing ON (specifikacija)" << std::endl;
            } else {
                faceCullingEnabled = !faceCullingEnabled;
                if (faceCullingEnabled) {
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_BACK);
                    glFrontFace(GL_CCW);
                    std::cout << "Face culling: ON" << std::endl;
                } else {
                    glDisable(GL_CULL_FACE);
                    std::cout << "Face culling: OFF" << std::endl;
                }
            }
        } else if (g_cinema) {
            g_cinema->handleKeyPress(key);
        }
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    
    int windowWidth = mode->width;
    int windowHeight = mode->height;
    
    g_window = glfwCreateWindow(windowWidth, windowHeight, "Bioskop - Upravljanje sedistima", monitor, NULL);
    if (g_window == NULL) {
        return endProgram("Prozor nije uspeo da se kreira.");
    }
    
    glfwMakeContextCurrent(g_window);
    
    if (glewInit() != GLEW_OK) {
        return endProgram("GLEW nije uspeo da se inicijalizuje.");
    }
    
    GLFWcursor* cursor = createCameraCursor();
    if (cursor) {
        glfwSetCursor(g_window, cursor);
    }
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    Cinema cinema(windowWidth, windowHeight);
    Renderer renderer(windowWidth, windowHeight);
    
    Camera camera(
        glm::vec3(0.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        0.0f,
        -12.0f
    );
    g_camera = &camera;
    
    g_cinema = &cinema;
    g_renderer = &renderer;
    
    camera.setBounds(glm::vec3(cinema.getHallMinX() + 0.5f, cinema.getHallMinY() + 0.5f, cinema.getHallMinZ() + 0.5f),
                     glm::vec3(cinema.getHallMaxX() - 0.5f, cinema.getHallMaxY() - 0.5f, cinema.getHallMaxZ() - 0.5f));
    
    glfwSetMouseButtonCallback(g_window, mouseButtonCallback);
    glfwSetCursorPosCallback(g_window, mouseCallback);
    glfwSetScrollCallback(g_window, scrollCallback);
    glfwSetKeyCallback(g_window, keyCallback);
    
    glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    auto lastTime = std::chrono::high_resolution_clock::now();
    
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
    
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    while (!glfwWindowShouldClose(g_window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        if (g_camera) {
            if (glfwGetKey(g_window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(g_window, GLFW_KEY_W) == GLFW_PRESS)
                g_camera->processKeyboard(0, deltaTime);
            if (glfwGetKey(g_window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(g_window, GLFW_KEY_S) == GLFW_PRESS)
                g_camera->processKeyboard(1, deltaTime);
            if (glfwGetKey(g_window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(g_window, GLFW_KEY_A) == GLFW_PRESS)
                g_camera->processKeyboard(2, deltaTime);
            if (glfwGetKey(g_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                g_camera->processKeyboard(3, deltaTime);
        }
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(currentTime - lastTime).count();
        
        if (elapsed >= FRAME_TIME) {
            lastTime = currentTime;
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            cinema.update();
            
            renderer.render(cinema, *g_camera, windowWidth, windowHeight);
            
            glfwSwapBuffers(g_window);
        }
        
        glfwPollEvents();
    }
    
    if (cursor) {
        glfwDestroyCursor(cursor);
    }
    
    glfwDestroyWindow(g_window);
    glfwTerminate();
    return 0;
}
