#include "application.h" 


//unsigned Application::cam = 0;

bool pressed_keys[1024];
bool once_pressed_keys[1024];


bool pressed_mouse_buttons[8];
bool once_pressed_mouse_buttons[8];

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if(action == GLFW_PRESS) {
        pressed_keys[key] = true;
        once_pressed_keys[key] = true;
    } else if(action == GLFW_RELEASE) {
        pressed_keys[key] = false;
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if(action == GLFW_PRESS) {
        pressed_mouse_buttons[button] = true;
        once_pressed_mouse_buttons[button] = true;
    } else if(action == GLFW_RELEASE) {
        pressed_mouse_buttons[button] = false;
    }
}


void Application::run() {

    initWindow();

    createResources();

    mainLoop();

    cleanup();
}


void Application::initWindow() {

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Scene", nullptr, nullptr);

    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, 0, 0);
    glfwGetCursorPos(window, &prevMousePosX, &prevMousePosY);
    
    if (glfwRawMouseMotionSupported()){
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
}



void Application::createResources() {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    render.init(window, &light, &objects);


    //light.setDevice(device, physicalDevice, shadowMapRenderPass, shadowMapPool ,shadowMapSetLayout);
    light.setRender(render.getLightNecessary());
    light.gen();

    
    setScene();

    render.updateRender();

    setPlayer();
}


