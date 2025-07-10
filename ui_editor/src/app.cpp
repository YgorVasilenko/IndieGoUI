// is it possible to:
// - switch shaders in same graphics pipeline?
// - have several vertex Buffers call? (likely pass all at once to vkCmdBindVertexBuffers)
#include <filesystem>

#include <editor_renderers.h>
#include <IndieGoUI.h>
#include <queue>
#include <functional>
#include <memory>

using namespace IndieGo::UI;
using namespace IndieGo::vkI;
using namespace IndieGo::vkI::aux;
using namespace std;

namespace fs = filesystem;

#ifndef WIDTH 
#define WIDTH 1920
#endif

#ifndef HEIGHT 
#define HEIGHT 1080
#endif

WIDGET creator_widget;
WIDGET styling_widget;

// Global DTO for avoiding spaghetti interfaces
EditorState editorGlobals;

// Plain simple code for window creation
// Created window will hold a widget
std::queue<std::function<void()>> delayedFunctions;

vector<float> font_load_sizes = {
    16., 18., 20., 24., 30., 36., 42., 48., 60., 72.
};

extern void initProjectDir();
extern void initWidgets();

extern shared_ptr<ScreenQuadRenderer> screen_quad_renderer;
extern unique_ptr<LayoutRenderer> layout_renderer;
extern void renderLayout();

extern void initRenderers(GLFWwindow * w);
extern void drawFrame();

// load_items.first -> resources path
// load_items.second -> project name
pair<string, string> getResourcesPath() {
    char* p = getenv("PROJECT_DIR");
    string path = "";
    string project_name = "";
    if (p) {
        path = p;
        project_name = fs::path( path ).filename().string();
    } else {
        p = getenv("INDIEGO_HOME");
        if (p)
            path = p;
        else
            path = fs::current_path().string();
    }
    return pair<string, string>(path, project_name);
}

string home_dir = "";

int main(int argc, char** argv) {
    // Get current application path
    home_dir = fs::path(argv[0]).parent_path().string();

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    IndieGo::vkI::window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan_Nuklear UI", nullptr, nullptr);
    // glfwSetFramebufferSizeCallback(GLwindow, GLFWaux::framebufferResizeCallback);
    initRenderers(IndieGo::vkI::window);

    Manager::init(IndieGo::vkI::window, screen_quad_renderer);
    Manager::screen_size.w = WIDTH;
    Manager::screen_size.h = HEIGHT;

    initWidgets();
    initProjectDir();
    int maxFrames = vkRenderer::swapChainImageViews.size();
    int currFrame = 0;

    while (!glfwWindowShouldClose(window)) {
        layout_renderer->layout_rect_idx = 0;
        if (Manager::UIMap["layout borders"]._data.b) {
            renderLayout();
        }
        Manager::drawFrameStart();
        Manager::displayWidgets();
        drawFrame();
        glfwPollEvents();

        while (delayedFunctions.size()) {
            delayedFunctions.front()();
            delayedFunctions.pop();
        }
    }

    return 0;
}