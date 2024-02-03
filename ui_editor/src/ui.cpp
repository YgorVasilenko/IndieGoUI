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

#include <editor_structs.h>
#include <Shader.h>
#include <vector>
#include <list>
#include <filesystem>
#include <queue>
#include <functional>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#endif

#include "backends/Nuklear/nuklear.h"

namespace fs = std::filesystem;

using namespace IndieGo::UI;

extern Manager GUI;
extern EditorState editorGlobals;
extern std::queue<std::function<void()>> delayedFunctions;

extern void serializeCropsData(const std::string & path);
extern void deserializeCropsData(const std::string & path);

std::string getTextAlignLabel(IndieGo::UI::TEXT_ALIGN align) {
    if (align == IndieGo::UI::TEXT_ALIGN::LEFT)
        return "text align: left";
    
    if (align == IndieGo::UI::TEXT_ALIGN::CENTER)
        return "text align: center";

    if (align == IndieGo::UI::TEXT_ALIGN::RIGHT)
        return "text align: right";

    return "Unexpected value!";
}


void updateUIFromElement(void*) {
    UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
    ui_string_group & elements_list = *UIMap["elements list"]._data.usgPtr;
    if (elements_list.selected_element == -1)
        editorGlobals.selectedElement = "None";

    editorGlobals.selectedElement = elements_list.getSelected();

    if (editorGlobals.selectedElement == "None")
        return;

    UI_element & e = UIMap[editorGlobals.selectedElement];

    UIMap["element border"]._data.f = e.border * UI_FLT_VAL_SCALE;
    UIMap["element pad x"]._data.f = e.padding.w * UI_FLT_VAL_SCALE;
    UIMap["element pad y"]._data.f = e.padding.h * UI_FLT_VAL_SCALE;
    UIMap["element rounding"]._data.f = e.rounding * UI_FLT_VAL_SCALE;
    UIMap["element width"]._data.f = e.width * UI_FLT_VAL_SCALE;
    UIMap["element height"]._data.f = e.height * UI_FLT_VAL_SCALE;

    *UIMap["elt label"]._data.strPtr = e.label;
    UIMap["element text align"].label = getTextAlignLabel(e.text_align);

    // font
    if (e.font != "None") {
        UIMap["current font"].label = e.font + " " + std::to_string(e.font_size);
    } else {
        UIMap["current font"].label = "None";
    }
}

void updateUIFromWidget(void*) {
    UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
    ui_string_group & widgets_list = *UIMap["widgets list"]._data.usgPtr;

    if (widgets_list.selected_element == -1) {
        editorGlobals.selectedWidget = "None";
        return;
    }

    editorGlobals.selectedWidget = widgets_list.getSelected();
    WIDGET & w = GUI.getWidget(editorGlobals.selectedWidget, editorGlobals.winID);

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

    // font
    if (w.font != "None") {
        UIMap["current font"].label = w.font + " " + std::to_string(w.font_size);
    } else {
        UIMap["current font"].label = "None";
    }

    UIMap["widget border"]._data.f = w.border_size;

    // show/hide
    UIMap["visible"]._data.b = !w.hidden;

    if (editorGlobals.styling_element != -1) {
        UIMap["red"]._data.ui = w.style.elements[editorGlobals.styling_element].r;
        UIMap["green"]._data.ui = w.style.elements[editorGlobals.styling_element].g;
        UIMap["blue"]._data.ui = w.style.elements[editorGlobals.styling_element].b;
        UIMap["alpha"]._data.ui = w.style.elements[editorGlobals.styling_element].a;
    }

    ui_string_group & elements_list = *UIMap["elements list"]._data.usgPtr;
    ui_string_group & rows_list = *UIMap["rows list"]._data.usgPtr;
    ui_string_group & cols_list = *UIMap["cols list"]._data.usgPtr;

    elements_list.selected_element = -1;
    elements_list.elements.clear();

    rows_list.selected_element = -1;
    rows_list.elements.clear();

    cols_list.selected_element = -1;
    cols_list.elements.clear();

    // Udate displayed elements lists with what is in selected widget
    for (auto element : w.widget_elements) {
        elements_list.elements.push_back(element);
    }

    // update rows and cols lists
    for (int i = 0; i < w.layout_grid.size(); i++) {
        rows_list.elements.push_back(std::to_string(i));
    }
    UIMap["w selected widget"].label = "selected widget: " + editorGlobals.selectedWidget;
}

extern void addElement(
    std::string widID, 
    std::string winID, 
    std::string elt_name, 
    std::string anchor,
    bool push_after, 
    UI_ELEMENT_TYPE type
);

std::vector<std::string> getPaths(
    bool single_item = false,
    bool select_folders = false,
    std::string start_folder = "None"
);

