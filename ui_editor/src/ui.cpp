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
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <list>
#include <filesystem>
namespace fs = std::filesystem;

using namespace IndieGo::UI;

extern Manager GUI;

void updateUIFromLayout(
    std::string widID, 
    std::string winID,
    bool upd_row = false,
    bool upd_col = false
) {
    WIDGET & w = GUI.getWidget(widID, winID);
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    ui_string_group & rows_list = *UIMap["rows list"]._data.usgPtr;
    ui_string_group & cols_list = *UIMap["cols list"]._data.usgPtr;

    if (upd_row) {
        UIMap["row height"]._data.f = w.layout_grid[ rows_list.selected_element ].min_height * UI_FLT_VAL_SCALE;

        if (upd_col) {
            UIMap["col width"]._data.f = w.layout_grid[ rows_list.selected_element ].cells[ cols_list.selected_element ].min_width * UI_FLT_VAL_SCALE;
        }
    }
}

void updateLayoutFromUI(
    std::string widID, 
    std::string winID,
    bool upd_row = false,
    bool upd_col = false
) {
    WIDGET & w = GUI.getWidget(widID, winID);
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    ui_string_group & rows_list = *UIMap["rows list"]._data.usgPtr;
    ui_string_group & cols_list = *UIMap["cols list"]._data.usgPtr;

    if (upd_row) {
        w.updateRowHeight(rows_list.selected_element, UIMap["row height"]._data.f / UI_FLT_VAL_SCALE);

        if (upd_col) {
            w.updateColWidth(rows_list.selected_element, cols_list.selected_element, UIMap["col width"]._data.f / UI_FLT_VAL_SCALE);
        }
    }
}

void updateElementFromUI(
    std::string elementName, 
    std::string winID,
    std::string widID // required for element's renaming
) {
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    UI_element & e = UIMap[elementName];

    e.border = UIMap["element border"]._data.f / UI_FLT_VAL_SCALE;
    e.padding.w = UIMap["element pad x"]._data.f / UI_FLT_VAL_SCALE;
    e.padding.h = UIMap["element pad y"]._data.f / UI_FLT_VAL_SCALE;
    e.rounding = UIMap["element rounding"]._data.f / UI_FLT_VAL_SCALE;
    e.width = UIMap["element width"]._data.f / UI_FLT_VAL_SCALE;
    e.height = UIMap["element height"]._data.f / UI_FLT_VAL_SCALE;

    e.label = *UIMap["elt label"]._data.strPtr;

    if (UIMap["rename element"]._data.b) {
        std::string new_name = *UIMap["new element name"]._data.strPtr;
        if (new_name.size() > 0 && UIMap.elements.find(new_name) == UIMap.elements.end()) {

            UIMap.renameElement(
                elementName,
                new_name,
                & GUI.getWidget(widID, winID)
            );
        }
    }
}

void updateUIFromElement(
    std::string elementName, 
    std::string winID
) {
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    UI_element & e = UIMap[elementName];

    UIMap["element border"]._data.f = e.border * UI_FLT_VAL_SCALE;
    UIMap["element pad x"]._data.f = e.padding.w * UI_FLT_VAL_SCALE;
    UIMap["element pad y"]._data.f = e.padding.h * UI_FLT_VAL_SCALE;
    UIMap["element rounding"]._data.f = e.rounding * UI_FLT_VAL_SCALE;
    UIMap["element width"]._data.f = e.width * UI_FLT_VAL_SCALE;
    UIMap["element height"]._data.f = e.height * UI_FLT_VAL_SCALE;

    *UIMap["elt label"]._data.strPtr = e.label;
}

