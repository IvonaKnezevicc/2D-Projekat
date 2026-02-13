#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>

class Cinema;
class Renderer;
class Camera;

class Application {
public:
    Application();
    ~Application();
    
    bool initialize();
    void run();
    
private:
    GLFWwindow* window;
    GLFWcursor* cursor;
    std::unique_ptr<Cinema> cinema;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Camera> camera;
    
    bool depthTestEnabled;
    bool faceCullingEnabled;
    bool firstMouse;
    float lastX;
    float lastY;
    
    static constexpr double TARGET_FPS = 75.0;
    static constexpr double FRAME_TIME = 1.0 / TARGET_FPS;
    
    void setupCallbacks();
    void setupOpenGL();
    
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
    static Application* getInstance(GLFWwindow* window);
};
