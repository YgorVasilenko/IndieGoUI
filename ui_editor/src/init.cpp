#include <IndieGoUI.h>
#include <filesystem>

namespace fs = std::filesystem;
using namespace IndieGo::UI;

extern Manager GUI;
extern WIDGET creator_widget;
extern WIDGET styling_widget;
extern std::string winID;

extern std::string getColorPropName(COLOR_ELEMENTS prop);
extern std::string getSkinPropName(IMAGE_SKIN_ELEMENT prop);
void initWidgets() {
    // initialize creator widget
    creator_widget.screen_region.x = (float)(WIDTH / 4) / ((float)WIDTH);
    creator_widget.screen_region.y = ((float)HEIGHT / 4) / ((float)HEIGHT);
    creator_widget.screen_region.w = 0.3f;
    creator_widget.screen_region.h = 0.3f;
    creator_widget.name = "UI creator";

    WIDGET & c_widget = GUI.addWidget(creator_widget, winID);
    
    // control elements to create ui
    UI_elements_map & UIMap = GUI.UIMaps[winID];

    UIMap.addElement("widgets list", UI_ITEMS_LIST, &c_widget);
    UIMap["widgets list"].label = "widgets list";
    c_widget.updateRowHeight(c_widget.layout_grid.size() - 1, 0.8f);

    UIMap.addElement("new widget name label", UI_STRING_LABEL, &c_widget, to_new_col);
    UIMap["new widget name label"].label = "new widget name";

    // create new widget button
    UIMap.addElement("add new widget", UI_BUTTON, &c_widget, to_new_subrow);
    UIMap["add new widget"].label = "add widget";

    // different flag settings
    UIMap.addElement("bordered", UI_BOOL, &c_widget, to_new_subrow);
    UIMap["bordered"].label = "bordered";
    UIMap["bordered"].height = 0.06f;
    UIMap.addElement("titled", UI_BOOL, &c_widget, to_new_subrow);
    UIMap["titled"].label = "titled";
    UIMap["titled"].height = 0.06f;
    UIMap.addElement("minimizable", UI_BOOL, &c_widget, to_new_subrow);
    UIMap["minimizable"].label = "minimizable";
    UIMap["minimizable"].height = 0.06f;
    UIMap.addElement("scalable", UI_BOOL, &c_widget, to_new_subrow);
    UIMap["scalable"].label = "scalable";
    UIMap["scalable"].height = 0.06f;
    UIMap.addElement("movable", UI_BOOL, &c_widget, to_new_subrow);
    UIMap["movable"].label = "movable";
    UIMap["movable"].height = 0.06f;

    // switch to edit widget's elements
    UIMap.addElement("edit widget elements", UI_BUTTON, &c_widget, to_new_subrow);
    UIMap["edit widget elements"].label = "edit widget elements";
    UIMap.addElement("skins and styling", UI_BUTTON, &c_widget, to_new_subrow);
    UIMap["skins and styling"].label = "skins and styling";

    UIMap.addElement("save ui", UI_BUTTON, &c_widget, to_new_subrow);
    UIMap["save ui"].label = "save ui";
    UIMap.addElement("load ui", UI_BUTTON, &c_widget, to_new_subrow);
    UIMap["load ui"].label = "load ui";

    UIMap.addElement("new widget name", UI_STRING_INPUT, &c_widget, to_new_col);
    UIMap.addElement("space fill", UI_EMPTY, &c_widget, to_new_subrow);


    // coordinates of new widget
    UIMap.addElement("location x", UI_FLOAT, &c_widget, to_new_subrow);
    UIMap["location x"].label = "loc x";
    UIMap["location x"]._data.f = 10.f;
    UIMap.addElement("location y", UI_FLOAT, &c_widget, to_new_subrow);
    UIMap["location y"].label = "loc y";
    UIMap["location y"]._data.f = 10.f;
    UIMap.addElement("size x", UI_FLOAT, &c_widget, to_new_subrow);
    UIMap["size x"].label = "size x";
    UIMap["size x"]._data.f = 20.f;
    UIMap.addElement("size y", UI_FLOAT, &c_widget, to_new_subrow);
    UIMap["size y"].label = "size y";
    UIMap["size y"]._data.f = 20.f;
    UIMap.addElement("widget border", UI_FLOAT, &c_widget, to_new_subrow);
    UIMap["widget border"].label = "border";
    UIMap["widget border"]._data.f = 1.f;

    // "AddElement" calls with "autowidth" modify width settings, so keep them after all
    // such calls
    // first column and it's elements
    c_widget.layout_grid[0].cells[0].min_width = 0.5f;
    UIMap["widgets list"].width = 0.5f;

    // second column and it's elements
    c_widget.layout_grid[0].cells[1].min_width = 0.2f;
    UIMap["new widget name label"].height = 0.08f;
    UIMap["new widget name label"].width = 0.2f;
    UIMap["add new widget"].width = 0.48f;
    UIMap["add new widget"].height = 0.07f;
    UIMap["edit widget elements"].width = 0.48f;
    UIMap["edit widget elements"].height = 0.07f;
    UIMap["skins and styling"].width = 0.48f;
    UIMap["skins and styling"].height = 0.07f;
    UIMap["save ui"].width = 0.48f;
    UIMap["save ui"].height = 0.07f;
    UIMap["load ui"].width = 0.48f;
    UIMap["load ui"].height = 0.07f;

    // third column and it's elements
    c_widget.layout_grid[0].cells[2].min_width = 0.2f;
    UIMap["new widget name"].width = 0.28f;
    UIMap["new widget name"].height = 0.08f;
    UIMap["space fill"].height = 0.07f;
    UIMap["space fill"].width = 0.28f;
    UIMap["location x"].width = 0.28f;
    UIMap["location x"].height = 0.06;
    UIMap["location y"].width = 0.28f;
    UIMap["location y"].height = 0.06;
    UIMap["size x"].width = 0.28f;
    UIMap["size x"].height = 0.06f;
    UIMap["size y"].width = 0.28f;
    UIMap["size y"].height = 0.06f;
    UIMap["widget border"].width = 0.28f;
    UIMap["widget border"].height = 0.06f;

    WIDGET elements_edit;
    elements_edit.screen_region.x = (float)(WIDTH / 4) / ((float)WIDTH);
    elements_edit.screen_region.y = ((float)HEIGHT / 4) / ((float)HEIGHT);
    elements_edit.screen_region.w = 0.3f;
    elements_edit.screen_region.h = 0.3f;
    elements_edit.name = "Edit elements";
    
    WIDGET & e_widget = GUI.addWidget(elements_edit, winID);
    e_widget.hidden = true;

    UIMap.addElement("elements list", UI_ITEMS_LIST, &e_widget);
    UIMap["elements list"].label = "elements list";    

    UIMap.addElement("rows list", UI_ITEMS_LIST, &e_widget, to_new_col);
    UIMap["rows list"].label = "rows";

    UIMap.addElement("cols list", UI_ITEMS_LIST, &e_widget, to_new_col);
    UIMap["cols list"].label = "cols";

    UIMap.addElement("new element name label", UI_STRING_LABEL, &e_widget, to_new_col);
    UIMap["new element name label"].label = "new element name:";
    
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

    UIMap.addElement("new element name", UI_STRING_INPUT, &e_widget, to_new_col);
    UIMap["new element name"].label = "new element name";

    UIMap.addElement("add image", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add image"].label = "add image";

    UIMap.addElement("add label", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add label"].label = "add label";

    UIMap.addElement("add text", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add text"].label = "add text";
    
    UIMap.addElement("add empty", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add empty"].label = "add empty";

    UIMap.addElement("add float", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add float"].label = "add float";

    e_widget.updateRowHeight(e_widget.layout_grid.size() - 1, 0.5f);

    e_widget.updateColWidth(e_widget.layout_grid.size() - 1, 1, 0.1f);
    e_widget.updateColWidth(e_widget.layout_grid.size() - 1, 2, 0.1f);
    e_widget.updateColWidth(e_widget.layout_grid.size() - 1, 3, 0.29f);
    e_widget.updateColWidth(e_widget.layout_grid.size() - 1, 4, 0.29f);

    // width and height of specific element
    UIMap.addElement("element width", UI_FLOAT, &e_widget);
    UIMap["element width"].label = "width";
    UIMap.addElement("element height", UI_FLOAT, &e_widget, to_new_subrow);
    UIMap["element height"].label = "height";
    UIMap.addElement("element pad x", UI_FLOAT, &e_widget, to_new_subrow);
    UIMap["element pad x"].label = "pad x";
    UIMap.addElement("element pad y", UI_FLOAT, &e_widget, to_new_subrow);
    UIMap["element pad y"].label = "pad y";
    UIMap.addElement("element border", UI_FLOAT, &e_widget, to_new_subrow);
    UIMap["element border"].label = "border";
    UIMap.addElement("element rounding", UI_FLOAT, &e_widget, to_new_subrow);
    UIMap["element rounding"].label = "rounding";

    UIMap.addElement("row height", UI_FLOAT, &e_widget, to_new_col);
    UIMap["row height"].label = "row height";

    UIMap.addElement("col width", UI_FLOAT, &e_widget, to_new_subrow);
    UIMap["col width"].label = "col width";
    
    UIMap.addElement("delete element", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["delete element"].label = "delete elt";

    UIMap.addElement("keep space", UI_BOOL, &e_widget, to_new_subrow);
    UIMap["keep space"].label = "keep space";

    UIMap.addElement("switch type", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["switch type"].label = "switch type";

    UIMap.addElement("push opt", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["push opt"].label = "push: to new row";

    UIMap.addElement("elt label label", UI_STRING_LABEL, &e_widget, to_new_col);
    UIMap["elt label label"].label = "label:";

    UIMap.addElement("selected widget", UI_STRING_TEXT, &e_widget, to_new_subrow);
    UIMap["selected widget"].label = "selected widget:";

    UIMap.addElement("back to widgets", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["back to widgets"].label = "back to widgets";

    UIMap.addElement("elt label", UI_STRING_INPUT, &e_widget, to_new_col);

    UIMap.addElement("skins", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["skins"].label = "skins";

    e_widget.layout_grid.back().cells[2].min_width = 0.22f;
    UIMap["elt label"].width = 0.2f;
    UIMap["elt label"].height = 0.09f;

    UIMap["skins"].width = 0.2f;
    UIMap["skins"].height = 0.09f;

    e_widget.layout_grid.back().cells[0].min_width = 0.28f;
    UIMap["elements list"].height = 0.5f;
    UIMap["element width"].height = 0.06f;
    UIMap["element width"].width = 0.28f;
    UIMap["element height"].height = 0.06f;
    UIMap["element height"].width = 0.28f;
    UIMap["element pad x"].height = 0.06f;
    UIMap["element pad x"].width = 0.28f;
    UIMap["element pad y"].height = 0.06f;
    UIMap["element pad y"].width = 0.28f;
    UIMap["element border"].height = 0.06f;
    UIMap["element border"].width = 0.28f;
    UIMap["element rounding"].height = 0.06f;
    UIMap["element rounding"].width = 0.28f;

    e_widget.layout_grid.back().cells[1].min_width = 0.28f;
    UIMap["row height"].width = 0.28f;
    UIMap["row height"].height = 0.06f;
    UIMap["col width"].width = 0.28f;
    UIMap["col width"].height = 0.06f;
    UIMap["delete element"].width = 0.28f;
    UIMap["delete element"].height = 0.06f;
    UIMap["keep space"].width = 0.28f;
    UIMap["keep space"].height = 0.06f;
    UIMap["switch type"].width = 0.28f;
    UIMap["switch type"].height = 0.06f;
    UIMap["push opt"].width = 0.28f;
    UIMap["push opt"].height = 0.06f;

    WIDGET widgets_style;
    widgets_style.screen_region.x = (float)(WIDTH / 4) / ((float)WIDTH);
    widgets_style.screen_region.y = ((float)HEIGHT / 4) / ((float)HEIGHT);
    widgets_style.screen_region.w = 0.3f;
    widgets_style.screen_region.h = 0.3f;
    widgets_style.name = "Widgets style";

    WIDGET & ws_widget = GUI.addWidget(widgets_style, winID);
    ws_widget.hidden = true;

    // skinning:
    // load skin image, skinning elements list, crop settings, apply skin
    UIMap.addElement("w load skin image", UI_BUTTON, &ws_widget);
    UIMap["w load skin image"].label = "load skin image";

    UIMap.addElement("w skin image path", UI_STRING_TEXT, &ws_widget, to_new_subrow);
    UIMap["w skin image path"].label = "skin image path: None";

    UIMap.addElement("w crop x", UI_FLOAT, &ws_widget, to_new_subrow);
    UIMap["w crop x"].label = "crop x";
    UIMap.addElement("w crop y", UI_FLOAT, &ws_widget, to_new_subrow);
    UIMap["w crop y"].label = "crop y";
    UIMap.addElement("w crop w", UI_FLOAT, &ws_widget, to_new_subrow);
    UIMap["w crop w"].label = "crop w";
    UIMap.addElement("w crop h", UI_FLOAT, &ws_widget, to_new_subrow);
    UIMap["w crop h"].label = "crop h";

    UIMap.addElement("w apply skin", UI_BUTTON, &ws_widget, to_new_subrow);
    UIMap["w apply skin"].label = "apply skin";

    UIMap.addElement("w skinning properties", UI_ITEMS_LIST, &ws_widget, to_new_col);
    UIMap["w skinning properties"].label = "skinning properties";

    ui_string_group & w_skinning_props_list = *UIMap["w skinning properties"]._data.usgPtr;
    for (int i = background; i != hover_active; i++) {
        w_skinning_props_list.elements.push_back(
            getSkinPropName((IMAGE_SKIN_ELEMENT)i)
        );
    }
    ws_widget.updateRowHeight(0, 0.4f);
    
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

    UIMap.addElement("red", UI_UINT, &ws_widget, to_new_col);
    UIMap["red"].label = "red";
    UIMap.addElement("green", UI_UINT, &ws_widget, to_new_subrow);
    UIMap["green"].label = "green";
    UIMap.addElement("blue", UI_UINT, &ws_widget, to_new_subrow);
    UIMap["blue"].label = "blue";
    UIMap.addElement("alpha", UI_UINT, &ws_widget, to_new_subrow);
    UIMap["alpha"].label = "alpha";
    
    UIMap.addElement("to widgets from style", UI_BUTTON, &ws_widget, to_new_subrow);
    UIMap["to widgets from style"].label = "back to widgets";

    UIMap.addElement("style selected widget", UI_STRING_LABEL, &ws_widget, to_new_subrow);
    UIMap["style selected widget"].label = "selected widget: None";

    ws_widget.updateRowHeight(1, 0.4f);

    UIMap["red"].height = 0.05f;
    UIMap["green"].height = 0.05f;
    UIMap["blue"].height = 0.05f;
    UIMap["alpha"].height = 0.05f;
    UIMap["to widgets from style"].height = 0.05f;
    
    WIDGET elements_style;
    elements_style.screen_region.x = (float)(WIDTH / 4) / ((float)WIDTH);
    elements_style.screen_region.y = ((float)HEIGHT / 4) / ((float)HEIGHT);
    elements_style.screen_region.w = 0.3f;
    elements_style.screen_region.h = 0.3f;
    elements_style.name = "Elements style";

    WIDGET & es_widget = GUI.addWidget(elements_style, winID);
    es_widget.hidden = true;

    // skinning:
    // load skin image, skinning elements list, crop settings, apply skin
    UIMap.addElement("e load skin image", UI_BUTTON, &es_widget);
    UIMap["e load skin image"].label = "load skin image";

    UIMap.addElement("e skin image path", UI_STRING_TEXT, &es_widget, to_new_subrow);
    UIMap["e skin image path"].label = "skin image path: None";

    UIMap.addElement("e crop x", UI_FLOAT, &es_widget, to_new_subrow);
    UIMap["e crop x"].label = "crop x";
    UIMap.addElement("e crop y", UI_FLOAT, &es_widget, to_new_subrow);
    UIMap["e crop y"].label = "crop y";
    UIMap.addElement("e crop w", UI_FLOAT, &es_widget, to_new_subrow);
    UIMap["e crop w"].label = "crop w";
    UIMap.addElement("e crop h", UI_FLOAT, &es_widget, to_new_subrow);
    UIMap["e crop h"].label = "crop h";

    UIMap.addElement("e apply skin", UI_BUTTON, &es_widget, to_new_subrow);
    UIMap["e apply skin"].label = "apply skin";
    
    UIMap.addElement("to elements from style", UI_BUTTON, &es_widget, to_new_subrow);
    UIMap["to elements from style"].label = "back to elements";

    UIMap.addElement("e selected element", UI_STRING_LABEL, &es_widget, to_new_subrow);
    UIMap["e selected element"].label = "selected element: None";

    UIMap.addElement("e skinning properties", UI_ITEMS_LIST, &es_widget, to_new_col);
    UIMap["e skinning properties"].label = "skinning properties";
    ui_string_group & e_skinning_props_list = *UIMap["e skinning properties"]._data.usgPtr;
    for (int i = background; i != hover_active; i++) {
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
    fonts.screen_region.x = 0.7f;
    fonts.screen_region.y = 0.f;
    fonts.screen_region.w = 0.2f;
    fonts.screen_region.h = 0.2f;
    fonts.name = "Fonts";

    WIDGET & fonts_widget = GUI.addWidget(fonts, winID);
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

    UIMap.addElement("loaded fonts", UI_ITEMS_LIST, &fonts_widget, to_new_col);
    UIMap["loaded fonts"].label = "loaded fonts";

    UIMap.addElement("font sizes", UI_ITEMS_LIST, &fonts_widget, to_new_col);
    UIMap["font sizes"].label = "sizes";
    fonts_widget.updateRowHeight(0, 0.8f);
    fonts_widget.updateColWidth(0, 0, 0.37f);
    fonts_widget.updateColWidth(0, 1, 0.45f);
    fonts_widget.updateColWidth(0, 2, 0.15f);

    UIMap["load font"].height = 0.1f;
    UIMap["load size"].height = 0.1f;
    UIMap["apply font"].height = 0.1f;
  
    std::string home_dir = fs::current_path().string();
    if (home_dir.find("ui_editor") == std::string::npos) {
        std::cout << "[WARNING] cwd does not contain 'ui_editor'! Can't find default font" << std::endl;
        std::cout << "[WARNING] First loaded font will be used as default." << std::endl;
        return;
    }

    std::string font_path = home_dir.substr(
        0, home_dir.find("ui_editor")
    ) + "ui_editor/fonts/Roboto-Regular.ttf";
    GUI.loadFont(font_path, winID);
}