void processAddImage(
    const std::string & new_element_name,
    const std::string & anchor_element,
    bool push_after_anchor
) {
    UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
    UI_ELEMENT_TYPE t = UI_IMAGE;
    if (UIMap["crop as img"]._data.b) {
        if (UIMap["switch type"]._data.b) {
            if (editorGlobals.selectedElement == "None")
                return;
            UIMap[editorGlobals.selectedElement].type = t;
            UIMap[editorGlobals.selectedElement]._data.i = -1;
        } else {
            addElement(
                editorGlobals.selectedWidget, 
                editorGlobals.winID, 
                new_element_name, 
                anchor_element,
                push_after_anchor,
                t
            );
        }
    } else {
        std::vector<std::string> paths = getPaths(true, false, GUI.project_dir);
        if (paths.size() > 0) {
            std::string img_path = *paths.begin();
            TexData td = Manager::load_image(img_path.c_str(), true);
            if (UIMap["switch type"]._data.b) {
                if (editorGlobals.selectedElement == "None")
                    return;
                UIMap[editorGlobals.selectedElement].type = t;
            } else {
                addElement(
                    editorGlobals.selectedWidget, 
                    editorGlobals.winID,
                    new_element_name,
                    anchor_element,
                    push_after_anchor,
                    t
                );
            }
            region<float> crop = { 0.f, 0.f, 1.f, 1.f };
            UIMap[new_element_name].initImage(td.texID, td.w, td.h, crop);
            UIMap[new_element_name].label = td.path;
            *UIMap["elt label"]._data.strPtr = UIMap[new_element_name].label;
        }
    }
}

std::string addElement(UI_ELEMENT_TYPE et) {
    UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
    bool use_anchor = UIMap["selected anchor"]._data.b;
    bool push_after_anchor = UIMap["push after anchor"]._data.b;
    
    std::string new_element_name = *UIMap["new element name"]._data.strPtr;
    if (new_element_name.size() == 0) 
        return "None";
    
    std::string anchor_element = use_anchor ? editorGlobals.selectedElement : "None";

    if (et == UI_IMAGE) {
        processAddImage(
            new_element_name,
            anchor_element,
            push_after_anchor
        );
        return "None";
    }

    if (UIMap["switch type"]._data.b) {
        if (editorGlobals.selectedElement == "None")
            return "None";
        UIMap[editorGlobals.selectedElement].type = et;
    } else {
        addElement(
            editorGlobals.selectedWidget, 
            editorGlobals.winID, 
            new_element_name, 
            anchor_element,
            push_after_anchor,
            et
        );
        UIMap[new_element_name].label = new_element_name;
    }
    return editorGlobals.selectedElement;
}

std::vector<std::string> skip_save_widgets = { 
    "UI creator", 
    "Edit elements", 
    "Widgets style", 
    "Elements style",
    "Fonts",
    "Skinning",
    "Element properties"
};

void saveUI(void*) {
    std::vector<std::string> paths = getPaths(true, false, GUI.project_dir);
    if (paths.size() > 0) {
        GUI.serialize(
            editorGlobals.winID,
            paths.front(),
            skip_save_widgets
        );
        std::string editor_file_name = fs::path( paths.front() ).stem().string();
        std::string editor_file = fs::path( paths.front() ).parent_path().append(editor_file_name + "_editor.ui").string();
        serializeCropsData(editor_file);
    }
}

extern LayoutRect skin_img_rect;
extern Shader skinningShader;
extern std::map<std::string, std::pair<TexData, region<float>>> skin_crops;
extern std::unordered_map<unsigned int, std::vector<std::pair<struct nk_image, IndieGo::UI::region<float>>>> images;
extern std::map<std::string, TexData> loaded_textures;
extern GLFWwindow* screen;
extern void initWidgets();
extern void initProjectDir();

void closeUI() {
    auto& UIMap = GUI.UIMaps[editorGlobals.winID];
    auto& widgets_list = *UIMap["widgets list"]._data.usgPtr;

    for (const auto& widgetName : widgets_list.elements)
        GUI.deleteWidget(widgetName, editorGlobals.winID);

    widgets_list.elements.clear();
    widgets_list.selected_element = -1;
    skinningShader.skin_tex_id = 0;
    skin_crops.clear();
    
    EditorState es;
    std::swap(editorGlobals.winID, es.winID);
    editorGlobals = std::move(es);

    images.clear();

    for(const auto& [path, texData] : loaded_textures)
        glDeleteTextures(1, &texData.texID);

    delayedFunctions.emplace([]() {
        GUI = Manager{};

        GUI.init(editorGlobals.winID, screen);
        GUI.screen_size.w = WIDTH;
        GUI.screen_size.h = HEIGHT;

        initWidgets();
        initProjectDir();
    });
}

void closeUICallback(void*) {
#ifdef _WIN32
    if(MessageBoxA(glfwGetWin32Window(screen), "Close project?", "Confirmation", MB_OKCANCEL) == IDCANCEL)
        return;
#endif
    closeUI();
}

