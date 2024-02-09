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

#ifndef INDIEGO_UI_H_
#define INDIEGO_UI_H_

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cassert>
// #include <unordered_set>

#ifndef DEFAULT_WINDOW_NAME
// If app will maintain sinlge window, designer may define it's defautl name
#define DEFAULT_WINDOW_NAME ""
#endif

namespace IndieGo {
	namespace UI {
		enum select_method {
			LIST_SELECT, RADIO_SELECT, BUTTON_SELECT
		};

		struct color { unsigned char r, g, b, a; };

		struct colorf { float r, g, b, a; };

		enum COLOR_ELEMENTS {
			UI_COLOR_TEXT,
			UI_COLOR_WINDOW,
			UI_COLOR_HEADER,
			UI_COLOR_BORDER,
			UI_COLOR_BUTTON,
			UI_COLOR_BUTTON_HOVER,
			UI_COLOR_BUTTON_ACTIVE,
			UI_COLOR_TOGGLE,
			UI_COLOR_TOGGLE_HOVER,
			UI_COLOR_TOGGLE_CURSOR,
			UI_COLOR_SELECT,
			UI_COLOR_SELECT_ACTIVE,
			UI_COLOR_SLIDER,
			UI_COLOR_SLIDER_CURSOR,
			UI_COLOR_SLIDER_CURSOR_HOVER,
			UI_COLOR_SLIDER_CURSOR_ACTIVE,
			UI_COLOR_PROPERTY,
			UI_COLOR_EDIT,
			UI_COLOR_EDIT_CURSOR,
			UI_COLOR_COMBO,
			UI_COLOR_CHART,
			UI_COLOR_CHART_COLOR,
			UI_COLOR_CHART_COLOR_HIGHLIGHT,
			UI_COLOR_SCROLLBAR,
			UI_COLOR_SCROLLBAR_CURSOR,
			UI_COLOR_SCROLLBAR_CURSOR_HOVER,
			UI_COLOR_SCROLLBAR_CURSOR_ACTIVE,
			UI_COLOR_TAB_HEADER
		};

		struct color_table {
			color elements[28] = {
				// default - Nuklear style
				{ 175, 175, 175, 255 },
				{ 45,  45,  45,  255 },
				{ 40,  40,  40,  255 },
				{ 65,  65,  65,  255 },
				{ 50,  50,  50,  255 },
				{ 40,  40,  40,  255 },
				{ 35,  35,  35,  255 },
				{ 100, 100, 100, 255 },
				{ 120, 120, 120, 255 },
				{  45,  45,  45, 255 },
				{  45,  45,  45, 255 },
				{  35,  35,  35, 255 },
				{  38,  38,  38, 255 },
				{ 100, 100, 100, 255 },
				{ 120, 120, 120, 255 },
				{ 150, 150, 150, 255 },
				{  38,  38,  38, 255 },
				{  38,  38,  38, 255 },
				{ 175, 175, 175, 255 },
				{  45,  45,  45, 255 },
				{ 120, 120, 120, 255 },
				{  45,  45,  45, 255 },
				{ 255,   0,   0, 255 },
				{  40,  40,  40, 255 },
				{ 100, 100, 100, 255 },
				{ 120, 120, 120, 255 },
				{ 150, 150, 150, 255 },
				{  40,  40,  40, 255 }
			};
		};

		enum IMAGE_SKIN_ELEMENT {
			background,
			button_normal,
			button_hover,
			button_active,
			
			progress_normal,
			progress_hover,
			progress_active,

			cursor_normal,
			cursor_hover,
			cursor_active,
			pressed,
			pressed_active,
			normal_active,
			hover_active,

			prop_active,
			prop_normal,
			prop_hover
		};

		enum CUSTOM_ELEMENTS {
			border, 
			rounding, 
			padding
		};
		struct image_props {
			// IMAGE_SKIN_ELEMENT : <idx in vector of global images, loaded by backend> : <index of crop from that image>
			std::map<IMAGE_SKIN_ELEMENT, std::pair<int, int>> props = {
				{ background, { -1, -1 } },
				{ button_normal, { -1, -1 } },
				{ button_hover, { -1, -1 } },
				{ button_active, { -1, -1 } },
				{ progress_normal, { -1, -1 } },
				{ progress_hover, { -1, -1 } },
				{ progress_active, { -1, -1 } },
				{ cursor_normal, { -1, -1 } },
				{ cursor_hover, { -1, -1 } },
				{ cursor_active, { -1, -1 } },
				{ pressed, { -1, -1 } },
				{ pressed_active, { -1, -1 } },
				{ normal_active, { -1, -1 } },
				{ hover_active , { -1, -1 } },
				{ prop_active, { -1, -1 } },
				{ prop_normal, { -1, -1 } },
				{ prop_hover, { -1, -1 } }
			};
		};

