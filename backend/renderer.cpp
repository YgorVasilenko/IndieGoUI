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
#include <string>
#include <regex>
#include <filesystem>
#include <unordered_map>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_VULKAN_IMPLEMENTATION

#include "nuklear.h"
#include "nuklear_glfw_vulkan.h"

#include <Renderer.h>

using namespace IndieGo::vkI;
using namespace IndieGo::vkI::aux;

using namespace std;
namespace fs = filesystem;

shared_ptr<vkRenderer> renderer;

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

// font_name = font_size : <fontPtr>
map<string, map<float, nk_font *>> backend_loaded_fonts;

// texture_id = vector<sub_images>
unordered_map<void *, vector<pair<struct nk_image, IndieGo::UI::region<float>>>> images;

// Use winID to store window's context. If window destroyed and re-initializes
// context should be same for respective winID
// unordered_map<string, nk_glfw*> glfw_storage;

unordered_map<string, nk_font> fonts;

nk_context * ctx;
nk_font_atlas * atlas;
VkSemaphore nk_semaphore;
uint32_t image_index;
VkImageView font_image_view;
nk_draw_null_texture * tex_null_ptr;

// ----------------------------------------------------------
using namespace IndieGo::UI;
void (*Manager::custom_ui_uniforms)(void*) = 0;
void * Manager::uniforms_data_ptr = NULL;
int Manager::draw_idx = 0;

