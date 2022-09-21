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
extern void updateWidgetFromUI(std::string widID, std::string winID, bool do_styling, int styling_element);
extern void updateUIFromWidget(std::string widID, std::string winID, bool do_styling, int styling_element);

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

std::vector<std::string> skip_save_widgets = { 
    "UI creator", 
    "Edit elements", 
    "Widgets style", 
    "Elements style",
    "Fonts"
};

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
    
    ui_string_group & fonts_list = *UIMap["loaded fonts"]._data.usgPtr;
    ui_string_group & font_sizes_list = *UIMap["font sizes"]._data.usgPtr;

    ui_string_group & style_elements_list = *UIMap["styling elements"]._data.usgPtr;
    ui_string_group & w_skinning_props_list = *UIMap["w skinning properties"]._data.usgPtr;
    ui_string_group & e_skinning_props_list = *UIMap["e skinning properties"]._data.usgPtr;

    // control widgets
    WIDGET & widgets = GUI.getWidget("UI creator", winID);
    WIDGET & elements = GUI.getWidget("Edit elements", winID);

	// set initial time to zero
	glfwSetTime(0.0);

    int prev_selected_widget = -1;
    int prev_selected_element = -1;
    int prev_selected_style_element = -1;
    int prev_selected_row = -1;
    int prev_selected_col = -1;
    // int prev_selected_font_size = -1;
    int width, height;
    std::string selected_for_font_update = "None";
    bool update_widget_font = false;

    while (!glfwWindowShouldClose(screen)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if (e_skinning_props_list.selected_element != -1 && UIMap["e apply skin"]._data.b) {
            TexData td = Manager::load_image(UIMap["e skin image path"].label);
            UI_element& e = UIMap[elements_list.getSelected()];
            region<float> crop;
            crop.x = UIMap["e crop x"]._data.f / UI_FLT_VAL_SCALE;
            crop.y = UIMap["e crop y"]._data.f / UI_FLT_VAL_SCALE;
            crop.w = UIMap["e crop w"]._data.f / UI_FLT_VAL_SCALE;
            crop.h = UIMap["e crop h"]._data.f / UI_FLT_VAL_SCALE;
            e.useSkinImage(
                td.texID,
                td.w,
                td.h,
                crop,
                (IMAGE_SKIN_ELEMENT)e_skinning_props_list.selected_element
            );
        }

        prev_selected_element = elements_list.selected_element;
        elements_list.elements.clear();
        elements_list.unselect();

        prev_selected_row = rows_list.selected_element;
        rows_list.elements.clear();
        rows_list.unselect();

        prev_selected_col = cols_list.selected_element;
        cols_list.elements.clear();
        cols_list.unselect();

        if (UIMap["e load skin image"]._data.b || UIMap["w load skin image"]._data.b) {
            std::list<std::string> paths = getPaths();
            if (paths.size() > 0) {
                std::string skinning_img_path = *paths.begin();
                // TODO : account for PROJECT_DIR variable
                TexData skin_tex = Manager::load_image(skinning_img_path.c_str());
                UIMap["w skin image path"].label = skinning_img_path;
                UIMap["e skin image path"].label = skinning_img_path;
            }
        }

        if (widgets_list.selected_element != -1) {
            updateUIFromWidget(
                widgets_list.getSelected(),
                winID,
                style_edit_mode == widget_edit,
                style_elements_list.selected_element
            );

            WIDGET& w = GUI.getWidget(widgets_list.getSelected(), winID);

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
                for (int i = 0; i < w.layout_grid[rows_list.selected_element].cells.size(); i++) {
                    cols_list.elements.push_back(std::to_string(i));
                }
                cols_list.selected_element = prev_selected_col;
            }

        }

        if (!widgets.hidden) {
            elements_list.unselect();
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
                selected_for_font_update = widgets_list.getSelected();
                update_widget_font = true;
            }
        } else {
            if (elements_list.selected_element != -1) {
                selected_for_font_update = elements_list.getSelected();
                update_widget_font = false;
            }
        }

        UIMap["selected for font update"].label = "selected: " + selected_for_font_update;
        if (UIMap["apply font"]._data.b) {
            if (selected_for_font_update != "None") {
                if (update_widget_font) {
                    WIDGET& w = GUI.getWidget(selected_for_font_update, winID);
                    w.font = fonts_list.getSelected();
                    if (font_sizes_list.selected_element != -1)
                        w.font_size = std::stof(font_sizes_list.getSelected());
                    else 
                        w.font_size = std::stof(*font_sizes_list.elements.begin());
                } else {
                    UI_element& e = UIMap[selected_for_font_update];
                    e.font = fonts_list.getSelected();
                    if (font_sizes_list.selected_element != -1)
                        e.font_size = std::stof(font_sizes_list.getSelected());
                    else
                        e.font_size = std::stof(*font_sizes_list.elements.begin());
                }
            }
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
            updateUIFromLayout(
                widgets_list.getSelected(), 
                winID,
                rows_list.selected_element != -1,
                cols_list.selected_element != -1
            );
        }

        prev_selected_widget = widgets_list.selected_element;
        prev_selected_style_element = style_elements_list.selected_element;
        

        if (UIMap["save ui"]._data.b) {
            std::string save_path = getenv("PROJECT_DIR");
            GUI.serialize(
                winID,
                save_path + "/ui_test.indg",
                skip_save_widgets
            );
        }

        if (UIMap["load ui"]._data.b) {
            std::string load_path = *getPaths().begin();
            GUI.deserialize(winID, load_path);
            // TODO : 
            // - add all loaded widgets to widgets_list
            // - check skinning save/load
            // - check fonts save/load
            // - check styling save/load
            // - use getResourcesPaths() to get save/load location
        }

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

        if (UIMap["load font"]._data.b) {
            GUI.loadFont(
                *getPaths().begin(), 
                winID, 
                UIMap["load size"]._data.f
            );
            // update fonts list
            fonts_list.elements.clear();
            for (auto font_path : GUI.loaded_fonts) {
                fonts_list.elements.push_back( fs::path(font_path.first).stem().string() );
            }
        }

        if (w_skinning_props_list.selected_element != -1 && UIMap["w apply skin"]._data.b) {
            TexData td = Manager::load_image(UIMap["w skin image path"].label);
            WIDGET & w = GUI.getWidget(
                widgets_list.getSelected(), 
                winID
            );
            region<float> crop;
            crop.x = UIMap["w crop x"]._data.f / UI_FLT_VAL_SCALE;
            crop.y = UIMap["w crop y"]._data.f / UI_FLT_VAL_SCALE;
            crop.w = UIMap["w crop w"]._data.f / UI_FLT_VAL_SCALE;
            crop.h = UIMap["w crop h"]._data.f / UI_FLT_VAL_SCALE;
            w.useSkinImage(
                td.texID, 
                td.w, 
                td.h, 
                crop, 
                (IMAGE_SKIN_ELEMENT)w_skinning_props_list.selected_element
            );
        }

        // prev_selected_font_size = font_sizes_list.selected_element;
        font_sizes_list.elements.clear();
        if (fonts_list.selected_element != -1) {
            for (auto size : GUI.loaded_fonts[fonts_list.getSelected()].sizes) {
                font_sizes_list.elements.push_back(
                    std::to_string(size)
                );
            }
             // font_sizes_list.selected_element = prev_selected_font_size;
             // switch global font abd size to selected font and size
             /*GUI.main_font = available_fonts_list.getSelected();
             if (font_sizes_list.selected_element != -1) {
                 GUI.main_font_size = std::stof(
                     font_sizes_list.getSelected()
                 );
             }*/
        }

        if (widgets_list.selected_element != -1) {
            if (prev_selected_widget == widgets_list.selected_element) {
                updateWidgetFromUI(
                    widgets_list.getSelected(), 
                    winID, 
                    style_edit_mode == widget_edit, 
                    style_elements_list.selected_element == prev_selected_style_element ? style_elements_list.selected_element : -1
                );
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