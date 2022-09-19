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

// widgest style <-> UI update functions
extern void updateWidgetFromUI(std::string widID, std::string winID, bool do_styling, int styling_element, int layout_row);
extern void updateUIFromWidget(std::string widID, std::string winID, bool do_styling, int styling_element, int layout_row);

extern void switchUIscreens(std::string winID);

// element style <-> UI update functions
extern void updateElementFromUI(std::string elementName, std::string winID);
void updateUIFromElement(std::string elementName, std::string winID);

// layout <-> UI update functions
extern void updateUIFromLayout(std::string widID, std::string winID, bool upd_row = false, bool upd_col = false);
extern void updateLayoutFromUI(std::string widID, std::string winID, bool upd_row = false, bool upd_col = false);

extern void useBackgroundImage(std::string widID, std::string winID, unsigned int texID);
extern unsigned int load_image(const char *filename, bool load_skinning_image = false);
extern std::list<std::string> getPaths();

extern void initWidgets();

enum STYLE_EDIT_MODE {
    widget_edit, element_edit
} style_edit_mode;

extern void processAddOptions(std::string winID);
// extern void addElement(
//     std::string widID, 
//     std::string winID, 
//     std::string elt_name, 
//     UI_ELEMENT_TYPE type
// );

extern std::string skinning_img_path;
extern unsigned int skinning_image_id;
extern unsigned int si_w;
extern unsigned int si_h;

template<typename T> 
void useSkin(T & item, std::string winID) {
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    ui_string_group & skinning_props_list = *UIMap["skinning property"]._data.usgPtr;
    region<float> crop = {
        si_w * (UIMap["img x"]._data.f / 100.f),
        si_h * (UIMap["img y"]._data.f / 100.f),
        si_w * (UIMap["img size x"]._data.f / 100.f),
        si_h * (UIMap["img size y"]._data.f / 100.f)
    };

    item.useSkinImage(
        skinning_image_id,
        si_w,
        si_h,
        crop,
        (IMAGE_SKIN_ELEMENT)skinning_props_list.selected_element
    );
}

// load_items.first -> resources path
// load_items.second -> project name
std::pair<std::string, std::string> getResourcesPath() {
    char * p = getenv("PROJECT_DIR");
    std::string path = "";
    std::string project_name = "";
    if (p) {
        path = p;
        project_name = fs::path( path ).filename().string();
        path += "/current_scene";
    } else {
        path = getenv("INDIEGO_HOME");
    }
    return std::pair<std::string, std::string>(path, project_name);
}