float apply_highlight_indices[50] = { 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
float apply_shading_indices[50] = { 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
int last_apply_highlight_idx = -1;
int last_apply_shading_idx = -1;

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

void prepareUIRenderer(GLFWwindow* window) {
    QueueFamilyIndices indices = IndieGo::vkI::aux::findQueueFamilies(vkRenderer::physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    nk_glfw * glfw_vulkan = nk_glfw3_init(
        window, 
        vkRenderer::device, 
        vkRenderer::physicalDevice,
        queueFamilyIndices[0],
        renderer->textureImageViews.data(),
        vkRenderer::swapChainImagesCount,
        renderer->imageViewInitFormat,
        NK_GLFW3_INSTALL_CALLBACKS,
        MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER
    );
    ctx = &glfw_vulkan->ctx;
    font_image_view = glfw_vulkan->vulkan.font_image_view;
    tex_null_ptr = &glfw_vulkan->vulkan.tex_null;

    nk_glfw3_font_stash_begin(&atlas);
    // load MercutioNbp
    std::vector<float> sizes = { 16, 18, 20, 24, 30, 36, 42, 48, 60, 72 };
    std::string path = "C:\\Users\\vasil\\IndieGo\\ElvenCitySimulator\\MercutioNbpBasic.ttf";

    for (auto size : sizes) {
       backend_loaded_fonts["MercutioNbpBasic"][size] = nk_font_atlas_add_from_file(
            atlas,
            path.c_str(), size, 0
        );
    }

    nk_glfw3_font_stash_end(vkRenderer::graphicsQueue);
    nk_style_set_font(ctx, &backend_loaded_fonts["MercutioNbpBasic"][18.f]->handle);
}


using namespace IndieGo::UI;
void (*Manager::buttonClickCallback)(void*) = NULL;
void (*Manager::disabledButtonClickCallback)(void*) = NULL;
void (*Manager::checkboxClickCallback)(void*) = NULL;

void Manager::scroll(GLFWwindow * window, double xoff, double yoff) {
    nk_gflw3_scroll_callback(window, xoff, yoff);
}

#ifndef NK_GLFW_DOUBLE_CLICK_LO
#define NK_GLFW_DOUBLE_CLICK_LO 0.02
#endif
#ifndef NK_GLFW_DOUBLE_CLICK_HI
#define NK_GLFW_DOUBLE_CLICK_HI 0.2
#endif


void Manager::mouse_button(GLFWwindow * window, int button, int action, int mods){
    nk_glfw3_mouse_button_callback(window, button, action, mods);
}

void Manager::char_input(GLFWwindow * window, unsigned int codepoint) {
    nk_glfw3_char_callback(window, codepoint);
}

void Manager::key_input(GLFWwindow *win, int key, int scancode, int action, int mods) {
    nk_glfw3_key_callback(win, key, scancode, action, mods);
}

// Memory for string input storage
char text[512] = {};
int text_len;

void stringToText(string & str) {
    for (int i = 0; i < str.length(); i++)
        text[i] = str[i];
    text_len = str.length();
}

void textToString(string & str) {
    str.clear();
    if (text_len > 0) {
        for (int i = 0; i < text_len; i++) {
            str.push_back(text[i]);
        }
    }
}

//--------------------------------------------------------
//
//            Core concept.
// callUIfunction abstracts away different UI elements.
//
//-------------------------------------------------------

int max_shading_idx = -1;

map<string, int> debug_array;
void UI_element::callUIfunction(float x, float y, float space_w, float space_h) {
    if (apply_highlight) {
        last_apply_highlight_idx++;
        apply_highlight_indices[last_apply_highlight_idx] = Manager::draw_idx;
    }
    if (apply_shading) {
        last_apply_shading_idx++;
        apply_shading_indices[last_apply_shading_idx] = Manager::draw_idx;
        if (max_shading_idx < last_apply_shading_idx)
            max_shading_idx = last_apply_shading_idx;
    }

    if (font != "None") {
        nk_style_set_font(
            ctx,
            &backend_loaded_fonts[font][font_size]->handle
        );
    }

    // prepare space for element
    nk_layout_space_push(
        ctx, 
        nk_rect(
            x + space_w * width * padding.w, // left border
            y + space_h * height * padding.h,
            space_w * width - (space_w * width * padding.w * 2.f), // right border
            space_h * height - (space_h * height * padding.h * 2.f)
        )
    );

    string full_name;
    nk_bool nk_val;
    static const float ratio[] = { 100, 120 };
    float dbgVal;
    if (type == UI_BOOL) {
        // // ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        if (skinned_style.props[checkbox_normal].first != nullptr) {
            ctx->style.checkbox.normal = nk_style_item_image(
                images[skinned_style.props[checkbox_normal].first][skinned_style.props[checkbox_normal].second].first
            );
        }
        if (skinned_style.props[checkbox_hover].first != nullptr) {
            ctx->style.checkbox.hover = nk_style_item_image(
                images[skinned_style.props[checkbox_hover].first][skinned_style.props[checkbox_hover].second].first
            );
        }
        if (skinned_style.props[checkbox_active].first != nullptr) {
            ctx->style.checkbox.active = nk_style_item_image(
                images[skinned_style.props[checkbox_active].first][skinned_style.props[checkbox_active].second].first
            );
        }
        if (skinned_style.props[checkbox_cursor].first != nullptr) {
            ctx->style.checkbox.cursor_normal = nk_style_item_image(
                images[skinned_style.props[checkbox_cursor].first][skinned_style.props[checkbox_cursor].second].first
            );
            ctx->style.checkbox.cursor_hover = nk_style_item_image(
                images[skinned_style.props[checkbox_cursor].first][skinned_style.props[checkbox_cursor].second].first
            );
        }

        nk_val = _data.b;
        nk_checkbox_label(ctx, label.c_str(), &nk_val);
        if (nk_val != _data.b) {
            _data.b = nk_val;
            // evoke active callbacks
            unsigned int cbIdx = 0;
            for (auto callback : activeCallbacks) {
                callback(activeDatas[cbIdx]);
                cbIdx++;
            }

            if (Manager::checkboxClickCallback) {
                Manager::checkboxClickCallback(NULL);
            }
        } else {
            _data.b = nk_val;
        }
    }

    if (type == UI_FLOAT) {
        // // ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        // TODO : add skinning
        full_name = "#" + label + ":";
        float currData = _data.f;

        if (skinned_style.props[prop_active].first != nullptr) {
            ctx->style.property.active = nk_style_item_image(
                images[skinned_style.props[prop_active].first][skinned_style.props[prop_active].second].first
            );
        }
        if (skinned_style.props[prop_normal].first != nullptr) {
            ctx->style.property.normal = nk_style_item_image(
                images[skinned_style.props[prop_normal].first][skinned_style.props[prop_normal].second].first
            );
        }
        if (skinned_style.props[prop_hover].first != nullptr) {
            ctx->style.property.hover = nk_style_item_image(
                images[skinned_style.props[prop_hover].first][skinned_style.props[prop_hover].second].first
            );
        }

        nk_property_float(ctx, full_name.c_str(), minf, &_data.f, maxf, 1, flt_px_incr);
        // ctx->style.property.
        if (currData != _data.f) {
            // evoke callbacks
            unsigned int cbIdx = 0;
            for (auto callback : activeCallbacks) {
                callback(activeDatas[cbIdx]);
                cbIdx++;
            }
        }
    }

    if (type == UI_INT) {
        // ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        // TODO : add skinning
        full_name = "#" + label + ":";
        int currData = _data.i;
        nk_property_int(ctx, full_name.c_str(), min, &_data.i, max, 1, 0.5f);

        if (currData != _data.i) {
            // evoke callbacks
            unsigned int cbIdx = 0;
            for (auto callback : activeCallbacks) {
                callback(activeDatas[cbIdx]);
                cbIdx++;
            }
        }
    }

    if (type == UI_UINT) {
        // ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        // TODO : add skinning
        full_name = "#" + label + ":";
        int currData = _data.i;
        nk_property_int(ctx, full_name.c_str(), 0, &_data.i, max, 1, 0.5f);

        if (currData != _data.i) {
            // evoke callbacks
            unsigned int cbIdx = 0;
            for (auto callback : activeCallbacks) {
                callback(activeDatas[cbIdx]);
                cbIdx++;
            }
        }
    }

    if (type == UI_STRING_INPUT) {
        // ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        // TODO : add skinning
        string curr_str = *_data.strPtr;
        string& stringRef = *_data.strPtr;
        stringToText(stringRef);

        // nk_draw_set_color_inline(ctx, NK_COLOR_INLINE_NONE);
        nk_edit_string(ctx, NK_EDIT_SIMPLE | NK_EDIT_SELECTABLE, text, &text_len, 512, nk_filter_default);
        textToString(stringRef);
        
        if (curr_str != *_data.strPtr) {
            // evoke callbacks
            unsigned int cbIdx = 0;
            for (auto callback : activeCallbacks) {
                callback(activeDatas[cbIdx]);
                cbIdx++;
            }
        }
    }

    if (type == UI_BUTTON) {
        // ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        ctx->style.button.border = border;
        ctx->style.button.rounding = rounding;
        if (skinned_style.props[button_normal].first != nullptr) {
            ctx->style.button.normal = nk_style_item_image(
                images[skinned_style.props[button_normal].first][skinned_style.props[button_normal].second].first
            );
        }
        if (skinned_style.props[button_hover].first != nullptr) {
            ctx->style.button.hover = nk_style_item_image(
                images[skinned_style.props[button_hover].first][skinned_style.props[button_hover].second].first
            );
        }
        if (skinned_style.props[button_active].first != nullptr) {
            ctx->style.button.active = nk_style_item_image(
                images[skinned_style.props[button_active].first][skinned_style.props[button_active].second].first
            );
        }
        // struct nk_rect bounds = nk_widget_bounds(ctx);
        if (ui_button_image != nullptr) {
            _data.b = nk_button_image(ctx, images[ui_button_image][cropId].first);
        } else {
            _data.b = nk_button_label(ctx, label.c_str());
        }
        nk_bool button_hovered = nk_widget_is_hovered(ctx);
        if (button_hovered) {
            isHovered = true;
            // evoke hovered callbacks
            unsigned int cbIdx = 0;
            for (auto callback : hoverCallbacks) {
                callback(hoverDatas[cbIdx]);
                cbIdx++;
            }
        } else {
            if (isHovered) {
                // evoke hover end callbacks
                unsigned int cbIdx = 0;
                for (auto callback : hoverEndCallbacks) {
                    callback(hoverEndDatas[cbIdx]);
                    cbIdx++;
                }
            }
            isHovered = false;
        }

        if (tooltip_display) {
            struct nk_rect bounds = nk_widget_bounds(ctx);
            if (nk_input_is_mouse_hovering_rect(&ctx->input, bounds)) {
                // color_table tooltip_style = style;
                // tooltip_style.elements[UI_COLOR_TEXT].r = 255;
                // tooltip_style.elements[UI_COLOR_TEXT].g = 255;
                // tooltip_style.elements[UI_COLOR_TEXT].b = 255;
                nk_style_from_table(ctx, (struct nk_color*)tooltip_style.elements);
                nk_tooltip(ctx, tooltip_text.c_str());
                nk_style_from_table(ctx, (struct nk_color*)style.elements);
            }
        }

        if (disabled) {
            if (_data.b && Manager::disabledButtonClickCallback) {
                Manager::disabledButtonClickCallback(NULL);
            }
            rmb_click = false;
            _data.b = false;
        }

        if (_data.b && Manager::buttonClickCallback) {
            Manager::buttonClickCallback(NULL);
        }

        if (selected_by_keys)
            _data.b = true;

        lmb_click = nk_widget_is_mouse_clicked(ctx, NK_BUTTON_LEFT);
        rmb_click = nk_widget_is_mouse_clicked(ctx, NK_BUTTON_RIGHT);

        // evoke callbacks
        if(lmb_click && !disabled) {
            unsigned int cbIdx = 0;
            for(auto&& callback : clickCallbacks)
                callback(clickDatas[cbIdx++]);
        }

        if (_data.b) {
            unsigned int cbIdx = 0;
            for (auto&& callback : activeCallbacks)
                callback(activeDatas[cbIdx++]);
        }
    }

    // nk_button_image_label
    if (type == UI_EMPTY) {
        nk_spacing(ctx, 1);

        if (tooltip_display) {
            struct nk_rect bounds = nk_widget_bounds(ctx);
            if (nk_input_is_mouse_hovering_rect(&ctx->input, bounds)) {
                // color_table tooltip_style = style;
                // tooltip_style.elements[UI_COLOR_TEXT].r = 255;
                // tooltip_style.elements[UI_COLOR_TEXT].g = 255;
                // tooltip_style.elements[UI_COLOR_TEXT].b = 255;
                nk_style_from_table(ctx, (struct nk_color*)tooltip_style.elements);
                nk_tooltip(ctx, tooltip_text.c_str());
                nk_style_from_table(ctx, (struct nk_color*)style.elements);
            }
        }

        nk_bool button_hovered = nk_widget_is_hovered(ctx);
        if (button_hovered) {
            isHovered = true;
            // evoke hovered callbacks
            unsigned int cbIdx = 0;
            for (auto callback : hoverCallbacks) {
                callback(hoverDatas[cbIdx]);
                cbIdx++;
            }
        } else {
            if (isHovered) {
                // evoke hover end callbacks
                unsigned int cbIdx = 0;
                for (auto callback : hoverEndCallbacks) {
                    callback(hoverEndDatas[cbIdx]);
                    cbIdx++;
                }
            }
            isHovered = false;
        }
    }

    if (type == UI_BUTTON_SWITCH) {
        // ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        bool prev_val = _data.b;

        if (skinned_style.props[button_normal].first != nullptr) {
            ctx->style.button.normal = nk_style_item_image(
                images[skinned_style.props[button_normal].first][skinned_style.props[button_normal].second].first
            );
        }
        if (skinned_style.props[button_hover].first != nullptr) {
            ctx->style.button.hover = nk_style_item_image(
                images[skinned_style.props[button_hover].first][skinned_style.props[button_hover].second].first
            );
        }
        if (skinned_style.props[button_active].first != nullptr) {
            ctx->style.button.active = nk_style_item_image(
                images[skinned_style.props[button_active].first][skinned_style.props[button_active].second].first
            );
        }

        if (_data.b) {
            // use "active" skin for normal and hover properties
            if (skinned_style.props[button_active].first != nullptr) {
                ctx->style.button.normal = nk_style_item_image(
                    images[skinned_style.props[button_active].first][skinned_style.props[button_active].second].first
                );
                ctx->style.button.hover = nk_style_item_image(
                    images[skinned_style.props[button_active].first][skinned_style.props[button_active].second].first
                );
            } else {
                // default styling options
                struct nk_style_button button;
                button = ctx->style.button;
                ctx->style.button.normal = nk_style_item_color(nk_rgb(40, 40, 40));
                ctx->style.button.hover = nk_style_item_color(nk_rgb(40, 40, 40));
                ctx->style.button.active = nk_style_item_color(nk_rgb(40, 40, 40));
                ctx->style.button.border_color = nk_rgb(60, 60, 60);
                ctx->style.button.text_background = nk_rgb(60, 60, 60);
                ctx->style.button.text_normal = nk_rgb(60, 60, 60);
                ctx->style.button.text_hover = nk_rgb(60, 60, 60);
                ctx->style.button.text_active = nk_rgb(60, 60, 60);
                ctx->style.button = button;
            }
            if (nk_button_label(ctx, label.c_str())) {
                _data.b = false;
            }
        } else if (nk_button_label(ctx, label.c_str())) 
            _data.b = true;
        else
            _data.b = false;
        
        if (prev_val != _data.b) {
            // button was switched, evoke callbacks
            unsigned int cbIdx = 0;
            for (auto callback : activeCallbacks) {
                callback(activeDatas[cbIdx]);
                cbIdx++;
            }

            cbIdx = 0;
            for (auto callback : clickCallbacks) {
                callback(clickDatas[cbIdx]);
                cbIdx++;
            }
        }
    }

    if (type == UI_DROPDOWN) {
        // allocate memory for items from uiStringGroup
        // const char **lst = reinterpret_cast<const char **>( _data.usgPtr->getCharArrays() );
        // _data.usgPtr->selected_element = nk_combo(ctx, lst, NK_LEN(lst), _data.usgPtr->selected_element, 25, nk_vec2(200, 200));
        // _data.usgPtr->disposeCharsArray();
    }

    if (type == UI_COLOR_PICKER) {
        // TODO : add skinning
        struct nk_colorf & curr_widget_color = *reinterpret_cast<nk_colorf*>(&_data.c);
        if (nk_combo_begin_color(ctx, nk_rgb_cf(curr_widget_color), nk_vec2(nk_widget_width(ctx), 400))) {
            color_picker_unwrapped = true;
            nk_layout_row_dynamic(ctx, 120, 1);
            curr_widget_color = nk_color_picker(ctx, curr_widget_color, NK_RGBA);
            nk_layout_row_dynamic(ctx, 25, 1);
            curr_widget_color.r = nk_propertyf(ctx, "#R:", 0, curr_widget_color.r, 1.0f, 0.01f, 0.005f);
            curr_widget_color.g = nk_propertyf(ctx, "#G:", 0, curr_widget_color.g, 1.0f, 0.01f, 0.005f);
            curr_widget_color.b = nk_propertyf(ctx, "#B:", 0, curr_widget_color.b, 1.0f, 0.01f, 0.005f);
            curr_widget_color.a = nk_propertyf(ctx, "#A:", 0, curr_widget_color.a, 1.0f, 0.01f, 0.005f);
            nk_combo_end(ctx);
        } else {
            color_picker_unwrapped = false;
        }
    }
    
    if (type == UI_IMAGE) {
        // ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        if (imgPtr != nullptr)
            nk_image(ctx, images[imgPtr][cropId].first);
    }

    if (type == UI_STRING_LABEL) {
        // ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;

        nk_flags align;
        switch(text_align){
            case LEFT:
            align = NK_TEXT_LEFT;
            break;
            case CENTER:
            align = NK_TEXT_CENTERED;
            break;
            case RIGHT:
            align = NK_TEXT_RIGHT;
            break;
        }
        // nk_draw_set_color_inline(ctx, NK_COLOR_INLINE_TAG);
        nk_label(ctx, label.c_str(), align);

        if (hasClickableText) {
            // check for coloring and set original, if it was saved
            for (auto clk_rgn = clickable_regions.begin(); clk_rgn != clickable_regions.end(); clk_rgn++) {
                if (clk_rgn->original_color != "") {
                    label.replace(
                        label.find("[color=#") + 8, 6, clk_rgn->original_color
                    );
                    clk_rgn->original_color = "";
                }
            }
            if (nk_widget_is_hovered(ctx)) {
                // Note: we don't use any scrollbars!
                struct nk_rect area;
                struct nk_rect bounds = nk_widget_bounds(ctx);
                area.y = bounds.y + ctx->style.edit.padding.y + ctx->style.edit.border;
                float mouse_y = (ctx->input.mouse.pos.y - area.y);
                if (mouse_y > 0.f) {
                    area.x = bounds.x + ctx->style.edit.padding.x + ctx->style.edit.border;
                    float mouse_x = (ctx->input.mouse.pos.x - area.x);
                    int glyph_len = 0;
                    nk_rune unicode = 0;
                    glyph_len = nk_utf_decode(label.data(), &unicode, 1);
                    float glyph_width = ctx->style.font->width(
                        ctx->style.font->userdata,
                        ctx->style.font->height,
                        label.data(),
                        glyph_len
                    );
                    int cursor = mouse_x / glyph_width;
                    for (auto click_region = clickable_regions.begin(); click_region != clickable_regions.end(); click_region++) {
                        // w == left, h == right
                        if (click_region->click_region.h <= cursor && cursor <= click_region->click_region.w) {
                            if (nk_input_has_mouse_click(&ctx->input, NK_BUTTON_LEFT)) {
                                if (click_region->clickCallback)
                                    click_region->clickCallback(click_region->objectID);
                            } else {
                                // update color
                                bool mouse_down = nk_input_is_mouse_down(&ctx->input, NK_BUTTON_LEFT);
                                click_region->original_color = label.substr(
                                    label.find("[color=#") + 8, 6
                                );
                                if (label.find("[color=#") != string::npos && label.find("[/color]") != string::npos) {
                                    label.replace(
                                        label.find("[color=#") + 8, 6, mouse_down ? "aaaaff" : "ff00ff"
                                    );
                                }
                                if (click_region->hoverCallback)
                                    click_region->hoverCallback(click_region->objectID);
                            }
                        }
                    }
                }
            }
        }
    }

    if (type == UI_STRING_TEXT) {
        // ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;

        // nk_draw_set_color_inline(ctx, NK_COLOR_INLINE_TAG);
        nk_label_wrap(ctx, label.c_str());
    }
    
    if (type == UI_PROGRESS) {
        // ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        if (use_custom_element_style) {
            ctx->style.progress.cursor_normal = nk_style_item_color(
                *(struct nk_color*)&style.elements[COLOR_ELEMENTS::UI_COLOR_SLIDER_CURSOR]
            );
            ctx->style.progress.cursor_hover = nk_style_item_color(
                *(struct nk_color*)&style.elements[COLOR_ELEMENTS::UI_COLOR_SLIDER_CURSOR_HOVER]
            );
            ctx->style.progress.cursor_active = nk_style_item_color(
                *(struct nk_color*)&style.elements[COLOR_ELEMENTS::UI_COLOR_SLIDER_CURSOR_ACTIVE]
            );
        }

        if (skinned_style.props[progress_normal].first != nullptr) {
            ctx->style.progress.normal = nk_style_item_image(
                images[skinned_style.props[progress_normal].first][skinned_style.props[progress_normal].second].first
            );
        }
        if (skinned_style.props[progress_hover].first != nullptr) {
            ctx->style.progress.hover = nk_style_item_image(
                images[skinned_style.props[progress_hover].first][skinned_style.props[progress_hover].second].first
            );
        }
        if (skinned_style.props[progress_active].first != nullptr) {
            ctx->style.progress.active = nk_style_item_image(
                images[skinned_style.props[progress_active].first][skinned_style.props[progress_active].second].first
            );
        }
        if (skinned_style.props[cursor_normal].first != nullptr) {
            ctx->style.progress.cursor_normal = nk_style_item_image(
                images[skinned_style.props[cursor_normal].first][skinned_style.props[cursor_normal].second].first
            );
        }
        if (skinned_style.props[cursor_hover].first != nullptr) {
            ctx->style.progress.cursor_hover = nk_style_item_image(
                images[skinned_style.props[cursor_hover].first][skinned_style.props[cursor_hover].second].first
            );
        }
        if (skinned_style.props[cursor_active].first != nullptr) {
            ctx->style.progress.cursor_active = nk_style_item_image(
                images[skinned_style.props[cursor_active].first][skinned_style.props[cursor_active].second].first
            );
        }

        ctx->style.progress.border = border;
        ctx->style.progress.rounding = rounding;
        ctx->style.progress.padding = nk_vec2(padding.h, padding.w);

        nk_size curr = _data.ui;
        nk_progress(ctx, &curr, 100, modifyable_progress_bar);
        _data.ui = curr;
    }

    if (type == UI_ITEMS_LIST) {
        // ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;

        // TODO : add skinning
        ui_string_group& uiGroupRef = *_data.usgPtr;
        uiGroupRef.selection_switch = false;
        if (uiGroupRef.selectMethod == RADIO_SELECT) {
            nk_layout_row_dynamic(ctx, 25, 1);
            for (int i = 0; i < uiGroupRef.elements.size(); i++) {
                if (nk_option_label(ctx, uiGroupRef.elements[i].c_str(), i == uiGroupRef.selected_element))  {
                    if ( uiGroupRef.selected_element != i )
                        uiGroupRef.selection_switch = true;

                    uiGroupRef.selected_element = i;
                }
            }
        } else if (uiGroupRef.selectMethod == LIST_SELECT) {
            nk_flags align;
            switch(text_align){
            case LEFT:
                align = NK_TEXT_LEFT;
                break;
            case CENTER:
                align = NK_TEXT_CENTERED;
                break;
            case RIGHT:
                align = NK_TEXT_RIGHT;
                break;
            }
            regex e(uiGroupRef.regrex_filter);
            if (nk_group_begin(ctx, label.c_str(), NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
                int selected = 0;
                for (int i = 0; i < uiGroupRef.elements.size(); i++) {
                    if (uiGroupRef.elements[i] == "") continue;
                    if (i == uiGroupRef.selected_element)
                        selected = 1;
                    else
                        selected = 0;
                    if (uiGroupRef.regrex_filter != "") {
                        if (!regex_match(uiGroupRef.elements[i], e))
                            continue;
                    }
                    nk_layout_row_dynamic(ctx, 20, 1);
                    nk_selectable_label(ctx, uiGroupRef.elements[i].c_str(), align, &selected);
                    if (selected) {
                        if (uiGroupRef.selected_element != i)
                            uiGroupRef.selection_switch = true;
                        uiGroupRef.selected_element = i;
                    }
                }
                nk_group_end(ctx);
            }
        } else if (uiGroupRef.selectMethod == BUTTON_SELECT) {
            bool useLabels = uiGroupRef.elements.size() == uiGroupRef.element_labels.size();
            int i = 0;
            /*struct nk_key_selector ks;
            ks.focused = true;*/
            for (auto it = uiGroupRef.elements.begin(); it != uiGroupRef.elements.end(); it++) {
                if (useLabels) {
                    nk_layout_row_dynamic(ctx, 25, 2);
                    nk_label(ctx, uiGroupRef.element_labels[i].c_str(), NK_TEXT_LEFT);
                } else {
                    nk_layout_row_dynamic(ctx, 25, 1);
                }
                if (i == uiGroupRef.selected_element) {
                    // display code for selected button
                    struct nk_style_button button;
                    button = ctx->style.button;
                    ctx->style.button.normal = nk_style_item_color(nk_rgb(40, 40, 40));
                    ctx->style.button.hover = nk_style_item_color(nk_rgb(40, 40, 40));
                    ctx->style.button.active = nk_style_item_color(nk_rgb(40, 40, 40));
                    ctx->style.button.border_color = nk_rgb(60, 60, 60);
                    ctx->style.button.text_background = nk_rgb(60, 60, 60);
                    ctx->style.button.text_normal = nk_rgb(60, 60, 60);
                    ctx->style.button.text_hover = nk_rgb(60, 60, 60);
                    ctx->style.button.text_active = nk_rgb(60, 60, 60);
                    if (uiGroupRef.use_selected_string) {
                        nk_button_label(ctx, uiGroupRef.selectedString.c_str());
                    }
                    //} else {
                    //    //nk_button_label(ctx, it->c_str(), uiGroupRef.focused_by_key_element == i ? &ks : 0);
                    //    nk_button_label(ctx, it->c_str(), uiGroupRef.focused_by_key_element == i ? &ks : 0);
                    //}
                    ctx->style.button = button;
                //} else if (nk_button_label(ctx, it->c_str(), uiGroupRef.focused_by_key_element == i ? &ks : 0 )) {
                }
                else if (nk_button_label(ctx, it->c_str())) {
                    if ( uiGroupRef.selected_element != i )
                        uiGroupRef.selection_switch = true;
                    uiGroupRef.selected_element = i;
                }
                i++;
            }
        }
        if (uiGroupRef.selection_switch) {
            // active callbacks
            unsigned int cbIdx = 0;
            for (auto callback : activeCallbacks) {
                callback(activeDatas[cbIdx]);
                cbIdx++;
            }
        }
    }
    Manager::draw_idx++;
    struct nk_rect wid_rect = nk_widget_bounds(ctx);
    layout_border.x = wid_rect.x; 
    layout_border.y = wid_rect.y;
    layout_border.w = wid_rect.w;
    layout_border.h = wid_rect.h;
}

void UI_element::initImage(void * texID, unsigned int w, unsigned int h, region<float> crop) {
    if (type != UI_IMAGE && type != UI_BUTTON) {
        // TODO : add error message
        return;
    }

    Manager::addImage(
        texID,
        w,
        h,
        crop
    );

    if (type == UI_IMAGE)
        imgPtr = texID;
    else
        ui_button_image = texID;
    cropId = images[texID].size() - 1;
}

void UI_element::useSkinImage(
    void * texID,
    unsigned short w,
    unsigned short h,
    region<float> crop,
    IMAGE_SKIN_ELEMENT elt
) {
    Manager::addImage(texID, w, h, crop);
    skinned_style.props[elt].first = texID;
    skinned_style.props[elt].second = images[texID].size() - 1;
}

IndieGo::UI::region<float> UI_element::getImgCrop(IndieGo::UI::IMAGE_SKIN_ELEMENT elt) {
    void * idx = skinned_style.props[elt].first;
    if (idx == nullptr) return { 0.f, 0.f, 0.f, 0.f };
    return images[ skinned_style.props[elt].first ][ skinned_style.props[elt].second ].second;
}

extern Manager GUI;

bool Manager::show_main_tooltip = false;
string Manager::main_tooltip = "None";

string Manager::main_font = "None";
float Manager::main_font_size = 16.f;

void Manager::showMainTooltip() {
    color_table style;
    style.elements[UI_COLOR_WINDOW].a = 0;
    nk_style_from_table(ctx, (struct nk_color*)style.elements);
    // if (main_font != "None") {
        nk_style_set_font(
            ctx,
            &backend_loaded_fonts["MercutioNbpBasic"][24.f]->handle
        );
    // }

    nk_begin(
        ctx,
        "Tooltip region",
        nk_rect(
            0,
            0,
            screen_size.w,
            screen_size.h
        ),
        NK_WINDOW_NO_INPUT | NK_WINDOW_BACKGROUND
    );
    color_table tooltip_style;
    tooltip_style.elements[UI_COLOR_TEXT].r = 255;
    tooltip_style.elements[UI_COLOR_TEXT].g = 255;
    tooltip_style.elements[UI_COLOR_TEXT].b = 255;

    tooltip_style.elements[UI_COLOR_WINDOW].r = 8;
    tooltip_style.elements[UI_COLOR_WINDOW].g = 31;
    tooltip_style.elements[UI_COLOR_WINDOW].b = 45;
    tooltip_style.elements[UI_COLOR_WINDOW].a = 140;

    nk_style_from_table(ctx, (struct nk_color*)tooltip_style.elements);
    nk_tooltip(ctx, main_tooltip.c_str());
    nk_end(ctx);
}

//--------------------------------------------------------
//
//            Widget display function. May use
//            different Immadiate-Mode Ui libs
//
//-------------------------------------------------------
void WIDGET::callImmediateBackend(){
    nk_flags flags = 0;
    header_height = 0.f;
    if (border)
        flags = flags | NK_WINDOW_BORDER;

    if (minimizable)
        flags = flags | NK_WINDOW_MINIMIZABLE;

    if (title) {
        flags = flags | NK_WINDOW_TITLE;
    }

    if (movable)
        flags = flags | NK_WINDOW_MOVABLE;

    if (scalable)
        flags = flags | NK_WINDOW_SCALABLE;

    if (!has_scrollbar)
        flags = flags | NK_WINDOW_NO_SCROLLBAR;

    if (forceNoFocus)
        flags = flags | NK_WINDOW_NO_INPUT | NK_WINDOW_BACKGROUND;

    if (custom_style)
        nk_style_from_table(ctx, (struct nk_color*)style.elements);
    else
        nk_style_default(ctx);

    // check various styling possibilities
    // Background
    if (skinned_style.props[background].first != nullptr) {
        ctx->style.window.fixed_background = nk_style_item_image(
            images[skinned_style.props[background].first][skinned_style.props[background].second].first
        );
    }

    // Header
    // if (skinned_style.props[progress_normal].first != -1) {
    //     ctx->style.window.header.normal = nk_style_item_image(
    //         images[skinned_style.props[normal].first][skinned_style.props[normal].second].first
    //     );
    // }
    // if (skinned_style.props[hover].first != -1) {
    //     ctx->style.window.header.hover = nk_style_item_image(
    //         images[skinned_style.props[hover].first][skinned_style.props[hover].second].first
    //     );
    // }
    // if (skinned_style.props[active].first != -1) {
    //     ctx->style.window.header.active = nk_style_item_image(
    //         images[skinned_style.props[active].first][skinned_style.props[active].second].first
    //     );
    // }

    ctx->style.window.spacing = nk_vec2(spacing.h, spacing.w);
    ctx->style.window.padding = nk_vec2(padding.h, padding.w);
    ctx->style.window.border = border_size;
    
    if (font != "None") {
        nk_style_set_font(
            ctx,
            &backend_loaded_fonts[font][font_size]->handle
        );
    } else {
        // use main font, if available
        if (GUI.main_font != "None") {
            nk_style_set_font(
                ctx,
                &backend_loaded_fonts[GUI.main_font][GUI.main_font_size]->handle
            );
        }
    }
    // ctx->draw_idx = Manager::draw_idx;
    if (apply_highlight) {
        last_apply_highlight_idx++;
        apply_highlight_indices[last_apply_highlight_idx] = Manager::draw_idx;
    }
    if (apply_shading) {
        last_apply_shading_idx++;
        apply_shading_indices[last_apply_shading_idx] = Manager::draw_idx;
    }
    if (
        nk_begin(
            ctx,
            name.c_str(),
            nk_rect(
                screen_size.w * screen_region.x,
                screen_size.h * screen_region.y,
                screen_size.w * screen_region.w,
                screen_size.h * screen_region.h
            ),
            flags 
        )
    ) {
        if (setFocus) {
            nk_window_set_focus(ctx, name.c_str());
            setFocus = false;
        }
        
        if (has_scrollbar && updateScrollPosReq) {
            unsigned int X, Y;
            nk_window_get_scroll(ctx, &X, &Y);
            if (scroll_offsets.w != -1) {
                /*unsigned int upd = (100.f / (float)scroll_offsets.w) * (screen_region.x - screen_region.w) * screen_size.w;
                upd += screen_region.x * screen_size.w;
                nk_window_set_scroll(ctx, upd, Y);*/
            }

            if (scroll_offsets.h != -1) {
                float scroll_percent = ((float)scroll_offsets.h / 100.f);
                float offset_region_from_widget_persent = scroll_percent * screen_region.h;
                float final_scroll = ( offset_region_from_widget_persent + screen_region.y) * screen_size.h + screen_size.h * screen_region.h;
                unsigned int upd = final_scroll;
                nk_window_set_scroll(ctx, X, upd);
                
            }
            updateScrollPosReq = false;
        }
        
        if (title || movable || minimizable)
            header_height = ctx->current->layout->header_height;
        
        Manager::draw_idx++;
        callWidgetUI(Manager::UIMap);
        minimized = false;
    } else {
        minimized = true;
    }
    if (has_scrollbar) {
        unsigned int X, Y;
        nk_window_get_scroll(ctx, &X, &Y);
        scroll_offsets.h = Y;
        scroll_offsets.w = X;
    }

    focused = nk_window_has_focus(ctx);
    hasCursor = nk_window_is_hovered(ctx);

    if (movable || scalable) {
        // update screen_region with current bounds, if those are changed by user
        struct nk_rect updBounds = nk_window_get_bounds(ctx);
        screen_region.x = updBounds.x / screen_size.w;
        screen_region.y = updBounds.y / screen_size.h;
        screen_region.w = updBounds.w / screen_size.w;
        screen_region.h = updBounds.h / screen_size.h;
    }
    nk_end(ctx);
}

void Manager::addImage(
    void * texID,
    unsigned short w,
    unsigned short h,
    region<float> crop
) {

    images[texID].push_back(
        pair<struct nk_image, region<float>> { 
            nk_subimage_ptr(
                texID, 
                w,
                h, 
                nk_rect(
                    crop.x * w, 
                    crop.y * h, 
                    crop.w * w, 
                    crop.h * h
                )
            ), 
            crop 
        }
    );
}

void WIDGET::useSkinImage(
    void * texID,
    unsigned short w,
    unsigned short h,
    region<float> crop,
    IMAGE_SKIN_ELEMENT elt
) {
    Manager::addImage(texID, w, h, crop);
    skinned_style.props[elt].first = texID;
    skinned_style.props[elt].second = images[texID].size() - 1;
}

IndieGo::UI::region<float> WIDGET::getImgCrop(IndieGo::UI::IMAGE_SKIN_ELEMENT elt) {
    void * idx = skinned_style.props[elt].first;
    if (idx == nk_subimage_id) return { 0.f, 0.f, 0.f, 0.f };
    return images[ skinned_style.props[elt].first ][ skinned_style.props[elt].second ].second;
}

// TODO : calculate elements count in row
// so far - 64 == max
float WIDGET::allocateRow(unsigned int cols, float min_height, bool in_pixels) {
    float height = in_pixels ? min_height : screen_size.h * screen_region.h * min_height;
    nk_layout_space_begin(
        ctx, 
        NK_STATIC, 
        height,
        64
    );
    struct nk_rect total_space = nk_layout_space_bounds(ctx);
    // cout << total_space.h << endl;
    return total_space.h;
}

void WIDGET::endRow() {
    nk_layout_space_end(ctx);
}

#define NK_GLFW_GL3_MOUSE_GRABBING

char uncode_to_char(unsigned int codepoint) {
    nk_rune u = codepoint;
    char c;
    nk_utf_encode(u, &c, 1);
    return c;
}

unsigned int char_to_uncode(char c) {
    nk_rune u;
    nk_utf_decode((const char*)&c, &u, 2);
    return (unsigned int)u;
}

void Manager::drawFrameStart() {
    nk_glfw3_new_frame();
}

void Manager::drawFrameEnd() {
    VkSemaphore s = nk_glfw3_render( // <- subsequent calls will wait for this semaphore to be signalled!
        vkRenderer::graphicsQueue,
        vkRenderer::currFrame,
        vkRenderer::getLastUsedSemaphore(), 
        NK_ANTI_ALIASING_ON
    );
    vkRenderer::setNextSemaphore(s);
}

void Manager::init(
    GLFWwindow * w,
    shared_ptr<vkI::vkRenderer> rendererPtr
) {
    renderer = rendererPtr;
    prepareUIRenderer(w);
}

// string project_dir = "None";
void * img_data = NULL;

void Manager::resize(int new_width, int new_height) {
    nk_glfw3_resize(new_width, new_height);
    screen_size.w = new_width;
    screen_size.h = new_height;
};

void Manager::loadFont(string path, float font_size, bool useProjectDir, bool cutProjDirFromPath) {
    string font_name = fs::path(path).stem().string();
    if (find(loaded_fonts[font_name].sizes.begin(), loaded_fonts[font_name].sizes.end(), font_size) != loaded_fonts[font_name].sizes.end())
        return;

    loaded_fonts[font_name].sizes.push_back(font_size);
    // const void *image; int w, h;
    // struct nk_font_config cfg = nk_font_config(0);
    // cfg.range = nk_font_cyrillic_glyph_ranges();

    // nk_font_atlas_init_default(atlas);
    // nk_font_atlas_begin(atlas);

    // loaded_fonts[font_name].sizes.push_back(font_size);
    // if (backend_loaded_fonts.find(font_name) != backend_loaded_fonts.end()) {
    //     if (backend_loaded_fonts[font_name].find(font_size) != backend_loaded_fonts[font_name].end()) {
    //         return;
    //     }
    // }


    // string pdir = project_dir;
    // if (fs::exists(fs::path(project_dir)))
    //     pdir = project_dir;
    // else
    //     pdir = "";

    // if (fs::path(path).is_absolute() && pdir != "") {
    //     loaded_fonts[font_name].path = fs::relative(fs::path(path), fs::path(pdir)).string();
    // } else {
    //     loaded_fonts[font_name].path = path;
    // }

    // backend_loaded_fonts[font_name][font_size] = nk_font_atlas_add_from_file(
    //     atlas, 
    //     fs::path(pdir).append(loaded_fonts[font_name].path).string().c_str(),
    //     font_size, 
    //     &cfg
    // );

    // backend_loaded_fonts.clear();

    // bool mainFont = false;
    // for (auto font : loaded_fonts) {
    //     mainFont = font.first == main_font;
    //     for (auto font_size : font.second.sizes) {
    //         if (mainFont) // never use PROJECT_DIR for main font loading
    //             backend_loaded_fonts[font.first][font_size] = nk_font_atlas_add_from_file(&atlas, font.second.path.c_str(), font_size, &cfg );
    //         else
    //             backend_loaded_fonts[font.first][font_size] = nk_font_atlas_add_from_file(
    //                 &atlas, 
    //                 fs::path(pdir).append(font.second.path).string().c_str(),
    //                 font_size, 
    //                 &cfg
    //             );
    //     }
    // }

    // image = nk_font_atlas_bake(atlas, &w, &h, NK_FONT_ATLAS_RGBA32);

    // nk_glfw3_device_upload_atlas(vkRenderer::graphicsQueue, image, w, h);
    // nk_font_atlas_end(atlas, nk_handle_ptr(font_image_view), nullptr);
    // if (main_font == "None") {
    //     main_font = font_name;
    //     main_font_size = font_size;
    // }
}