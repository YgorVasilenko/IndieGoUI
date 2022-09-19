#include <IndieGoUI.h>

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
    // unsigned int current_line = 0;

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
    // UIMap["edit widget elements"].hidden = true;
    UIMap.addElement("fonts skins and styling", UI_BUTTON, &c_widget, to_new_subrow);
    UIMap["fonts skins and styling"].label = "fonts, skins and styling";

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
    UIMap["fonts skins and styling"].width = 0.48f;
    UIMap["fonts skins and styling"].height = 0.07f;

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
    UIMap["rows list"].label = "rows list";

    UIMap.addElement("cols list", UI_ITEMS_LIST, &e_widget, to_new_col);
    UIMap["cols list"].label = "cols list";

    UIMap.addElement("subrows list", UI_ITEMS_LIST, &e_widget, to_new_col);
    UIMap["subrows list"].label = "subrows list";

    UIMap.addElement("new element name label", UI_STRING_LABEL, &e_widget, to_new_col);
    UIMap["new element name label"].label = "new element name label";

    UIMap.addElement("new element name", UI_STRING_INPUT, &e_widget, to_new_subrow);
    UIMap["new element name"].label = "new element name";

    UIMap.addElement("add button", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add button"].label = "add button";

    UIMap.addElement("add progress", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add progress"].label = "add progress";

    UIMap.addElement("add checkbox", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add checkbox"].label = "add checkbox";

    UIMap.addElement("add image", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add image"].label = "add image";

    UIMap.addElement("add label", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add label"].label = "add label";

    UIMap.addElement("add text", UI_BUTTON, &e_widget, to_new_subrow);
    UIMap["add text"].label = "add text";
    e_widget.updateRowHeight(e_widget.layout_grid.size() - 1, 0.5f);

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
       
    UIMap.addElement("selected widget", UI_STRING_LABEL, &e_widget, to_new_col);
    UIMap["selected widget"].label = "selected widget:";

    UIMap.addElement("back to widgets", UI_BUTTON, &e_widget, to_new_subrow);
    e_widget.layout_grid.back().cells[2].min_width = 0.44f;
    UIMap["back to widgets"].label = "back to widgets";
    UIMap["back to widgets"].height = 0.3f;
    UIMap["back to widgets"].width = 0.44f;
    UIMap["back to widgets"].padding.h = 30.f;
    UIMap["back to widgets"].padding.w = 30.f;

    // TODO : make "updateCellWidth()" method
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

    // UIMap["rows list"].height = 0.5f;
    // UIMap["cols list"].height = 0.5f;
    // UIMap["subrows list"].height = 0.5f;
    UIMap["new element name"].height = 0.09f;

    // UIMap.addElement("save widgets", UI_BUTTON, &c_widget, current_line);
    // UIMap["save widgets"].label = "save widgets";
    // UIMap.addElement("load widgets", UI_BUTTON, &c_widget, current_line++);
    // UIMap["load widgets"].label = "load widgets";

    // // Row with lists
    // // ---------------------------------------------------------------------
    // UIMap.addElement("skinning property", UI_ITEMS_LIST, &c_widget, current_line++);
    // UIMap["skinning property"].label = "skinning property";
    // c_widget.layout_grid[current_line - 1].min_height = 150.f;
    // // ---------------------------------------------------------------------

    // UIMap.addElement("hide widget", UI_BOOL, &c_widget, current_line);
    // UIMap["hide widget"].label = "hide widget";

    // UIMap.addElement("new element name label", UI_STRING_LABEL, &c_widget, current_line);
    // UIMap["new element name label"].label = "new element name:";

    // UIMap.addElement("new element name", UI_STRING_INPUT, &c_widget, current_line);
    // UIMap["new element name"].label = "new element name";

    // UIMap.addElement("row height", UI_FLOAT, &c_widget, current_line++);
    // UIMap["row height"].label = "row height";

    // UIMap.addElement("add image", UI_BUTTON, &c_widget, current_line);
    // UIMap["add image"].label = "add image";

    // UIMap.addElement("add text", UI_BUTTON, &c_widget, current_line);
    // UIMap["add text"].label = "add text";

    // UIMap.addElement("add button", UI_BUTTON, &c_widget, current_line);
    // UIMap["add button"].label = "add button";

    // UIMap.addElement("add slider", UI_BUTTON, &c_widget, current_line);
    // UIMap["add slider"].label = "add slider";

    // UIMap.addElement("add items list", UI_BUTTON, &c_widget, current_line);
    // UIMap["add items list"].label = "add items list";

    // UIMap.addElement("to same row", UI_BOOL, &c_widget, current_line++);
    // UIMap["to same row"].label = "to same row";
    // UIMap["to same row"]._data.b = true;

    // // text editor field
    // UIMap.addElement("selected text label", UI_STRING_LABEL, &c_widget, current_line);
    // UIMap["selected text label"].label = "edit selected text:";

    // UIMap.addElement("selected text", UI_STRING_INPUT, &c_widget, current_line++);

    // // skinning fields:
    // UIMap.addElement("loaded skin image label", UI_STRING_LABEL, &c_widget, current_line);
    // UIMap["loaded skin image label"].label = "loaded skin image:";

    // UIMap.addElement("loaded skin image", UI_STRING_LABEL, &c_widget, current_line);
    // UIMap["loaded skin image"].label = "No image";

    // UIMap.addElement("load skin image", UI_BUTTON, &c_widget, current_line);
    // UIMap["load skin image"].label = "load skin image";

    // UIMap.addElement("add skin", UI_BUTTON, &c_widget, current_line++);
    // UIMap["add skin"].label = "add skin";

    // UIMap.addElement("img x", UI_FLOAT, &c_widget, current_line);
    // UIMap["img x"].label = "img x";
    // UIMap["img x"]._data.f = 0.f;
    // UIMap.addElement("img y", UI_FLOAT, &c_widget, current_line);
    // UIMap["img y"].label = "img y";
    // UIMap["img y"]._data.f = 0.f;
    // UIMap.addElement("img size x", UI_FLOAT, &c_widget, current_line);
    // UIMap["img size x"].label = "img size x";
    // UIMap["img size x"]._data.f = 100.f;
    // UIMap.addElement("img size y", UI_FLOAT, &c_widget, current_line++);
    // UIMap["img size y"].label = "img size y";
    // UIMap["img size y"]._data.f = 100.f;
    
    // // border and rounding
    // UIMap.addElement("border size", UI_FLOAT, &c_widget, current_line);
    // UIMap["border size"].label = "border size";
    // UIMap["border size"]._data.f = 1.f; // TODO : check it's non-zero

    // UIMap.addElement("rounding", UI_FLOAT, &c_widget, current_line);
    // UIMap["rounding"].label = "rounding";
    // UIMap["rounding"]._data.f = 1.f; // TODO : check it's non-zero

    // // padding 
    // UIMap.addElement("padding x", UI_FLOAT, &c_widget, current_line);
    // UIMap["padding x"].label = "padding x";
    // UIMap["padding x"]._data.f = 1.f; // TODO : check it's non-zero

    // UIMap.addElement("padding y", UI_FLOAT, &c_widget, current_line++);
    // UIMap["padding y"].label = "padding y";
    // UIMap["padding y"]._data.f = 1.f; // TODO : check it's non-zero

    // // widget's spacing
    // UIMap.addElement("spacing x", UI_FLOAT, &c_widget, current_line);
    // UIMap["spacing x"].label = "spacing x";
    // UIMap["spacing x"]._data.f = 1.f; // TODO : check it's non-zero

    // UIMap.addElement("spacing y", UI_FLOAT, &c_widget, current_line++);
    // UIMap["spacing y"].label = "spacing y";
    // UIMap["spacing y"]._data.f = 1.f; // TODO : check it's non-zero

    // // font loading elements
    // // "load font" button
    // UIMap.addElement("load font", UI_BUTTON, &c_widget, current_line);
    // UIMap["load font"].label = "load font";

    // // use selected font for currently selected element/widget
    // UIMap.addElement("use font", UI_BUTTON, &c_widget, current_line);
    // UIMap["use font"].label = "use font";

    // // font, currently used by element
    // UIMap.addElement("current font", UI_STRING_LABEL, &c_widget, current_line);
    // UIMap["current font"].label = "current font";

    // // "native" size of font (one, that it was loaded with)
    // UIMap.addElement("load font size", UI_FLOAT, &c_widget, current_line);
    // UIMap["load font size"].label = "load size";
    // UIMap["load font size"]._data.f = 16.f;

    // // displayed size of font, setted programmaticaly. Loses quality on bigger values
    // UIMap.addElement("programmed font size", UI_FLOAT, &c_widget, current_line++);
    // UIMap["programmed font size"].label = "programmed size";

    // // list of all available fonts
    // UIMap.addElement("available fonts", UI_ITEMS_LIST, &c_widget, current_line);
    // UIMap["available fonts"].label = "available fonts";

    // UIMap.addElement("font sizes", UI_ITEMS_LIST, &c_widget, current_line++);
    // UIMap["font sizes"].label = "font sizes";
    // c_widget.layout_grid[current_line - 1].min_height = 150.f;

    // ui_string_group & skinning_props_list = *UIMap["skinning property"]._data.usgPtr;
    // for (int i = background; i != hover_active; i++) {
    //     skinning_props_list.elements.push_back(
    //         getSkinPropName((IMAGE_SKIN_ELEMENT)i)
    //     );
    // }

    // Styling widget
    // initialize styling widget
    // styling_widget.screen_region.x = 0.82f;
    // styling_widget.screen_region.y = 0.05f;
    // styling_widget.screen_region.w = 0.18f;
    // styling_widget.screen_region.h = 0.83f;
    // styling_widget.name = "style editor";
    // current_line = 0;

    // WIDGET & s_widget = GUI.addWidget(styling_widget, winID);

    // UIMap.addElement("style edit mode", UI_BUTTON, &s_widget, current_line++);
    // UIMap["style edit mode"].label = "edit mode: widget";

    // UIMap.addElement("style elements list", UI_ITEMS_LIST, &s_widget, current_line++);
    // UIMap["style elements list"].label = "edit element styling";
    // UIMap["style elements list"].text_align = LEFT;
    
    // s_widget.layout_grid[current_line - 1].min_height = 693.f;
    // ui_string_group & style_elements_list = *UIMap["style elements list"]._data.usgPtr;

    // for (int i = UI_COLOR_TEXT; i != UI_COLOR_TAB_HEADER; i++) {
    //     style_elements_list.elements.push_back(
    //         getColorPropName((COLOR_ELEMENTS)i)
    //     );
    // }
    
    // UIMap.addElement("Red property color", UI_UINT, &s_widget, current_line++);
    // UIMap["Red property color"].label = "Red";
    // UIMap.addElement("Green property color", UI_UINT, &s_widget, current_line++);
    // UIMap["Green property color"].label = "Green";
    // UIMap.addElement("Blue property color", UI_UINT, &s_widget, current_line++);
    // UIMap["Blue property color"].label = "Blue";
    // UIMap.addElement("Alpha property color", UI_UINT, &s_widget, current_line++);
    // UIMap["Alpha property color"].label = "Alpha";
}