		struct ui_string_group {
			char **arr = NULL;
			std::vector<std::string> elements;
			std::string unselected = "__None__";

			// additional vector, used to add additional labels to elements
			std::vector<std::string> element_labels;
			int selected_element = -1;

			// Monitor item switch
			bool selection_switch = false;

			select_method selectMethod = LIST_SELECT;


			std::string regrex_filter = "";
			int focused_by_key_element = -1;

			// symbols, that used, if selectMethod is BUTTON_SELECT
			bool use_selected_string = false;
			std::string selectedString = "_";

			void copyLablesFrom(const std::vector<std::string>& s) {
				element_labels.clear();
				element_labels = s;
			}

			void copyFrom(const std::vector<std::string> & s) {
				elements.clear();
				elements = s;
				selected_element = -1;
			}

			void copyFrom(const ui_string_group & usg){
				copyFrom(usg.elements);
			}

			void unselect() {
				selected_element = -1;
			}

			void select(const int & Idx) {
				selected_element = Idx;
			}

			std::string & getSelected() {
				if (selected_element == -1)
					return unselected;
					
				return elements[selected_element];
			}

			std::string& getSelectedLabel() {
				if (elements.size() != element_labels.size())
					return elements[selected_element];
				else
					return element_labels[selected_element];
			}

			int getElementIdx(const std::string & elt) {
				int i = 0;
				for (auto internal_elt : elements){
					if (elt == internal_elt){
						return i;
					}
					i++;
				}
				return -1;
			}

			char ** getCharArrays() {
				if (arr) delete[] arr;
				arr = new char*[ elements.size() ];
				unsigned int idx = 0;
				for (auto elt : elements) {
					arr[idx] = new char[ elt.size() ];
					idx++;
				}
				return arr;
			}

			void disposeCharsArray() {
				if (!arr) return;
				delete[] arr;
				arr = NULL;
			}
		};

		enum UI_ELEMENT_TYPE {
			// parameterize BUTTON styling
			UI_FLOAT,
			UI_INT,
			UI_UINT,
			UI_BOOL,
			UI_STRING_INPUT,
			UI_STRING_LABEL,
			UI_STRING_TEXT,
			UI_BUTTON,
			UI_BUTTON_SWITCH,
			UI_EMPTY,
			UI_ITEMS_LIST,
			UI_COLOR_PICKER,
			UI_IMAGE,
			UI_PROGRESS,
			UI_DROPDOWN
		};

		enum TEXT_ALIGN {
			LEFT, CENTER, RIGHT
		};

		template<typename T>
		struct region { T x, y, w, h; };
		
		template<typename T>
		struct region_size { T w, h; };

		enum ELT_PUSH_OPT {
			to_new_row, // push element to completely new row (grid_rows + 1)
			to_new_col, // push element to last existing row, but new column (grid_rows.back().cells + 1) 
			to_new_subrow // push element to existing row and existing col, but subdivide latter top-down (grid_rows.back().cells.elements + 1) 
		};

		struct TextClickData {
			region_size<unsigned int> click_region;
			std::string original_color = "";
			void(*clickCallback)(std::string &) = 0;
			void(*hoverCallback)(std::string &) = 0;
			// void* dataPtr = 0;
			std::string objectID = "";
		};

		struct UI_element {
			union ui_data {
				bool b;
				float f;
				// in case of UI_IMAGE this contains indxe of imgae in global images vector, it it's initialized.
				// -1 otherwise
				int i;
				unsigned int ui;
				colorf c;
				ui_string_group * usgPtr;
				std::string * strPtr;
			} _data;
			
			
			int min = -1024;
			int max = 1024;
			float minf = -300000.0f;
			float maxf = 300000.0f;
			// For UI_STRING_LABEL:
			// If true, and mouse is hovered over one of clickable_regions, 
			// respective clickCallbacl is called (if set)
			bool hasClickableText = false;
			std::vector<TextClickData> clickable_regions = {};

