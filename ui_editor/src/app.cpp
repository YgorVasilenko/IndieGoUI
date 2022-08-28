// TODO :
// 2. Fonts
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
    region<float> screen_region,
    bool bordered,
    bool titled,
    bool minimizable,
    bool scalable,
    bool movable,
    const std::string & winID
);

extern void checkUIValues(std::string winID);

extern void updateWidgetFromUI(std::string widID, std::string winID);
extern void updateUIFromWidget(std::string widID, std::string winID);
extern void useBackgroundImage(std::string widID, std::string winID, unsigned int texID);
extern unsigned int load_image(const char *filename, bool load_skinning_image = false);
extern std::list<std::string> getPaths();

extern void initWidgets();

enum STYLE_EDIT_MODE {
    widget_edit, element_edit
} style_edit_mode;

extern void addElement(
    std::string widID, 
    std::string winID, 
    std::string elt_name, 
    UI_ELEMENT_TYPE type,
    bool add_on_new_row
);

std::string skinning_img_path;
unsigned int skinning_image_id = 0;
unsigned int si_w = 0;
unsigned int si_h = 0;

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
    ui_string_group & rows_list = *UIMap["widget rows"]._data.usgPtr;
    ui_string_group & skinning_props_list = *UIMap["skinning property"]._data.usgPtr;
    ui_string_group & style_elements_list = *UIMap["style elements list"]._data.usgPtr;

	// set initial time to zero
	glfwSetTime(0.0);

    int prev_selected_widget = -1;
    int prev_selected_element = -1;
    int prev_selected_row = -1;
    int width, height;
    while (!glfwWindowShouldClose(screen)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        elements_list.elements.clear();
        rows_list.elements.clear();
        prev_selected_element = elements_list.selected_element;
        prev_selected_row = rows_list.selected_element;

        if (UIMap["load skin image"]._data.b) {
            std::string skinning_img_path = *getPaths().begin();
            // TODO : account for PROJECT_DIR variable
            load_image(skinning_img_path.c_str(), true);
            UIMap["loaded skin image"].label = skinning_img_path;
        }

        if (UIMap["add new widget"]._data.b) {
            std::string new_widget_name = *UIMap["new widget name"]._data.strPtr;
            if (new_widget_name.size() > 0 && widgets_fill.find(new_widget_name) == widgets_fill.end()) {
                region<float> new_widget_size_loc;
                new_widget_size_loc.w = UIMap["size x"]._data.f / 100.f;
                new_widget_size_loc.h = UIMap["size y"]._data.f / 100.f;
                new_widget_size_loc.x = UIMap["location x"]._data.f / 100.f;
                new_widget_size_loc.y = UIMap["location y"]._data.f / 100.f;
                createNewWidget(
                    new_widget_name,
                    new_widget_size_loc,
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
            WIDGET& w = GUI.getWidget( widgets_list.getSelected(), winID );
            if ( UIMap["add image"]._data.b ) {
                UI_ELEMENT_TYPE t = UI_IMAGE;
                std::string img_path = *getPaths().begin();
                unsigned int texID = load_image(img_path.c_str());
                addElement(widgets_list.getSelected(), winID, *UIMap["new element name"]._data.strPtr, t, !UIMap["to same row"]._data.b);
                UIMap[*UIMap["new element name"]._data.strPtr].initImage(texID, img_path);
            }
            if (UIMap["add text"]._data.b) {
                UI_ELEMENT_TYPE t = UI_STRING_LABEL;
                addElement(widgets_list.getSelected(), winID, *UIMap["new element name"]._data.strPtr, t, !UIMap["to same row"]._data.b);
            }
            if (UIMap["add slider"]._data.b) {
                UI_ELEMENT_TYPE t = UI_PROGRESS;
                addElement(widgets_list.getSelected(), winID, *UIMap["new element name"]._data.strPtr, t, !UIMap["to same row"]._data.b);
                UIMap[*UIMap["new element name"]._data.strPtr].modifyable_progress_bar = true; // hard-code for now
            }
            updateUIFromWidget(widgets_list.getSelected(), winID);
            for (auto element : w.widget_elements) {
                elements_list.elements.push_back(element);
            }
            if (prev_selected_element < elements_list.elements.size()) {
                elements_list.selected_element = prev_selected_element;
            }
            if (elements_list.selected_element != -1) {
                if (UIMap[elements_list.getSelected()].type == UI_STRING_LABEL) {
                    // modify text in selected text element
                    *UIMap["selected text"]._data.strPtr = UIMap[elements_list.getSelected()].label;
                }
            }

            for (int i = 0; i < w.layout_grid.size(); i++) {
                rows_list.elements.push_back(std::to_string(i));
            }
            if (prev_selected_row < rows_list.elements.size()) {
                rows_list.selected_element = prev_selected_row;
            }

            if (rows_list.selected_element != -1) {
                UIMap["row height"]._data.ui = w.layout_grid[rows_list.selected_element].min_height;
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

        if (UIMap["save widgets"]._data.b) {
            std::string IndieGo_home = getenv("INDIEGO_HOME");
            std::string project_name = fs::path( getenv("PROJECT_DIR") ).filename().string();
            GUI.serialize(winID, IndieGo_home + "/ui_" + project_name + ".indg", { "UI creator", "style editor" });
        }

        if (UIMap["load widgets"]._data.b) {
            std::string path = *getPaths().begin();
            GUI.deserialize(winID, path);
        }

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
                WIDGET& w = GUI.getWidget(widgets_list.getSelected(), winID);
                if (style_elements_list.selected_element != -1) {
                    if (style_edit_mode == widget_edit) {
                        w.style.elements[style_elements_list.selected_element].r = UIMap["Red property color"]._data.ui;
                        w.style.elements[style_elements_list.selected_element].g = UIMap["Green property color"]._data.ui;
                        w.style.elements[style_elements_list.selected_element].b = UIMap["Blue property color"]._data.ui;
                        w.style.elements[style_elements_list.selected_element].a = UIMap["Alpha property color"]._data.ui;
                    }
                }

                if (elements_list.selected_element != -1 && prev_selected_element == elements_list.selected_element) {
                    if (UIMap[elements_list.getSelected()].type == UI_STRING_LABEL) {
                        // modify text in selected text element
                        UIMap[elements_list.getSelected()].label = *UIMap["selected text"]._data.strPtr;
                    }
                }

                if (rows_list.selected_element != -1 && prev_selected_row == rows_list.selected_element) {
                    w.layout_grid[rows_list.selected_element].min_height = UIMap["row height"]._data.ui;
                }

                if (skinning_props_list.selected_element != -1) {
                    if (UIMap["add skin"]._data.b) {
                        region<float> crop = {
                                si_w * (UIMap["img x"]._data.f / 100.f),
                                si_h * (UIMap["img y"]._data.f / 100.f),
                                si_w * (UIMap["img size x"]._data.f / 100.f),
                                si_h * (UIMap["img size y"]._data.f / 100.f)
                        };
                        if (style_edit_mode == widget_edit) {
                            w.useSkinImage(
                                skinning_image_id,
                                si_w,
                                si_h,
                                crop,
                                (IMAGE_SKIN_ELEMENT)skinning_props_list.selected_element
                            );
                        } else {
                            if (elements_list.selected_element != -1 && prev_selected_element == elements_list.selected_element) {
                                UIMap[elements_list.getSelected()].useSkinImage(
                                    skinning_image_id,
                                    si_w,
                                    si_h,
                                    crop,
                                    (IMAGE_SKIN_ELEMENT)skinning_props_list.selected_element
                                );
                            }
                        }
                    }
                }
            }
        }

        glfwSwapBuffers(screen);
	    glfwPollEvents();
    }
    return 0;
}