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
#include <editor_test.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cassert>
#include <IndieGoUI.h>



#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;
namespace fs = filesystem;
using namespace IndieGo::UI;

extern EditorState editorGlobals;
extern vector<string> editorWidgets;
extern string winID;

extern unique_ptr<SkinningShader> skinning_shader;
extern unique_ptr<LayoutShader> layout_shader;
extern unique_ptr<ScreenQuadShader> screen_quad_shader;
extern unique_ptr<LayoutRenderer> layout_renderer;

// extern unique_ptr<SkinningShader> skinningShader;

extern string home_dir;

// TODO :
// - parametrize image and imageView creation flags

void drawLayout(LayoutRect widget_layout) {
    layout_renderer->drawLayout(widget_layout);
}

void renderLayout() {
    UI_elements_map & UIMap = Manager::UIMap;
    // listst of data
    ui_string_group & elements_list =   *UIMap["elements list"]._data.usgPtr;
    ui_string_group & rows_list =       *UIMap["rows list"]._data.usgPtr;
    ui_string_group & cols_list =       *UIMap["cols list"]._data.usgPtr;

    // draw layout of currently selected widget
    std::string widID = editorGlobals.selectedWidget;
    if (!std::any_of(editorWidgets.begin(), editorWidgets.end(), [widID](const std::string & elt) { return widID == elt; }) ) {
        float row_bias = 0.f;
        unsigned int curr_row = 0;
        for (auto row : Manager::widgets[widID].layout_grid) {
            LayoutRect widget_layout;
            if (UIMap["layout borders"]._data.b) {
                // row defines height and yPos
                // widget_layout.height = ( row.allocated_height / GUI.widgets[winID][widID].screen_size.h ) * GUI.widgets[winID][widID].screen_region.h * 2.f;
                widget_layout.height = (row.allocated_height / Manager::widgets[widID].screen_size.h) * 2.f;
                widget_layout.y = ((Manager::widgets[widID].header_height / Manager::widgets[widID].screen_size.h + row_bias * 0.5f + Manager::widgets[widID].screen_region.y + widget_layout.height * 0.25f) * 2.f - 1.f) * -1.f;
                row_bias += widget_layout.height;
            }
            float cell_bias = 0.f;
            unsigned int curr_cell = 0;
            for (auto cell : row.cells) {
                if (UIMap["layout borders"]._data.b) {
                    // cell defines width and xPos
                    widget_layout.width = cell.min_width * Manager::widgets[widID].screen_region.w * 2.f;
                    widget_layout.x = (cell_bias * 0.5f + Manager::widgets[widID].screen_region.x + widget_layout.width * 0.25f) * 2.f - 1.f;
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
                        elt_layout.x = ((UIMap[elements_list.getSelected()].layout_border.x + UIMap[elements_list.getSelected()].layout_border.w * 0.5f) / Manager::widgets[widID].screen_size.w) * 2.f - 1.f;
                        elt_layout.y = (((UIMap[elements_list.getSelected()].layout_border.y + UIMap[elements_list.getSelected()].layout_border.h * 0.5f) / Manager::widgets[widID].screen_size.h) * 2.f - 1.f) * -1.f;
                        elt_layout.width = (UIMap[elements_list.getSelected()].layout_border.w / Manager::widgets[widID].screen_size.w) * 2.f;
                        elt_layout.height = (UIMap[elements_list.getSelected()].layout_border.h / Manager::widgets[widID].screen_size.h) * 2.f;

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


void SkinningRenderer::loadTextureImage() {
    // int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(
        TEST_IMAGE_PATH,
        &texWidth, &texHeight, &texChannels, STBI_rgb_alpha
    );
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    // createTextureImages(1);
    initTextureImage(texWidth, texHeight, 4, pixels);
    vector<VkImage> newImage = { textureImages.back() };
    createTextureImageViews(newImage);
    // Create 3 samplers to align with shader init
    skinImgID = textureImageViews.back();
    editorGlobals.skinImgID = skinImgID;
    createTextureImageSamplers(textureImageViews.size());
}