			bool apply_highlight = false;
			bool apply_shading = false;

			bool isHovered = false;
			bool lmb_click = false;
			bool rmb_click = false;

			// disabled button don't save state
			bool disabled = false;

			// Nuklear does not have options for buttons usage,
			// so this is a workaround
			bool hovered_by_keys = false;
			bool selected_by_keys = false;
			bool tooltip_display = false;
			std::string tooltip_text = "    Long tooltip text    ";
			color_table tooltip_style;

			// interaction callbacks
			std::vector<std::function<void(void*)>> activeCallbacks;
			std::vector<void *> activeDatas;

			std::vector<std::function<void(void*)>> hoverCallbacks;
			std::vector<void *> hoverDatas;

			std::vector<std::function<void(void*)>> clickCallbacks;
			std::vector<void*> clickDatas;

			void setHoverCallback(
				std::function<void(void*)> callbackPtr,
				void* dataPtr = NULL
			) {
				hoverCallbacks.emplace_back(std::move(callbackPtr));
				hoverDatas.push_back(dataPtr);
			};

			void setActiveCallback(
				std::function<void(void*)> callbackPtr,
				void* dataPtr = NULL
			) {
				activeCallbacks.emplace_back(std::move(callbackPtr));
				activeDatas.push_back(dataPtr);
			};

			void setClickCallback(
				std::function<void(void*)> callbackPtr,
				void* dataPtr = NULL
			) {
				clickCallbacks.emplace_back(std::move(callbackPtr));
				clickDatas.push_back(dataPtr);
			};

			// interaction callbacks
			std::vector<std::function<void(void*)>> activeCallbacks;
			std::vector<void *> activeDatas;

			void setActiveCallback(
				void (*callbackPtr)(void*), 
				void * dataPtr = NULL
			) {
				activeCallbacks.push_back(callbackPtr);
				activeDatas.push_back(dataPtr);
			};

			bool modifyable_progress_bar = false;

			TEXT_ALIGN text_align = CENTER;
			UI_ELEMENT_TYPE type;

			// required for serialization
			ELT_PUSH_OPT push_opt;

			// in case of UI_IMAGE this is used as a path to loaded image
			std::string label = "";
			int ui_button_image = -1;
			float flt_px_incr = 0.5f;
			bool color_picker_unwrapped = false;

			float height = 0.1f; // % from widget's height
			float width = 1.f;

			// special properties, that can be called "common"
			float border = 1.f; 
			float rounding = 1.f;
			region_size<float> padding = { 0.01f, 0.01f };

			region<float> layout_border;
			float font_height = 0.023f;
			std::string font = "None";
			float font_size = 16.f;

			bool hidden = false;
			bool takeSpaceIfHidden = true;

			// style settings for various elements
			color_table style;
			bool use_custom_element_style = false;
			image_props skinned_style;
			virtual region<float> getImgCrop(IMAGE_SKIN_ELEMENT elt);

			// default implementation could be overrided
			virtual void callUIfunction(float x, float y, float widget_w, float widget_h);

			// this is for image elements
			virtual void initImage(unsigned int texID, unsigned int w, unsigned int h, region<float> crop);
			unsigned int cropId = 0;

			// this is for various properties, that have option of 
			// using image texture
			virtual void useSkinImage(
				unsigned int texID,
				unsigned short w,
				unsigned short h,
				region<float> crop,
				IMAGE_SKIN_ELEMENT elt
			);
		};
		
		struct row_cell {
			// each cell may contain 1 or more elements
			std::vector<std::string> elements;
			float min_width = 1.f;
			std::string & operator[](unsigned int idx) {
				return elements[idx];
			}
		};

		struct grid_row {
			std::vector<row_cell> cells;
			float min_height = 0.1f;

			// TODO : parametrize member addition
			float allocated_height = 0.f;

			bool in_pixels = false;
			row_cell & operator[](unsigned int idx) {
				return cells[idx];
			}
		};

		// All elements in "folding group" should be consecutive
		struct elements_group {
			bool unfolded = false;
			int row_items_count = 0;
			std::string name;
			std::string start;
			std::string end;
		};

		// defines memory items, that will be used by UI_elements_map
		struct UI_elements_map;