void updateWidgetFromUI(
    std::string widID, 
    std::string winID, 
    bool do_styling,
    int styling_element
) {
    WIDGET & w = GUI.getWidget(widID, winID);
    UI_elements_map & UIMap = GUI.UIMaps[winID];

    // location
    w.screen_region.x = UIMap["location x"]._data.f / 100.f;
    w.screen_region.y = UIMap["location y"]._data.f / 100.f;

    // sizes
    w.screen_region.w = UIMap["size x"]._data.f / 100.f;
    w.screen_region.h = UIMap["size y"]._data.f / 100.f;

    // additional properties
    w.border = UIMap["bordered"]._data.b;
    w.title = UIMap["titled"]._data.b;
    w.minimizable = UIMap["minimizable"]._data.b;
    w.scalable = UIMap["scalable"]._data.b;
    w.movable = UIMap["movable"]._data.b;
    w.has_scrollbar = UIMap["has scrollbar"]._data.b;

    w.border_size = UIMap["widget border"]._data.f;

    // show/hide on screen
    w.hidden = !UIMap["visible"]._data.b;

    if (styling_element != -1) {
        w.style.elements[styling_element].r = UIMap["red"]._data.ui;
        w.style.elements[styling_element].g = UIMap["green"]._data.ui;
        w.style.elements[styling_element].b = UIMap["blue"]._data.ui;
        w.style.elements[styling_element].a = UIMap["alpha"]._data.ui;
    }

    if (UIMap["delete widget"]._data.b) {
        GUI.deleteWidget(widID, winID);
        ui_string_group& widgets_list = *UIMap["widgets list"]._data.usgPtr;
        widgets_list.elements.erase(
            std::find(
                widgets_list.elements.begin(), 
                widgets_list.elements.end(), 
                widID
            )
        );
    }

    if (UIMap["rename widget"]._data.b) {
        ui_string_group& widgets_list = *UIMap["widgets list"]._data.usgPtr;
        std::string new_name = *UIMap["new widget name"]._data.strPtr;
        if (new_name.size() > 0 && std::count(widgets_list.elements.begin(), widgets_list.elements.end(), new_name) <= 1) {
            // can't use new name if there aleady is such widget
            WIDGET & w = GUI.getWidget(widID, winID);
            auto element = std::find(widgets_list.elements.begin(), widgets_list.elements.end(), w.name);
            
            *element = new_name;
            GUI.widgets[winID][new_name] = w;
            GUI.widgets[winID][new_name].name = new_name;
            GUI.widgets[winID].erase(widID);
        }
    }
}

void switchUIscreens(std::string winID) {
    WIDGET & widgets = GUI.getWidget("UI creator", winID);
    WIDGET & widgets_style = GUI.getWidget("Widgets style", winID);

    WIDGET & elements = GUI.getWidget("Edit elements", winID);
    WIDGET & elements_style = GUI.getWidget("Elements style", winID);
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    ui_string_group & widgets_list = *UIMap["widgets list"]._data.usgPtr;
    ui_string_group & elements_list = *UIMap["elements list"]._data.usgPtr;

    if (!widgets.hidden) { // if "main widgets" visible
        if (widgets_list.selected_element != -1) {
            if(UIMap["edit widget elements"]._data.b) {
                widgets.hidden = true;
                elements.hidden = false;
                elements.screen_region = widgets.screen_region;
                UIMap["edit widget elements"]._data.b = false;
                UIMap["selected widget"].label = "selected widget: " + widgets_list.getSelected();
            } else if (UIMap["skins and styling"]._data.b) {
                widgets.hidden = true;
                widgets_style.hidden = false;
                widgets_style.screen_region = widgets.screen_region;
                UIMap["skins and styling"]._data.b = false;
                UIMap["style selected widget"].label = "selected widget: " + widgets_list.getSelected();
            }
        }
    }
    
    if (!elements.hidden) {
        if (UIMap["back to widgets"]._data.b) {
            widgets.hidden = false;
            elements.hidden = true;
            widgets.screen_region = elements.screen_region;
            UIMap["back to widgets"]._data.b = false;
        } else if (UIMap["skins"]._data.b && elements_list.selected_element != -1) {
            elements.hidden = true;
            elements_style.hidden = false;
            elements_style.screen_region = elements.screen_region;
            UIMap["skins"]._data.b = false;
            UIMap["e selected element"].label = "selected element: " + elements_list.getSelected();
        }
    }

    if (!widgets_style.hidden) {
        if(UIMap["to widgets from style"]._data.b) {
            widgets.hidden = false;
            widgets_style.hidden = true;
            widgets.screen_region = widgets_style.screen_region;
            UIMap["to widgets from style"]._data.b = false;
        }
    }

    if (!elements_style.hidden) {
        if(UIMap["to elements from style"]._data.b) {
            elements.hidden = false;
            elements_style.hidden = true;
            elements.screen_region = elements_style.screen_region;
            UIMap["to elements from style"]._data.b = false;
        }
    }
}

