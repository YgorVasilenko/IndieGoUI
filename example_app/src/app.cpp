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
#include <filesystem>

namespace fs = std::filesystem;

// Plain simple code for window creation
// Created window will hold loaded widget
GLFWwindow * screen;

using namespace IndieGo::UI;

// Create Manager
// Can also be created in runtime
Manager GUI;

WIDGET creator_widget;
WIDGET styling_widget;
std::string winID = "UI test app";

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

void clickCallback(std::string &) {
    std::cout << "Clickable text was clicked!" << std::endl;
}

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
    
    // Initialize Manager with window's string name and pointer to window.
    // Current backend version indeed uses GLFWwindow, but all usage can be replaced with other type
    GUI.init(winID, screen);

    // Provide screen sizes - will be used for widgets positions calculation
    GUI.screen_size.w = WIDTH;
    GUI.screen_size.h = HEIGHT;
    
    // PROJECT_DIR should contain created ui
    char * p = getenv("PROJECT_DIR");
    if (!p) {
        std::cout << "[ERROR] PROJECT_DIR env var is not set! Please, run <IndieGoUI dir>\\env.ps1 and start this app from same terminal," << std::endl;
        std::cout << "Or set as system environment variable. PROJECT_DIR should point to <IndieGoUI dir>\\example_app" << std::endl;
        return -1;
    }
    std::string path = p;
    if (!fs::exists(path + "/ui_example_app.indg")) {
        std::cout << "[ERROR] " << path << "is not <IndieGoUI dir>\\example_app or ui_example_app.indg was deleted!" << std::endl;
        std::cout << "Please, make sure PROJECT_DIR env var points to <IndieGoUI dir>\\example_app and ui_example_app.indg exists." << std::endl;
        return -1;
    }

    // Init GUI.project_dir here. Will be used for loading images and fonts
    GUI.project_dir = path;
    GUI.deserialize(winID, path + "/ui_example_app.indg");

    // get reference for UIMap
    UI_elements_map & UIMap = GUI.UIMaps[winID];

    // Update text so it will be aligned to left border
    UIMap["FPS counter"].text_align = LEFT;
    UIMap["FPS counter"].label = "This is some text with [color=#0011ff]clickable[/color] region";
    
    // Clickable text setup
    // -------------------------------------------------
    TextClickData clickData;
    clickData.click_region.h = 22;
    clickData.click_region.w = 31;
    clickData.clickCallback = clickCallback;
    UIMap["FPS counter"].clickable_regions.push_back(
        clickData
    );
    UIMap["FPS counter"].hasClickableText = true;
    // -------------------------------------------------

	// set initial time to zero
	glfwSetTime(0.0);
    int width, height;
    unsigned int fps_counter = 0;
    while (!glfwWindowShouldClose(screen)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Check ui element state
        if (UIMap["press me"]._data.b) {

            // Do stuff
            std::cout << "[INFO] button pressed!" << std::endl;
        }

        // update screen size each frame before calling immediate backend
        glfwGetWindowSize(screen, &width, &height);

        // Update screen sizes, if screen changed
        GUI.screen_size.w = width;
        GUI.screen_size.h = height;

        // Call frame updates!
        GUI.drawFrameStart(winID);
        GUI.displayWidgets(winID);
        GUI.drawFrameEnd(winID);

        glfwSwapBuffers(screen);
	    glfwPollEvents();

        fps_counter++;
        if (glfwGetTime() >= 1.f) {
            glfwSetTime(0.0);
            // UIMap["FPS counter"].label = "b";
            fps_counter = 0;
        }
    }
    return 0;
}