		struct WIDGET_BASE {
			// widgets provide place on screen to display ui elements
			std::vector<std::string> widget_elements;
			std::string name = "";
			bool apply_highlight = false;
			bool apply_shading = false;
			
			// index in global array of images
			// maintained by backend
			unsigned int img_idx = 0;

			// font is switched in runtime
			float font_height = 0.023f;
			std::string font = "None";
			float font_size = 16.f;

			float border_size = 1.f; 
			region_size<float> padding = { 0.01f, 0.01f };
			region_size<float> spacing = { 1.f, 1.f };

			// 	layout_grid :
			// i/j | 0 |    1     | 2
			// ----+---+----------+---
			//  0  |   | elt name |
			// ----+---+----------+---
			//  1  |   |          |
			// ----+---+----------+---
			//  2  |   |          |
			std::vector<grid_row> layout_grid;
			std::vector<elements_group> elements_groups;

			UI_elements_map* uiMapPtr = NULL;
			void updateRowHeight(unsigned int row, float newHeight);
			void updateColWidth(unsigned int row, unsigned int col, float newWidth);
			void copyLayout(WIDGET_BASE * other);

			// true on successfull renaming, false otherwise
			bool renameElement(const std::string & old_name, const std::string & new_name) {
				if (std::find(widget_elements.begin(), widget_elements.end(), old_name) == widget_elements.end())
					return false;
				
				// 1. Rename in widget_elements
				auto element = std::find(widget_elements.begin(), widget_elements.end(), old_name);
				*element = new_name;

				// 2. Rename in layout_grid
				bool renamed = false;
				for (auto row = layout_grid.begin(); row != layout_grid.end(); row++) {
					for (auto cell = row->cells.begin(); cell != row->cells.end(); cell++) {
						if (std::find(cell->elements.begin(), cell->elements.end(), old_name) != cell->elements.end()) {
							auto cell_element = std::find(cell->elements.begin(), cell->elements.end(), old_name);
							*cell_element = new_name;
							renamed = true;
							break;
						}
					}
					if (renamed) break;
				}

				if (!renamed) {
					std::cout << "[RENAME_ELEMENT::WARNING] was not able to find " << old_name << " in " << name << " widget layout_grid!" << std::endl;
					*element = old_name;
				}

				return renamed;
			}

