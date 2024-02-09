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
    
#include <filesystem>
#include <Shader.h>
#include <editor_structs.h>
#include <IndieGoUI.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cassert>

namespace fs = std::filesystem;

using namespace IndieGo::UI;

extern Manager GUI;
extern EditorState editorGlobals;
extern std::vector<std::string> editorWidgets;
extern std::string winID;

unsigned int VAO = 0;
unsigned int VBO = 0;
unsigned int EBO = 0;

Shader layoutShader;
Shader skinningShader;
extern std::string home_dir;

void loadShader(const std::string & name) {
    std::string vertex = "", geometry = "", fragment = "";
    std::string load_path = home_dir + "/../../" + name;
    for (auto f : fs::directory_iterator(load_path)) {
        if ( f.symlink_status().type() == fs::file_type::regular ) {
            // filter non-glsl files
            if (fs::path(f).extension() != ".glsl") continue;
            // check name
            if (fs::path(f).filename() == "vertex.glsl"){
                vertex = fs::path(f).string();
            }
            if (fs::path(f).filename() == "geometry.glsl"){
                geometry = fs::path(f).string();
            }
            if (fs::path(f).filename() == "fragment.glsl"){
                fragment = fs::path(f).string();
            }
        }
    }
    if (name == "layout_shader") {
        layoutShader.load(vertex.c_str(), fragment.c_str(), geometry.c_str());
        layoutShader.updateProj();
    } else {
        skinningShader.load(vertex.c_str(), fragment.c_str(), geometry.c_str());
        skinningShader.skinning = true;
        skinningShader.updateProj();
    }
};

void initBuffers() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Opengl blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void drawLayout(LayoutRect element) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

	layoutShader.use();
    
	glBufferData( GL_ARRAY_BUFFER, sizeof(LayoutRect), &element, GL_DYNAMIC_DRAW );
	glDrawArrays( GL_POINTS, 0, 1 );
}

void drawSkinImage(LayoutRect element) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

	skinningShader.use();
    
	glBufferData( GL_ARRAY_BUFFER, sizeof(LayoutRect), &element, GL_DYNAMIC_DRAW );
    GLuint err = glGetError();
	glDrawArrays( GL_POINTS, 0, 1 );
    err = glGetError();
    assert(err == 0);
}

void renderLayout() {
    region_size<unsigned int> screen_size = GUI.getWidget("Edit elements", winID).screen_size;
    region<float> main_screen_region = GUI.getWidget("Edit elements", winID).screen_region;
    region<float> skinning_screen_region = GUI.getWidget("Skinning", winID).screen_region;
    region<float> fonts_screen_region = GUI.getWidget("Fonts", winID).screen_region;
    region<float> ep_screen_region = GUI.getWidget("Element properties", winID).screen_region;

    layoutShader.main_square = { 
        screen_size.w * main_screen_region.x,
        screen_size.h - screen_size.h * (main_screen_region.y + main_screen_region.h),
        screen_size.w * (main_screen_region.x + main_screen_region.w),
        screen_size.h - screen_size.h * main_screen_region.y
    };
    layoutShader.skinning_square = { 
        screen_size.w * skinning_screen_region.x,
        screen_size.h - screen_size.h * (skinning_screen_region.y + skinning_screen_region.h),
        screen_size.w * (skinning_screen_region.x + skinning_screen_region.w),
        screen_size.h - screen_size.h * skinning_screen_region.y
    };
    layoutShader.elements_square = { 
        screen_size.w * ep_screen_region.x,
        screen_size.h - screen_size.h * (ep_screen_region.y + ep_screen_region.h),
        screen_size.w * (ep_screen_region.x + ep_screen_region.w),
        screen_size.h - screen_size.h * ep_screen_region.y
    };
    layoutShader.fonts_square = { 
        screen_size.w * fonts_screen_region.x,
        screen_size.h - screen_size.h * (fonts_screen_region.y + fonts_screen_region.h),
        screen_size.w * (fonts_screen_region.x + fonts_screen_region.w),
        screen_size.h - screen_size.h * fonts_screen_region.y
    };

    UI_elements_map & UIMap = GUI.UIMaps[winID];

    // listst of data
    ui_string_group & widgets_list = *UIMap["widgets list"]._data.usgPtr;
    ui_string_group & elements_list = *UIMap["elements list"]._data.usgPtr;
    ui_string_group & rows_list = *UIMap["rows list"]._data.usgPtr;
    ui_string_group & cols_list = *UIMap["cols list"]._data.usgPtr;

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
}