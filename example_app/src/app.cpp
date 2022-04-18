#include <IndieGoUI.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>

// Plain simple code for window creation
// Created window will hold a widget
#define WIDTH 1280
#define HEIGHT 800

GLFWwindow * screen;

using namespace IndieGo::UI;

Manager GUI;
WIDGET test_widget, logging_widget;

// Window input callbacks
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	GUI.mouse_move(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    GUI.mouse_button(button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    GUI.scroll(xoffset, yoffset);
}

void char_callback(GLFWwindow* window, unsigned int codepoint) {
    GUI.char_input(codepoint);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    GUI.key_input(key, glfwGetKey(window, key) == GLFW_PRESS);
}

unsigned int buttons_count = 1;

std::vector<std::string> list_items = {
    "Spam", "Eggs"
};

std::string getColorPropName(COLOR_ELEMENTS prop) {
    if (prop == COLOR_TEXT) return "COLOR_TEXT";
    if (prop == COLOR_WINDOW) return "COLOR_WINDOW";
    if (prop == COLOR_HEADER) return "COLOR_HEADER";
    if (prop == COLOR_BORDER) return "COLOR_BORDER";
    if (prop == COLOR_BUTTON) return "COLOR_BUTTON";
    if (prop == COLOR_BUTTON_HOVER) return "COLOR_BUTTON_HOVER";
    if (prop == COLOR_BUTTON_ACTIVE) return "COLOR_BUTTON_ACTIVE";
    if (prop == COLOR_TOGGLE) return "COLOR_TOGGLE";
    if (prop == COLOR_TOGGLE_HOVER) return "COLOR_TOGGLE_HOVER";
    if (prop == COLOR_TOGGLE_CURSOR) return "COLOR_TOGGLE_CURSOR";
    if (prop == COLOR_SELECT) return "COLOR_SELECT";
    if (prop == COLOR_SELECT_ACTIVE) return "COLOR_SELECT_ACTIVE";
    if (prop == COLOR_SLIDER) return "COLOR_SLIDER";
    if (prop == COLOR_SLIDER_CURSOR) return "COLOR_SLIDER_CURSOR";
    if (prop == COLOR_SLIDER_CURSOR_HOVER) return "COLOR_SLIDER_CURSOR_HOVER";
    if (prop == COLOR_SLIDER_CURSOR_ACTIVE) return "COLOR_SLIDER_CURSOR_ACTIVE";
    if (prop == COLOR_PROPERTY) return "COLOR_PROPERTY";
    if (prop == COLOR_EDIT) return "COLOR_EDIT";
    if (prop == COLOR_EDIT_CURSOR) return "COLOR_EDIT_CURSOR";
    if (prop == COLOR_COMBO) return "COLOR_COMBO";
    if (prop == COLOR_CHART) return "COLOR_CHART";
    if (prop == COLOR_CHART_COLOR) return "COLOR_CHART_COLOR";
    if (prop == COLOR_CHART_COLOR_HIGHLIGHT) return "COLOR_CHART_COLOR_HIGHLIGHT";
    if (prop == COLOR_SCROLLBAR) return "COLOR_SCROLLBAR";
    if (prop == COLOR_SCROLLBAR_CURSOR) return "COLOR_SCROLLBAR_CURSOR";
    if (prop == COLOR_SCROLLBAR_CURSOR_HOVER) return "COLOR_SCROLLBAR_CURSOR_HOVER";
    if (prop == COLOR_SCROLLBAR_CURSOR_ACTIVE) return "COLOR_SCROLLBAR_CURSOR_ACTIVE";
    if (prop == COLOR_TAB_HEADER) return "COLOR_TAB_HEADER";
    return "NO_COLOR_PROPERTY";
}

double curr_time = 0.0, prev_time = 0.0;
unsigned int frames = 0;

int main(){
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

    GUI.init(screen);

    // initialize test widget
    test_widget.screen_region.x = WIDTH / 4;
    test_widget.screen_region.y = HEIGHT / 4;
    test_widget.screen_region.w = WIDTH / 2;
    test_widget.screen_region.h = HEIGHT / 3;
    test_widget.name = "OpenGL_Nuklear_UI widget";
    test_widget.custom_style = false;
    test_widget.style.elements[COLOR_WINDOW].a = 175;


	// logging widget initialization. Place at top-left corner,
	// body is fully-transparent, so only text is visible
	logging_widget.screen_region.x = 0;
	logging_widget.screen_region.y = 0;
	logging_widget.screen_region.w = WIDTH / 4;
	logging_widget.screen_region.h = HEIGHT / 4;
	logging_widget.name = "log";
	logging_widget.style.elements[COLOR_WINDOW].a = 0;
	logging_widget.border = false;
	logging_widget.title = false;
	logging_widget.minimizable = false;
	logging_widget.scalable = false;
	logging_widget.movable = false;

    WIDGET & widget = GUI.addWidget("OpenGL_Nuklear_UI", test_widget);
	WIDGET & log = GUI.addWidget("OpenGL_Nuklear_UI", logging_widget);

    UI_elements_map & UIMap = GUI.UIMaps["OpenGL_Nuklear_UI"];

    // Example adding elements (properties) to UIMap
    UIMap.addElement("add row", UI_BUTTON, &widget);
    UIMap["add row"].label = "add row";

    UIMap.addElement("add column", UI_BUTTON, &widget);
    UIMap["add column"].label = "add column";

    UIMap.addElement("show list", UI_BOOL, &widget, 1);
    UIMap["show list"].label = "show list";

    UIMap.addElement("items list size", UI_UINT, &widget, 1);
    UIMap["items list size"].label = "items list size";
    UIMap["items list size"]._data.i = 100;

    UIMap.addElement("string items list", UI_ITEMS_LIST, &widget, 2);
    UIMap["string items list"].label = "widget items list";
    ui_string_group & example_list = *UIMap["string items list"]._data.usgPtr;
    example_list.copyFrom(list_items);

    UIMap.addElement("selected UI item", UI_STRING_LABEL, &widget, 3);
    UIMap["selected UI item"].label = "selected UI item:";
    UIMap["selected UI item"].hidden = true;

    UIMap.addElement("current list item", UI_STRING_INPUT, &widget, 3);
    UIMap["current list item"].label = "current list item:";
    UIMap["current list item"].hidden = true;

    UIMap.addElement("test color picker", UI_COLOR_PICKER, &widget, 4);
    UIMap["test color picker"].label = "test color picker";

	// Adding logging text field to log widget
	UIMap.addElement("fps counter", UI_STRING_LABEL, &log);
    UIMap["fps counter"].label = "fps";
	UIMap["fps counter"].text_align = LEFT;

	// set initial time to zero
	glfwSetTime(0.0);

    // float color;
    while (!glfwWindowShouldClose(screen)) {
        glClearColor(0.778f, 0.906f, 0.867f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        UIMap["string items list"].hidden = !UIMap["show list"]._data.b;

        if (UIMap["show list"]._data.b) {
            // Modify text edit filed, basing on user's input
            if (example_list.selected_element > -1 ) {
                std::string & curr_selected_item = example_list.getSelected();
                if (example_list.selection_switch){
                    *UIMap["current list item"]._data.strPtr = curr_selected_item;
                    if (UIMap["current list item"].hidden) {
                        UIMap["current list item"].hidden = false;
                        UIMap["selected UI item"].hidden = false;
                    }
                } else {
                    curr_selected_item = *UIMap["current list item"]._data.strPtr;
                }
            }
        } else {
            UIMap["current list item"].hidden = true;
            UIMap["selected UI item"].hidden = true;
            example_list.unselect();
        }

        GUI.drawFrameStart();
        GUI.displayWidgets("OpenGL_Nuklear_UI");
        GUI.drawFrameEnd();
        
        curr_time = glfwGetTime();
        if ((curr_time - prev_time) > 1.0) {
            prev_time = curr_time;
            // update log woth fps
            UIMap["fps counter"].label = "FPS: " + std::to_string(frames);
            frames = 0;
        }
		frames++;

        // Modify height of specific layout row, depending on data from ui element
        widget.layout_grid[2].min_height = UIMap["items list size"]._data.i;

        glfwSwapBuffers(screen);
	    glfwPollEvents();
    }
    return 0;
}