void Application::setScene() {
    cam = 0;
    mode = PLAY;

    // world settings
    lowerBoundX = -100.0f;
    upperBoundX =  300.0f;
    lowerBoundY =    0.2f;
    upperBoundY =  100.0f;
    lowerBoundZ = -100.0f;
    upperBoundZ =  300.0f;
    //light settings
    glm::vec3 lightPos = {200.0f, 200.0f, 0.0f};

    float lightAngleX = M_PIF / 4.0f;
    float lightAngleY = M_PIF + M_PIF / 4.0f;
    //player settings
    playerPosX = 100.0f;
    playerPosY = 35.0f;
    playerPosZ = -100.0f;


    playerAddF = 2.0f;
    playerAddB = 2.0f;
    playerAddS = 2.0f;

    //playerAngleX = M_PIF / 6.0f;
    playerAngleX = 0.0f;
    playerAngleY = M_PIF;

    //playerAngleX = lightAngleX;
    //playerAngleY = lightAngleY;

    playerAddX = M_PIF / 72.0f * 0.04f;
    playerAddY = M_PIF / 72.0f * 0.04f;

    // camera proj

    glm::mat4 proj = glm::perspective(M_PIF / 4.0f, (float) WIDTH / (float) HEIGHT, 0.1f, 400.0f);
    proj[1][1] *= -1;

    /// shadow proj

    glm::mat4 shadowMapProj = glm::perspective(M_PIF / 2.0f, (float) SHADOWMAP_WIDTH / (float) SHADOWMAP_HEIGHT, 0.1f, 500.0f); 
    shadowMapProj[1][1] *= -1;

    /// light matrix
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), lightAngleX, glm::vec3(1.0f, 0.0f, 0.0f));
              rot = glm::rotate(rot            , lightAngleY, glm::vec3(0.0f, 1.0f, 0.0f));


    glm::mat4 conv = glm::translate(glm::mat4(1.0), -lightPos);
    conv = rot * conv;


    render.setVertBufferMemory(nullptr, &proj, &conv, &shadowMapProj);
    
    render.setShadowBufferMemory(&conv, &shadowMapProj);
    
    render.setFragBufferMemory(&lightPos);
    

    Point3 begPos;
    Image image;

    objects.resize(11);

    begPos = {-100.0f, 0.0f, -100.0f};
    image.load("textures/grass.jpg");
    objects[0].setRender(render.getObjectNecessary());
    objects[0].genField(begPos, 400.0f, 400.0f, 0.0f, 0.0f, 0.0f);
    objects[0].setNormals();
    objects[0].setTexture(image);
    objects[0].gen();

    
    //////
    begPos = {125.0f, 0.0f, 125.0f};
    image.load("textures/diamond_ore.png");

    objects[1].setRender(render.getObjectNecessary());
    objects[1].genCube(begPos, 50.0f, 50.0f, 50.0f, 0.0f, 0.0f, 0.0f);
    objects[1].setNormals();
    objects[1].setTexture(image);
    objects[1].gen();
    
    //////
    begPos = {25.0f, 0.0f, 125.0f};
    image.load("textures/creeper_head.png");

    objects[2].setRender(render.getObjectNecessary());
    objects[2].genCube2(begPos, 70.0f, 70.0f, 70.0f, 0.0f, 0.0f, 0.0f);
    objects[2].setNormals();
    objects[2].setTexture(image);
    objects[2].gen();
    
    //////
    begPos = {72.0f, 36.0f, 92.0f};
    image.load("textures/creeper_head.png");

    objects[3].setRender(render.getObjectNecessary());
    objects[3].genCube2(begPos, 16.0f, 16.0f, 16.0f, 0.0f, 0.0f, 0.0f);
    objects[3].setNormals();
    objects[3].setTexture(image);
    objects[3].gen();
    
    //////
    begPos = {72.0f, 12.0f, 96.0f};
    image.load("textures/creeper_body.png");

    objects[4].setRender(render.getObjectNecessary());
    objects[4].genCube2(begPos, 16.0f, 24.0f, 8.0f, 0.0f, 0.0f, 0.0f);
    objects[4].setNormals();
    objects[4].setTexture(image);
    objects[4].gen();
    
    //////
    begPos = {72.0f, 0.0f, 88.0f};
    image.load("textures/creeper_leg.png");

    objects[5].setRender(render.getObjectNecessary());
    objects[5].genCube2(begPos, 8.0f, 12.0f, 8.0f, 0.0f, 0.0f, 0.0f);
    objects[5].setNormals();
    objects[5].setTexture(image);
    objects[5].gen();
    
    //////
    begPos = {80.0f, 0.0f, 88.0f};
    image.load("textures/creeper_leg.png");

    objects[6].setRender(render.getObjectNecessary());
    objects[6].genCube2(begPos, 8.0f, 12.0f, 8.0f, 0.0f, 0.0f, 0.0f);
    objects[6].setNormals();
    objects[6].setTexture(image);
    objects[6].gen();
    
    //////
    begPos = {72.0f, 0.0f, 104.0f};
    image.load("textures/creeper_leg.png");

    objects[7].setRender(render.getObjectNecessary());
    objects[7].genCube2(begPos, 8.0f, 12.0f, 8.0f, 0.0f, 0.0f, 0.0f);
    objects[7].setNormals();
    objects[7].setTexture(image);
    objects[7].gen();
    
    //////
    begPos = {80.0f, 0.0f, 104.0f};
    image.load("textures/creeper_leg.png");

    objects[8].setRender(render.getObjectNecessary());
    objects[8].genCube2(begPos, 8.0f, 12.0f, 8.0f, 0.0f, 0.0f, 0.0f);
    objects[8].setNormals();
    objects[8].setTexture(image);
    objects[8].gen();


    begPos = {170.0f, 40.0f, 70.0f};
    image.load("textures/bee.png");

    objects[9].setRender(render.getObjectNecessary());
    objects[9].genCube2(begPos, 6.0f, 6.0f, 8.0f, 0.0f, 0.0f, 0.0f);
    objects[9].setNormals();
    objects[9].setTexture(image);
    objects[9].gen();

    begPos = {50.0f, 0.0f, 50.0f};
    image.load("textures/goose.jpg");

    objects[10].setRender(render.getObjectNecessary());
    objects[10].loadModel(begPos, "models/goose.obj", 40, 0.0f, 0.0f, M_PIF / 2.0f);
    objects[10].setTexture(image);
    objects[10].gen();

}


