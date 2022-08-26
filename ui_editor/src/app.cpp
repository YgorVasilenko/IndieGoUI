#include <IndieGoUI.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <list>

// Plain simple code for window creation
// Created window will hold a widget
// #define WIDTH 1280
// #define HEIGHT 800

GLFWwindow * screen;

using namespace IndieGo::UI;

Manager GUI;
WIDGET creator_widget;
WIDGET styling_widget;
std::string winID = "OpenGL_Nuklear UI";

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


extern void createNewWidget(
    std::string newWidName, 
    region_size<float> size,
    region_size<float> location,
    bool bordered,
    bool titled,
    bool minimizable,
    bool scalable,
    bool movable,
    std::string & winID
);

extern void checkUIValues(std::string winID);

extern void updateWidgetFromUI(std::string widID, std::string winID);
extern void updateUIFromWidget(std::string widID, std::string winID);
extern void useBackgroundImage(std::string widID, std::string winID, unsigned int texID);
extern unsigned int load_image(const char *filename);
extern std::list<std::string> getPaths();

extern void initWidgets();

enum STYLE_EDIT_MODE {
    widget_edit, element_edit
} style_edit_mode;

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

    initWidgets();
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    ui_string_group & widgets_list = *UIMap["widgets list"]._data.usgPtr;
    ui_string_group & elements_list = *UIMap["elements list"]._data.usgPtr;
    ui_string_group & style_elements_list = *UIMap["style elements list"]._data.usgPtr;

	// set initial time to zero
	glfwSetTime(0.0);

    int prev_selected_widget = -1;
    int prev_selected_element = -1;
    int width, height;
    while (!glfwWindowShouldClose(screen)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        elements_list.elements.clear();
        prev_selected_element = elements_list.selected_element;

        if (UIMap["add new widget"]._data.b) {
            std::string new_widget_name = *UIMap["new widget name"]._data.strPtr;
            if (new_widget_name.size() > 0 && widgets_fill.find(new_widget_name) == widgets_fill.end()) {
                region_size<float> new_widget_size = {
                    UIMap["size x"]._data.f / 100.f,
                    UIMap["size y"]._data.f / 100.f
                };
                region_size<float> new_widget_location = {
                    UIMap["location x"]._data.f / 100.f,
                    UIMap["location y"]._data.f / 100.f
                };
                createNewWidget(
                    new_widget_name,
                    new_widget_size,
                    new_widget_location,
                    UIMap["bordered"]._data.b,
                    UIMap["titled"]._data.b,
                    UIMap["minimizable"]._data.b,
                    UIMap["scalable"]._data.b,
                    UIMap["movable"]._data.b,
                    winID
                );

                // add new widget to list
                widgets_list.elements.push_back(new_widget_name);
                widgets_fill[new_widget_name] = 0;
            }
        }

        if (widgets_list.selected_element != -1) {
            // TODO : move this to addElement()
            // reuse addElement() to load saved UIMap description
            // add images save, fonts update
            WIDGET& w = GUI.getWidget( widgets_list.getSelected(), winID );
            if (UIMap["add image"]._data.b) {
                std::string img_path = *getPaths().begin();
                unsigned int texID = load_image(img_path.c_str());
                if (!UIMap["to same row"]._data.b) {
                    widgets_fill[widgets_list.getSelected()]++;
                }
                UIMap.addElement(img_path, UI_IMAGE, &w, widgets_fill[widgets_list.getSelected()]);
                UIMap[img_path].initImage(texID, img_path);
                w.layout_grid[widgets_fill[widgets_list.getSelected()] - 1].min_height = w.screen_region.h * (float)w.screen_size.h;
            }
            updateUIFromWidget(widgets_list.getSelected(), winID);
            for (auto element : w.widget_elements) {
                elements_list.elements.push_back(element);
            }
            elements_list.selected_element = prev_selected_element;
            if (UIMap["add text"]._data.b) {
                std::string text_field_name = *UIMap["new element name"]._data.strPtr;
                if (text_field_name.size() > 0 && std::find(w.widget_elements.begin(), w.widget_elements.end(), text_field_name) == w.widget_elements.end()) {
                    if (!UIMap["to same row"]._data.b) {
                        widgets_fill[widgets_list.getSelected()]++;
                    }
                    UIMap.addElement(text_field_name, UI_STRING_LABEL, &w, widgets_fill[widgets_list.getSelected()]);
                }
            }

            if (UIMap["add slider"]._data.b) {
                std::string slider_field_name = *UIMap["new element name"]._data.strPtr;
                if (slider_field_name.size() > 0 && std::find(w.widget_elements.begin(), w.widget_elements.end(), slider_field_name) == w.widget_elements.end()) {
                    if (!UIMap["to same row"]._data.b) {
                        widgets_fill[widgets_list.getSelected()]++;
                    }
                    UIMap.addElement(slider_field_name, UI_PROGRESS, &w, widgets_fill[widgets_list.getSelected()]);
                    // TODO : parametrize this later
                    UIMap[slider_field_name].modifyable_progress_bar = true;
                }
            }
            if (elements_list.selected_element != -1) {
                if (UIMap[elements_list.getSelected()].type == UI_STRING_LABEL) {
                    // modify text in selected text element
                    *UIMap["selected text"]._data.strPtr = UIMap[elements_list.getSelected()].label;
                }
            }

            if (style_elements_list.selected_element != -1) {
                if (style_edit_mode == widget_edit) {
                    UIMap["Red property color"]._data.ui = w.style.elements[style_elements_list.selected_element].r;
                    UIMap["Green property color"]._data.ui = w.style.elements[style_elements_list.selected_element].g;
                    UIMap["Blue property color"]._data.ui = w.style.elements[style_elements_list.selected_element].b;
                    UIMap["Alpha property color"]._data.ui = w.style.elements[style_elements_list.selected_element].a;
                }
            }
        }

        if (UIMap["style edit mode"]._data.b) {
            if (style_edit_mode == widget_edit) {
                UIMap["style edit mode"].label = "edit mode: element";
                style_edit_mode = element_edit;
            } else {
                UIMap["style edit mode"].label = "edit mode: widget";
                style_edit_mode = widget_edit;
            }
        }

        // TODO : update style editing widget with proper element/widget style_enum values

        prev_selected_widget = widgets_list.selected_element;

        // update screen size each frame before calling immediate backend
        glfwGetWindowSize(screen, &width, &height);

        GUI.screen_size.w = width;
        GUI.screen_size.h = height;

        GUI.drawFrameStart(winID);
        GUI.displayWidgets(winID);
        GUI.drawFrameEnd(winID);
        
        // back updates of widgets from ui selection
        checkUIValues(winID);

        if (widgets_list.selected_element != -1) {
            if (prev_selected_widget == widgets_list.selected_element) {
                updateWidgetFromUI(widgets_list.getSelected(), winID);

                if (style_elements_list.selected_element != -1) {
                    if (style_edit_mode == widget_edit) {
                        WIDGET& w = GUI.getWidget( widgets_list.getSelected(), winID );
                        w.style.elements[style_elements_list.selected_element].r = UIMap["Red property color"]._data.ui;
                        w.style.elements[style_elements_list.selected_element].g = UIMap["Green property color"]._data.ui;
                        w.style.elements[style_elements_list.selected_element].b = UIMap["Blue property color"]._data.ui;
                        w.style.elements[style_elements_list.selected_element].a = UIMap["Alpha property color"]._data.ui;
                    }
                }
            }

            if (elements_list.selected_element != -1 && prev_selected_element == elements_list.selected_element) {
                if (UIMap[elements_list.getSelected()].type == UI_STRING_LABEL) {
                    // modify text in selected text element
                    UIMap[elements_list.getSelected()].label = *UIMap["selected text"]._data.strPtr;
                }
            }
        }

        glfwSwapBuffers(screen);
	    glfwPollEvents();
    }
    return 0;
}