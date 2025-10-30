#include <iostream>
#include <vector>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/graph.cpp"
#include "renderer/graph_renderer.cpp"
#include "utils/camera.cpp"

static bool mouseDown=false; static double lastX=0,lastY=0;
static OrbitCamera cam;

void cursorPos(GLFWwindow*, double x, double y){
    if(!mouseDown) { lastX=x; lastY=y; return; }
    float dx = float(x - lastX)*0.005f;
    float dy = float(y - lastY)*0.005f;
    cam.addYaw(dx);
    cam.addPitch(-dy);
    lastX=x; lastY=y;
}

void mouseButton(GLFWwindow*, int btn, int action, int){
    if(btn==GLFW_MOUSE_BUTTON_LEFT) mouseDown = (action==GLFW_PRESS);
}

void scroll(GLFWwindow*, double , double yoff){
    cam.addZoom(float(-yoff*0.2f));
}

int main(){
    if(!glfwInit()){ std::cerr<<"glfw init fail\n"; return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(1280, 720, "Bastau 3D Live Cells", nullptr, nullptr);
    if(!win){ std::cerr<<"win fail\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr<<"glad fail\n"; return -1;
    }

    glfwSetCursorPosCallback(win, cursorPos);
    glfwSetMouseButtonCallback(win, mouseButton);
    glfwSetScrollCallback(win, scroll);

    glEnable(GL_DEPTH_TEST);

    Graph g; g.makeDemo(64);
    GraphRenderer renderer;
    renderer.init("assets/shaders");

    auto t0 = std::chrono::high_resolution_clock::now();

    while(!glfwWindowShouldClose(win)){
        int w,h; glfwGetFramebufferSize(win,&w,&h);
        glViewport(0,0,w,h);
        glClearColor(0.05f,0.07f,0.12f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        auto t1 = std::chrono::high_resolution_clock::now();
        float t = std::chrono::duration<float>(t1 - t0).count();

        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)w/h, 0.05f, 50.0f);
        glm::mat4 view = cam.view();

        // аздап “тірі” қозғалыс: түйін позициясын update
        for (auto& n : g.nodes) {
            float s = 0.15f;
            n.pos.y += sinf(t*1.2f + n.id)*0.0009f;
            n.pos.x += cosf(t*0.7f + n.id*0.3f)*0.0007f;
            n.size = 0.038f + 0.008f*sinf(t*2.0f + n.id);
        }

        renderer.draw(g, view, proj, t);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