void Application::setPlayer() {

    
    glm::mat4 conv = glm::rotate(glm::mat4(1.0f), -playerAngleY, glm::vec3(0.0f, 1.0f, 0.0f));
              conv = glm::rotate(conv, -playerAngleX, glm::vec3(1.0f, 0.0f, 0.0f));

    if(pressed_keys[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }


    if (pressed_keys[GLFW_KEY_F1]) {
        cam = 0;
    } 
    if (pressed_keys[GLFW_KEY_F2]) {
        cam = 1;
    }

    if (once_pressed_mouse_buttons[GLFW_MOUSE_BUTTON_RIGHT]) {
        if (mode == PLAY) {
            mode = PAUSE;

            glfwGetCursorPos(window, &prevMousePosX, &prevMousePosY);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
            
        } else if (mode == PAUSE) {
            mode = PLAY;

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetCursorPos(window, prevMousePosX, prevMousePosY);
        }
        once_pressed_mouse_buttons[GLFW_MOUSE_BUTTON_RIGHT] = false;
    }


    if (once_pressed_keys[GLFW_KEY_P]) {
        objects.resize(12);

            Point3 begPos = {130.0f, 20.0f, 80.0f};
            Image image;
    image.load("textures/moon.png");

    objects[11].setRender(render.getObjectNecessary());
    objects[11].loadModel(begPos, "models/moon.obj", 10, 0.0f, 0.0f, M_PIF / 2.0f);
    objects[11].setTexture(image);
    objects[11].gen();

        render.updateRender();

        once_pressed_keys[GLFW_KEY_P] = false;
    }

    if (mode == PLAY) {

        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        playerAngleY += (mousePosX - prevMousePosX) * playerAddY; 
        while (playerAngleY > 2.0f * M_PIF) {
            playerAngleY -= 2.0f * M_PIF;
        }

        while (playerAngleY < 0.0f) {
            playerAngleY += 2.0f * M_PIF;
        }

        playerAngleX += (mousePosY - prevMousePosY) * playerAddX; 

        if (playerAngleX > 0.5f * M_PIF) {
            playerAngleX = 0.5f * M_PIF;
        }
        if (playerAngleX < -0.5f * M_PIF) {
            playerAngleX = -0.5f * M_PIF;
        }

        prevMousePosX = mousePosX;
        prevMousePosY = mousePosY;

        if (pressed_keys[GLFW_KEY_W]) {
            glm::vec4 pos = {0.0f, 0.0f, -1.0f, 0.0f};

            pos = conv * pos;

            playerPosX += playerAddF * pos[0];
            playerPosY += playerAddF * pos[1];
            playerPosZ += playerAddF * pos[2];
        }
        if (pressed_keys[GLFW_KEY_S]) {
            glm::vec4 pos = {0.0f, 0.0f, 1.0f, 0.0f};

            pos = conv * pos;

            playerPosX += playerAddB * pos[0];
            playerPosY += playerAddB * pos[1];
            playerPosZ += playerAddB * pos[2];
        }
        if (pressed_keys[GLFW_KEY_A]) {
            glm::vec4 pos = {-1.0f, 0.0f, 0.0f, 0.0f};

            pos = conv * pos;

            playerPosX += playerAddS * pos[0];
            playerPosY += playerAddS * pos[1];
            playerPosZ += playerAddS * pos[2];
        }
        if (pressed_keys[GLFW_KEY_D]) {
            glm::vec4 pos = {1.0f, 0.0f, 0.0f, 0.0f};

            pos = conv * pos;

            playerPosX += playerAddS * pos[0];
            playerPosY += playerAddS * pos[1];
            playerPosZ += playerAddS * pos[2];
        }


        if (playerPosX < lowerBoundX) {
            playerPosX = lowerBoundX;
        } 
        if (playerPosX > upperBoundX) {
            playerPosX = upperBoundX;
        } 
        if (playerPosY < lowerBoundY) {
            playerPosY = lowerBoundY;
        } 
        if (playerPosY > upperBoundY) {
            playerPosY = upperBoundY;
        } 
        if (playerPosZ < lowerBoundZ) {
            playerPosZ = lowerBoundZ;
        } 
        if (playerPosZ > upperBoundZ) {
            playerPosZ = upperBoundZ;
        } 
    }

    float posX = playerPosX;
    float posY = playerPosY;
    float posZ = playerPosZ;

    float angleX = playerAngleX;
    float angleY = playerAngleY;

    glm::vec3 pos = {posX, posY, posZ};

    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), angleX, glm::vec3(1.0f, 0.0f, 0.0f));
              rot = glm::rotate(rot            , angleY, glm::vec3(0.0f, 1.0f, 0.0f));


    conv = glm::translate(glm::mat4(1.0), -pos);
    conv = rot * conv;

    render.setVertBufferMemory(&conv, nullptr, nullptr, nullptr);
}


void Application::updateScene() {
    if (time / TICK > 0) {
        setPlayer();
        
        unsigned add = time / TICK;
        
        sec += add;
        if (sec * TICK > 1000000) {
            sec = 0;
            std::cout << fps << std::endl;
            fps = 0;
        }
    }

    time %= TICK;
}


void Application::mainLoop() {

    startCycle = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        render.drawFrame(cam, &currentFrame);
        endCycle = std::chrono::high_resolution_clock::now();
        time += std::chrono::duration_cast<std::chrono::microseconds>(endCycle - startCycle).count();
        startCycle = endCycle;
        updateScene();
        fps++;
    }

    vkDeviceWaitIdle(render.getDevice());
}


void Application::cleanup() { 

    light.cleanup();
    for (size_t i = 0; i < objects.size(); i++) {
        objects[i].cleanup();
    }

    render.cleanup();

    glfwDestroyWindow(window);

    glfwTerminate();
}