			// true on successfull deletion, false otherwise
			bool deleteElement(
				const std::string & elt_name
			) {
				if (std::find(widget_elements.begin(), widget_elements.end(), elt_name) == widget_elements.end())
					return false;
				widget_elements.erase(
					std::find(widget_elements.begin(), widget_elements.end(), elt_name)
				);
				// 1. delete element from grid
				bool deleted = false;
				// if cell conains 0 elements it sould be deleted!
				// same goes for rows with 0 cells
				int deleteRow = -1, deleteCell = -1, r = 0, c = 0;
				for (auto row = layout_grid.begin(); row != layout_grid.end(); row++) {
					c = 0;
					for (auto cell = row->cells.begin(); cell != row->cells.end(); cell++) {
						if (std::find(cell->elements.begin(), cell->elements.end(), elt_name) != cell->elements.end()) {
							cell->elements.erase(
								std::find(cell->elements.begin(), cell->elements.end(), elt_name)
							);
							deleted = true;
							if (cell->elements.size() == 0)
								deleteCell = c;
							break;
						}
						c++;
					}
					if (deleted) {
						if (deleteCell != -1 && (row->cells.size() - 1 ) == 0)
							deleteRow = r;
						break;
					}
					r++;
				}
				if (!deleted) return false;

				// delete cell and row, if chosen
				if (deleteCell != -1) {
					layout_grid[r].cells.erase(
						layout_grid[r].cells.begin() + deleteCell
					);
				}
				if (deleteRow != -1) {
					layout_grid.erase(
						layout_grid.begin() + deleteRow
					);
				}
				return true;
			} 
			// returns position on layout, that element was eventually added to
			std::pair<int, int> addElement(
				const std::string & elt_name, 
				ELT_PUSH_OPT push_opt = to_new_row,
				const std::string & anchor = "None",
				bool push_after = false
			) {
				// Guard against widgets elements doubling
				if (std::find(widget_elements.begin(), widget_elements.end(), elt_name) != widget_elements.end()){
					std::cout << "[ERROR] addElement: element '" << elt_name << "' already exists in widget " << name << std::endl;
					return std::pair<int, int>(-1, -1);
				}

				if (anchor != "None") {
					if (std::find(widget_elements.begin(), widget_elements.end(), anchor) == widget_elements.end()) {
						std::cout << "[WARNING] addElement: element '" << anchor << "' don't exists in widget " << name << ", pushing " << elt_name << " to grid back" << std::endl;
						return pushToGridBack(elt_name, push_opt);
					}
					return pushToGridAfter(elt_name, push_opt, anchor, push_after);
				} else {
					return pushToGridBack(elt_name, push_opt);
				}
			}
			private:
				std::pair<int, int> pushToGridAfter(
					const std::string & elt_name, 
					ELT_PUSH_OPT push_opt,
					const std::string & anchor,
					bool push_after = false
				) {
					unsigned int rowIdx = 0;
					unsigned int cellIdx = 0;
					bool found = false;
					for (auto row : layout_grid) {
						cellIdx = 0;
						for (auto cell : row.cells) {
							if (std::find(cell.elements.begin(), cell.elements.end(), anchor) != cell.elements.end()) {
								found = true;
								break;
							}
							cellIdx++;
						}
						if (found)
							break;
						rowIdx++;
					}
					if (push_opt == to_new_row) {
						// insert new row
						grid_row new_row;
						row_cell new_cell;
						new_cell.elements.push_back(elt_name);
						new_row.cells.push_back(new_cell);
						if (push_after)
							rowIdx++;
						layout_grid.insert(
							layout_grid.begin() + rowIdx,
							new_row
						);
						// rowIdx++;
					}

					if (push_opt == to_new_col) {
						// insert new column in specified row
						row_cell new_cell;
						new_cell.elements.push_back(elt_name);
						if (push_after)
							cellIdx++;
						layout_grid[rowIdx].cells.insert(
							layout_grid[rowIdx].cells.begin() + cellIdx,
							new_cell
						);
					}

					if (push_opt == to_new_subrow) {
						// push to specified cell.elements after required element
						layout_grid[rowIdx].cells[cellIdx].elements.insert(
							std::find(
								layout_grid[rowIdx].cells[cellIdx].elements.begin(),
								layout_grid[rowIdx].cells[cellIdx].elements.end(),
								anchor
							),
							elt_name
						);
					}
					int shift_idx = push_after ? 1 : 0;
					widget_elements.insert(
						std::find(
							widget_elements.begin(),
							widget_elements.end(),
							anchor
						) + shift_idx,
						elt_name
					);
					return std::pair<int, int>(rowIdx, cellIdx);
				}

				std::pair<int, int> pushToGridBack(
					const std::string & elt_name, 
					ELT_PUSH_OPT push_opt
				) {
					if (push_opt == to_new_row) {
						layout_grid.push_back(grid_row());
						layout_grid.back().cells.push_back(row_cell());
						layout_grid.back().cells.back().elements.push_back(elt_name);
					}

					if (push_opt == to_new_col) {
						if (layout_grid.size() == 0)
							layout_grid.push_back(grid_row());
						layout_grid.back().cells.push_back(row_cell());
						layout_grid.back().cells.back().elements.push_back(elt_name);
					}

					if (push_opt == to_new_subrow) {
						if (layout_grid.size() == 0)
							layout_grid.push_back(grid_row());
						if (layout_grid.back().cells.size() == 0)
							layout_grid.back().cells.push_back(row_cell());
						layout_grid.back().cells.back().elements.push_back(elt_name);
					}
					widget_elements.push_back(elt_name);
					return std::pair<int, int>(layout_grid.size() - 1, layout_grid.back().cells.size() - 1);
				}
		};

		struct UI_elements_map {
			std::map<std::string, UI_element> elements;

			// convenience operator
			UI_element & operator[](const std::string& id){
				// TODO : remove assert in production
				assert(elements.find(id) != elements.end());
				return elements[id];
			}

