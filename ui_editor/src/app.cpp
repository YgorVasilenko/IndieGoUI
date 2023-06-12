/*  runtime state-saving Map for Immediate-mode libraries
    Copyright (C) 2022  Igor Vasilenko

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.*/

#include <IndieGoUI.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <list>
#include <filesystem>
#include <editor_structs.h>
#include <Shader.h>
// #include <editor_data.pb.h>

namespace fs = std::filesystem;

// Plain simple code for window creation
// Created window will hold a widget
GLFWwindow * screen;

using namespace IndieGo::UI;

Manager GUI;
WIDGET creator_widget;
WIDGET styling_widget;
std::string winID = "OpenGL_Nuklear UI";

LayoutRect testRect = {
    0., 0., 0.5, 0.5
};

void loadShader(const std::string & name);
void initBuffers();
void initProjectDir();
void drawLayout(LayoutRect element);
// void drawSkinImage(LayoutRect element);

extern void serializeCropsData(const std::string & path);
extern void deserializeCropsData(const std::string & path);

// [widID] = current_line
std::map<std::string, unsigned int> widgets_fill;
extern Shader skinningShader;
extern Shader layoutShader;

// hotkey combination
bool ctrl_press = false;
bool h_press = false;

// Window callbacks
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

    if (glfwGetKey(window, key) == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT_CONTROL)
            ctrl_press = true;

        if (key == GLFW_KEY_H)
            h_press = true;
    } else {
        if (key == GLFW_KEY_LEFT_CONTROL)
            ctrl_press = false;

        if (key == GLFW_KEY_H)
            h_press = false;
    }
}

void window_size_callback(GLFWwindow* window, int width, int height) {
    skinningShader.viewWidth = width;
    skinningShader.viewHeight = height;
    skinningShader.updateProj();

    layoutShader.viewWidth = width;
    layoutShader.viewHeight = height;
    layoutShader.updateProj();
}

unsigned int buttons_count = 1;

double curr_time = 0.0, prev_time = 0.0;
unsigned int frames = 0;

// Global skinning image draw data
// ---------------------------------------
extern LayoutRect skin_img_rect;
extern LayoutRect crop_img_rect;
// ---------------------------------------

extern void createNewWidget(
    std::string newWidName, 
    region<float> screen_region,
    bool bordered,
    bool titled,
    bool minimizable,
    bool scalable,
    bool movable,
    bool has_scrollbar,
    const std::string & winID
);

extern void checkUIValues(std::string winID);

// widgest style <-> UI update functions
extern void updateWidgetFromUI(std::string widID, std::string winID, bool do_styling, int styling_element);
extern void updateUIFromWidget(std::string widID, std::string winID, bool do_styling, int styling_element);

extern void switchUIscreens(std::string winID);

// element style <-> UI update functions
extern void updateElementFromUI(std::string elementName, std::string winID, std::string widID);
void updateUIFromElement(std::string elementName, std::string winID);

// layout <-> UI update functions
extern void updateUIFromLayout(std::string widID, std::string winID, bool upd_row = false, bool upd_col = false);
extern void updateLayoutFromUI(std::string widID, std::string winID, bool upd_row = false, bool upd_col = false);

// extern std::list<std::string> getPaths();
extern std::vector<std::string> getPaths(
    bool single_item = false,
    bool select_folders = false,
    std::string start_folder = "None"
);

extern void initWidgets();
extern void processSkinning(int prev_selected_crop = -1);

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
    "Fonts",
    "Skinning",
    "Element properties"
};

std::vector<float> font_load_sizes = {
    16., 18., 20., 24., 30., 36., 42., 48., 60., 72.
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
    } else {
        p = getenv("INDIEGO_HOME");
        if (p)
            path = p;
        else
            path = fs::current_path().string();
    }
    return std::pair<std::string, std::string>(path, project_name);
}

extern ELT_PUSH_OPT push_opt;

#ifdef _WIN32 // it seems there is no cpp cross-platform way to get executable path
#include <windows.h>
#endif

fs::path binary_path = "";
std::string home_dir = "";

extern std::vector<std::string> editorWidgets;

