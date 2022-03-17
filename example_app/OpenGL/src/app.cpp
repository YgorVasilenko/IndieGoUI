#include <IndieGoUI.h>
#include <backends/Nuklear/glad/glad.h>
#include <backends/Nuklear/GLFW/glfw3.h>
#include <chrono>

// Plain simple code for window creation
// Created window will hold a widget
#define WIDTH 1280
#define HEIGHT 800

GLFWwindow * screen;

using namespace IndieGo::UI;

IndieGoUI GUI;
WIDGET test_widget;

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
    test_widget.custom_style = true;
    test_widget.style.elements[COLOR_WINDOW].a = 175;

    GUI.addWidget("OpenGL_Nuklear_UI", test_widget);
    UI_elements_map & UIMap = GUI.UIMaps["OpenGL_Nuklear_UI"];

    // Example adding elements (properties) to UIMap
    UIMap.addElement("add row", UI_BUTTON, &GUI.widgets["OpenGL_Nuklear_UI"]["OpenGL_Nuklear_UI widget"]);
    UIMap["add row"].label = "add row";

    UIMap.addElement("add column", UI_BUTTON, &GUI.widgets["OpenGL_Nuklear_UI"]["OpenGL_Nuklear_UI widget"]);
    UIMap["add column"].label = "add column";

    UIMap.addElement("show list", UI_BOOL, &GUI.widgets["OpenGL_Nuklear_UI"]["OpenGL_Nuklear_UI widget"], 1);
    UIMap["show list"].label = "show list";

    UIMap.addElement("items list size", UI_UINT, &GUI.widgets["OpenGL_Nuklear_UI"]["OpenGL_Nuklear_UI widget"], 1);
    UIMap["items list size"].label = "items list size";
    UIMap["items list size"]._data.i = 100;

    UIMap.addElement("string items list", UI_ITEMS_LIST, &GUI.widgets["OpenGL_Nuklear_UI"]["OpenGL_Nuklear_UI widget"], 2);
    UIMap["string items list"].label = "template items list";

    ui_string_group & example_list = *UIMap["string items list"]._data.usgPtr;
    example_list.copyFrom(list_items);

    UIMap.addElement("current list item", UI_STRING_INPUT, &GUI.widgets["OpenGL_Nuklear_UI"]["OpenGL_Nuklear_UI widget"], 3);
    UIMap["current list item"].label = "current list item:";
    UIMap["current list item"].hidden = true;

    // Add color pickers for each possibly midifiable value
    std::string color_prop_name;
    for (unsigned char i = COLOR_TEXT; i < COLOR_TAB_HEADER + 1; i++ ) {
        color_prop_name = getColorPropName((COLOR_ELEMENTS)i);
        UIMap.addElement("style " + color_prop_name, UI_STRING_LABEL, &GUI.widgets["OpenGL_Nuklear_UI"]["OpenGL_Nuklear_UI widget"], 4 + i);
        UIMap["style " + color_prop_name].label = "style " + color_prop_name;
        UIMap.addElement(color_prop_name, UI_COLOR_PICKER, &GUI.widgets["OpenGL_Nuklear_UI"]["OpenGL_Nuklear_UI widget"], 4 + i);
    }

    float color;
    while (!glfwWindowShouldClose(screen)) {
        glClearColor(0.1f, 0.3f, 0.2f, 1.f);
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
                    }
                } else {
                    curr_selected_item = *UIMap["current list item"]._data.strPtr;
                }
            }
        }

        GUI.drawFrameStart();
        GUI.displayWidgets("OpenGL_Nuklear_UI");
        GUI.drawFrameEnd();
        

        // Using UI data from user's input to update layout:
        if (UIMap["add column"]._data.b){
            UIMap.addElement("example button " + std::to_string(buttons_count), UI_BUTTON, &GUI.widgets["OpenGL_Nuklear_UI"]["OpenGL_Nuklear_UI widget"], 0, 0, false, false);
            UIMap["example button " + std::to_string(buttons_count)].label = "example button " + std::to_string(buttons_count);
            buttons_count++;
        }

        if (UIMap["add row"]._data.b){
            UIMap.addElement("example button " + std::to_string(buttons_count), UI_BUTTON, &GUI.widgets["OpenGL_Nuklear_UI"]["OpenGL_Nuklear_UI widget"], 0, 0, true, false);
            UIMap["example button " + std::to_string(buttons_count)].label = "example button " + std::to_string(buttons_count);
            buttons_count++;
        }

        // Modify height of specific layout row, basing on data from element
        GUI.widgets["OpenGL_Nuklear_UI"]["OpenGL_Nuklear_UI widget"].layout_grid[2].min_height = UIMap["items list size"]._data.i;

        // Update color accordingly
        for (unsigned char i = COLOR_TEXT; i < COLOR_TAB_HEADER + 1; i ++ ){
            color_prop_name = getColorPropName((COLOR_ELEMENTS)i);
            if (UIMap[color_prop_name].color_picker_unwrapped) {
                colorf & curr_color = UIMap[color_prop_name]._data.c;
                GUI.widgets["OpenGL_Nuklear_UI"]["OpenGL_Nuklear_UI widget"].style.elements[i].r = (unsigned char)(255 * curr_color.r);
                GUI.widgets["OpenGL_Nuklear_UI"]["OpenGL_Nuklear_UI widget"].style.elements[i].g = (unsigned char)(255 * curr_color.g);
                GUI.widgets["OpenGL_Nuklear_UI"]["OpenGL_Nuklear_UI widget"].style.elements[i].b = (unsigned char)(255 * curr_color.b);
                GUI.widgets["OpenGL_Nuklear_UI"]["OpenGL_Nuklear_UI widget"].style.elements[i].a = (unsigned char)(255 * curr_color.a);
            }
        }

        glfwSwapBuffers(screen);
	    glfwPollEvents();
    }
    return 0;
}