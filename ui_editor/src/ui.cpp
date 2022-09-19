#include <IndieGoUI.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"
#include <vector>
#include <list>

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
    std::string winID
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
    int styling_element,
    int layout_row
) {
    WIDGET & w = GUI.getWidget(widID, winID);
    UI_elements_map & UIMap = GUI.UIMaps[winID];

    // if (layout_row != -1) {
    //     w.layout_grid[layout_row].min_height = UIMap["row height"]._data.f;
    // }

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

    w.border_size = UIMap["widget border"]._data.f;

    // if (do_styling) {
    //     // "general" elements update
    //     w.border_size = UIMap["border size"]._data.f;
    //     w.padding.w = UIMap["padding x"]._data.f;
    //     w.padding.h = UIMap["padding y"]._data.f;
    //     w.spacing.w = UIMap["spacing x"]._data.f;
    //     w.spacing.h = UIMap["spacing y"]._data.f;
        
    //     if (UIMap["use font"]._data.b) {
    //         ui_string_group& available_fonts_list = *UIMap["available fonts"]._data.usgPtr;
    //         ui_string_group& font_sizes_list = *UIMap["font sizes"]._data.usgPtr;

    //         if (available_fonts_list.selected_element != -1 && font_sizes_list.selected_element != -1) {
    //             w.font = available_fonts_list.getSelected();
    //             w.font_size = std::stof(font_sizes_list.getSelected());
    //         }
    //     }
    // }

    // if (styling_element != -1) {
    //     w.style.elements[styling_element].r = UIMap["Red property color"]._data.ui;
    //     w.style.elements[styling_element].g = UIMap["Green property color"]._data.ui;
    //     w.style.elements[styling_element].b = UIMap["Blue property color"]._data.ui;
    //     w.style.elements[styling_element].a = UIMap["Alpha property color"]._data.ui;
    // }
}

void switchUIscreens(std::string winID) {
    WIDGET & widgets = GUI.getWidget("UI creator", winID);
    WIDGET & elements = GUI.getWidget("Edit elements", winID);
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    ui_string_group & widgets_list = *UIMap["widgets list"]._data.usgPtr;

    if (!widgets.hidden) { // if "main widgets" visible
        if (widgets_list.selected_element != -1 && UIMap["edit widget elements"]._data.b) {
            widgets.hidden = true;
            elements.hidden = false;
            elements.screen_region = widgets.screen_region;
            UIMap["edit widget elements"]._data.b = false;
            UIMap["selected widget"].label = "selected widget: " + widgets_list.getSelected();
        }
    }
    
    if (!elements.hidden) {
        if (UIMap["back to widgets"]._data.b) {
            widgets.hidden = false;
            elements.hidden = true;
            widgets.screen_region = elements.screen_region;
            UIMap["back to widgets"]._data.b = false;
        }
    }
}

void updateUIFromWidget(
    std::string widID, 
    std::string winID, 
    bool do_styling,
    int styling_element,
    int layout_row
) {
    WIDGET & w = GUI.getWidget(widID, winID);
    UI_elements_map & UIMap = GUI.UIMaps[winID];

    // if (layout_row != -1) {
    //     UIMap["row height"]._data.f = w.layout_grid[layout_row].min_height;
    // }

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

    UIMap["widget border"]._data.f = w.border_size;
    // if (do_styling) {
    //     if (styling_element != -1) {
    //         UIMap["Red property color"]._data.ui = w.style.elements[styling_element].r;
    //         UIMap["Green property color"]._data.ui = w.style.elements[styling_element].g;
    //         UIMap["Blue property color"]._data.ui = w.style.elements[styling_element].b;
    //         UIMap["Alpha property color"]._data.ui = w.style.elements[styling_element].a;
    //     }

    //     UIMap["border size"]._data.f = w.border_size;
    //     UIMap["padding x"]._data.f = w.padding.w;
    //     UIMap["padding y"]._data.f = w.padding.h;
    //     UIMap["spacing x"]._data.f = w.spacing.w;
    //     UIMap["spacing y"]._data.f = w.spacing.h;
    // }
}

extern void addElement(
    std::string widID, 
    std::string winID, 
    std::string elt_name, 
    UI_ELEMENT_TYPE type
);
extern std::list<std::string> getPaths();

