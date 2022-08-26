#include <IndieGoUI.h>

using namespace IndieGo::UI;

extern Manager GUI;
extern WIDGET creator_widget;
extern WIDGET styling_widget;
extern std::string winID;

extern std::string getColorPropName(COLOR_ELEMENTS prop);

void initWidgets() {
    // initialize creator widget
    creator_widget.screen_region.x = (float)(WIDTH / 4) / ((float)WIDTH);
    creator_widget.screen_region.y = ((float)HEIGHT / 4) / ((float)HEIGHT);
    creator_widget.screen_region.w = ((float)WIDTH / 2) / ((float)WIDTH);
    creator_widget.screen_region.h = 0.66f;
    creator_widget.name = "UI creator";

    WIDGET & c_widget = GUI.addWidget(creator_widget, winID);

    // control elements to create ui
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    unsigned int current_line = 0;

    UIMap.addElement("new widget name label", UI_STRING_LABEL, &c_widget, current_line);
    UIMap["new widget name label"].label = "widget name";

    UIMap.addElement("new widget name", UI_STRING_INPUT, &c_widget, current_line++);
    // different flag settings
    UIMap.addElement("bordered", UI_BOOL, &c_widget, current_line);
    UIMap["bordered"].label = "bordered";
    UIMap.addElement("titled", UI_BOOL, &c_widget, current_line);
    UIMap["titled"].label = "titled";
    UIMap.addElement("minimizable", UI_BOOL, &c_widget, current_line);
    UIMap["minimizable"].label = "minimizable";
    UIMap.addElement("scalable", UI_BOOL, &c_widget, current_line);
    UIMap["scalable"].label = "scalable";
    UIMap.addElement("movable", UI_BOOL, &c_widget, current_line++);
    UIMap["movable"].label = "movable";


    // coordinates of new widget
    UIMap.addElement("location x", UI_FLOAT, &c_widget, current_line);
    UIMap["location x"].label = "loc x";
    UIMap["location x"]._data.f = 10.f;
    UIMap.addElement("location y", UI_FLOAT, &c_widget, current_line);
    UIMap["location y"].label = "loc y";
    UIMap["location y"]._data.f = 10.f;
    UIMap.addElement("size x", UI_FLOAT, &c_widget, current_line);
    UIMap["size x"].label = "size x";
    UIMap["size x"]._data.f = 20.f;
    UIMap.addElement("size y", UI_FLOAT, &c_widget, current_line);
    UIMap["size y"].label = "size y";
    UIMap["size y"]._data.f = 20.f;

    // create new widget button
    UIMap.addElement("add new widget", UI_BUTTON, &c_widget, current_line++);
    UIMap["add new widget"].label = "add widget";

    // list of all available widgets
    UIMap.addElement("widgets list", UI_ITEMS_LIST, &c_widget, current_line);
    UIMap["widgets list"].label = "widgets on screen";

    UIMap.addElement("elements list", UI_ITEMS_LIST, &c_widget, current_line++);
    UIMap["elements list"].label = "elements in selected widget";
    c_widget.layout_grid[current_line - 1].min_height = 150;

    UIMap.addElement("add image", UI_BUTTON, &c_widget, current_line);
    UIMap["add image"].label = "add image";

    UIMap.addElement("add text", UI_BUTTON, &c_widget, current_line);
    UIMap["add text"].label = "add text";

    UIMap.addElement("add button", UI_BUTTON, &c_widget, current_line);
    UIMap["add button"].label = "add button";

    UIMap.addElement("add slider", UI_BUTTON, &c_widget, current_line);
    UIMap["add slider"].label = "add slider";

    UIMap.addElement("add items list", UI_BUTTON, &c_widget, current_line);
    UIMap["add items list"].label = "add items list";

    UIMap.addElement("to same row", UI_BOOL, &c_widget, current_line++);
    UIMap["to same row"].label = "to same row";
    UIMap["to same row"]._data.b = true;

    UIMap.addElement("new element name label", UI_STRING_LABEL, &c_widget, current_line);
    UIMap["new element name label"].label = "new element name:";

    UIMap.addElement("new element name", UI_STRING_INPUT, &c_widget, current_line++);
    UIMap["new element name"].label = "new element name";

    // text editor field
    UIMap.addElement("selected text label", UI_STRING_LABEL, &c_widget, current_line);
    UIMap["selected text label"].label = "edit selected text:";

    UIMap.addElement("selected text", UI_STRING_INPUT, &c_widget, current_line++);

    // Styling widget
    // initialize styling widget
    styling_widget.screen_region.x = 0.82f;
    styling_widget.screen_region.y = 0.05f;
    styling_widget.screen_region.w = 0.18f;
    styling_widget.screen_region.h = 0.83f;
    styling_widget.name = "style editor";
    current_line = 0;

    WIDGET & s_widget = GUI.addWidget(styling_widget, winID);

    UIMap.addElement("style edit mode", UI_BUTTON, &s_widget, current_line++);
    UIMap["style edit mode"].label = "edit mode: widget";

    UIMap.addElement("style elements list", UI_ITEMS_LIST, &s_widget, current_line++);
    UIMap["style elements list"].label = "edit element styling";
    UIMap["style elements list"].text_align = LEFT;
    
    s_widget.layout_grid[current_line - 1].min_height = 695;
    ui_string_group & style_elements_list = *UIMap["style elements list"]._data.usgPtr;

    for (int i = UI_COLOR_TEXT; i != UI_COLOR_TAB_HEADER; i++) {
        style_elements_list.elements.push_back(
            getColorPropName((COLOR_ELEMENTS)i)
        );
    }
    
    UIMap.addElement("Red property color", UI_UINT, &s_widget, current_line++);
    UIMap["Red property color"].label = "Red";
    UIMap.addElement("Green property color", UI_UINT, &s_widget, current_line++);
    UIMap["Green property color"].label = "Green";
    UIMap.addElement("Blue property color", UI_UINT, &s_widget, current_line++);
    UIMap["Blue property color"].label = "Blue";
    UIMap.addElement("Alpha property color", UI_UINT, &s_widget, current_line++);
    UIMap["Alpha property color"].label = "Alpha";
}