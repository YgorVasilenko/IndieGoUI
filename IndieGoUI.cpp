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