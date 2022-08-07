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
std::string winID = "OpenGL_Nuklear UI";
// Window input callbacks
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	GUI.mouse_move( &winID, xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    GUI.mouse_button( &winID,button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    GUI.scroll( &winID, xoffset, yoffset);
}

void char_callback(GLFWwindow* window, unsigned int codepoint) {
    GUI.char_input( &winID, codepoint);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    GUI.key_input( &winID, key, glfwGetKey(window, key) == GLFW_PRESS);
}

unsigned int buttons_count = 1;

std::vector<std::string> list_items = {
    "Spam", "Eggs"
};

std::string getColorPropName(COLOR_ELEMENTS prop) {
    if (prop == UI_COLOR_TEXT) return "UI_COLOR_TEXT";
    if (prop == UI_COLOR_WINDOW) return "UI_COLOR_WINDOW";
    if (prop == UI_COLOR_HEADER) return "UI_COLOR_HEADER";
    if (prop == UI_COLOR_BORDER) return "UI_COLOR_BORDER";
    if (prop == UI_COLOR_BUTTON) return "UI_COLOR_BUTTON";
    if (prop == UI_COLOR_BUTTON_HOVER) return "UI_COLOR_BUTTON_HOVER";
    if (prop == UI_COLOR_BUTTON_ACTIVE) return "UI_COLOR_BUTTON_ACTIVE";
    if (prop == UI_COLOR_TOGGLE) return "UI_COLOR_TOGGLE";
    if (prop == UI_COLOR_TOGGLE_HOVER) return "UI_COLOR_TOGGLE_HOVER";
    if (prop == UI_COLOR_TOGGLE_CURSOR) return "UI_COLOR_TOGGLE_CURSOR";
    if (prop == UI_COLOR_SELECT) return "UI_COLOR_SELECT";
    if (prop == UI_COLOR_SELECT_ACTIVE) return "UI_COLOR_SELECT_ACTIVE";
    if (prop == UI_COLOR_SLIDER) return "UI_COLOR_SLIDER";
    if (prop == UI_COLOR_SLIDER_CURSOR) return "UI_COLOR_SLIDER_CURSOR";
    if (prop == UI_COLOR_SLIDER_CURSOR_HOVER) return "UI_COLOR_SLIDER_CURSOR_HOVER";
    if (prop == UI_COLOR_SLIDER_CURSOR_ACTIVE) return "UI_COLOR_SLIDER_CURSOR_ACTIVE";
    if (prop == UI_COLOR_PROPERTY) return "UI_COLOR_PROPERTY";
    if (prop == UI_COLOR_EDIT) return "UI_COLOR_EDIT";
    if (prop == UI_COLOR_EDIT_CURSOR) return "UI_COLOR_EDIT_CURSOR";
    if (prop == UI_COLOR_COMBO) return "UI_COLOR_COMBO";
    if (prop == UI_COLOR_CHART) return "UI_COLOR_CHART";
    if (prop == UI_COLOR_CHART_COLOR) return "UI_COLOR_CHART_COLOR";
    if (prop == UI_COLOR_CHART_COLOR_HIGHLIGHT) return "UI_COLOR_CHART_COLOR_HIGHLIGHT";
    if (prop == UI_COLOR_SCROLLBAR) return "UI_COLOR_SCROLLBAR";
    if (prop == UI_COLOR_SCROLLBAR_CURSOR) return "UI_COLOR_SCROLLBAR_CURSOR";
    if (prop == UI_COLOR_SCROLLBAR_CURSOR_HOVER) return "UI_COLOR_SCROLLBAR_CURSOR_HOVER";
    if (prop == UI_COLOR_SCROLLBAR_CURSOR_ACTIVE) return "UI_COLOR_SCROLLBAR_CURSOR_ACTIVE";
    if (prop == UI_COLOR_TAB_HEADER) return "UI_COLOR_TAB_HEADER";
    return "NO_COLOR_PROPERTY";
}

double curr_time = 0.0, prev_time = 0.0;
unsigned int frames = 0;

struct Win_init_data {
    std::string winID = winID;
    void* screen = NULL;
} _init_data;

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

    _init_data.screen = screen;
    _init_data.winID = winID;
    GUI.init( &_init_data);

    // initialize test widget
    test_widget.screen_region.x = WIDTH / 4;
    test_widget.screen_region.y = HEIGHT / 4;
    test_widget.screen_region.w = WIDTH / 2;
    test_widget.screen_region.h = HEIGHT / 3;
    test_widget.name = "OpenGL_Nuklear_UI widget";
    test_widget.custom_style = true;
    test_widget.style.elements[UI_COLOR_WINDOW].a = 175;

	// logging widget initialization. Place at top-left corner,
	// body is fully-transparent, so only text is visible
	logging_widget.screen_region.x = 0;
	logging_widget.screen_region.y = 0;
	logging_widget.screen_region.w = WIDTH;
	logging_widget.screen_region.h = HEIGHT / 4;
	logging_widget.name = "log";
    logging_widget.custom_style = true;
	logging_widget.style.elements[UI_COLOR_WINDOW].a = 100;
	logging_widget.border = false;
	logging_widget.title = false;
	logging_widget.minimizable = false;
	logging_widget.scalable = false;
	logging_widget.movable = false;

    WIDGET & widget = GUI.addWidget(test_widget, "OpenGL_Nuklear_UI");
	WIDGET & log = GUI.addWidget(logging_widget, "OpenGL_Nuklear_UI");

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

    // Adding logging text field to log widget
	UIMap.addElement("Cursor hovered over", UI_STRING_LABEL, &log);
    UIMap["Cursor hovered over"].label = "Cursor hovered over: ";
	UIMap["Cursor hovered over"].text_align = LEFT;


	// set initial time to zero
	glfwSetTime(0.0);

    // float color;
    while (!glfwWindowShouldClose(screen)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
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

        // Always keep log widget in background
        if (log.focused){
            widget.setFocus = true;
        }

        if (GUI.hoveredWidgets["OpenGL_Nuklear_UI"]){
            UIMap["Cursor hovered over"].label = "Cursor hovered over: " + GUI.hoveredWidgets["OpenGL_Nuklear_UI"]->name;
        } else {
            UIMap["Cursor hovered over"].label = "Cursor hovered over: None";
        }

        GUI.drawFrameStart(std::string("OpenGL_Nuklear UI"));
        GUI.displayWidgets(std::string("OpenGL_Nuklear_UI"));
        GUI.drawFrameEnd(std::string("OpenGL_Nuklear UI"));
        
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
#ifdef CI_BUILD
        break;
#endif
    }
    return 0;
}
