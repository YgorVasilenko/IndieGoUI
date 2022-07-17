#include <backends/Imgui/imgui.h>
#include <backends/Imgui/backends/imgui_impl_glfw.h>
#include <backends/Imgui/backends/imgui_impl_opengl3.h>
#include <IndieGoUI.h>

const char* glsl_version = "#version 130";

using namespace IndieGo::UI;
using namespace ImGui;

GLFWwindow* window = NULL;

void Manager::init(void * initData) {
    window = (GLFWwindow*)initData;
    IMGUI_CHECKVERSION();
    CreateContext();
    StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void Manager::scroll(double xoff, double yoff) {

}

void Manager::mouse_move(double x, double y) {

}

void Manager::mouse_button(int button, int action, int mods){
    
}

void Manager::char_input(unsigned int codepoint){
    
}

void Manager::key_input(unsigned int codepoint, bool pressed){
    
}

void Manager::drawFrameStart(){
   // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();
}

void Manager::drawFrameEnd(){
    Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//--------------------------------------------------------
//
//            Widget display function. May use 
//            different Immadiate-Mode Ui libs
//
//-------------------------------------------------------

region curr_widget_region;
unsigned int curr_line_cols;
float min_row_height;

ImGuiCol_ getColorParamMapping(COLOR_ELEMENTS c_elt){
    // Original COLOR_ properties are taken from Nuklear.
    // ImGuiCol_COUNT as null for properties, that I'm not familiar with.

    if (c_elt == COLOR_TEXT) return ImGuiCol_Text;
    if (c_elt == COLOR_WINDOW) return ImGuiCol_WindowBg;
    if (c_elt == COLOR_HEADER) return ImGuiCol_TitleBg;
    if (c_elt == COLOR_BORDER) return ImGuiCol_Border;
    
    if (c_elt == COLOR_BUTTON) return ImGuiCol_Button;
    if (c_elt == COLOR_BUTTON_HOVER) return ImGuiCol_ButtonHovered;
    if (c_elt == COLOR_BUTTON_ACTIVE) return ImGuiCol_ButtonActive;
    
    if (c_elt == COLOR_TOGGLE) return ImGuiCol_CheckMark;
    if (c_elt == COLOR_TOGGLE_HOVER) return ImGuiCol_CheckMark;
    if (c_elt == COLOR_TOGGLE_CURSOR) return ImGuiCol_CheckMark;

    if (c_elt == COLOR_SELECT) return ImGuiCol_TextSelectedBg;
    if (c_elt == COLOR_SELECT_ACTIVE) return ImGuiCol_TextSelectedBg;

    if (c_elt == COLOR_SLIDER) return ImGuiCol_SliderGrab;
    if (c_elt == COLOR_SLIDER_CURSOR) return ImGuiCol_COUNT;
    if (c_elt == COLOR_SLIDER_CURSOR_HOVER) return ImGuiCol_COUNT;
    if (c_elt == COLOR_SLIDER_CURSOR_ACTIVE) return ImGuiCol_SliderGrabActive;

    if (c_elt == COLOR_PROPERTY) return ImGuiCol_COUNT;

    if (c_elt == COLOR_EDIT) return ImGuiCol_COUNT;
    if (c_elt == COLOR_EDIT_CURSOR) return ImGuiCol_COUNT;

    if (c_elt == COLOR_COMBO) return ImGuiCol_COUNT;
    if (c_elt == COLOR_CHART) return ImGuiCol_COUNT;
    if (c_elt == COLOR_CHART_COLOR) return ImGuiCol_COUNT;
    if (c_elt == COLOR_CHART_COLOR_HIGHLIGHT) return ImGuiCol_COUNT;

    if (c_elt == COLOR_SCROLLBAR) return ImGuiCol_ScrollbarBg;
    if (c_elt == COLOR_SCROLLBAR_CURSOR) return ImGuiCol_ScrollbarGrab;
    if (c_elt == COLOR_SCROLLBAR_CURSOR_HOVER) return ImGuiCol_ScrollbarGrabHovered;
    if (c_elt == COLOR_SCROLLBAR_CURSOR_ACTIVE) return ImGuiCol_ScrollbarGrabActive;

    if (c_elt == COLOR_TAB_HEADER) return ImGuiCol_COUNT;
}

// IndieGoUI color parameters:
			// COLOR_TEXT,
			// COLOR_WINDOW,
			// COLOR_HEADER,
			// COLOR_BORDER,
			// COLOR_BUTTON,
			// COLOR_BUTTON_HOVER,
			// COLOR_BUTTON_ACTIVE,
			// COLOR_TOGGLE,
			// COLOR_TOGGLE_HOVER,
			// COLOR_TOGGLE_CURSOR,
			// COLOR_SELECT,
			// COLOR_SELECT_ACTIVE,
			// COLOR_SLIDER,
			// COLOR_SLIDER_CURSOR,
			// COLOR_SLIDER_CURSOR_HOVER,
			// COLOR_SLIDER_CURSOR_ACTIVE,
			// COLOR_PROPERTY,
			// COLOR_EDIT,
			// COLOR_EDIT_CURSOR,
			// COLOR_COMBO,
			// COLOR_CHART,
			// COLOR_CHART_COLOR,
			// COLOR_CHART_COLOR_HIGHLIGHT,
			// COLOR_SCROLLBAR,
			// COLOR_SCROLLBAR_CURSOR,
			// COLOR_SCROLLBAR_CURSOR_HOVER,
			// COLOR_SCROLLBAR_CURSOR_ACTIVE,
			// COLOR_TAB_HEADER

// imgui color parameters:
//  ImGuiCol_Text,
//     ImGuiCol_TextDisabled,
//     ImGuiCol_WindowBg,              // Background of normal windows
//     ImGuiCol_ChildBg,               // Background of child windows
//     ImGuiCol_PopupBg,               // Background of popups, menus, tooltips windows
//     ImGuiCol_Border,
//     ImGuiCol_BorderShadow,
//     ImGuiCol_FrameBg,               // Background of checkbox, radio button, plot, slider, text input
//     ImGuiCol_FrameBgHovered,
//     ImGuiCol_FrameBgActive,
//     ImGuiCol_TitleBg,
//     ImGuiCol_TitleBgActive,
//     ImGuiCol_TitleBgCollapsed,
//     ImGuiCol_MenuBarBg,
//     ImGuiCol_ScrollbarBg,
//     ImGuiCol_ScrollbarGrab,
//     ImGuiCol_ScrollbarGrabHovered,
//     ImGuiCol_ScrollbarGrabActive,
//     ImGuiCol_CheckMark,
//     ImGuiCol_SliderGrab,
//     ImGuiCol_SliderGrabActive,
//     ImGuiCol_Button,
//     ImGuiCol_ButtonHovered,
//     ImGuiCol_ButtonActive,
//     ImGuiCol_Header,                // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
//     ImGuiCol_HeaderHovered,
//     ImGuiCol_HeaderActive,
//     ImGuiCol_Separator,
//     ImGuiCol_SeparatorHovered,
//     ImGuiCol_SeparatorActive,
//     ImGuiCol_ResizeGrip,
//     ImGuiCol_ResizeGripHovered,
//     ImGuiCol_ResizeGripActive,
//     ImGuiCol_Tab,
//     ImGuiCol_TabHovered,
//     ImGuiCol_TabActive,
//     ImGuiCol_TabUnfocused,
//     ImGuiCol_TabUnfocusedActive,
//     ImGuiCol_PlotLines,
//     ImGuiCol_PlotLinesHovered,
//     ImGuiCol_PlotHistogram,
//     ImGuiCol_PlotHistogramHovered,
//     ImGuiCol_TableHeaderBg,         // Table header background
//     ImGuiCol_TableBorderStrong,     // Table outer and header borders (prefer using Alpha=1.0 here)
//     ImGuiCol_TableBorderLight,      // Table inner borders (prefer using Alpha=1.0 here)
//     ImGuiCol_TableRowBg,            // Table row background (even rows)
//     ImGuiCol_TableRowBgAlt,         // Table row background (odd rows)
//     ImGuiCol_TextSelectedBg,
//     ImGuiCol_DragDropTarget,
//     ImGuiCol_NavHighlight,          // Gamepad/keyboard: current highlighted item
//     ImGuiCol_NavWindowingHighlight, // Highlight window when using CTRL+TAB
//     ImGuiCol_NavWindowingDimBg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
//     ImGuiCol_ModalWindowDimBg,      // Darken/colorize entire screen behind a modal window, when one is active
//     ImGuiCol_COUNT

ImGuiCol_ curr_color;

void WIDGET::callImmediateBackend(UI_elements_map & UIMap){
    ImGuiWindowFlags flags = 0;
    // if (border)
    //     flags = flags | NK_WINDOW_BORDER;

    if (!minimizable)
        flags = flags | ImGuiWindowFlags_NoCollapse;

    if (!title)
        flags = flags | ImGuiWindowFlags_NoTitleBar;

    if (!movable)
        flags = flags | ImGuiWindowFlags_NoMove;
    
    if (!scalable)
        flags = flags | ImGuiWindowFlags_NoResize;

    if (custom_style){
        ImGuiStyle& style_ref = GetStyle();
        for (int i = COLOR_TEXT; i < COLOR_TAB_HEADER; i++){
            curr_color = getColorParamMapping((COLOR_ELEMENTS)i);
            if (curr_color != ImGuiCol_COUNT){
                style_ref.Colors[i] = ImVec4(
                    style.elements[i].r / 255, 
                    style.elements[i].g / 255, 
                    style.elements[i].b / 255, 
                    style.elements[i].a / 255
                );
            }
        }
    }

    SetNextWindowPos(ImVec2(screen_region.x, screen_region.y), ImGuiCond_Once);
    SetNextWindowSize(ImVec2(screen_region.w, screen_region.h), ImGuiCond_Once);

    if (Begin(name.c_str(), NULL, flags))
        callWidgetUI(UIMap);

    ImVec2 curr_win_size = GetWindowSize();
    ImVec2 curr_win_pos = GetWindowPos();

    screen_region.x = curr_win_pos.x;
    screen_region.y = curr_win_pos.y;
    screen_region.w = curr_win_size.x;
    screen_region.h = curr_win_size.y;

    curr_widget_region = screen_region;
    End();
}


bool newLine = false;

void WIDGET::allocateRow(unsigned int cols, float min_height){
    // By default Imgui places next UIfunc call on new row. So, opposing 
    // to Nuklear, we specify here ABSCENCE of SameLine call 
    newLine = true;
    curr_line_cols = cols;
    min_row_height = min_height;
}

void WIDGET::allocateEmptySpace(unsigned int fill_count){
    // nk_spacing(ctx, fill_count);
}

void WIDGET::allocateGroupStart(elements_group & g, float min_height){
    // g.unfolded = nk_tree_push(ctx, NK_TREE_TAB, g.name.c_str(), NK_MINIMIZED);
    // if (g.unfolded)
    //     nk_layout_row_dynamic(ctx, min_height, g.row_items_count);
}

void WIDGET::allocateGroupEnd(){
    // nk_tree_pop(ctx);
}

// Memory for string input storage
char text[64] = {};
int text_len;

void stringToText(std::string & str) {
    for (int i = 0; i < str.length(); i++)
        text[i] = str[i];
    text_len = str.length();
}

void textToString(std::string & str) {
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

void UI_element::callUIfunction() {
     if (!newLine)
         SameLine();

    newLine = false;
    // if (custom_style)
    //     nk_style_from_table(ctx, (struct nk_color*)style.elements);

    // std::string full_name; 
    // nk_bool nk_val;
    // static const float ratio[] = { 100, 120 };
    // float dbgVal;
    if (type == UI_BOOL) {
        Checkbox(label.c_str(), &_data.b);
        return;
    }

    if (type == UI_FLOAT) {
        // full_name = "#" + label + ":";
        // nk_property_float(ctx, full_name.c_str(), -300000.0f, &_data.f, 300000.0f, 1, 0.5f);
        return;
    }

    if (type == UI_INT) {
        // full_name = "#" + label + ":";
        // nk_property_int(ctx, full_name.c_str(), -1024, &_data.i, 1024, 1, 0.5f);
        return;
    }

    if (type == UI_UINT) {
        DragInt(label.c_str(), &_data.i, 1, 0, 1000, "%d", ImGuiSliderFlags_AlwaysClamp);
        return;
    }

    if (type == UI_STRING_INPUT) {
        std::string& stringRef = *_data.strPtr;
        stringToText(stringRef);
        InputText(label.c_str(), text, text_len);
        textToString(stringRef);
        return;
    }

    if (type == UI_BUTTON) {
        _data.b = Button(label.c_str(), ImVec2(curr_widget_region.w / curr_line_cols, min_row_height));
        return;
    }

    if (type == UI_BUTTON_SWITCH) {
        // if (_data.b) {
        //         struct nk_style_button button;
        //         button = ctx->style.button;
        //         ctx->style.button.normal = nk_style_item_color(nk_rgb(40, 40, 40));
        //         ctx->style.button.hover = nk_style_item_color(nk_rgb(40, 40, 40));
        //         ctx->style.button.active = nk_style_item_color(nk_rgb(40, 40, 40));
        //         ctx->style.button.border_color = nk_rgb(60, 60, 60);
        //         ctx->style.button.text_background = nk_rgb(60, 60, 60);
        //         ctx->style.button.text_normal = nk_rgb(60, 60, 60);
        //         ctx->style.button.text_hover = nk_rgb(60, 60, 60);
        //         ctx->style.button.text_active = nk_rgb(60, 60, 60);
        //         nk_button_label(ctx, label.c_str());
        //         ctx->style.button = button;
        //     } else if (nk_button_label(ctx, label.c_str()))
        //         _data.b = true;
        //     else
        //     _data.b = false;
        return;
    }

    if (type == UI_COLOR_PICKER) {
        float * color = reinterpret_cast<float*>(&_data.c);
        if ( ColorEdit4(label.c_str(), color)){
            color_picker_unwrapped = true;
        } else {
            color_picker_unwrapped = false;
        }
        // struct nk_colorf & curr_widget_color = *reinterpret_cast<nk_colorf*>(&_data.c);
        // if (nk_combo_begin_color(ctx, nk_rgb_cf(curr_widget_color), nk_vec2(nk_widget_width(ctx), 400))) {
        //     color_picker_unwrapped = true;
        //     nk_layout_row_dynamic(ctx, 120, 1);
        //     curr_widget_color = nk_color_picker(ctx, curr_widget_color, NK_RGBA);
        //     nk_layout_row_dynamic(ctx, 25, 1);
        //     curr_widget_color.r = nk_propertyf(ctx, "#R:", 0, curr_widget_color.r, 1.0f, 0.01f, 0.005f);
        //     curr_widget_color.g = nk_propertyf(ctx, "#G:", 0, curr_widget_color.g, 1.0f, 0.01f, 0.005f);
        //     curr_widget_color.b = nk_propertyf(ctx, "#B:", 0, curr_widget_color.b, 1.0f, 0.01f, 0.005f);
        //     curr_widget_color.a = nk_propertyf(ctx, "#A:", 0, curr_widget_color.a, 1.0f, 0.01f, 0.005f);
        //     nk_combo_end(ctx);
        // } else {
        //     color_picker_unwrapped = false;
        // }
    }

    if (type == UI_STRING_LABEL){
        TextWrapped(label.c_str());
        return;
    }

    if (type == UI_ITEMS_LIST) {
        ui_string_group& uiGroupRef = *_data.usgPtr;
        uiGroupRef.selection_switch = false;
        if (uiGroupRef.selectMethod == LIST_SELECT) {
            if (BeginListBox(label.c_str(), ImVec2(curr_widget_region.w / curr_line_cols, min_row_height))){
                int elt_idx = 0;
                bool is_selected = false;
                for (auto elt : uiGroupRef.elements){
                    is_selected = elt_idx == uiGroupRef.selected_element;
                    if (Selectable(elt.c_str(), is_selected)){
                        if (!is_selected)
                            uiGroupRef.selection_switch = true;
                        uiGroupRef.selected_element = elt_idx;
                    }
                    if (is_selected)
                        SetItemDefaultFocus();
                    elt_idx++;
                }
                EndListBox();
            }
        }
        // ui_string_group& uiGroupRef = *_data.usgPtr;
        // uiGroupRef.selection_switch = false;
        // if (uiGroupRef.selectMethod == RADIO_SELECT) {
        //     nk_layout_row_dynamic(ctx, 25, 1);
        //     for (int i = 0; i < uiGroupRef.elements.size(); i++) {
        //         if (nk_option_label(ctx, uiGroupRef.elements[i].c_str(), i == uiGroupRef.selected_element))  {
        //             if ( uiGroupRef.selected_element != i )
        //                 uiGroupRef.selection_switch = true;

        //             uiGroupRef.selected_element = i;
        //         }
        //     }
        // } else if (uiGroupRef.selectMethod == LIST_SELECT) {
        //     std::regex e(uiGroupRef.regrex_filter);
        //     if (nk_group_begin(ctx, label.c_str(), NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
        //         int selected = 0;
        //         for (int i = 0; i < uiGroupRef.elements.size(); i++) {
        //             if (uiGroupRef.elements[i] == "") continue;
        //             if (i == uiGroupRef.selected_element)
        //                 selected = 1;
        //             else
        //                 selected = 0;
        //             if (uiGroupRef.regrex_filter != "") {
        //                 if (!std::regex_match(uiGroupRef.elements[i], e))
        //                     continue;
        //             }
        //             nk_layout_row_dynamic(ctx, 20, 1);
        //             nk_selectable_label(ctx, uiGroupRef.elements[i].c_str(), NK_TEXT_CENTERED, &selected);
        //             if (selected) {
        //                 if ( uiGroupRef.selected_element != i )
        //                     uiGroupRef.selection_switch = true;
        //                 uiGroupRef.selected_element = i;
        //             }
        //         }
        //         nk_group_end(ctx);
        //     }
        // } else if (uiGroupRef.selectMethod == BUTTON_SELECT) {
        //     bool useLabels = uiGroupRef.elements.size() == uiGroupRef.element_labels.size();
        //     int i = 0;
        //     struct nk_key_selector ks;
        //     ks.focused = true;
        //     for (auto it = uiGroupRef.elements.begin(); it != uiGroupRef.elements.end(); it++) {
        //         if (useLabels) {
        //             nk_layout_row_dynamic(ctx, 25, 2);
        //             nk_label(ctx, uiGroupRef.element_labels[i].c_str(), NK_TEXT_LEFT);
        //         } else {
        //             nk_layout_row_dynamic(ctx, 25, 1);
        //         }
        //         if (i == uiGroupRef.selected_element) {
        //             // display code for selected button
        //             struct nk_style_button button;
        //             button = ctx->style.button;
        //             ctx->style.button.normal = nk_style_item_color(nk_rgb(40, 40, 40));
        //             ctx->style.button.hover = nk_style_item_color(nk_rgb(40, 40, 40));
        //             ctx->style.button.active = nk_style_item_color(nk_rgb(40, 40, 40));
        //             ctx->style.button.border_color = nk_rgb(60, 60, 60);
        //             ctx->style.button.text_background = nk_rgb(60, 60, 60);
        //             ctx->style.button.text_normal = nk_rgb(60, 60, 60);
        //             ctx->style.button.text_hover = nk_rgb(60, 60, 60);
        //             ctx->style.button.text_active = nk_rgb(60, 60, 60);
        //             if (uiGroupRef.use_selected_string) {
        //                 nk_button_label(ctx, uiGroupRef.selectedString.c_str());
        //             } else {
        //                 nk_button_label(ctx, it->c_str(), uiGroupRef.focused_by_key_element == i ? &ks : 0);
        //             }
        //             ctx->style.button = button;
        //         } else if (nk_button_label(ctx, it->c_str(), uiGroupRef.focused_by_key_element == i ? &ks : 0 )) {
        //             if ( uiGroupRef.selected_element != i )
        //                 uiGroupRef.selection_switch = true;
        //             uiGroupRef.selected_element = i;
        //         }
        //         i++;
        //     }
        // }
    }
}