void updateUIFromWidget(
    std::string widID, 
    std::string winID, 
    bool do_styling,
    int styling_element
) {
    WIDGET & w = GUI.getWidget(widID, winID);
    UI_elements_map & UIMap = GUI.UIMaps[winID];

    // location
    UIMap["location x"]._data.f = w.screen_region.x * 100.f;
    UIMap["location y"]._data.f = w.screen_region.y * 100.f;

    // sizes
    UIMap["size x"]._data.f = w.screen_region.w * 100.f;
    UIMap["size y"]._data.f = w.screen_region.h * 100.f;

    // additional properties
    UIMap["bordered"]._data.b = w.border;
    UIMap["titled"]._data.b = w.title;
    UIMap["minimizable"]._data.b = w.minimizable;
    UIMap["scalable"]._data.b = w.scalable;
    UIMap["movable"]._data.b = w.movable;
    UIMap["has scrollbar"]._data.b = w.has_scrollbar;

    UIMap["widget border"]._data.f = w.border_size;

    // show/hide
    UIMap["visible"]._data.b = !w.hidden;

    if (styling_element != -1) {
        UIMap["red"]._data.ui = w.style.elements[styling_element].r;
        UIMap["green"]._data.ui = w.style.elements[styling_element].g;
        UIMap["blue"]._data.ui = w.style.elements[styling_element].b;
        UIMap["alpha"]._data.ui = w.style.elements[styling_element].a;
    }
}

extern void addElement(
    std::string widID, 
    std::string winID, 
    std::string elt_name, 
    UI_ELEMENT_TYPE type
);

std::vector<std::string> getPaths(
    bool single_item = false,
    bool select_folders = false,
    std::string start_folder = "None"
);