void processAddOptions(std::string winID) {
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    ui_string_group & widgets_list = *UIMap["widgets list"]._data.usgPtr;
    
    std::string new_element_name = *UIMap["new element name"]._data.strPtr;
    if (new_element_name.size() == 0) return;

    if ( UIMap["add image"]._data.b ) {
        UI_ELEMENT_TYPE t = UI_IMAGE;
        std::list<std::string> paths = getPaths();
        if (paths.size() > 0) {
            std::string img_path = *paths.begin();
            TexData td = Manager::load_image(img_path.c_str());
            addElement(widgets_list.getSelected(), winID, new_element_name, t);
            region<float> crop = { 0.f, 0.f, 1.f, 1.f };
            UIMap[new_element_name].initImage(td.texID, td.w, td.h, crop);
        }
    }
    if (UIMap["add text"]._data.b) {
        UI_ELEMENT_TYPE t = UI_STRING_TEXT;
        addElement(widgets_list.getSelected(), winID, new_element_name, t);
        UIMap[new_element_name].label = new_element_name;
    }
    if (UIMap["add label"]._data.b) {
        UI_ELEMENT_TYPE t = UI_STRING_LABEL;
        addElement(widgets_list.getSelected(), winID, new_element_name, t);
        UIMap[new_element_name].label = new_element_name;
    }
    if (UIMap["add progress"]._data.b) {
        UI_ELEMENT_TYPE t = UI_PROGRESS;
        addElement(widgets_list.getSelected(), winID, new_element_name, t);
        UIMap[new_element_name].modifyable_progress_bar = true; // hard-code for now
    }
    if (UIMap["add button"]._data.b) {
        UI_ELEMENT_TYPE t = UI_BUTTON;
        addElement(widgets_list.getSelected(), winID, new_element_name, t);
        UIMap[new_element_name].label = new_element_name;
    }
    if (UIMap["add checkbox"]._data.b) {
        UI_ELEMENT_TYPE t = UI_BOOL;
        addElement(widgets_list.getSelected(), winID, new_element_name, t);
        UIMap[new_element_name].label = new_element_name;
    }
    // TODO : 
    // add items list, 
    // string input field, 
    // int, uint, float, 
    // empty space
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


    // if (UIMap["Red property color"]._data.ui > 255)
    //     UIMap["Red property color"]._data.ui = 255;

    // if (UIMap["Green property color"]._data.ui > 255)
    //     UIMap["Green property color"]._data.ui = 255;
    

    // if (UIMap["Blue property color"]._data.ui > 255)
    //     UIMap["Blue property color"]._data.ui = 255;
    
    // if (UIMap["Alpha property color"]._data.ui > 255)
    //     UIMap["Alpha property color"]._data.ui = 255;
}

// std::vector<unsigned int> loadedImages;

// extern unsigned int skinning_image_id;
// extern unsigned int si_w;
// extern unsigned int si_h;

// helper function lo load image through stbi
// in other engine parts ImageLoader will do that
// unsigned int load_image(const char *filename, bool load_skinning_image = false) {
//     int x,y,n;
//     unsigned int tex;
//     unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
//     // if (!data) die("[SDL]: failed to load image: %s", filename);
//     if (!data) {
//         std::cout << "[ERROR] failed to load image " << filename << std::endl;
//         return 0;
//     }
//     glGenTextures(1, &tex);
//     glBindTexture(GL_TEXTURE_2D, tex);
//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//     glGenerateMipmap(GL_TEXTURE_2D);
//     stbi_image_free(data);
//     loadedImages.push_back(tex);

//     if (load_skinning_image) {
//         skinning_image_id = tex;
//         si_w = x;
//         si_h = y;
//     }
//     return tex;
// }

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
// #endif
// #include <list>

#include <list>
#include <string>

std::list<std::string> getPaths() {
	std::string selected_path;
	std::list<std::string> selected_paths;

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
			hr = pFileOpen->SetOptions(FOS_FORCEFILESYSTEM | FOS_ALLOWMULTISELECT);
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr)) {
				//IShellItem* pItem;
				IShellItemArray* pItem;
				hr = pFileOpen->GetResults(&pItem);
				if (SUCCEEDED(hr)) {
					PWSTR pszFilePath;
					DWORD dwNumItems = 0; // number of items in multiple selection
					//std::wstring strSelected; // will hold file paths of selected items
					std::string strSelected;

					hr = pItem->GetCount(&dwNumItems);  // get number of selected items
					for (DWORD i = 0; i < dwNumItems; i++) {
						IShellItem* psi = NULL;
						hr = pItem->GetItemAt(i, &psi); // get a selected item from the IShellItemArray
						if (SUCCEEDED(hr)) {
							hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
							if (SUCCEEDED(hr)) {
								strSelected.clear();
								int path_size = wcslen(pszFilePath);
								wchar_t* path_pointer = (wchar_t*)pszFilePath;
								for (int i = 0; i < path_size; i++) {
									strSelected += *path_pointer;
									path_pointer++;
								}
								selected_paths.push_back(strSelected);
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