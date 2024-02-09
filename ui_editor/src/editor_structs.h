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

#include <string>
#include <map>

struct LayoutRect {
    // position and size
    float x, y, width, height;

    // color
    float red = 1.f, 
        green = 1.f, 
        blue = 1.f, 
        alpha = 1.f;
};

struct EditorState {

    int styling_element = -1;
    std::string selectedWidget = "None";
    std::string selectedElement = "None";

    int selectedSkinningElement = -1;
    int selectedSkinningWidget = -1;

    // if widget is selected and user is not on "elements" screen, font will be applied to widget
    // otherwise - to selected element, if any
    bool updateWidgetFont = false;
    std::string selectedFont = "None";
    int fontSize = -1;
    std::string winID = "None";

    std::string selectedSkinCrop = "None";

    // [widID] = current_line
    std::map<std::string, unsigned int> widgets_fill;
};