void processAddOptions(std::string winID) {
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    ui_string_group & widgets_list = *UIMap["widgets list"]._data.usgPtr;
    ui_string_group & elements_list = *UIMap["elements list"]._data.usgPtr;

    if (UIMap["delete element"]._data.b && elements_list.selected_element != -1) {
        WIDGET& w = GUI.getWidget(widgets_list.getSelected(), winID);
        UIMap.deleteElement(elements_list.getSelected(), &w);
        // delete element from list to avoid errors
        elements_list.elements.erase(
            elements_list.elements.begin() + elements_list.selected_element
        );
        elements_list.unselect();
        ui_string_group& rows_list = *UIMap["rows list"]._data.usgPtr;
        ui_string_group& cols_list = *UIMap["cols list"]._data.usgPtr;
        rows_list.unselect();
        cols_list.unselect();
    }

    std::string new_element_name = *UIMap["new element name"]._data.strPtr;
    // if (new_element_name.size() == 0) return;

    if ( UIMap["add image"]._data.b ) {
        UI_ELEMENT_TYPE t = UI_IMAGE;
        std::vector<std::string> paths = getPaths(true, false, GUI.project_dir);
        if (paths.size() > 0) {
            std::string img_path = *paths.begin();
            TexData td = Manager::load_image(img_path.c_str(), true);
            if (UIMap["switch type"]._data.b) {
                new_element_name = elements_list.getSelected();
            } else {
                addElement(widgets_list.getSelected(), winID, new_element_name, t);
            }
            region<float> crop = { 0.f, 0.f, 1.f, 1.f };
            UIMap[new_element_name].initImage(td.texID, td.w, td.h, crop);
            UIMap[new_element_name].label = td.path;
        }
    }
    if (UIMap["add text"]._data.b) {
        if (UIMap["switch type"]._data.b) {
            UIMap[elements_list.getSelected()].type = UI_STRING_TEXT;
        } else {
            UI_ELEMENT_TYPE t = UI_STRING_TEXT;
            addElement(widgets_list.getSelected(), winID, new_element_name, t);
            UIMap[new_element_name].label = new_element_name;
        }
    }
    if (UIMap["add label"]._data.b) {
        if (UIMap["switch type"]._data.b) {
            UIMap[elements_list.getSelected()].type = UI_STRING_LABEL;
        } else {
            UI_ELEMENT_TYPE t = UI_STRING_LABEL;
            addElement(widgets_list.getSelected(), winID, new_element_name, t);
            UIMap[new_element_name].label = new_element_name;
        }
    }
    if (UIMap["add progress"]._data.b) {
        if (UIMap["switch type"]._data.b) {
            UIMap[elements_list.getSelected()].type = UI_PROGRESS;
        } else {
            UI_ELEMENT_TYPE t = UI_PROGRESS;
            addElement(widgets_list.getSelected(), winID, new_element_name, t);
            UIMap[new_element_name].modifyable_progress_bar = true; // hard-code for now
        }
    }
    if (UIMap["add button"]._data.b) {
        if (UIMap["switch type"]._data.b) {
            UIMap[elements_list.getSelected()].type = UI_BUTTON;
        } else {
            UI_ELEMENT_TYPE t = UI_BUTTON;
            addElement(widgets_list.getSelected(), winID, new_element_name, t);
            UIMap[new_element_name].label = new_element_name;
        }
    }
    if (UIMap["add checkbox"]._data.b) {
        if (UIMap["switch type"]._data.b) {
            UIMap[elements_list.getSelected()].type = UI_BOOL;
        } else {
            UI_ELEMENT_TYPE t = UI_BOOL;
            addElement(widgets_list.getSelected(), winID, new_element_name, t);
            UIMap[new_element_name].label = new_element_name;
        }
    }
    // TODO : 
    // add items list, 
    // string input field, 
    // int, uint, float, 
    if (UIMap["add empty"]._data.b) {
        if (UIMap["switch type"]._data.b) {
            UIMap[elements_list.getSelected()].type = UI_EMPTY;
        } else {
            UI_ELEMENT_TYPE t = UI_EMPTY;
            addElement(widgets_list.getSelected(), winID, new_element_name, t);
            UIMap[new_element_name].label = new_element_name;
        }
    }

    if (UIMap["add input"]._data.b) {
        if (UIMap["switch type"]._data.b) {
            UIMap[elements_list.getSelected()].type = UI_STRING_INPUT;
        } else {
            UI_ELEMENT_TYPE t = UI_STRING_INPUT;
            addElement(widgets_list.getSelected(), winID, new_element_name, t);
            UIMap[new_element_name].label = new_element_name;
        }
    }

    if (UIMap["add float"]._data.b) {
        if (UIMap["switch type"]._data.b) {
            UIMap[elements_list.getSelected()].type = UI_FLOAT;
        } else {
            UI_ELEMENT_TYPE t = UI_FLOAT;
            addElement(widgets_list.getSelected(), winID, new_element_name, t);
            UIMap[new_element_name].label = new_element_name;
        }
    }
};