ELT_PUSH_OPT push_opt = to_new_row;

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

    // listst of data
    ui_string_group & widgets_list = *UIMap["widgets list"]._data.usgPtr;
    ui_string_group & elements_list = *UIMap["elements list"]._data.usgPtr;
    ui_string_group & rows_list = *UIMap["rows list"]._data.usgPtr;
    ui_string_group & cols_list = *UIMap["cols list"]._data.usgPtr;
    // ui_string_group & skinning_props_list = *UIMap["skinning property"]._data.usgPtr;
    // ui_string_group & style_elements_list = *UIMap["style elements list"]._data.usgPtr;
    // ui_string_group& available_fonts_list = *UIMap["available fonts"]._data.usgPtr;
    // ui_string_group& font_sizes_list = *UIMap["font sizes"]._data.usgPtr;

    // control widgets
    WIDGET & widgets = GUI.getWidget("UI creator", winID);
    WIDGET & elements = GUI.getWidget("Edit elements", winID);

	// set initial time to zero
	glfwSetTime(0.0);

    int prev_selected_widget = -1;
    int prev_selected_element = -1;
    // int prev_selected_style_element = -1;
    int prev_selected_row = -1;
    int prev_selected_col = -1;
    // int prev_selected_font_size = -1;
    int width, height;
    while (!glfwWindowShouldClose(screen)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        prev_selected_element = elements_list.selected_element;
        elements_list.elements.clear();
        elements_list.unselect();

        prev_selected_row = rows_list.selected_element;
        rows_list.elements.clear();
        rows_list.unselect();

        prev_selected_col = cols_list.selected_element;
        cols_list.elements.clear();
        cols_list.unselect();

        // if (UIMap["load skin image"]._data.b) {
        //     std::string skinning_img_path = *getPaths().begin();
        //     // TODO : account for PROJECT_DIR variable
        //     load_image(skinning_img_path.c_str(), true);
        //     UIMap["loaded skin image"].label = skinning_img_path;
        // }

        if (!widgets.hidden) {
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
                // UIMap["edit widget elements"].hidden = false;
                // WIDGET& w = GUI.getWidget( widgets_list.getSelected(), winID );
                //     if ( UIMap["add image"]._data.b ) {
                //         UI_ELEMENT_TYPE t = UI_IMAGE;
                //         std::list<std::string> paths = getPaths();
                //         if (paths.size() > 0) {
                //             std::string img_path = *paths.begin();
                //             unsigned int texID = load_image(img_path.c_str());
                //             addElement(widgets_list.getSelected(), winID, *UIMap["new element name"]._data.strPtr, t, !UIMap["to same row"]._data.b);
                //             UIMap[*UIMap["new element name"]._data.strPtr].initImage(texID, img_path);
                //         }
                //     }
                //     if (UIMap["add text"]._data.b) {
                //         UI_ELEMENT_TYPE t = UI_STRING_TEXT;
                //         addElement(widgets_list.getSelected(), winID, *UIMap["new element name"]._data.strPtr, t, !UIMap["to same row"]._data.b);
                //     }
                //     if (UIMap["add slider"]._data.b) {
                //         UI_ELEMENT_TYPE t = UI_PROGRESS;
                //         addElement(widgets_list.getSelected(), winID, *UIMap["new element name"]._data.strPtr, t, !UIMap["to same row"]._data.b);
                //         UIMap[*UIMap["new element name"]._data.strPtr].modifyable_progress_bar = true; // hard-code for now
                //     }

                //     for (auto element : w.widget_elements) {
                //         elements_list.elements.push_back(element);
                //     }
                //     if (prev_selected_element < elements_list.elements.size()) {
                //         elements_list.selected_element = prev_selected_element;
                //     }
                //     if (elements_list.selected_element != -1) {
                //         updateUIFromElement(
                //             elements_list.getSelected(),
                //             winID,
                //             style_edit_mode == element_edit
                //         );
                //     }

                //     for (int i = 0; i < w.layout_grid.size(); i++) {
                //         rows_list.elements.push_back(std::to_string(i));
                //     }
                //     if (prev_selected_row < rows_list.elements.size()) {
                //         rows_list.selected_element = prev_selected_row;
                //     }
                updateUIFromWidget(
                    widgets_list.getSelected(),
                    winID,
                    style_edit_mode == widget_edit,
                    -1,
                    -1
                    // style_elements_list.selected_element,
                    // rows_list.selected_element
                );
            } /*else {
                UIMap["edit widget elements"].hidden = true;
            }*/
        }

        if (!elements.hidden) {
            // Widget's elements editng stuff
            if (UIMap["push opt"]._data.b) {
                if (push_opt == to_new_row) {
                    push_opt = to_new_col;
                    UIMap["push opt"].label = "push: to new col";
                } else if (push_opt == to_new_col) {
                    push_opt = to_new_subrow;
                    UIMap["push opt"].label = "push: to new subrow";
                } else if (push_opt == to_new_subrow) {
                    push_opt = to_new_row;
                    UIMap["push opt"].label = "push: to new row";
                }
            }
            processAddOptions(winID);
            WIDGET& w = GUI.getWidget( widgets_list.getSelected(), winID );

            // Udate displayed elements list for widget
            for (auto element : w.widget_elements) {
                elements_list.elements.push_back(element);
            }
            elements_list.selected_element = prev_selected_element;
            if (elements_list.selected_element != -1) {
                updateUIFromElement(
                    elements_list.getSelected(),
                    winID
                );
            }

            // update rows and cols lists
            for (int i = 0; i < w.layout_grid.size(); i++) {
                rows_list.elements.push_back(std::to_string(i));
            }
            rows_list.selected_element = prev_selected_row;
            if (rows_list.selected_element != -1) {
                // cols list depends on selected element from rows list
                for (int i = 0; i < w.layout_grid[ rows_list.selected_element ].cells.size(); i++) {
                    cols_list.elements.push_back(std::to_string(i));
                }
                cols_list.selected_element = prev_selected_col;
            }

            updateUIFromLayout(
                widgets_list.getSelected(), 
                winID,
                rows_list.selected_element != -1,
                cols_list.selected_element != -1
            );
        }
        // if (UIMap["style edit mode"]._data.b) {
        //     if (style_edit_mode == widget_edit) {
        //         UIMap["style edit mode"].label = "edit mode: element";
        //         style_edit_mode = element_edit;
        //     } else {
        //         UIMap["style edit mode"].label = "edit mode: widget";
        //         style_edit_mode = widget_edit;
        //     }
        // }

        // if (UIMap["save widgets"]._data.b) {
        //     // std::string IndieGo_home = getenv("INDIEGO_HOME");
        //     // std::string project_name = fs::path( getenv("PROJECT_DIR") ).filename().string();
        //     std::pair<std::string, std::string> save_items = getResourcesPath();
        //     GUI.serialize(winID, save_items.first + "/ui_" + save_items.second + ".indg", { "UI creator", "style editor" });
        // }

        // if (UIMap["load widgets"]._data.b) {
        //     std::string path = *getPaths().begin();
        //     GUI.deserialize(winID, path);

        //     // update widgets list
        //     widgets_list.elements.clear();
        //     for (auto widget : GUI.widgets[winID]) {
        //         if (widget.first == "UI creator") continue;
        //         if (widget.first == "style editor") continue;
        //         widgets_list.elements.push_back(widget.first);
        //     }
        //     available_fonts_list.elements.clear();
        //     for (auto font : GUI.loaded_fonts) {
        //         available_fonts_list.elements.push_back(font.first);
        //     }
        // }

        prev_selected_widget = widgets_list.selected_element;
        // prev_selected_style_element = style_elements_list.selected_element;
        
        // update screen size each frame before calling immediate backend
        glfwGetWindowSize(screen, &width, &height);

        GUI.screen_size.w = width;
        GUI.screen_size.h = height;

        GUI.drawFrameStart(winID);
        GUI.displayWidgets(winID);
        GUI.drawFrameEnd(winID);
        
        // back updates of widgets from ui selection
        checkUIValues(winID);
        switchUIscreens(winID);

        // if (UIMap["load font"]._data.b) {
        //     GUI.loadFont(
        //         *getPaths().begin(), 
        //         winID, 
        //         UIMap["load font size"]._data.f
        //     );
        //     // update fonts list
        //     available_fonts_list.elements.clear();
        //     for (auto font_path : GUI.loaded_fonts) {
        //         available_fonts_list.elements.push_back( fs::path(font_path.first).stem().string() );
        //     }
        // }

        // prev_selected_font_size = font_sizes_list.selected_element;
        // font_sizes_list.elements.clear();
        // if (available_fonts_list.selected_element != -1) {
        //     for (auto size : GUI.loaded_fonts[available_fonts_list.getSelected()].sizes) {
        //         font_sizes_list.elements.push_back(
        //             std::to_string(size)
        //         );
        //     }
        //     font_sizes_list.selected_element = prev_selected_font_size;
        //     // switch global font abd size to selected font and size
        //     /*GUI.main_font = available_fonts_list.getSelected();
        //     if (font_sizes_list.selected_element != -1) {
        //         GUI.main_font_size = std::stof(
        //             font_sizes_list.getSelected()
        //         );
        //     }*/
        // }

        if (!widgets.hidden) {
            if (widgets_list.selected_element != -1) {
                if (prev_selected_widget == widgets_list.selected_element) {
                    updateWidgetFromUI(
                        widgets_list.getSelected(), 
                        winID, 
                        style_edit_mode == widget_edit, 
                        -1,
                        -1
                        // style_elements_list.selected_element == prev_selected_style_element ? style_elements_list.selected_element : -1,
                        // rows_list.selected_element == prev_selected_row ? rows_list.selected_element : -1
                    );
                    // WIDGET& w = GUI.getWidget(widgets_list.getSelected(), winID);
                    // if (skinning_props_list.selected_element != -1) {
                    //     if (UIMap["add skin"]._data.b) {
                    //         if (style_edit_mode == widget_edit) {
                    //             useSkin(w, winID);
                    //         } else {
                    //             useSkin(UIMap[elements_list.getSelected()], winID);
                    //         }
                    //     }
                    // }
                }
            }
        }

        if (!elements.hidden) {
            if (elements_list.selected_element != -1 && prev_selected_element == elements_list.selected_element) {
                updateElementFromUI(
                    elements_list.getSelected(),
                    winID
                );
            }
            updateLayoutFromUI(
                widgets_list.getSelected(), 
                winID,
                rows_list.selected_element != -1 && prev_selected_row == rows_list.selected_element,
                cols_list.selected_element != -1 && prev_selected_col == cols_list.selected_element
            );
        }
        glfwSwapBuffers(screen);
	    glfwPollEvents();
    }
    return 0;
}