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

namespace fs = std::filesystem;
using namespace IndieGo::UI;

extern Manager GUI;
extern WIDGET creator_widget;
extern WIDGET styling_widget;
extern std::string winID;
extern EditorState editorGlobals;

extern std::string getColorPropName(COLOR_ELEMENTS prop);
extern std::string getSkinPropName(IMAGE_SKIN_ELEMENT prop);
std::vector<std::string> editorWidgets = {};

extern void setCallbacks();


void initWidgets() {
    // initialize creator widget
    creator_widget.screen_region.x = (float)(WIDTH / 4) / ((float)WIDTH);
    creator_widget.screen_region.y = ((float)HEIGHT / 4) / ((float)HEIGHT);
    creator_widget.screen_region.w = 0.4f;
    creator_widget.screen_region.h = 0.56f;
    creator_widget.name = "UI creator";
    editorWidgets.push_back(creator_widget.name);
    creator_widget.has_scrollbar = false;

    WIDGET & c_widget = GUI.addWidget(creator_widget, winID);
    
    // control elements to create ui
    UI_elements_map & UIMap = GUI.UIMaps[winID];

    UIMap.addElement("widgets list", UI_ITEMS_LIST, &c_widget);
    UIMap["widgets list"].label = "widgets list";
    UIMap["widgets list"].text_align = IndieGo::UI::TEXT_ALIGN::LEFT;
    c_widget.updateRowHeight(c_widget.layout_grid.size() - 1, 0.855f);

    UIMap.addElement("new widget name label", UI_STRING_LABEL, &c_widget, to_new_col);
    UIMap["new widget name label"].label = "new widget name";

    // create new widget button
    UIMap.addElement("add new widget", UI_BUTTON, &c_widget, to_new_subrow);
    UIMap["add new widget"].label = "add widget";

    UIMap.addElement("rename widget", UI_BUTTON, &c_widget, to_new_subrow);
    UIMap["rename widget"].label = "rename widget";

    // different flag settings
    UIMap.addElement("bordered", UI_BOOL, &c_widget, to_new_subrow);
    UIMap["bordered"].label = "bordered";
    UIMap["bordered"].height = 0.055f;
    UIMap.addElement("titled", UI_BOOL, &c_widget, to_new_subrow);

    UIMap["titled"].label = "titled";
    UIMap["titled"].height = 0.055f;
    UIMap.addElement("minimizable", UI_BOOL, &c_widget, to_new_subrow);
    UIMap["minimizable"].label = "minimizable";
    UIMap["minimizable"].height = 0.055f;
    UIMap.addElement("scalable", UI_BOOL, &c_widget, to_new_subrow);
    UIMap["scalable"].label = "scalable";
    UIMap["scalable"].height = 0.055f;
    UIMap.addElement("movable", UI_BOOL, &c_widget, to_new_subrow);
    UIMap["movable"].label = "movable";
    UIMap["movable"].height = 0.055f;
    UIMap.addElement("has scrollbar", UI_BOOL, &c_widget, to_new_subrow);
    UIMap["has scrollbar"].label = "scrollbar";
    UIMap["has scrollbar"].height = 0.055f;

    // switch to edit widget's elements
    UIMap.addElement("edit widget elements", UI_BUTTON, &c_widget, to_new_subrow);
    UIMap["edit widget elements"].label = "edit widget elements";
    UIMap.addElement("skins and styling", UI_BUTTON, &c_widget, to_new_subrow);
    UIMap["skins and styling"].label = "skins and styling";

    UIMap.addElement("save ui", UI_BUTTON, &c_widget, to_new_subrow);
    UIMap["save ui"].label = "save ui";
    UIMap.addElement("load ui", UI_BUTTON, &c_widget, to_new_subrow);
    UIMap["load ui"].label = "load ui";
    UIMap.addElement("close ui", UI_BUTTON, &c_widget, to_new_subrow);
    UIMap["close ui"].label = "close ui";

    UIMap.addElement("new widget name", UI_STRING_INPUT, &c_widget, to_new_col);

    UIMap.addElement("delete widget", UI_BUTTON, &c_widget, to_new_subrow);
    UIMap["delete widget"].label = "delete widget";

    UIMap.addElement("space fill", UI_EMPTY, &c_widget, to_new_subrow);

    // coordinates of new widget
    UIMap.addElement("location x", UI_FLOAT, &c_widget, to_new_subrow);
    UIMap["location x"].minf = 0.f;
    UIMap["location x"].maxf = 100.f;
    UIMap["location x"].label = "loc x";
    UIMap["location x"]._data.f = 10.f;
    UIMap.addElement("location y", UI_FLOAT, &c_widget, to_new_subrow);
    UIMap["location y"].minf = 0.f;
    UIMap["location y"].maxf = 100.f;
    UIMap["location y"].label = "loc y";
    UIMap["location y"]._data.f = 10.f;
    UIMap.addElement("size x", UI_FLOAT, &c_widget, to_new_subrow);
    UIMap["size x"].label = "size x";
    UIMap["size x"].minf = 0.f;
    UIMap["size x"].maxf = 100.f;
    UIMap["size x"]._data.f = 20.f;
    UIMap.addElement("size y", UI_FLOAT, &c_widget, to_new_subrow);
    UIMap["size y"].label = "size y";
    UIMap["size y"]._data.f = 20.f;
    UIMap["size y"].minf = 0.f;
    UIMap["size y"].maxf = 100.f;
    UIMap.addElement("widget border", UI_FLOAT, &c_widget, to_new_subrow);
    UIMap["widget border"].label = "border";
    UIMap["widget border"]._data.f = 1.f;
    UIMap.addElement("visible", UI_BOOL, &c_widget, to_new_subrow);
    UIMap["visible"].label = "visible";

    UIMap.addElement("project_dir_label", UI_STRING_LABEL, &c_widget, to_new_row);
    UIMap["project_dir_label"].label = "PROJECT_DIR :";

    UIMap.addElement("project_dir_path", UI_STRING_INPUT, &c_widget, to_new_col);
    UIMap["project_dir_path"].label = "None";

    UIMap.addElement("select_project_dir", UI_BUTTON, &c_widget, to_new_col);
    UIMap["select_project_dir"].label = "Select PROJECT_DIR";

    c_widget.updateColWidth(1, 0, 0.15f);
    c_widget.updateColWidth(1, 1, 0.6f);
    c_widget.updateColWidth(1, 2, 0.24f);
    c_widget.updateRowHeight(1, 0.07f);

    // "AddElement" calls with "autowidth" modify width settings, so keep them after all
    // such calls
    // first column and it's elements
    c_widget.updateColWidth(0, 0, 0.5f);
    UIMap["widgets list"].width = 0.5f;

    // second column and it's elements
    c_widget.updateColWidth(0, 1, 0.21f);

    // c_widget.layout_grid[0].cells[1].min_width = 0.2f;
    UIMap["new widget name label"].height = 0.065f;
    UIMap["new widget name label"].width = 0.2f;
    UIMap["add new widget"].width = 0.2f;
    UIMap["add new widget"].height = 0.065f;
    UIMap["rename widget"].height = 0.065f;
    UIMap["rename widget"].width = 0.2f;
    UIMap["edit widget elements"].width = 0.493f;
    UIMap["edit widget elements"].height = 0.065f;
    UIMap["skins and styling"].width = 0.493f;
    UIMap["skins and styling"].height = 0.065f;
    UIMap["save ui"].width = 0.493f;
    UIMap["save ui"].height = 0.065f;
    UIMap["load ui"].width = 0.493f;
    UIMap["load ui"].height = 0.065f;
    UIMap["close ui"].width = 0.493f;
    UIMap["close ui"].height = 0.065f;

    // third column and it's elements
    c_widget.updateColWidth(0, 2, 0.24f);

    UIMap["new widget name"].width = 0.28f;
    UIMap["new widget name"].height = 0.065f;
    UIMap["delete widget"].height = 0.065f;
    UIMap["delete widget"].width = 0.28f;
    UIMap["space fill"].height = 0.065f;
    UIMap["location x"].width = 0.28f;
    UIMap["location x"].height = 0.055f;
    UIMap["location y"].width = 0.28f;
    UIMap["location y"].height = 0.055f;
    UIMap["size x"].width = 0.28f;
    UIMap["size x"].height = 0.055f;
    UIMap["size y"].width = 0.28f;
    UIMap["size y"].height = 0.055f;
    UIMap["widget border"].width = 0.28f;
    UIMap["widget border"].height = 0.055f;
    UIMap["visible"].width = 0.28f;
    UIMap["visible"].height = 0.055f;

    WIDGET elements_edit;
    elements_edit.screen_region.x = (float)(WIDTH / 4) / ((float)WIDTH);
    elements_edit.screen_region.y = ((float)HEIGHT / 4) / ((float)HEIGHT);
    elements_edit.screen_region.w = 0.4f;
    elements_edit.screen_region.h = 0.4f;
    elements_edit.name = "Edit elements";
    editorWidgets.push_back(elements_edit.name);

    WIDGET & e_widget = GUI.addWidget(elements_edit, winID);
    e_widget.hidden = true;
    e_widget.has_scrollbar = false;

    // Column 0 - takes full widget space
    // --------------------------------------------------------
    UIMap.addElement("elements list", UI_ITEMS_LIST, &e_widget);
    UIMap["elements list"].label = "elements list";    
    UIMap["elements list"].text_align = IndieGo::UI::TEXT_ALIGN::LEFT;

    // --------------------------------------------------------

    // Column 1 - under it "width" and "height" elements, also "selected widget" text
    // --------------------------------------------------------
    UIMap.addElement("rows list", UI_ITEMS_LIST, &e_widget, to_new_col);
    UIMap["rows list"].label = "rows";
    UIMap["rows list"].text_align = IndieGo::UI::TEXT_ALIGN::LEFT;

    UIMap.addElement("row height", UI_FLOAT, &e_widget, to_new_subrow);
    UIMap["row height"].label = "row height";

    UIMap.addElement("col width", UI_FLOAT, &e_widget, to_new_subrow);
    UIMap["col width"].label = "col width";

    UIMap.addElement("element borders", UI_BOOL, &e_widget, to_new_subrow);
    UIMap["element borders"].label = "element borders";

    UIMap.addElement("layout borders", UI_BOOL, &e_widget, to_new_subrow);
    UIMap["layout borders"].label = "layout borders";

    UIMap.addElement("selected widget", UI_STRING_TEXT, &e_widget, to_new_subrow);
    UIMap["selected widget"].label = "selected widget:";

    // --------------------------------------------------------

    // Column 2 - nothing under it
    // --------------------------------------------------------
    UIMap.addElement("cols list", UI_ITEMS_LIST, &e_widget, to_new_col);
    UIMap["cols list"].label = "cols";
    UIMap["cols list"].text_align = IndieGo::UI::TEXT_ALIGN::LEFT;

    // Column 3 - half of buttons
    // --------------------------------------------------------
    UIMap.addElement("new element name label", UI_STRING_LABEL, &e_widget, to_new_col);
    UIMap["new element name label"].label = "new element name:";
    UIMap["new element name label"].text_align = IndieGo::UI::TEXT_ALIGN::LEFT;
    
    UIMap.addElement("new element name", UI_STRING_INPUT, &e_widget, to_new_subrow);
    UIMap["new element name"].label = "new element name";

    UIMap.addElement("switch type", UI_BOOL, &e_widget, to_new_subrow);
    UIMap["switch type"].label = "switch type";

    UIMap.addElement("selected anchor", UI_BOOL, &e_widget, to_new_subrow);
    UIMap["selected anchor"].label = "Use selected as anchor";

    UIMap.addElement("push after anchor", UI_BOOL, &e_widget, to_new_subrow);
    UIMap["push after anchor"].label = "push after anchor";
    // UIMap.addElement("rename element", UI_BUTTON, &e_widget, to_new_subrow);
    // UIMap["rename element"].label = "rename element";

    UIMap.addElement("push opt", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["push opt"].label = "push: to new row";

    UIMap.addElement("delete element", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["delete element"].label = "delete elt";

    UIMap.addElement("keep space", UI_BOOL, &e_widget, to_new_subrow);
    UIMap["keep space"].label = "keep space";

    UIMap.addElement("back to widgets", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["back to widgets"].label = "back to widgets";

    UIMap.addElement("empty_0", UI_EMPTY, &e_widget, to_new_subrow);
    // UIMap["add input"].label = "add input";

    // UIMap.addElement("empty_1", UI_EMPTY, &e_widget, to_new_subrow);

    // UIMap.addElement("empty_2", UI_EMPTY, &e_widget, to_new_subrow);
    // UIMap["add int"].label = "add int";

    // Column 3 - last column
    // --------------------------------------------------------
    UIMap.addElement("add image", UI_BUTTON, &e_widget, to_new_col);

    UIMap["add image"].label = "add image";

    UIMap.addElement("add label", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add label"].label = "add label";

    UIMap.addElement("add text", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add text"].label = "add text";
    
    UIMap.addElement("add empty", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add empty"].label = "add empty";

    UIMap.addElement("add float", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add float"].label = "add float";

    UIMap.addElement("add button", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add button"].label = "add button";

    UIMap.addElement("add progress", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add progress"].label = "add progress";

    UIMap.addElement("add checkbox", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add checkbox"].label = "add checkbox";

    UIMap.addElement("add input", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add input"].label = "add input";

    UIMap.addElement("add int", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add int"].label = "add int";

    e_widget.updateRowHeight(e_widget.layout_grid.size() - 1, 0.9f);
    e_widget.updateColWidth(e_widget.layout_grid.size() - 1, 0, 0.35f);
    e_widget.updateColWidth(e_widget.layout_grid.size() - 1, 1, 0.1f);
    e_widget.updateColWidth(e_widget.layout_grid.size() - 1, 2, 0.1f);
    e_widget.updateColWidth(e_widget.layout_grid.size() - 1, 3, 0.225f);
    e_widget.updateColWidth(e_widget.layout_grid.size() - 1, 4, 0.225f);
    
    UIMap["rows list"].height *= 4.f;
    UIMap["cols list"].height = UIMap["rows list"].height;

    std::vector<std::string> similar_changes = {
        "row height",
        "col width",
        "element borders",
        "layout borders",
        "selected widget"
    };

    for (auto name : similar_changes) {
        UIMap[name].width *= 2.f;    
        UIMap[name].height /= 3.f;
    }

    WIDGET widgets_style;
    widgets_style.screen_region.x = (float)(WIDTH / 4) / ((float)WIDTH);
    widgets_style.screen_region.y = ((float)HEIGHT / 4) / ((float)HEIGHT);
    widgets_style.screen_region.w = 0.4f;
    widgets_style.screen_region.h = 0.4f;
    widgets_style.name = "Widgets style";
    editorWidgets.push_back(widgets_style.name);

    WIDGET & ws_widget = GUI.addWidget(widgets_style, winID);
    ws_widget.hidden = true;
    ws_widget.has_scrollbar = false;

    // styling:
    // elements list for style update, rgba values
    UIMap.addElement("styling elements", UI_ITEMS_LIST, &ws_widget);
    UIMap["styling elements"].label = "styling elements";
    ui_string_group & style_elements_list = *UIMap["styling elements"]._data.usgPtr;
    for (int i = UI_COLOR_TEXT; i != UI_COLOR_TAB_HEADER; i++) {
        style_elements_list.elements.push_back(
            getColorPropName((COLOR_ELEMENTS)i)
        );
    }
    UIMap.addElement("red", UI_UINT, &ws_widget, to_new_subrow);
    UIMap["red"].label = "red";
    UIMap["red"].max = 255;
    UIMap.addElement("green", UI_UINT, &ws_widget, to_new_subrow);
    UIMap["green"].label = "green";
    UIMap["green"].max = 255;
    UIMap.addElement("blue", UI_UINT, &ws_widget, to_new_subrow);
    UIMap["blue"].label = "blue";
    UIMap["blue"].max = 255;
    UIMap.addElement("alpha", UI_UINT, &ws_widget, to_new_subrow);
    UIMap["alpha"].label = "alpha";
    UIMap["alpha"].max = 255;
    UIMap.addElement("to widgets from style", UI_BUTTON, &ws_widget, to_new_subrow);
    UIMap["to widgets from style"].label = "back to widgets";

    UIMap.addElement("style selected widget", UI_STRING_LABEL, &ws_widget, to_new_subrow);
    UIMap["style selected widget"].label = "selected widget: None";
    

    UIMap["styling elements"].height = 0.57f;
    UIMap["red"].height = 0.05f;
    UIMap["green"].height = 0.05f;
    UIMap["blue"].height = 0.05f;
    UIMap["alpha"].height = 0.05f;
    UIMap["to widgets from style"].height = 0.05f;

    UIMap.addElement("w skinning properties", UI_ITEMS_LIST, &ws_widget, to_new_col);
    UIMap["w skinning properties"].label = "skinning properties";

    ui_string_group & w_skinning_props_list = *UIMap["w skinning properties"]._data.usgPtr;
    for (int i = background; i < prop_hover + 1; i++) {
        w_skinning_props_list.elements.push_back(
            getSkinPropName((IMAGE_SKIN_ELEMENT)i)
        );
    }
    UIMap["w skinning properties"].height = 0.9f;
    
    WIDGET elements_style;
    elements_style.screen_region.x = (float)(WIDTH / 4) / ((float)WIDTH);
    elements_style.screen_region.y = ((float)HEIGHT / 4) / ((float)HEIGHT);
    elements_style.screen_region.w = 0.3f;
    elements_style.screen_region.h = 0.3f;
    elements_style.name = "Elements style";
    editorWidgets.push_back(elements_style.name);

    WIDGET & es_widget = GUI.addWidget(elements_style, winID);
    es_widget.hidden = true;
    es_widget.has_scrollbar = false;

    // skinning:
    // load skin image, skinning elements list, crop settings, apply skin
    UIMap.addElement("e load skin image", UI_BUTTON, &es_widget);
    UIMap["e load skin image"].label = "load skin image";

    UIMap.addElement("e skin image path", UI_STRING_TEXT, &es_widget, to_new_subrow);
    UIMap["e skin image path"].label = "skin image path: None";

    UIMap.addElement("e crop x", UI_FLOAT, &es_widget, to_new_subrow);
    UIMap["e crop x"].label = "crop x";
    UIMap["e crop x"].maxf = 100.f;
    UIMap["e crop x"].minf = 0.f;
    UIMap.addElement("e crop y", UI_FLOAT, &es_widget, to_new_subrow);
    UIMap["e crop y"].label = "crop y";
    UIMap["e crop y"].maxf = 100.f;
    UIMap["e crop y"].minf = 0.f;
    UIMap.addElement("e crop w", UI_FLOAT, &es_widget, to_new_subrow);
    UIMap["e crop w"].label = "crop w";
    UIMap["e crop w"].maxf = 100.f;
    UIMap["e crop w"].minf = 0.f;
    UIMap.addElement("e crop h", UI_FLOAT, &es_widget, to_new_subrow);
    UIMap["e crop h"].label = "crop h";
    UIMap["e crop h"].maxf = 100.f;
    UIMap["e crop h"].minf = 0.f;

    UIMap.addElement("e apply skin", UI_BUTTON, &es_widget, to_new_subrow);
    UIMap["e apply skin"].label = "apply skin";
    
    UIMap.addElement("to elements from style", UI_BUTTON, &es_widget, to_new_subrow);
    UIMap["to elements from style"].label = "back to elements";

    UIMap.addElement("e selected element", UI_STRING_LABEL, &es_widget, to_new_subrow);
    UIMap["e selected element"].label = "selected element: None";

    UIMap.addElement("e skinning properties", UI_ITEMS_LIST, &es_widget, to_new_col);
    UIMap["e skinning properties"].label = "skinning properties";
    ui_string_group & e_skinning_props_list = *UIMap["e skinning properties"]._data.usgPtr;
    for (int i = background; i < prop_hover + 1; i++) {
        e_skinning_props_list.elements.push_back(
            getSkinPropName((IMAGE_SKIN_ELEMENT)i)
        );
    }

    es_widget.updateRowHeight(0, 0.8f);

    // custom sizes for elements
    UIMap["e load skin image"].height = 0.05f;
    UIMap["e skin image path"].height = 0.15f;
    UIMap["e crop x"].height = 0.05f;
    UIMap["e crop y"].height = 0.05f;
    UIMap["e crop w"].height = 0.05f;
    UIMap["e crop h"].height = 0.05f;

    UIMap["e apply skin"].height = 0.05f;
    UIMap["to elements from style"].height = 0.05f;
    UIMap["e selected element"].height = 0.15f;

    WIDGET fonts;
    fonts.screen_region.x = 0.6f;
    fonts.screen_region.y = 0.f;
    fonts.screen_region.w = 0.3f;
    fonts.screen_region.h = 0.25f;
    fonts.name = "Fonts";
    editorWidgets.push_back(fonts.name);

    WIDGET & fonts_widget = GUI.addWidget(fonts, winID);
    fonts_widget.has_scrollbar = false;

    // Fonts:
    // load button, fonts list, sizes per font list, size on load spec
    UIMap.addElement("load font", UI_BUTTON, &fonts_widget);
    UIMap["load font"].label = "load font";

    UIMap.addElement("load size", UI_FLOAT, &fonts_widget, to_new_subrow);
    UIMap["load size"].label = "load size";
    UIMap["load size"]._data.f = 16.f;

    UIMap.addElement("selected for font update", UI_STRING_TEXT, &fonts_widget, to_new_subrow);
    UIMap["selected for font update"].label = "selected: None";

    UIMap.addElement("apply font", UI_BUTTON, &fonts_widget, to_new_subrow);
    UIMap["apply font"].label = "apply font";

    UIMap.addElement("current font", UI_STRING_LABEL, &fonts_widget, to_new_subrow);
    UIMap["current font"].label = "None";
    UIMap["current font"].text_align = IndieGo::UI::TEXT_ALIGN::LEFT;

    UIMap.addElement("loaded fonts", UI_ITEMS_LIST, &fonts_widget, to_new_col);
    UIMap["loaded fonts"].label = "loaded fonts";
    UIMap["loaded fonts"].text_align = IndieGo::UI::TEXT_ALIGN::LEFT;

    UIMap.addElement("font sizes", UI_ITEMS_LIST, &fonts_widget, to_new_col);
    UIMap["font sizes"].label = "sizes";
    fonts_widget.updateRowHeight(0, 0.8f);
    fonts_widget.updateColWidth(0, 0, 0.37f);
    fonts_widget.updateColWidth(0, 1, 0.45f);
    fonts_widget.updateColWidth(0, 2, 0.15f);

    UIMap["load font"].height = 0.1f;
    UIMap["load size"].height = 0.1f;
    UIMap["apply font"].height = 0.1f;
  

    // Skinning elements widget:
    // load skin image button and path, display image, selected skinning property coords, image scale and location on screen
    WIDGET skinning;
    skinning.screen_region.x = 0.83f;
    skinning.screen_region.y = 0.29f;
    skinning.screen_region.w = 0.12f;
    skinning.screen_region.h = 0.7f;
    skinning.name = "Skinning";
    editorWidgets.push_back(skinning.name);

    WIDGET & skinning_widget = GUI.addWidget(skinning, winID);
    skinning_widget.has_scrollbar = false;

    UIMap.addElement("w load skin image", UI_BUTTON, &skinning_widget);
    UIMap["w load skin image"].label = "load skin image";

    UIMap.addElement("w display skin image", UI_BOOL, &skinning_widget, to_new_row);
    UIMap["w display skin image"].label = "show skin image";

    UIMap.addElement("w skin image scale", UI_FLOAT, &skinning_widget, to_new_row);
    UIMap["w skin image scale"].maxf = 100000.f;
    UIMap["w skin image scale"].minf = 0.f;
    UIMap["w skin image scale"].label = "skin image scale";
    UIMap["w skin image scale"]._data.f = 1000.f;

    UIMap.addElement("w skin image x", UI_FLOAT, &skinning_widget, to_new_row);
    UIMap["w skin image x"].maxf = 100000.f;
    UIMap["w skin image x"].minf = -100000.f;
    UIMap["w skin image x"].label = "skin image loc x";
    UIMap["w skin image x"]._data.f = 0.f;
    UIMap.addElement("w skin image y", UI_FLOAT, &skinning_widget, to_new_row);
    UIMap["w skin image y"].maxf = 100000.f;
    UIMap["w skin image y"].minf = -100000.f;
    UIMap["w skin image y"].label = "skin image loc y";
    UIMap["w skin image y"]._data.f = 0.f;

    UIMap.addElement("w crop x", UI_FLOAT, &skinning_widget, to_new_row);
    UIMap["w crop x"].label = "crop x";
    UIMap["w crop x"].maxf = 100.f;
    UIMap["w crop x"].minf = 0.f;
    UIMap["w crop x"]._data.f = 0.25f * UI_FLT_VAL_SCALE;
    UIMap["w crop x"].flt_px_incr = 0.005f;

    UIMap.addElement("w crop y", UI_FLOAT, &skinning_widget, to_new_row);
    UIMap["w crop y"].label = "crop y";
    UIMap["w crop y"].maxf = 100.f;
    UIMap["w crop y"].minf = 0.f;
    UIMap["w crop y"]._data.f = 0.25f * UI_FLT_VAL_SCALE;
    UIMap["w crop y"].flt_px_incr = 0.005f;

    UIMap.addElement("w crop w", UI_FLOAT, &skinning_widget, to_new_row);
    UIMap["w crop w"].label = "crop w";
    UIMap["w crop w"].maxf = 100.f;
    UIMap["w crop w"].minf = 0.f;
    UIMap["w crop w"]._data.f = 0.5f * UI_FLT_VAL_SCALE;
    UIMap["w crop w"].flt_px_incr = 0.005f;

    UIMap.addElement("w crop h", UI_FLOAT, &skinning_widget, to_new_row);
    UIMap["w crop h"].label = "crop h";
    UIMap["w crop h"].maxf = 100.f;
    UIMap["w crop h"].minf = 0.f;
    UIMap["w crop h"]._data.f = 0.5f * UI_FLT_VAL_SCALE;
    UIMap["w crop h"].flt_px_incr = 0.005f;

    UIMap.addElement("w new crop", UI_BUTTON, &skinning_widget, to_new_row);
    UIMap["w new crop"].label = "new crop";

    UIMap.addElement("w new crop name", UI_STRING_INPUT, &skinning_widget, to_new_row);

    UIMap.addElement("w reset crop", UI_BUTTON, &skinning_widget, to_new_row);
    UIMap["w reset crop"].label = "w reset crop";

    UIMap.addElement("w skin image path", UI_STRING_TEXT, &skinning_widget, to_new_row);
    UIMap["w skin image path"].label = "skin image path: None";

    UIMap.addElement("w skin crops list", UI_ITEMS_LIST, &skinning_widget, to_new_row);
    UIMap["w skin crops list"].label = "crops list";
    
    UIMap.addElement("w apply crop", UI_BUTTON, &skinning_widget, to_new_row);
    UIMap["w apply crop"].label = "apply crop";

    UIMap.addElement("w drop crop", UI_BUTTON, &skinning_widget, to_new_row);
    UIMap["w drop crop"].label = "drop crop";

    UIMap.addElement("w selected prop", UI_STRING_LABEL, &skinning_widget, to_new_row);
    UIMap["w selected prop"].label = "selected property: None";

    UIMap.addElement("w selected widget", UI_STRING_LABEL, &skinning_widget, to_new_row);
    UIMap["w selected widget"].label = "selected widget: None";

    skinning_widget.updateRowHeight(0, 0.031f);
    skinning_widget.updateRowHeight(1, 0.031f);
    skinning_widget.updateRowHeight(2, 0.031f);
    skinning_widget.updateRowHeight(3, 0.031f);
    skinning_widget.updateRowHeight(4, 0.031f);
    skinning_widget.updateRowHeight(5, 0.031f);
    skinning_widget.updateRowHeight(6, 0.031f);
    skinning_widget.updateRowHeight(7, 0.031f);
    skinning_widget.updateRowHeight(8, 0.031f);
    skinning_widget.updateRowHeight(9, 0.031f);
    skinning_widget.updateRowHeight(10, 0.031f);
    skinning_widget.updateRowHeight(11, 0.031f);
    skinning_widget.updateRowHeight(12, 0.031f);
    skinning_widget.updateRowHeight(13, 0.35f); // crops list height
    skinning_widget.updateRowHeight(14, 0.031f);
    skinning_widget.updateRowHeight(15, 0.031f);
    skinning_widget.updateRowHeight(16, 0.031f);
    skinning_widget.updateRowHeight(17, 0.031f);

    // Element props widget:
    // all things for element live here
    WIDGET element_props;
    element_props.screen_region.x = 0.7f;
    element_props.screen_region.y = 0.29f;
    element_props.screen_region.w = 0.12f;
    element_props.screen_region.h = 0.5f;
    element_props.name = "Element properties";
    editorWidgets.push_back(element_props.name);

    WIDGET & elt_props_widget = GUI.addWidget(element_props, winID);
    elt_props_widget.has_scrollbar = false;

    // width and height of specific element
    UIMap.addElement("element width", UI_FLOAT, &elt_props_widget);
    UIMap["element width"].label = "width";
    UIMap.addElement("element height", UI_FLOAT, &elt_props_widget);
    UIMap["element height"].label = "height";
    UIMap.addElement("element pad x", UI_FLOAT, &elt_props_widget);
    UIMap["element pad x"].label = "pad x";
    UIMap.addElement("element pad y", UI_FLOAT, &elt_props_widget);
    UIMap["element pad y"].label = "pad y";
    UIMap.addElement("element border", UI_FLOAT, &elt_props_widget);
    UIMap["element border"].label = "border";
    UIMap.addElement("element rounding", UI_FLOAT, &elt_props_widget);
    UIMap["element rounding"].label = "rounding";

    UIMap.addElement("elt label label", UI_STRING_LABEL, &elt_props_widget);
    UIMap["elt label label"].label = "label:";
    UIMap["elt label label"].text_align = IndieGo::UI::TEXT_ALIGN::LEFT;

    // TODO: input callbacks
    UIMap.addElement("elt label", UI_STRING_INPUT, &elt_props_widget);

    UIMap.addElement("init button img", UI_BUTTON, &elt_props_widget);
    UIMap["init button img"].label = "init button img";

    UIMap.addElement("crop as img", UI_BOOL, &elt_props_widget);
    UIMap["crop as img"].label = "use crop as img";

    UIMap.addElement("crop to img", UI_BUTTON, &elt_props_widget);
    UIMap["crop to img"].label = "apply crop";

    UIMap.addElement("elt name label", UI_STRING_LABEL, &elt_props_widget);
    UIMap["elt name label"].label = "name:";
    UIMap["elt name label"].text_align = IndieGo::UI::TEXT_ALIGN::LEFT;

    UIMap.addElement("elt name", UI_STRING_INPUT, &elt_props_widget);

    UIMap.addElement("rename element", UI_BUTTON, &elt_props_widget);
    UIMap["rename element"].label = "rename element";
    
    UIMap.addElement("element text align", UI_BUTTON, &elt_props_widget);
    UIMap["element text align"].label = "text align: left";

    for (int i = 0; i < 15; i++) {
        elt_props_widget.updateRowHeight(i, 0.055f);
    }

    std::string home_dir = fs::current_path().string();
    if (home_dir.find("ui_editor") == std::string::npos) {
        std::cout << "[WARNING] cwd does not contain 'ui_editor'! Can't find default font" << std::endl;
        std::cout << "[WARNING] First loaded font will be used as default." << std::endl;
        return;
    }

    std::string font_path = home_dir.substr(
        0, home_dir.find("ui_editor")
    ) + "ui_editor/editor_font/Roboto-Regular_main.ttf";

    // Comment out this line to use dafule backend's font
    GUI.loadFont(font_path, winID, 18.f);

    setCallbacks();
}

extern std::string home_dir;

void initProjectDir() {
    char* pd = getenv("PROJECT_DIR");
    if (pd) {
        GUI.project_dir = pd;
        std::cout << "PROJECT_DIR initialized from env var with: " << GUI.project_dir << std::endl;
        std::cout << "All paths to resources will be saved relative to " << GUI.project_dir << std::endl;
    } else {
        GUI.project_dir = home_dir;
        std::cout << "PROJECT_DIR initialized with current home folder: " << GUI.project_dir << std::endl;
        std::cout << "All paths to resources will be saved relative to " << GUI.project_dir << std::endl;
    }
}