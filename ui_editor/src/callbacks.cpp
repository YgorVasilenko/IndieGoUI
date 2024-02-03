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
#include <filesystem>
#include <editor_structs.h>
#include <Shader.h>
#include <queue>
#include <functional>

namespace fs = std::filesystem;
using namespace IndieGo::UI;

extern std::queue<std::function<void()>> delayedFunctions;

extern Manager GUI;
extern std::string winID;
extern EditorState editorGlobals;

extern void updateUIFromWidget(void*);
extern void updateUIFromElement(void*);
extern std::string addElement(UI_ELEMENT_TYPE et);
extern std::string getTextAlignLabel(IndieGo::UI::TEXT_ALIGN align);
extern ELT_PUSH_OPT push_opt;

extern std::map<std::string, std::pair<TexData, region<float>>> skin_crops;

extern void saveUI(void*);
extern void loadUI(void*);
extern void closeUICallback(void*);
extern void loadFont(void*);
extern void loadSkinImage(void*);

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

extern std::string getSkinPropName(IMAGE_SKIN_ELEMENT prop);
extern Shader skinningShader;

void setCallbacks() {
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    
    // switch selected widget
    // -------------------------------------------
    UIMap["widgets list"].setActiveCallback(
        updateUIFromWidget
    );

    // Add new widget, rename widget, delete widget
    // -------------------------------------------
    UIMap["add new widget"].setActiveCallback(
        [] (void*) {
            auto& UIMap = GUI.UIMaps[editorGlobals.winID];
            ui_string_group& widgets_list = *UIMap["widgets list"]._data.usgPtr;

            std::string new_widget_name = *UIMap["new widget name"]._data.strPtr;
            if (!(new_widget_name.size() > 0 && editorGlobals.widgets_fill.find(new_widget_name) == editorGlobals.widgets_fill.end()))
                return;

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
            editorGlobals.widgets_fill[new_widget_name] = 0;
        }
    );

    UIMap["rename widget"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            ui_string_group& widgets_list = *UIMap["widgets list"]._data.usgPtr;
            std::string new_name = *UIMap["new widget name"]._data.strPtr;
            if (new_name.size() > 0 && std::count(widgets_list.elements.begin(), widgets_list.elements.end(), new_name) <= 1) {
                // can't use new name if there aleady is such widget
                WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
                auto element = std::find(widgets_list.elements.begin(), widgets_list.elements.end(), w.name);
                
                *element = new_name;
                GUI.widgets[editorGlobals.winID][new_name] = w;
                GUI.widgets[editorGlobals.winID][new_name].name = new_name;
                GUI.widgets[editorGlobals.winID].erase(editorGlobals.selectedWidget);        
            }
        }
    );
    UIMap["delete widget"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            GUI.deleteWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            ui_string_group& widgets_list = *UIMap["widgets list"]._data.usgPtr;
            widgets_list.elements.erase(
                std::find(
                    widgets_list.elements.begin(), 
                    widgets_list.elements.end(), 
                    editorGlobals.selectedWidget
                )
            );
            widgets_list.selected_element = -1;
        }
    );

    // Selected widget's flag settings
    // -------------------------------------------
    UIMap["bordered"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            w.border = UIMap["bordered"]._data.b;
        }
    );
    UIMap["titled"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            w.title = UIMap["titled"]._data.b;
        }
    );
    UIMap["minimizable"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            w.minimizable = UIMap["minimizable"]._data.b;
        }
    );
    UIMap["scalable"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            w.scalable = UIMap["scalable"]._data.b;
        }
    );
    UIMap["movable"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            w.movable = UIMap["movable"]._data.b;
        }
    );
    UIMap["has scrollbar"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            w.has_scrollbar = UIMap["has scrollbar"]._data.b;
        }
    );
        UIMap["visible"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            w.hidden = !UIMap["visible"]._data.b;
        }
    );


    // Selected widget's numeric properties
    // -------------------------------------------
    UIMap["location x"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            w.screen_region.x = UIMap["location x"]._data.f / 100.f;
        }
    );
    UIMap["location y"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            w.screen_region.y = UIMap["location y"]._data.f / 100.f;
        }
    );
    UIMap["size x"].setActiveCallback(
        [] (void*) {
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            w.screen_region.w = UIMap["size x"]._data.f / 100.f;
        }
    );
    UIMap["size y"].setActiveCallback(
        [] (void*) {
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            w.screen_region.h = UIMap["size y"]._data.f / 100.f;
        }
    );
    UIMap["widget border"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            w.border_size = UIMap["widget border"]._data.f;
        }
    );


    // "Main" editor widget switching (screens switching)
    // -------------------------------------------
    UIMap["edit widget elements"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            delayedFunctions.emplace([]() {
                WIDGET & widgets = GUI.getWidget("UI creator", winID);
                WIDGET & elements = GUI.getWidget("Edit elements", winID);
                widgets.hidden = true;
                elements.hidden = false;
                elements.screen_region = widgets.screen_region;
                editorGlobals.updateWidgetFont = false;
            });
        }
    );
    UIMap["skins and styling"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            delayedFunctions.emplace([]() {
                WIDGET & widgets = GUI.getWidget("UI creator", winID);
                WIDGET & widgets_style = GUI.getWidget("Widgets style", winID);
                widgets.hidden = true;
                widgets_style.hidden = false;
                widgets_style.screen_region = widgets.screen_region;
                UI_elements_map & UIMap = GUI.UIMaps[winID];
                UIMap["style selected widget"].label = "selected widget: " + editorGlobals.selectedWidget;
            });
        }
    );
    UIMap["back to widgets"].setActiveCallback(
        [] (void*) {
            delayedFunctions.emplace([]() {
                WIDGET & widgets = GUI.getWidget("UI creator", winID);
                WIDGET & elements = GUI.getWidget("Edit elements", winID);
                widgets.hidden = false;
                elements.hidden = true;
                widgets.screen_region = elements.screen_region;
                editorGlobals.updateWidgetFont = true;
            });
        }
    );
    UIMap["to widgets from style"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            delayedFunctions.emplace([]() {
                WIDGET & widgets = GUI.getWidget("UI creator", winID);
                WIDGET & widgets_style = GUI.getWidget("Widgets style", winID);
                widgets.hidden = false;
                widgets_style.hidden = true;
                widgets.screen_region = widgets_style.screen_region;
            });
        }
    );

    // Elements switching, layout switching
    // -------------------------------------------
    UIMap["elements list"].setActiveCallback(
        updateUIFromElement
    );
    UIMap["rows list"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;

            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            ui_string_group & rows_list = *UIMap["rows list"]._data.usgPtr;
            ui_string_group & cols_list = *UIMap["cols list"]._data.usgPtr;
            cols_list.selected_element = -1;
            cols_list.elements.clear();
            if (rows_list.selected_element == -1)
                return;

            for (int i = 0; i < w.layout_grid[rows_list.selected_element].cells.size(); i++) {
                cols_list.elements.push_back(std::to_string(i));
            }
            UIMap["row height"]._data.f = w.layout_grid[ rows_list.selected_element ].min_height * UI_FLT_VAL_SCALE;
        }
    );
    UIMap["cols list"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            ui_string_group & rows_list = *UIMap["rows list"]._data.usgPtr;
            if (rows_list.selected_element == -1)
                return;
            ui_string_group & cols_list = *UIMap["cols list"]._data.usgPtr;
            if (cols_list.selected_element == -1)
                return;
            UIMap["col width"]._data.f = w.layout_grid[ rows_list.selected_element ].cells[ cols_list.selected_element ].min_width * UI_FLT_VAL_SCALE;
        }
    );


    // Layout properties
    // -------------------------------------------
    UIMap["row height"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            ui_string_group & rows_list = *UIMap["rows list"]._data.usgPtr;
            if (rows_list.selected_element == -1)
                return;
            w.updateRowHeight(rows_list.selected_element, UIMap["row height"]._data.f / UI_FLT_VAL_SCALE);
        }
    );
    UIMap["col width"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            ui_string_group & rows_list = *UIMap["rows list"]._data.usgPtr;
            if (rows_list.selected_element == -1)
                return;
            ui_string_group & cols_list = *UIMap["cols list"]._data.usgPtr;
            if (cols_list.selected_element == -1)
                return;
            w.updateColWidth(rows_list.selected_element, cols_list.selected_element, UIMap["col width"]._data.f / UI_FLT_VAL_SCALE);
        }
    );


    // Elements push, switching or adding new element
    // -------------------------------------------
    UIMap["push opt"].setActiveCallback(
        [] (void*) {
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
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
    );
    UIMap["switch type"].setActiveCallback(
        [] (void*) {
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            if (editorGlobals.selectedElement == "None")
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
        }
    );


    // Adding new elements
    // -------------------------------------------
    UIMap["add image"].setActiveCallback(
        [] (void*) {
            addElement(UI_IMAGE);
        }
    );
    UIMap["add label"].setActiveCallback(
        [] (void*) {
            addElement(UI_STRING_LABEL);
        }
    );
    UIMap["add text"].setActiveCallback(
        [] (void*) {
            addElement(UI_STRING_TEXT);
        }
    );
    UIMap["add empty"].setActiveCallback(
        [] (void*) {
            addElement(UI_EMPTY);
        }
    );
    UIMap["add float"].setActiveCallback(
        [] (void*) {
            addElement(UI_FLOAT);
        }
    );
    UIMap["add button"].setActiveCallback(
        [] (void*) {
            addElement(UI_BUTTON);
        }
    );
    UIMap["add progress"].setActiveCallback(
        [] (void*) {
            std::string addedElement = addElement(UI_PROGRESS);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            UIMap[addedElement].modifyable_progress_bar = true;
        }
    );
    UIMap["add checkbox"].setActiveCallback(
        [] (void*) {
            addElement(UI_BOOL);
        }
    );
    UIMap["add int"].setActiveCallback(
        [] (void*) {
            addElement(UI_INT);
        }
    );
    UIMap["add input"].setActiveCallback(
        [] (void*) {
            addElement(UI_STRING_INPUT);
        }
    );



    // Element's numeric properties
    // -------------------------------------------
    UIMap["element width"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedElement == "None")
                return;
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            UI_element & e = UIMap[editorGlobals.selectedElement];
            e.width = UIMap["element width"]._data.f / UI_FLT_VAL_SCALE;
        }
    );
    UIMap["element height"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedElement == "None")
                return;
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            UI_element & e = UIMap[editorGlobals.selectedElement];
            e.height = UIMap["element height"]._data.f / UI_FLT_VAL_SCALE;
        }
    );
    UIMap["element pad x"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedElement == "None")
                return;
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            UI_element & e = UIMap[editorGlobals.selectedElement];
            e.padding.w = UIMap["element pad x"]._data.f / UI_FLT_VAL_SCALE;
        }
    );
    UIMap["element pad y"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedElement == "None")
                return;
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            UI_element & e = UIMap[editorGlobals.selectedElement];
            e.padding.h = UIMap["element pad y"]._data.f / UI_FLT_VAL_SCALE;
        }
    );
    UIMap["element border"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedElement == "None")
                return;
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            UI_element & e = UIMap[editorGlobals.selectedElement];
            e.border = UIMap["element border"]._data.f / UI_FLT_VAL_SCALE;
        }
    );
    UIMap["element rounding"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedElement == "None")
                return;
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            UI_element & e = UIMap[editorGlobals.selectedElement];
            e.rounding = UIMap["element rounding"]._data.f / UI_FLT_VAL_SCALE;
        }
    );
    UIMap["rename element"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedElement == "None" || editorGlobals.selectedWidget == "None")
                return;
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            UI_element & e = UIMap[editorGlobals.selectedElement];
            std::string new_name = *UIMap["elt name"]._data.strPtr;
            if (new_name.size() > 0 && UIMap.elements.find(new_name) == UIMap.elements.end()) {
                UIMap.renameElement(
                    editorGlobals.selectedElement,
                    new_name,
                    & GUI.getWidget(editorGlobals.selectedWidget, winID)
                );
            }
        }
    );
    UIMap["element text align"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedElement == "None")
                return;
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            UI_element & e = UIMap[editorGlobals.selectedElement];
            if (e.text_align == IndieGo::UI::TEXT_ALIGN::CENTER) {
                e.text_align = IndieGo::UI::TEXT_ALIGN::RIGHT;
            } else if (e.text_align == IndieGo::UI::TEXT_ALIGN::RIGHT) {
                e.text_align = IndieGo::UI::TEXT_ALIGN::LEFT;
            } else if (e.text_align == IndieGo::UI::TEXT_ALIGN::LEFT) {
                e.text_align = IndieGo::UI::TEXT_ALIGN::CENTER;
            }
            UIMap["element text align"].label = getTextAlignLabel(e.text_align);
        }
    );


    // Styling properties
    // -------------------------------------------
    UIMap["styling elements"].setActiveCallback(
        [] (void*) {
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            ui_string_group & style_elements_list = *UIMap["styling elements"]._data.usgPtr;
            editorGlobals.styling_element = style_elements_list.selected_element;
        }
    );
    UIMap["red"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None" || editorGlobals.styling_element == -1)
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            w.style.elements[editorGlobals.styling_element].r = UIMap["red"]._data.ui;
        }
    );
    UIMap["green"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None" || editorGlobals.styling_element == -1)
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            w.style.elements[editorGlobals.styling_element].g = UIMap["green"]._data.ui;
        }
    );
    UIMap["blue"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None" || editorGlobals.styling_element == -1)
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            w.style.elements[editorGlobals.styling_element].b = UIMap["blue"]._data.ui;
        }
    );
    UIMap["alpha"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None" || editorGlobals.styling_element == -1)
                return;
            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            w.style.elements[editorGlobals.styling_element].a = UIMap["alpha"]._data.ui;
        }
    );


    // Skinning properties
    // -------------------------------------------
    UIMap["e apply skin"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedSkinningElement == -1)
                return;
            if (editorGlobals.selectedElement == "None")
                return;
            
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            TexData td = Manager::load_image(UIMap["e skin image path"].label);
            GUI.skinning_image = UIMap["e skin image path"].label;
            UI_element& e = UIMap[editorGlobals.selectedElement];
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
                (IMAGE_SKIN_ELEMENT)editorGlobals.selectedSkinningElement
            );
        }
    );
    UIMap["e skinning properties"].setActiveCallback(
        [] (void*) {
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            ui_string_group & e_skinning_props_list = *UIMap["e skinning properties"]._data.usgPtr;
            editorGlobals.selectedSkinningElement = e_skinning_props_list.selected_element;
        }
    );
    UIMap["w skin crops list"].setActiveCallback(
        [] (void*) {
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            ui_string_group & skin_crops_list = *UIMap["w skin crops list"]._data.usgPtr;
            if (skin_crops_list.selected_element == -1)
                return;
            
            editorGlobals.selectedSkinCrop = skin_crops_list.getSelected();
            // update displayed crop to newly selected crop
            UIMap["w crop x"]._data.f = skin_crops[skin_crops_list.getSelected()].second.x * UI_FLT_VAL_SCALE;
            UIMap["w crop y"]._data.f = skin_crops[skin_crops_list.getSelected()].second.y * UI_FLT_VAL_SCALE;
            UIMap["w crop w"]._data.f = skin_crops[skin_crops_list.getSelected()].second.w * UI_FLT_VAL_SCALE;
            UIMap["w crop h"]._data.f = skin_crops[skin_crops_list.getSelected()].second.h * UI_FLT_VAL_SCALE;
        }
    );
    UIMap["w skinning properties"].setActiveCallback(
        [] (void*) {
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            ui_string_group & w_skinning_props_list = *UIMap["w skinning properties"]._data.usgPtr;
            if (w_skinning_props_list.selected_element == -1)
                return;
            
            editorGlobals.selectedSkinningWidget = w_skinning_props_list.selected_element;
            UIMap["w selected prop"].label = "selected property: " + getSkinPropName((IMAGE_SKIN_ELEMENT)editorGlobals.selectedSkinningWidget);
        }
    );


    // Skinning image crops
    // -------------------------------------------
    UIMap["w new crop"].setActiveCallback(
        [] (void*) {
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            std::string new_crop_name = *UIMap["w new crop name"]._data.strPtr;
            ui_string_group & skin_crops_list = *UIMap["w skin crops list"]._data.usgPtr;
            if (new_crop_name.size() > 0 && skin_crops.find(new_crop_name) == skin_crops.end()) {
                region<float> crop;
                crop.x = UIMap["w crop x"]._data.f / UI_FLT_VAL_SCALE;
                crop.y = UIMap["w crop y"]._data.f / UI_FLT_VAL_SCALE;
                crop.w = UIMap["w crop w"]._data.f / UI_FLT_VAL_SCALE;
                crop.h = UIMap["w crop h"]._data.f / UI_FLT_VAL_SCALE;

                TexData td = Manager::load_image(UIMap["w skin image path"].label, true);
                skin_crops[new_crop_name] = { td, crop };
                skin_crops_list.elements.push_back(new_crop_name);
            }
        }
    );
    UIMap["e load skin image"].setActiveCallback(
        loadSkinImage
    );
    UIMap["w load skin image"].setActiveCallback(
        loadSkinImage
    );
    UIMap["w apply crop"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;

            if (editorGlobals.selectedSkinCrop == "None")
                return;

            if (editorGlobals.selectedSkinningWidget == -1)
                return;

            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, winID);
            TexData td =  skin_crops[editorGlobals.selectedSkinCrop].first;
            region<float> crop = skin_crops[editorGlobals.selectedSkinCrop].second;
            td.texID = skinningShader.skin_tex_id;
            td.w = GUI.skin_img_size.w;
            td.h = GUI.skin_img_size.h;
            w.useSkinImage(
                td.texID,
                td.w,
                td.h,
                crop,
                (IMAGE_SKIN_ELEMENT)editorGlobals.selectedSkinningWidget
            );
        }
    );
    UIMap["w drop crop"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedWidget == "None")
                return;
            
            if (editorGlobals.selectedSkinningWidget == -1)
                return;

            WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, winID);
            w.skinned_style.props[(IMAGE_SKIN_ELEMENT)editorGlobals.selectedSkinningWidget] = { -1, -1 };
        }
    );
    UIMap["init button img"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedSkinCrop == "None")
                return;

            if (editorGlobals.selectedElement == "None")
                return;

            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            TexData td =  skin_crops[editorGlobals.selectedSkinCrop].first;
            region<float> crop = skin_crops[editorGlobals.selectedSkinCrop].second;
            UIMap[editorGlobals.selectedElement].initImage(
                td.texID,
                td.w,
                td.h,
                crop
            );
        }
    );
    UIMap["crop to img"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedSkinCrop == "None")
                return;

            if (editorGlobals.selectedSkinCrop == "None")
                return;

            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            TexData td =  skin_crops[editorGlobals.selectedSkinCrop].first;
            region<float> crop = skin_crops[editorGlobals.selectedSkinCrop].second;
            UIMap[editorGlobals.selectedSkinCrop].initImage(
                td.texID,
                td.w,
                td.h,
                crop
            );
        }
    );



    // Save/load UI data
    // -------------------------------------------
    UIMap["save ui"].setActiveCallback(
        saveUI
    );
    UIMap["load ui"].setActiveCallback(
        loadUI
    );
    UIMap["close ui"].setActiveCallback(
        closeUICallback
    );
    UIMap["select_project_dir"].setActiveCallback(
        [] (void*) {
            UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
            std::vector<std::string> paths = getPaths(true, true);
            if (paths.size() > 0) {
                GUI.project_dir = paths[0];
                *UIMap["project_dir_path"]._data.strPtr = GUI.project_dir;
            }
        }
    );


    // Font
    // -------------------------------------------
    UIMap["load font"].setActiveCallback(
        loadFont
    );
    UIMap["apply font"].setActiveCallback(
        [] (void*) {
            if (editorGlobals.selectedFont == "None")
                return;
            if (editorGlobals.fontSize == -1)
                return;
            if (editorGlobals.updateWidgetFont) {
                if (editorGlobals.selectedWidget == "None")
                    return;
                WIDGET& w = GUI.getWidget(editorGlobals.selectedWidget, winID);
                w.font = editorGlobals.selectedFont;
                w.font_size = editorGlobals.fontSize;
            } else {
                if (editorGlobals.selectedElement == "None")
                    return;
                UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
                UI_element& e = UIMap[editorGlobals.selectedElement];
                e.font = editorGlobals.selectedFont;
                e.font_size = editorGlobals.fontSize;
            }
        }
    );
    UIMap["loaded fonts"].setActiveCallback(
        [] (void*) {
            UI_elements_map & UIMap = GUI.UIMaps[winID];
            ui_string_group & fonts_list = *UIMap["loaded fonts"]._data.usgPtr;
            ui_string_group & font_sizes_list = *UIMap["font sizes"]._data.usgPtr;
            font_sizes_list.elements.clear();
            font_sizes_list.selected_element = -1;
            editorGlobals.fontSize = -1;
            if (fonts_list.selected_element == -1) {
                editorGlobals.selectedFont = "None";
            } else {
                editorGlobals.selectedFont = fonts_list.getSelected();
                for (auto size : GUI.loaded_fonts[editorGlobals.selectedFont].sizes) {
                    font_sizes_list.elements.push_back(
                        std::to_string(size)
                    );
                }
            }
        }
    );
    UIMap["font sizes"].setActiveCallback(
        [] (void*) {
            UI_elements_map & UIMap = GUI.UIMaps[winID];
            ui_string_group & font_sizes_list = *UIMap["font sizes"]._data.usgPtr;
            editorGlobals.fontSize = font_sizes_list.selected_element;
        } 
    );
}