void checkUIValues(std::string winID) {
    UI_elements_map& UIMap = GUI.UIMaps[winID];

    if (UIMap["location x"]._data.f > 100.f)
        UIMap["location x"]._data.f = 100.f;

    if (UIMap["location y"]._data.f > 100.f)
        UIMap["location y"]._data.f = 100.f;

    if (UIMap["size x"]._data.f > 100.f)
        UIMap["size x"]._data.f = 100.f;

    if (UIMap["size y"]._data.f > 100.f)
        UIMap["size y"]._data.f = 100.f;

    if (UIMap["location x"]._data.f < 0.f)
        UIMap["location x"]._data.f = 0.f;

    if (UIMap["location y"]._data.f < 0.f)
        UIMap["location y"]._data.f = 0.f;

    if (UIMap["size x"]._data.f < 0.f)
        UIMap["size x"]._data.f = 0.f;

    if (UIMap["size y"]._data.f < 0.f)
        UIMap["size y"]._data.f = 0.f;

    if (UIMap["red"]._data.ui > 255)
        UIMap["red"]._data.ui = 255;

    if (UIMap["green"]._data.ui > 255)
        UIMap["green"]._data.ui = 255;
    
    if (UIMap["blue"]._data.ui > 255)
        UIMap["blue"]._data.ui = 255;
    
    if (UIMap["alpha"]._data.ui > 255)
        UIMap["alpha"]._data.ui = 255;

    if (UIMap["w crop x"]._data.f > 100.f)
        UIMap["w crop x"]._data.f = 100.f;

    if (UIMap["w crop x"]._data.f < 0.f)
        UIMap["w crop x"]._data.f = 0.f;

    if (UIMap["w crop y"]._data.f > 100.f)
        UIMap["w crop y"]._data.f = 100.f;

    if (UIMap["w crop y"]._data.f < 0.f)
        UIMap["w crop y"]._data.f = 0.f;

    if (UIMap["w crop h"]._data.f > 100.f)
        UIMap["w crop h"]._data.f = 100.f;

    if (UIMap["w crop h"]._data.f < 0.f)
        UIMap["w crop h"]._data.f = 0.f;

    if (UIMap["w crop w"]._data.f > 100.f)
        UIMap["w crop w"]._data.f = 100.f;

    if (UIMap["w crop w"]._data.f < 0.f)
        UIMap["w crop w"]._data.f = 0.f;

    if (UIMap["e crop x"]._data.f > 100.f)
        UIMap["e crop x"]._data.f = 100.f;

    if (UIMap["e crop x"]._data.f < 0.f)
        UIMap["e crop x"]._data.f = 0.f;

    if (UIMap["e crop y"]._data.f > 100.f)
        UIMap["e crop y"]._data.f = 100.f;

    if (UIMap["e crop y"]._data.f < 0.f)
        UIMap["e crop y"]._data.f = 0.f;

    if (UIMap["e crop h"]._data.f > 100.f)
        UIMap["e crop h"]._data.f = 100.f;

    if (UIMap["e crop h"]._data.f < 0.f)
        UIMap["e crop h"]._data.f = 0.f;

    if (UIMap["e crop w"]._data.f > 100.f)
        UIMap["e crop w"]._data.f = 100.f;

    if (UIMap["e crop w"]._data.f < 0.f)
        UIMap["e crop w"]._data.f = 0.f;
}

std::string getColorPropName(COLOR_ELEMENTS prop) {
    if (prop == UI_COLOR_TEXT) return "TEXT";
    if (prop == UI_COLOR_WINDOW) return "WINDOW";
    if (prop == UI_COLOR_HEADER) return "HEADER";
    if (prop == UI_COLOR_BORDER) return "BORDER";
    if (prop == UI_COLOR_BUTTON) return "BUTTON";
    if (prop == UI_COLOR_BUTTON_HOVER) return "BUTTON_HOVER";
    if (prop == UI_COLOR_BUTTON_ACTIVE) return "BUTTON_ACTIVE";
    if (prop == UI_COLOR_TOGGLE) return "TOGGLE";
    if (prop == UI_COLOR_TOGGLE_HOVER) return "TOGGLE_HOVER";
    if (prop == UI_COLOR_TOGGLE_CURSOR) return "TOGGLE_CURSOR";
    if (prop == UI_COLOR_SELECT) return "SELECT";
    if (prop == UI_COLOR_SELECT_ACTIVE) return "SELECT_ACTIVE";
    if (prop == UI_COLOR_SLIDER) return "SLIDER";
    if (prop == UI_COLOR_SLIDER_CURSOR) return "SLIDER_CURSOR";
    if (prop == UI_COLOR_SLIDER_CURSOR_HOVER) return "SLIDER_CURSOR_HOVER";
    if (prop == UI_COLOR_SLIDER_CURSOR_ACTIVE) return "SLIDER_CURSOR_ACTIVE";
    if (prop == UI_COLOR_PROPERTY) return "PROPERTY";
    if (prop == UI_COLOR_EDIT) return "EDIT";
    if (prop == UI_COLOR_EDIT_CURSOR) return "EDIT_CURSOR";
    if (prop == UI_COLOR_COMBO) return "COMBO";
    if (prop == UI_COLOR_CHART) return "CHART";
    if (prop == UI_COLOR_CHART_COLOR) return "CHART_COLOR";
    if (prop == UI_COLOR_CHART_COLOR_HIGHLIGHT) return "CHART_COLOR_HIGHLIGHT";
    if (prop == UI_COLOR_SCROLLBAR) return "SCROLLBAR";
    if (prop == UI_COLOR_SCROLLBAR_CURSOR) return "SCROLLBAR_CURSOR";
    if (prop == UI_COLOR_SCROLLBAR_CURSOR_HOVER) return "SCROLLBAR_CURSOR_HOVER";
    if (prop == UI_COLOR_SCROLLBAR_CURSOR_ACTIVE) return "SCROLLBAR_CURSOR_ACTIVE";
    if (prop == UI_COLOR_TAB_HEADER) return "TAB_HEADER";
    return "NO_COLOR_PROPERTY";
}

