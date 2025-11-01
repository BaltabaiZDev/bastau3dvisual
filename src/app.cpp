#include "app.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/backends/imgui_impl_glfw.h"
#include "vendor/imgui/backends/imgui_impl_opengl3.h"

#include "core/graph.h"
#include "renderer/graph_renderer.h"
#include "modules/control/message_bus.h"
#include "modules/control/controller_panel.h"
#include "modules/control/worker_panel.h"
#include "utils/camera.h"
#include "ui/overlay.h"
#include "ui/theme.h"

// -------- Global camera/input state --------
static Camera3D gCam;
// Терезені бүкіл модульге қолжетімді қыламыз
static GLFWwindow* gWindow = nullptr;

static bool   gOrbiting = false, gPanning = false;
static double gLastX = 0.0, gLastY = 0.0;

// UI toggles
static bool gShowEdges  = true;
static bool gShowBounds = true;
static bool gShowLabels = true;

static int  gHoveredId  = -1;

// ---- Ray-sphere intersect (return t or +inf) ----
static float raySphereT(const Ray3D& r, const glm::vec3& c, float rad) {
    glm::vec3 m = r.o - c;
    float b = glm::dot(m, r.d);
    float c2 = glm::dot(m,m) - rad*rad;
    if (c2 > 0.0f && b > 0.0f) return 1e9f;
    float disc = b*b - c2;
    if (disc < 0.0f) return 1e9f;
    float t = -b - sqrtf(disc);
    return (t > 0.0f) ? t : 1e9f;
}

// --- input callbacks (3D) ---
static void scroll_cb(GLFWwindow*, double, double yoff) {
    float factor = (yoff > 0) ? 0.9f : 1.1f; // жақындау/алыстау
    gCam.dolly(factor);
}
static void mouse_button_cb(GLFWwindow* win, int button, int action, int) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            gOrbiting = true;
            glfwGetCursorPos(win, &gLastX, &gLastY);
        } else if (action == GLFW_RELEASE) {
            gOrbiting = false;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            gPanning = true;
            glfwGetCursorPos(win, &gLastX, &gLastY);
        } else if (action == GLFW_RELEASE) {
            gPanning = false;
        }
    }
}
static void cursor_pos_cb(GLFWwindow* win, double x, double y) {
    double dx = x - gLastX;
    double dy = y - gLastY;
    gLastX = x; gLastY = y;

    int w, h; glfwGetFramebufferSize(win, &w, &h);
    if (w == 0 || h == 0) return;

    if (gOrbiting) {
        float sx = (float)dx / (float)w;
        float sy = (float)dy / (float)h;
        gCam.orbit(-sx * 3.0f, -sy * 2.0f);
    }
    if (gPanning) {
        float scale = 2.0f * tanf(0.5f * gCam.fov_deg * 3.1415926f / 180.0f) * gCam.distance;
        float px =  (float)dx / (float)w * scale;
        float py = -(float)dy / (float)h * scale;
        gCam.pan(px, py);
    }
}

void App::run() {
    // --- GLFW init ---
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return;
    }
    // OpenGL (compat for immediate-mode)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "EduGraph 3D", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync
    gWindow = window;    // ✅ глобалға береміз

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return;
    }

    // Callbacks
    glfwSetScrollCallback(gWindow,        scroll_cb);
    glfwSetMouseButtonCallback(gWindow,   mouse_button_cb);
    glfwSetCursorPosCallback(gWindow,     cursor_pos_cb);

    // --- ImGui init ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(gWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // --- App state ---
    Graph graph(20);
    MessageBus bus(graph);
    GraphRenderer renderer;
    ControllerPanel controller{bus};
    WorkerPanel worker{bus};

    // Камера бастапқы мәндері
    gCam.target   = glm::vec3(0.0f, 0.0f, 0.0f);
    gCam.distance = 3.5f;
    gCam.yaw      = 0.7f;
    gCam.pitch    = -0.35f;

    // Time
    double lastTime = glfwGetTime();

    // --- Main loop ---
    while (!glfwWindowShouldClose(gWindow)) {   // ✅ window -> gWindow
        glfwPollEvents();

        double now = glfwGetTime();
        float dt = (float)(now - lastTime);
        lastTime = now;
        if (dt > 0.05f) dt = 0.05f;

        // --- Hover picking ---
        {
            double mx, my; int W, H;
            glfwGetCursorPos(gWindow, &mx, &my);        // ✅ window -> gWindow
            glfwGetFramebufferSize(gWindow, &W, &H);    // ✅ window -> gWindow
            Ray3D ray = gCam.rayFromScreen(mx, my, W, H);
            gHoveredId = -1;
            float bestT = 1e9f;
            for (const auto& n : graph.getNodes()) {
                float t = raySphereT(ray, n.pos, 0.08f);
                if (t < bestT) { bestT = t; gHoveredId = n.id; }
            }
        }

        // --- ImGui frame ---
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Controls panels
        ImGui::Begin("Controller");
        controller.draw();
        ImGui::Separator();
        ImGui::Checkbox("Show edges",  &gShowEdges);
        ImGui::Checkbox("Show bounds", &gShowBounds);
        ImGui::Checkbox("Show labels", &gShowLabels);
        ImGui::End();

        ImGui::Begin("Worker");
        worker.draw();
        ImGui::End();

        // Legend / Stats HUD
        drawLegendAndStats(graph);

        // --- Update world ---
        graph.update(dt);

        // --- Clear & render ---
        int display_w, display_h;
        glfwGetFramebufferSize(gWindow, &display_w, &display_h); // ✅
        glViewport(0, 0, display_w, display_h);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.07f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderOptions ro;
        ro.showEdges  = gShowEdges;
        ro.showBounds = gShowBounds;
        ro.haloHover  = true;

        renderer.render(graph, gCam, display_w, display_h, gHoveredId, ro);

        // Labels on top
        if (gShowLabels) drawLabelsOverlay(graph, gCam, display_w, display_h, gHoveredId);

        // ImGui draw
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(gWindow); // ✅
    }

    // --- Shutdown ---
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(gWindow);
    gWindow = nullptr;
    glfwTerminate();
}