			void addElement(
				const std::string & elt_name,
				UI_ELEMENT_TYPE type,
				WIDGET_BASE * widRef = NULL,
				ELT_PUSH_OPT push_opt = to_new_row,
				const std::string & anchor = "None",
				bool push_after = false,

				// addition to new col will assign width to elements like so : 1 / cols.size()
				bool autowidth = true
			) {
				if (elements.find(elt_name) != elements.end()) {
					std::cout << "[ERROR] addElement: element '" << elt_name << "' already exists!" << std::endl;
					return;
				}

				std::pair<int, int> elt_pos(-1, -1);
				if (widRef){
					elt_pos = widRef->addElement(elt_name, push_opt, anchor, push_after);
				} else {
					std::cout << "[WARNING] addElement: element '" << elt_name << "' added to UIMap, but no widget will display it! Make shure to add it to *some* widget.widget_elements later!" << std::endl;
				}

				UI_element element;
				element.type = type;
				if (element.type == UI_STRING_INPUT) {
					element._data.strPtr = new std::string;
				} else if (element.type == UI_ITEMS_LIST) {
					element._data.usgPtr = new ui_string_group;
				} else if (element.type == UI_COLOR_PICKER){
					element.height = 0.185f;
				} else if (element.type == UI_BUTTON || element.type == UI_BOOL) {
					// default all boolean switches to false
					element._data.b = false;
				} else if (element.type == UI_FLOAT){
					// all default float values are 0.f
					element._data.f = 0.f;
				} else if (element.type == UI_UINT){
					// all default uint values are 1
					element._data.ui = 1;
				} else if (element.type == UI_IMAGE) {
					element._data.i = -1;
				}
			
				elements[elt_name] = element;
				elements[elt_name].push_opt = push_opt;
				
				if (autowidth) {
					// for (auto cell = layout_grid.back().cells.begin(); cell != layout_grid.back().cells.end(); cell++) {
					for (auto cell = widRef->layout_grid.back().cells.begin(); cell != widRef->layout_grid.back().cells.end(); cell++) {
						cell->min_width = (1.f) / (float)(widRef->layout_grid.back().cells.size());
						for (auto elt : cell->elements) {
							elements[elt].width = (1.f) / (float)(widRef->layout_grid.back().cells.size());
						}
					}
				}

			};

			void deleteElement(const std::string & elt_name, WIDGET_BASE * widRef) {
				// TODO : insert assert(widRef != NULL)
				if (elements.find(elt_name) == elements.end()) {
					std::cout << "[DELETE_ELEMENT::ERROR] element " << elt_name << " does not exist!" << std::endl;
					return;
				}
				if (!widRef->deleteElement(elt_name)) {
					std::cout << "[DELETE_ELEMENT::ERROR] element " << elt_name << " was not found in " << widRef->name << " widget!" << std::endl;
					return;
				}
				elements.erase(
					elements.find(elt_name)
				);
			}

			void renameElement(
				const std::string & old_name, 
				const std::string & new_name, 
				WIDGET_BASE * widRef
			) {
				if (elements.find(old_name) == elements.end()) {
					std::cout << "[RENAME_ELEMENT::ERROR] element " << old_name << " does not exist!" << std::endl;
					return;
				}
				if (!widRef->renameElement(old_name, new_name)) {
					std::cout << "[RENAME_ELEMENT::ERROR] element " << old_name << " was not found in " << widRef->name << " widget!" << std::endl;
					return;
				}
				auto element = elements.extract(old_name);
				element.key() = new_name;
				elements.insert(std::move(element)); // { { 1, "banana" }, { 2, "potato" } }
			}

			void loadUI(std::string & ui_file_path){
				std::ifstream ui_descr(ui_file_path, std::ios::binary);
				ui_descr.seekg(0, std::ios::end);
				std::size_t file_size = ui_descr.tellg(), curr_pos = 0;
				ui_descr.seekg(0, std::ios::beg);
				while (curr_pos != file_size){
					// format is elt_name:elt_type per line
					// read symbols until end of line
				}
			};
		};

		struct Manager;
		struct WIDGET : public WIDGET_BASE {

			// widget's size and location on screen, in percentage with 0% 0% top-left
			region<float> screen_region;
			region_size<unsigned int> screen_size;
			region_size<int> scroll_offsets = { 0, 0 };

			bool hidden = false;

			// set this to "true" to force focus on this widget
			// in current frame, before displayWidgets call
			bool setFocus = false;

			// true, if this widget is currently active (was clicked on)
			bool focused = false;
			
			// true if widget is minimized
			bool minimized = false;

			// true, if cursor is hovered over this widget
			bool hasCursor = false;

			// if true, window shouldn't be focused
			bool forceNoFocus = false;