int main() {
    // Get current application path
#ifdef _WIN32
    TCHAR binary_path_[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, binary_path_, MAX_PATH);
    binary_path = std::string(binary_path_);
#else
    home_dir = fs::current_path();
#endif
    home_dir = binary_path.parent_path().string();

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
    glfwSetWindowSizeCallback(screen, window_size_callback);

    glViewport(0, 0, WIDTH, HEIGHT);

    initBuffers();
    loadShader("layout_shader");
    loadShader("skinning_shader");

    GUI.init(winID, screen);
    GUI.screen_size.w = WIDTH;
    GUI.screen_size.h = HEIGHT;

    initWidgets();
    initProjectDir();
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    *UIMap["project_dir_path"]._data.strPtr = GUI.project_dir;

    // listst of data
    ui_string_group & widgets_list = *UIMap["widgets list"]._data.usgPtr;
    ui_string_group & elements_list = *UIMap["elements list"]._data.usgPtr;
    ui_string_group & rows_list = *UIMap["rows list"]._data.usgPtr;
    ui_string_group & cols_list = *UIMap["cols list"]._data.usgPtr;
    
    ui_string_group & fonts_list = *UIMap["loaded fonts"]._data.usgPtr;
    ui_string_group & font_sizes_list = *UIMap["font sizes"]._data.usgPtr;

    ui_string_group & skin_crops_list = *UIMap["w skin crops list"]._data.usgPtr;

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
    int prev_selected_crop = -1;
    int width, height;
    std::string selected_for_font_update = "None";
    bool update_widget_font = false;

    // Skinning data initialization
    // -------------------------------
    skin_img_rect.x = 0.f;
    skin_img_rect.y = 0.f;
    skin_img_rect.width = 1.f;
    skin_img_rect.height = 1.f;
    skin_img_rect.red = 1.f;
    skin_img_rect.green = 1.f;
    skin_img_rect.blue = 0.f;
    skin_img_rect.alpha = 0.f;
    // -------------------------------
    crop_img_rect.x = 0.25f;
    crop_img_rect.y = 0.25f;
    crop_img_rect.width = 0.5f;
    crop_img_rect.height = 0.5f;
    crop_img_rect.red = 1.f;
    crop_img_rect.green = 1.f;
    crop_img_rect.blue = 1.f;
    crop_img_rect.alpha = 0.f;
    // -------------------------------

    while (!glfwWindowShouldClose(screen)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if (e_skinning_props_list.selected_element != -1 && UIMap["e apply skin"]._data.b) {
            TexData td = Manager::load_image(UIMap["e skin image path"].label);
            GUI.skinning_image = UIMap["e skin image path"].label;
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

        /*if (UIMap["e load skin image"]._data.b || UIMap["w load skin image"]._data.b) {
            std::vector<std::string> paths = getPaths(false, false, GUI.project_dir);
            if (paths.size() > 0) {
                std::string skinning_img_path = *paths.begin();
                TexData skin_tex = Manager::load_image(skinning_img_path.c_str());
                UIMap["w skin image path"].label = skinning_img_path;
                UIMap["e skin image path"].label = skinning_img_path;
                skinningShader.skin_tex_id = skin_tex.texID;
                skin_img_rect.height = (float)skin_tex.h / (float)skin_tex.w;
            }
        }*/

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
                        UIMap["has scrollbar"]._data.b,
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
        if (UIMap["apply font"]._data.b && fonts_list.selected_element != -1 && font_sizes_list.selected_element != -1) {
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

            if (elements_list.selected_element == -1)
                UIMap["switch type"]._data.b = false;

            if (UIMap["switch type"]._data.b) {
                UIMap["add image"].label = "to image";
                UIMap["add text"].label = "to text";    
                UIMap["add label"].label = "to label";
                UIMap["add progress"].label = "to progress";
                UIMap["add button"].label = "to button";
                UIMap["add checkbox"].label = "to checkbox";
                UIMap["add empty"].label = "to empty";
                UIMap["add input"].label = "to input";
                UIMap["add int"].label = "to int";
                UIMap["add float"].label = "to float";
            } else {
                UIMap["add image"].label = "add image";
                UIMap["add text"].label = "add text";
                UIMap["add label"].label = "add label";
                UIMap["add progress"].label = "add progress";
                UIMap["add button"].label = "add button";
                UIMap["add checkbox"].label = "add checkbox";
                UIMap["add empty"].label = "add empty";
                UIMap["add input"].label = "add input";
                UIMap["add int"].label = "add int";
                UIMap["add float"].label = "add float";
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
            std::vector<std::string> paths = getPaths(true, false, GUI.project_dir);
            if (paths.size() > 0) {
                GUI.serialize(
                    winID,
                    paths.front(),
                    skip_save_widgets
                );
                std::string editor_file_name = fs::path( paths.front() ).stem().string();
                std::string editor_file = fs::path( paths.front() ).parent_path().append(editor_file_name + "_editor.ui").string();
                serializeCropsData(editor_file);
            }
        }

        if (UIMap["load ui"]._data.b) {
            std::vector<std::string> paths = getPaths(false, false, GUI.project_dir);
            for (auto path : paths) {
                GUI.deserialize(winID, path);
                std::string editor_file_name = fs::path( paths.front() ).stem().string();
                std::string editor_file = fs::path( paths.front() ).parent_path().append(editor_file_name + "_editor.ui").string();
                if (fs::exists(editor_file)) {
                    deserializeCropsData(editor_file);
                }
                
                for (auto widget = GUI.widgets[winID].begin(); widget != GUI.widgets[winID].end(); widget++) {
                    if (std::find(skip_save_widgets.begin(), skip_save_widgets.end(), widget->first) != skip_save_widgets.end())
                        continue;
                    
                    widget->second.hidden = true;
                    // don't add already added elements
                    if (std::find(widgets_list.elements.begin(), widgets_list.elements.end(), widget->first) != widgets_list.elements.end())
                        continue;
                    widgets_list.elements.push_back(widget->first);
                }
                for (auto font : GUI.loaded_fonts) {
                    if (font.first == GUI.main_font) 
                        continue;

                    if (std::find(fonts_list.elements.begin(), fonts_list.elements.end(), font.first) != fonts_list.elements.end())
                        continue;

                    fonts_list.elements.push_back(font.first);
                }
                // set skinning image texID if loaded, populate crops list 
                if (GUI.skinning_image != "None") {
                    TexData skin_tex = Manager::load_image(GUI.skinning_image);
                    skinningShader.skin_tex_id = skin_tex.texID;
                    skin_img_rect.width = 1.f;
                    skin_img_rect.height = (float)skin_tex.h / (float)skin_tex.w;
                }
            }
        }

        // update screen size each frame before calling immediate backend
        glfwGetWindowSize(screen, &width, &height);

        GUI.screen_size.w = width;
        GUI.screen_size.h = height;

        if (UIMap["select_project_dir"]._data.b) {
            std::vector<std::string> paths = getPaths(true, true);
            if (paths.size() > 0) {
                GUI.project_dir = paths[0];
                *UIMap["project_dir_path"]._data.strPtr = GUI.project_dir;
            }
        }

        processSkinning(prev_selected_crop);

        prev_selected_crop = skin_crops_list.selected_element;

        // hotkey to close all windows except technical
        if (ctrl_press && h_press) {
            for (
                auto widget = GUI.widgets[winID].begin();
                widget != GUI.widgets[winID].end();
                widget++
            ) {
                if (
                    std::find(
                        skip_save_widgets.begin(), 
                        skip_save_widgets.end(), 
                        widget->first
                    ) == skip_save_widgets.end()
                ) {
                    widget->second.hidden = true;
                    if (widgets_list.selected_element != -1 && widgets_list.getSelected() == widget->first) {
                        UIMap["visible"]._data.b = false;
                    }
                }
            }
        }

        GUI.drawFrameStart(winID);
        GUI.displayWidgets(winID);
        GUI.drawFrameEnd(winID);

        // back updates of widgets from ui selection
        checkUIValues(winID);
        switchUIscreens(winID);

        if (UIMap["load font"]._data.b) {
            std::vector<std::string> paths = getPaths(false, false, GUI.project_dir);
            if (paths.size() > 0) {
                // load several sizes
                for (auto size : font_load_sizes) {
                    GUI.loadFont(
                        paths[0],
                        winID, 
                        size,
                        true
                    );
                }
                // additionally load specified font
                GUI.loadFont(
                    paths[0],
                    winID, 
                    UIMap["load size"]._data.f,
                    true
                );
                // update fonts list
                fonts_list.elements.clear();
                for (auto font_path : GUI.loaded_fonts) {
                    if (fs::path(font_path.first).stem().string() == GUI.main_font) 
                        continue; // don't display main font
                    fonts_list.elements.push_back( fs::path(font_path.first).stem().string() );
                }
            }
        }

        font_sizes_list.elements.clear();
        if (fonts_list.selected_element != -1) {
            for (auto size : GUI.loaded_fonts[fonts_list.getSelected()].sizes) {
                font_sizes_list.elements.push_back(
                    std::to_string(size)
                );
            }
        }

        if (widgets_list.selected_element != -1) {
            // draw layout of currently selected widget
            std::string widID = widgets_list.getSelected();
            if (!std::any_of(editorWidgets.begin(), editorWidgets.end(), [widID](const std::string & elt) { return widID == elt; }) ) {
                float row_bias = 0.f;
                unsigned int curr_row = 0;
                for (auto row : GUI.widgets[winID][widID].layout_grid) {
                    LayoutRect widget_layout;
                    if (UIMap["layout borders"]._data.b) {
                        // row defines height and yPos
                        // widget_layout.height = ( row.allocated_height / GUI.widgets[winID][widID].screen_size.h ) * GUI.widgets[winID][widID].screen_region.h * 2.f;
                        widget_layout.height = (row.allocated_height / GUI.widgets[winID][widID].screen_size.h) * 2.f;
                        widget_layout.y = ((GUI.widgets[winID][widID].header_height / GUI.widgets[winID][widID].screen_size.h + row_bias * 0.5f + GUI.widgets[winID][widID].screen_region.y + widget_layout.height * 0.25f) * 2.f - 1.f) * -1.f;
                        row_bias += widget_layout.height;
                    }
                    float cell_bias = 0.f;
                    unsigned int curr_cell = 0;
                    for (auto cell : row.cells) {
                        if (UIMap["layout borders"]._data.b) {
                            // cell defines width and xPos
                            widget_layout.width = cell.min_width * GUI.widgets[winID][widID].screen_region.w * 2.f;
                            widget_layout.x = (cell_bias * 0.5f + GUI.widgets[winID][widID].screen_region.x + widget_layout.width * 0.25f) * 2.f - 1.f;
                            cell_bias += widget_layout.width;
                            if (cols_list.selected_element == curr_cell && rows_list.selected_element == curr_row) {
                                widget_layout.red = 1.f;
                                widget_layout.green = 1.f;
                                widget_layout.blue = 0.f;
                            } else if (rows_list.selected_element == curr_row) {
                                widget_layout.red = 1.f;
                                widget_layout.green = 0.f;
                                widget_layout.blue = 0.f;
                            } else {
                                widget_layout.red = 1.f;
                                widget_layout.green = 1.f;
                                widget_layout.blue = 1.f;
                            }
                            drawLayout(widget_layout);
                        }
                        curr_cell++;
                        if (elements_list.selected_element != -1 && std::find(cell.elements.begin(), cell.elements.end(), elements_list.getSelected()) != cell.elements.end()) {
                            if (UIMap["element borders"]._data.b) {
                                LayoutRect elt_layout;
                                elt_layout.x = ((UIMap[elements_list.getSelected()].layout_border.x + UIMap[elements_list.getSelected()].layout_border.w * 0.5f) / GUI.widgets[winID][widID].screen_size.w) * 2.f - 1.f;
                                elt_layout.y = (((UIMap[elements_list.getSelected()].layout_border.y + UIMap[elements_list.getSelected()].layout_border.h * 0.5f) / GUI.widgets[winID][widID].screen_size.h) * 2.f - 1.f) * -1.f;
                                elt_layout.width = (UIMap[elements_list.getSelected()].layout_border.w / GUI.widgets[winID][widID].screen_size.w) * 2.f;
                                elt_layout.height = (UIMap[elements_list.getSelected()].layout_border.h / GUI.widgets[winID][widID].screen_size.h) * 2.f;

                                elt_layout.red = 0.f;
                                elt_layout.green = 1.f;
                                elt_layout.blue = 0.f;

                                drawLayout(elt_layout);
                            }
                        }
                    }
                    curr_row++;
                }
            }



            if (prev_selected_widget == widgets_list.selected_element) {
                updateWidgetFromUI(
                    widgets_list.getSelected(), 
                    winID, 
                    style_edit_mode == widget_edit, 
                    style_elements_list.selected_element == prev_selected_style_element ? style_elements_list.selected_element : -1
                );
            } else {
                rows_list.unselect();
                cols_list.unselect();
            }
        }

        if (!elements.hidden) {
            if (elements_list.selected_element != -1 && prev_selected_element == elements_list.selected_element) {
                updateElementFromUI(
                    elements_list.getSelected(),
                    winID,
                    widgets_list.getSelected()
                );
            }

            if (prev_selected_row != rows_list.selected_element)
                cols_list.unselect();

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