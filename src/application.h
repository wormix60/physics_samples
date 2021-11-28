#ifndef APPLICATION_H
#define APPLICATION_H

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#ifdef WIN32
#pragma comment(lib,"glfw3.lib")
#endif

#include <vulkan/vulkan.h>
#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <chrono>

#include "vk_utils.h"


//#define WIDTH 1600
//#define HEIGHT 900


extern bool pressed_keys[1024];
extern bool once_pressed_keys[1024];
extern bool pressed_mouse_buttons[8];
extern bool once_pressed_mouse_buttons[8];

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);


const float M_PIF = acosf(-1.0f);

const long long TICK = 25000;

#define PLAY 0
#define PAUSE 1


#include "object.h"
#include "image.h"
#include "light.h"
#include "render.h"


class Application{

public:
    void run();
    void initWindow();
    void createResources();
    void setScene();
    void setPlayer();
    void mainLoop();
    void cleanup();
    void updateScene();
    void drawFrame();

private:
    GLFWwindow * window;

    Render render;

    size_t currentFrame = 0;

    long long time = 0;
    std::chrono::high_resolution_clock::time_point startCycle;
    std::chrono::high_resolution_clock::time_point endCycle;
    
    unsigned vertOffset;

    unsigned cam;
    unsigned mode;

    float playerPosX;
    float playerPosY;
    float playerPosZ;

    float playerAddF;
    float playerAddS;
    float playerAddB;

    float playerAngleX;
    float playerAngleY;
    
    float playerAddX;
    float playerAddY;

    float lowerBoundX;
    float upperBoundX;
    float lowerBoundY;
    float upperBoundY;
    float lowerBoundZ;
    float upperBoundZ;

    double prevMousePosX;
    double prevMousePosY;

    std::vector<Object> objects;

    Light light;

    float objPos = 0.0f;

    unsigned fps = 0;
    unsigned sec = 0;
}; 


#endif