			// provide implementation with actual drawing calls
			virtual void callImmediateBackend(UI_elements_map & UIMap);

			// returns actually allocated space for row
			virtual float allocateRow(unsigned int cols, float min_height, bool in_pixels);

			virtual void endRow();

			void copyWidget(const std::string & add_name, WIDGET * other);

			virtual void useSkinImage(
				unsigned int texID,
				unsigned short w,
				unsigned short h,
				region<float> crop,
				IMAGE_SKIN_ELEMENT elt
			);

			// Translate to Immediate-Mode flags
			bool border = false;
			bool minimizable = true;
			bool title = true;
			bool movable = true;
			bool scalable = true;
			bool has_scrollbar = true;
			
			color_table style;
			image_props skinned_style;
			bool custom_style = true;
			friend struct Manager;
			float header_height = 0.f;

			void updateScrollPos(int xOffset = -1, unsigned int yOffset = -1) {
				updateScrollPosReq = true;
				scroll_offsets.w = xOffset;
				scroll_offsets.h = yOffset;
			}
		private:

			bool updateScrollPosReq = false;

			// required for serialization
			virtual region<float> getImgCrop(IMAGE_SKIN_ELEMENT elt);

			void callWidgetUI(UI_elements_map & UIMap) {
				bool curr_group_folded = false;
				std::vector<elements_group>::iterator curr_group;
				// float cell_indent = 0.f; // get cell indent + for each drawn row top-to-bottom
				for (auto row = layout_grid.begin(); row != layout_grid.end(); row++) {
					if (row->cells.size() == 0) continue;
					// make sure min_height 
					row->allocated_height = allocateRow(row->cells.size(), row->min_height, row->in_pixels);
					float row_indent = 0.f; // get row indent + for each drawn element left-to-right
					for (auto cell : row->cells) {
						float subcell_indent = 0.f; // get subcell indent for each drawn element in same cell top-to-bottom
						for (auto elt : cell.elements){
							if (UIMap.elements.find(elt) != UIMap.elements.end() && !UIMap.elements[elt].hidden) {
								UIMap.elements[elt].skinned_style = skinned_style;
								UIMap.elements[elt].callUIfunction(
									row_indent,
									subcell_indent,//cell_indent + subcell_indent,
									screen_size.w * screen_region.w, 
									screen_size.h * screen_region.h
								);
								subcell_indent += UIMap.elements[elt].height * screen_size.h * screen_region.h;
							} else {
								// TODO : print warning / throw error
							}
						}
						row_indent += cell.min_width * screen_size.w * screen_region.w;
					}
					endRow();
				}
			};
		};

		struct TEXT_WIDGET : public WIDGET {

		};
		
		struct font_data {
			std::string path;
			std::vector<float> sizes;
			float runtime_set_size;
		};

		struct TexData {
			unsigned int texID;
			int w, h, n;

			// used to identify image on loading
			std::string path;
		};

		// Main UI's controlling memory struct - contains all possible memory maps and widgets,
		// adds new ones and removes old, makes calls to Immediate-Mode backends
		struct Manager {
			
			static void (*buttonClickCallback)(void*);
			std::string project_dir = "";
			region_size<unsigned int> screen_size;

			static void (*custom_ui_uniforms)(void*);
			static void * uniforms_data_ptr;

			// draw index gets updated with each call
			static int draw_idx;

			// [win_id] = ui_map
			std::map<std::string, UI_elements_map> UIMaps;
			
			// required for serialization
			std::string skinning_image = "None";
			region_size<unsigned int> skin_img_size;

			// paths of fonts, used by widgets and vector of available sizes
			std::string main_font = "None";
			float main_font_size = 16.f;
			std::map<std::string, font_data> loaded_fonts;

			// widgets contain elements from specified maps
			// [win_id] = set of widgets
			// application may have several windows, each loading different UI
			std::map<std::string, std::map<std::string, WIDGET>> widgets;

			// idea : set of parameters for cosecutive calls of
			// addWidget, addElements add useSkinImage
			void serialize(const std::string & winID, const std::string & path, const std::vector<std::string> & skipWidgets = {});
			void deserialize(const std::string & winID, const std::string & path);

			void loadFont(std::string path, const std::string & winID, float font_size = 16.f, bool useProjectDir = false, bool cutProjDirFromPath = true);

