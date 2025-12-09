#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "../Header/Util.h"
#include "../Header/Cinema.h"
#include "../Header/Renderer.h"

Cinema* g_cinema = nullptr;
Renderer* g_renderer = nullptr;
GLFWwindow* g_window = nullptr;

const double TARGET_FPS = 75.0;
const double FRAME_TIME = 1.0 / TARGET_FPS;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && g_cinema) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        g_cinema->handleMouseClick(xpos, ypos);
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
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
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    Cinema cinema(windowWidth, windowHeight);
    Renderer renderer(windowWidth, windowHeight);
    
    g_cinema = &cinema;
    g_renderer = &renderer;
    
    glfwSetMouseButtonCallback(g_window, mouseButtonCallback);
    glfwSetKeyCallback(g_window, keyCallback);
    
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    std::cout << "Bioskop - Upravljanje sedistima" << std::endl;
    std::cout << "Kontrole:" << std::endl;
    std::cout << "  - Levi klik: Rezervacija/otkazivanje sedista" << std::endl;
    std::cout << "  - Tasteri 1-9: Kupovina karata" << std::endl;
    std::cout << "  - Enter: Pocetak projekcije" << std::endl;
    std::cout << "  - Escape: Izlaz" << std::endl;
    
    while (!glfwWindowShouldClose(g_window))
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(currentTime - lastTime).count();
        
        if (elapsed >= FRAME_TIME) {
            lastTime = currentTime;
            
            glClear(GL_COLOR_BUFFER_BIT);
            
            cinema.update();
            
            renderer.render(cinema);
            
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