std::string getSkinPropName(IMAGE_SKIN_ELEMENT prop) {
    if (prop == background) return "background";
    if (prop == normal) return "normal";
    if (prop == hover) return "hover";
    if (prop == active) return "active";
    if (prop == cursor_normal) return "cursor_normal";
    if (prop == cursor_hover) return "cursor_hover";
    if (prop == cursor_active) return "cursor_active";
    if (prop == pressed) return "pressed";
    if (prop == pressed_active) return "pressed_active";
    if (prop == normal_active) return "normal_active";
    if (prop == hover_active) return "hover_active";
    return "NO_SKIN_PROPERTY";
}

// This is windows-specific part
// #ifdef WIN32
#include <windows.h>
#include <shobjidl.h> 
#include <shlobj.h> 
// #endif
// #include <list>

// #include <list>
#include <string>

std::string getStringFromWSTR(PWSTR wstrPtr) {
	std::string ret_string;
	int path_size = wcslen(wstrPtr);
	wchar_t* wstr_pointer = (wchar_t*)wstrPtr;
	for (int i = 0; i < path_size; i++) {
		ret_string += *wstr_pointer;
		wstr_pointer++;
	}
	return ret_string;
}

std::vector<std::string> getPaths(
	bool single_item, 
	bool select_folders,
	std::string start_folder
) {
	std::string selected_path;
	std::vector<std::string> selected_paths;

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr)) {
		IFileOpenDialog* pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(
            CLSID_FileOpenDialog, 
            NULL, 
            CLSCTX_ALL,
			IID_IFileOpenDialog, 
            reinterpret_cast<void**>(&pFileOpen)
        );

		if (SUCCEEDED(hr)) {
            FILEOPENDIALOGOPTIONS fos = FOS_FORCEFILESYSTEM;
            if (!single_item)
                fos = fos | FOS_ALLOWMULTISELECT;
            
            if (select_folders)
                fos = fos | FOS_PICKFOLDERS;

			hr = pFileOpen->SetOptions(fos);
			if (start_folder != "None") {
                std::replace(start_folder.begin(), start_folder.end(), '/', '\\');
                //start_folder = "C:\\";
				PIDLIST_ABSOLUTE pidl;
				HRESULT hresult = SHParseDisplayName((PCWSTR)start_folder.c_str(), 0, &pidl, SFGAO_FOLDER, 0);
				if (SUCCEEDED(hresult)) {
					IShellItem* psi;
					hresult = SHCreateShellItem(NULL, NULL, pidl, &psi);
					if (SUCCEEDED(hresult)) {
						pFileOpen->SetFolder(psi);
					}
					ILFree(pidl);
				}
			}

			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr)) {
				IShellItemArray* pItem;
				hr = pFileOpen->GetResults(&pItem);
				if (SUCCEEDED(hr)) {
					PWSTR pszFilePath;
					DWORD dwNumItems = 0; // number of items in multiple selection

					hr = pItem->GetCount(&dwNumItems);  // get number of selected items
					for (DWORD i = 0; i < dwNumItems; i++) {
						IShellItem* psi = NULL;
						hr = pItem->GetItemAt(i, &psi); // get a selected item from the IShellItemArray
						if (SUCCEEDED(hr)) {
							hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
							if (SUCCEEDED(hr)) {
								selected_paths.push_back(getStringFromWSTR(pszFilePath));
 								CoTaskMemFree(pszFilePath);
							}
							psi->Release();
						}
					}
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
	return selected_paths;
}