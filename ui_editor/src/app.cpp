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
#include <queue>
#include <functional>

namespace fs = std::filesystem;

// Plain simple code for window creation
// Created window will hold a widget
GLFWwindow * screen;

using namespace IndieGo::UI;

Manager GUI;
WIDGET creator_widget;
WIDGET styling_widget;
std::string winID = "OpenGL_Nuklear UI";

// Global DTO for avoiding spaghetti interfaces
EditorState editorGlobals;

LayoutRect testRect = {
    0., 0., 0.5, 0.5
};

void loadShader(const std::string & name);
void initBuffers();
void initProjectDir();
void drawLayout(LayoutRect element);
extern void renderLayout();
extern void displaySkinImage();

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

std::vector<float> font_load_sizes = {
    16., 18., 20., 24., 30., 36., 42., 48., 60., 72.
};

// load_items.first -> resources path
// load_items.second -> project name
std::pair<std::string, std::string> getResourcesPath() {
    char* p = getenv("PROJECT_DIR");
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

std::queue<std::function<void()>> delayedFunctions;

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
    editorGlobals.winID = winID;

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

    // listst of data
    ui_string_group & widgets_list = *UIMap["widgets list"]._data.usgPtr;
    ui_string_group & elements_list = *UIMap["elements list"]._data.usgPtr;
    ui_string_group & rows_list = *UIMap["rows list"]._data.usgPtr;
    ui_string_group & cols_list = *UIMap["cols list"]._data.usgPtr;

	// set initial time to zero
	glfwSetTime(0.0);
    int width, height;

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


        // update screen size each frame before calling immediate backend
        glfwGetWindowSize(screen, &width, &height);
        GUI.screen_size.w = width;
        GUI.screen_size.h = height;

        // hotkey to close all windows except technical
        // if (false && ctrl_press && h_press) {
        //     for (
        //         auto widget = GUI.widgets[winID].begin();
        //         widget != GUI.widgets[winID].end();
        //         widget++
        //     ) {
        //         if (
        //             std::find(
        //                 skip_save_widgets.begin(), 
        //                 skip_save_widgets.end(), 
        //                 widget->first
        //             ) == skip_save_widgets.end()
        //         ) {
        //             widget->second.hidden = true;
        //             if (widgets_list.selected_element != -1 && widgets_list.getSelected() == widget->first) {
        //                 UIMap["visible"]._data.b = false;
        //             }
        //         }
        //     }
        // }
        if (GUI.UIMaps[winID]["w display skin image"]._data.b) {
            displaySkinImage();
        }


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

        if (widgets_list.selected_element != -1) {
            renderLayout();
        }

        glfwSwapBuffers(screen);
	    glfwPollEvents();

        while(delayedFunctions.size())
        {
            delayedFunctions.front()();
            delayedFunctions.pop();
        }
    }
    return 0;
}