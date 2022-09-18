// TODO :
// Fonts 
// - load fonts by adding to atlas
// - reload atlas each time new font is loaded
// extra. split rows and columns

#include <IndieGoUI.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <list>
#include <filesystem>
namespace fs = std::filesystem;

// Plain simple code for window creation
// Created window will hold a widget
// #define WIDTH 1280
// #define HEIGHT 800

GLFWwindow * screen;

using namespace IndieGo::UI;

Manager GUI;
WIDGET creator_widget;
WIDGET styling_widget;
std::string winID = "UI test app";

// [widID] = current_line
std::map<std::string, unsigned int> widgets_fill;

// Window input callbacks
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	GUI.mouse_move( &winID, xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    GUI.mouse_button( &winID,button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    GUI.scroll( &winID, xoffset, yoffset);
}

void char_callback(GLFWwindow* window, unsigned int codepoint) {
    GUI.char_input( &winID, codepoint);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    GUI.key_input( &winID, key, glfwGetKey(window, key) == GLFW_PRESS);
}

unsigned int buttons_count = 1;

double curr_time = 0.0, prev_time = 0.0;
unsigned int frames = 0;

extern unsigned int skinning_image_id;
extern unsigned int si_w;
extern unsigned int si_h;
extern unsigned int load_image(const char *filename, bool load_skinning_image);

int main() {

  	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    screen = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL_Nuklear UI", NULL, NULL);
	glfwMakeContextCurrent(screen);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

    glfwSetCursorPosCallback(screen, cursor_position_callback);
    glfwSetMouseButtonCallback(screen, mouse_button_callback);
    glfwSetScrollCallback(screen, scroll_callback);
    glfwSetCharCallback(screen, char_callback);
    glfwSetKeyCallback(screen, key_callback);

    glViewport(0, 0, WIDTH, HEIGHT);
    GUI.init(winID, screen);
    GUI.screen_size.w = WIDTH;
    GUI.screen_size.h = HEIGHT;

    WIDGET test_widget;
    test_widget.screen_region.h = 0.3f;
    test_widget.screen_region.w = 0.3f;
    test_widget.screen_region.x = 0.3f;
    test_widget.screen_region.y = 0.3f;

    test_widget.name = "test widget";

    GUI.addWidget(test_widget, winID);
    WIDGET & test_widget_h = GUI.getWidget("test widget", winID);

    test_widget_h.minimizable = false;
    test_widget_h.title = false;
    test_widget_h.movable = false;
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    
    UIMap.addElement("group", UI_ITEMS_LIST, &test_widget_h);
    test_widget_h.updateRowHeight(test_widget_h.layout_grid.size() - 1, 0.5f);
    
    UIMap.addElement("test button 1", UI_BUTTON, &test_widget_h, to_new_col);
    UIMap["test button 1"].label = "TEST BUTTON";
    UIMap["test button 1"].height = 0.1f;
    
    // TODO : set padding and border as % from widget
    UIMap["test button 1"].padding.h = 5.f;
    UIMap["test button 1"].padding.w = 10.f;
    UIMap["test button 1"].border = 1.f;

    UIMap.addElement("test button 2", UI_BUTTON, &test_widget_h, to_new_subrow);
    UIMap["test button 2"].label = "TEST BUTTON";
    UIMap["test button 2"].height = 0.1f;
    UIMap["test button 2"].padding.h = 5.f;
    UIMap["test button 2"].padding.w = 10.f;

    UIMap.addElement("test text", UI_STRING_TEXT, &test_widget_h, to_new_subrow);
    UIMap["test text"].label = "This is some text to test layout. This text should be long enough to get wrapped! Also cell height should be sufficient to display it fully.";
    UIMap["test text"].height = 0.2f;
    
    UIMap.addElement("test image 1", UI_IMAGE, &test_widget_h);
    UIMap["test image 1"].initImage("C:\\Users\\Public\\GobbyIsland\\Sprites\\GoblinSlayer\\World\\leaves_giant.png");
    test_widget_h.updateRowHeight(test_widget_h.layout_grid.size() - 1, 0.5f);

    UIMap.addElement("test button 3", UI_BUTTON, &test_widget_h, to_new_col);
    UIMap["test button 3"].label = "TEST BUTTON";
    UIMap["test button 3"].height = 0.1f;
    UIMap["test button 3"].rounding = 10.f;

    UIMap.addElement("test image 2", UI_IMAGE, &test_widget_h, to_new_subrow);
    UIMap["test image 2"].initImage("C:\\Users\\Public\\GobbyIsland\\Sprites\\GoblinSlayer\\World\\leaves_enormous_snow.png");
    UIMap["test image 2"].height = 0.4f;

    unsigned int skin_tex_id = load_image("C:\\Users\\Public\\GobbyIsland\\Sprites\\magic_bar_3_parts_empty.png", true);
    region<float> crop;
    crop.x = 0.f;
    crop.y = 0.f;
    crop.w = si_w;
    crop.h = si_h;
    test_widget_h.useSkinImage(
        skinning_image_id, 
        si_w, 
        si_h, 
        crop,
        background
    );

	// set initial time to zero
	glfwSetTime(0.0);
    int width, height;
    while (!glfwWindowShouldClose(screen)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // update screen size each frame before calling immediate backend
        glfwGetWindowSize(screen, &width, &height);

        GUI.screen_size.w = width;
        GUI.screen_size.h = height;

        GUI.drawFrameStart(winID);
        GUI.displayWidgets(winID);
        GUI.drawFrameEnd(winID);

        glfwSwapBuffers(screen);
	    glfwPollEvents();
    }
    return 0;
}