void loadUIInternal(std::vector<std::string> paths) {
    UI_elements_map& UIMap = GUI.UIMaps[editorGlobals.winID];
    ui_string_group& widgets_list = *UIMap["widgets list"]._data.usgPtr;
    ui_string_group& fonts_list = *UIMap["loaded fonts"]._data.usgPtr;

    for (const auto& path : paths) {
        GUI.deserialize(editorGlobals.winID, path);
        std::string editor_file_name = fs::path(paths.front()).stem().string();
        std::string editor_file = fs::path(paths.front()).parent_path().append(editor_file_name + "_editor.ui").string();
        if (fs::exists(editor_file)) {
            deserializeCropsData(editor_file);
        }

        for (auto widget = GUI.widgets[editorGlobals.winID].begin(); widget != GUI.widgets[editorGlobals.winID].end(); widget++) {
            if (std::find(skip_save_widgets.begin(), skip_save_widgets.end(), widget->first) != skip_save_widgets.end())
                continue;

            widget->second.hidden = true;
            // don't add already added elements
            if (std::find(widgets_list.elements.begin(), widgets_list.elements.end(), widget->first) != widgets_list.elements.end())
                continue;
            widgets_list.elements.push_back(widget->first);
        }
        for (const auto& font : GUI.loaded_fonts) {
            if (font.first == GUI.main_font)
                continue;

            if (std::find(fonts_list.elements.begin(), fonts_list.elements.end(), font.first) != fonts_list.elements.end())
                continue;

            fonts_list.elements.push_back(font.first);
        }
        // set skinning image texID if loaded, populate crops list 
        if (GUI.skinning_image != "None") {
            TexData skin_tex = Manager::load_image(GUI.skinning_image);
            skinningShader.skin_tex_id = skin_tex.texID;
            skin_img_rect.width = 1.f;
            skin_img_rect.height = (float)skin_tex.h / (float)skin_tex.w;
        }
    }
}
void loadUI(void*) {
    auto paths = getPaths(false, false, GUI.project_dir);
    if(paths.empty())
        return;

    closeUI();
    delayedFunctions.emplace(std::bind(loadUIInternal, std::move(paths)));
}

extern std::vector<float> font_load_sizes;
void loadFont(void*) {
    UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
    ui_string_group & fonts_list = *UIMap["loaded fonts"]._data.usgPtr;
    std::vector<std::string> paths = getPaths(false, false, GUI.project_dir);
    if (paths.size() > 0) {
        // load several sizes
        for (auto size : font_load_sizes) {
            GUI.loadFont(
                paths[0],
                editorGlobals.winID, 
                size,
                true
            );
        }
        // additionally load specified font
        GUI.loadFont(
            paths[0],
            editorGlobals.winID, 
            UIMap["load size"]._data.f,
            true
        );
        // update fonts list
        fonts_list.elements.clear();
        for (auto font_path : GUI.loaded_fonts) {
            if (fs::path(font_path.first).stem().string() == GUI.main_font) 
                continue; // don't display main font
            fonts_list.elements.push_back( fs::path(font_path.first).stem().string() );
        }
    }
}

void loadSkinImage(void*) {
    UI_elements_map & UIMap = GUI.UIMaps[editorGlobals.winID];
    std::vector<std::string> paths = getPaths(false, false, GUI.project_dir);
    if (paths.size() > 0) {
        std::string skinning_img_path = *paths.begin();
        TexData skin_tex = Manager::load_image(skinning_img_path.c_str());
        UIMap["w skin image path"].label = skinning_img_path;
        UIMap["e skin image path"].label = skinning_img_path;
        skinningShader.skin_tex_id = skin_tex.texID;
        skin_img_rect.height = (float)skin_tex.h / (float)skin_tex.w;
        GUI.skinning_image = skin_tex.path;
        GUI.skin_img_size.w = skin_tex.w;
        GUI.skin_img_size.h = skin_tex.h;
    }
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
    
    if (prop == button_normal) return "button_normal";
    if (prop == button_hover) return "button_hover";
    if (prop == button_active) return "button_active";
    
    if (prop == progress_normal) return "progress_normal";
    if (prop == progress_hover) return "progress_hover";
    if (prop == progress_active) return "progress_active";

    if (prop == cursor_normal) return "cursor_normal";
    if (prop == cursor_hover) return "cursor_hover";
    if (prop == cursor_active) return "cursor_active";
    if (prop == pressed) return "pressed";
    if (prop == pressed_active) return "pressed_active";
    if (prop == normal_active) return "normal_active";
    if (prop == hover_active) return "hover_active";

    if (prop == prop_active) return "prop_active";
    if (prop == prop_normal) return "prop_normal";
    if (prop == prop_hover) return "prop_hover";

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
	size_t path_size = wcslen(wstrPtr);

	for (size_t i = 0; i < path_size; i++)
		ret_string += static_cast<char>(*(wstrPtr++));

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