			// provide init functions in backend renderer module
			void init(
				std::string winID, void * initData = NULL // different backends may require different init data, so keep this as void pointer
			);
			void addWindow(
				std::string winID, void * initData = NULL // different backends may require different init data, so keep this as void pointer
			);
			void removeWindow(
				std::string winID, void * initData = NULL // different backends may require different init data, so keep this as void pointer
			);

			void drawFrameStart(std::string & winID);
			void drawFrameEnd(std::string & winID);

			bool guiHasCursor(const std::string & curr_ui_map = ""){
				if (hoveredWidgets.empty())
					return false;
				
				if (hoveredWidgets.find(curr_ui_map) == hoveredWidgets.end())
					return false;
				
				return hoveredWidgets[curr_ui_map] != NULL;
			}
			// If we want user to prevent focusing some widget, we need to switch back to previously focused
			// std::map<std::string, WIDGET*> prevFocusedWidgets = {};

			std::map<std::string, WIDGET*> hoveredWidgets = {};

			WIDGET & getWidget(const std::string & widget_name, const std::string & win_name = DEFAULT_WINDOW_NAME){
				assert(widgets[win_name].find(widget_name) != widgets[win_name].end());
				return widgets[win_name][widget_name];
			};

			// use this for window callbacks
			void scroll(void * window, double xoff, double yoff);
			void mouse_move(void * window, double x, double y);
			void mouse_button(void * window, int button, int action, int mods );
			void char_input(void * window, unsigned int codepoint);
			void key_input(void * window, unsigned int codepoint, bool pressed = false);

			void initNewMap(const std::string & win_name) {
				UI_elements_map map;
				if ( UIMaps.find(win_name) != UIMaps.end() ){
					UIMaps.erase(
						UIMaps.find(win_name)
					);
				}
				UIMaps[win_name] = map;
			};

			// adds new image to global vector of images. Returns index or recently added image
			static void addImage(unsigned int texID,unsigned short w, unsigned short h, region<float> crop);
			static TexData load_image(std::string path, bool useProjectDir = false);

			WIDGET & addWidget(WIDGET & new_widget, const std::string & win_name = DEFAULT_WINDOW_NAME) {
				// can't add widgets for window without map
				if (UIMaps.find(win_name) == UIMaps.end()){
					initNewMap(win_name);
				}

				if (widgets.find(win_name) == widgets.end()){
					std::map<std::string, WIDGET> container;
					widgets[win_name] = container;
					// prevFocusedWidgets[win_name] = NULL;
					hoveredWidgets[win_name] = NULL;
				}

				if (widgets[win_name].find(new_widget.name) != widgets[win_name].end()){
					// rewrite widget, if it's already exist
					widgets[win_name].erase(
						widgets[win_name].find(new_widget.name)
					);
				}
				widgets[win_name][new_widget.name] = new_widget;
				widgets[win_name][new_widget.name].uiMapPtr = &UIMaps[win_name];
				return widgets[win_name][new_widget.name];
			}

			void displayWidgets(std::string curr_ui_map = DEFAULT_WINDOW_NAME) {
				if (UIMaps.find(curr_ui_map) == UIMaps.end())
					return;

				if (widgets.find(curr_ui_map) == widgets.end())
					return;

				hoveredWidgets[curr_ui_map] = NULL;
				draw_idx = 0;
				for (auto widget = widgets[curr_ui_map].begin(); widget != widgets[curr_ui_map].end(); widget++) {
					if (!widget->second.hidden){
						widget->second.screen_size = screen_size;
						widget->second.callImmediateBackend(UIMaps[curr_ui_map]);
						if (widget->second.hasCursor)
							hoveredWidgets[curr_ui_map] = & widget->second;
					} else {
						// hidden widget loses focus
						widget->second.focused = false;
						widget->second.hasCursor = false;
					}
				}
			};

			void deleteWidget(const std::string & widget_name, const std::string & winID) {
				// 1. Delete all elements from respective UI map
				WIDGET * widRef = & widgets[winID][widget_name];
				std::vector<std::string> widgetElements = widRef->widget_elements;
				for (auto elt : widgetElements) {
					UIMaps[winID].deleteElement(elt, widRef);
				}

				// 2. delete widget
				widgets[winID].erase(
					widgets[winID].find(widget_name)
				);
			}
		};
	}
}

#endif
