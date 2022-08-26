// various functions for working with ui
#include <IndieGoUI.h>

using namespace IndieGo::UI;

extern Manager GUI;

void createNewWidget(
    std::string newWidName, 
    region_size<float> size, 
    region_size<float> location,
    bool bordered,
    bool titled,
    bool minimizable,
    bool scalable,
    bool movable,
    std::string & winID
) {
    WIDGET newWidget;

    // initialize new widget
    newWidget.screen_region.x = location.w;
    newWidget.screen_region.y = location.h;
    newWidget.screen_region.w = size.w;
    newWidget.screen_region.h = size.h;
    newWidget.name = newWidName;

    newWidget.border = bordered;
    newWidget.title = titled;
    newWidget.minimizable = minimizable;
    newWidget.scalable = scalable;
    newWidget.movable = movable;

    WIDGET & widget = GUI.addWidget(newWidget, winID);
}

void useBackgroundImage(std::string widID, std::string winID, unsigned int texID) {
   /* WIDGET & w = GUI.getWidget(widID, winID);
    w.img_idx = GUI.addImage(texID);
    w.backgroundImage = true;*/
}

void addElement(
    std::string widID, 
    std::string winID, 
    std::string elt_name, 
    UI_ELEMENT_TYPE type,
    bool add_on_new_row
) {
    if (elt_name.size() == 0)
        return;

    // get widget
    WIDGET& w = GUI.getWidget( widID, winID );

    // check if such element already exists, in which case don't add it
    if (std::find(w.widget_elements.begin(), w.widget_elements.end(), elt_name) != w.widget_elements.end())
        // TODO : add error message
        return;

    // figure row to add element on
    unsigned int row = w.layout_grid.size() - 1;
    if (add_on_new_row)
        row++;
    
    // get UIMap
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    // add element to widget
    UIMap.addElement(elt_name, type, &w, row);
    // TODO : process special cases
}