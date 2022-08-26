#ifndef INDIEGO_UI_H_
#define INDIEGO_UI_H_

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>

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
			normal,
			hover,
			active,
			cursor_normal,
			cursor_hover,
			cursor_active,
			pressed,
			pressed_active,
			normal_active,
			hover_active
		};

		struct image_props {
			// IMAGE_SKIN_ELEMENT : <idx in vector of global images, loaded by backend> : <index of crop from that image>
			std::map<IMAGE_SKIN_ELEMENT, std::pair<int, int>> props = {
				{ background, { -1, -1 } },
				{ normal, { -1, -1 } },
				{ hover, { -1, -1 } },
				{ active, { -1, -1 } },
				{ cursor_normal, { -1, -1 } },
				{ cursor_hover, { -1, -1 } },
				{ cursor_active, { -1, -1 } },
				{ pressed, { -1, -1 } },
				{ pressed_active, { -1, -1 } },
				{ normal_active, { -1, -1 } },
				{ hover_active , { -1, -1 } }
			};
		};

		struct ui_string_group {
			std::vector<std::string> elements;

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
		};

		enum UI_ELEMENT_TYPE {
			// parameterize BUTTON styling
			UI_FLOAT,
			UI_INT,
			UI_UINT,
			UI_BOOL,
			UI_STRING_INPUT,
			UI_STRING_LABEL,
			UI_BUTTON,
			UI_BUTTON_SWITCH,
			UI_ITEMS_LIST,
			UI_COLOR_PICKER,
			UI_IMAGE,
			UI_PROGRESS
		};

		enum TEXT_ALIGN {
			LEFT, CENTER, RIGHT
		};

		template<typename T>
		struct region { T x, y, w, h; };
		
		template<typename T>
		struct region_size { T w, h; };

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

			// Nuklear does not have options for buttons usage,
			// so this is a workaround
			bool hovered_by_keys = false;
			bool selected_by_keys = false;

			bool modifyable_progress_bar = false;
			TEXT_ALIGN text_align = CENTER;
			UI_ELEMENT_TYPE type;

			// in case of UI_IMAGE this is used as a path to loaded image
			std::string label = "";
			bool color_picker_unwrapped = false;

			float min_height = 25.f;

			bool hidden = false;
			bool takeSpaceIfHidden = true;

			// style settings for various elements
			color_table style;
			image_props skinned_style;

			bool custom_style = true;
			unsigned char layout_row = 0;
			unsigned char layout_col = 0;
			// default implementation could be overrided
			virtual void callUIfunction();

			// this is for image elements
			virtual void initImage(unsigned int texID, std::string path = "");

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
		struct grid_row {
			std::vector<std::string> cells;
			float min_height = 25.f;
		};


		// All elements in "folding group" should be consecutive
		struct elements_group {
			bool unfolded = false;
			int row_items_count = 0;
			std::string name;
			std::string start;
			std::string end;

			bool isElementInGroup(const std::string & element, const std::vector<grid_row> & layout_grid){
				// look for group start, group end, and element
				bool groupStarted = false, groupEnded = false, element_found = false;
				for (auto row : layout_grid){
					for (auto elt : row.cells) {
						if (!groupStarted && elt == start){
							groupStarted = true;
						}
                		if (!groupEnded && elt == end){
                    		groupEnded = true;
                		}

	    				if (element == elt) {
		    				if( !groupStarted )
			    				return false;
				    		else if (!groupEnded)
					    		return true;
                    		else
					    		return false;
                		}
					}
				}
        		return false;
			}
		};

		// defines memory items, that will be used by UI_elements_map
		struct WIDGET_BASE {
			// widgets provide place on screen to display ui elements
			std::vector<std::string> widget_elements;
			std::string name = "";
			// bool backgroundImage = false;
			
			// index in global array of images
			// maintained by backend
			unsigned int img_idx = 0;

			float row_size = 25.f,
				col_size = -1.f; // -1 == auto-scale

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

			// True, if element successfully added to folding group
			bool addElementToGroup(const std::string & elt_name, const std::string & group_name){
				std::vector<elements_group>::iterator target_group = find_if(
					elements_groups.begin(),
					elements_groups.end(),
					[&group_name](elements_group & g){
						return group_name == g.name;
					}
				);
				if (target_group == elements_groups.end()){
					// simply create new group
					elements_group new_group;
					new_group.name = group_name;
					new_group.start = elt_name;
					new_group.end = elt_name;
					new_group.row_items_count = 1;
					elements_groups.push_back(new_group);
					return true;
				} else {
					// Important note - elements in group should go consecutively in layout
					// make shure, that when adding element to existing group, it's located right after current group.end in layout
					int elt_idx = 0, grp_idx = 0;
					for (auto row : layout_grid){
						elt_idx = 0;
						for (auto elt : row.cells){
							if (elt == target_group->end){
								if ( ( elt_idx + 1 ) < row.cells.size()){
									if ( row.cells[elt_idx + 1] == elt_name){
										target_group->end = elt_name;
										target_group->row_items_count++;
										return true;
									} else {
										// next element after group's end is not target_element,
										// therefore group can't be extended to target_element
										return false;
									}
								} else {
									// check first element of next group - if it's target_element,
									// then we can extend group to include it
									if ((grp_idx + 1) < layout_grid.size()){
										if ( layout_grid[grp_idx + 1].cells.size() > 0 && layout_grid[grp_idx + 1].cells[0] == elt_name){
											target_group->end = elt_name;
											target_group->row_items_count++;
											return true;
										} else {
											return false;
										}
									} else {
										return false;
									}
								}
							}
							elt_idx++;
						}
					}
					return false;
				}

			};

			// returns position on layout, that element was eventually added to
			std::pair<int, int> addElement(const std::string & elt_name, int row = -1, int col = -1, bool insert_row = false, bool overwrite_col = true, float min_height = 25.f){
				// Guard against widgets elements doubling
				if (std::find(widget_elements.begin(), widget_elements.end(), elt_name) != widget_elements.end()){
					std::cout << "[ERROR] addElement: element '" << elt_name << "' already exists in widget " << name << std::endl;
					return std::pair<int, int>(-1, -1);
				}
				widget_elements.push_back(elt_name);
				grid_row new_row;
				if (row > -1){
					// make shure there is enough rows
					if( row >= layout_grid.size() ) {
						layout_grid.resize(layout_grid.size() + row + 1);
					} else if (insert_row){
						// if layout_row points to existing row, we should insert new row
						layout_grid.insert(
							layout_grid.begin() + row, new_row
						);
					}
					std::vector<std::string> & elt_row = layout_grid[row].cells;
					if (col > -1){
						// make shure there is enough cols
						if( col >= elt_row.size() ) {
							elt_row.resize(elt_row.size() + col + 1);
						} else if (!overwrite_col) {
							// if layout_row points to existing row, we should insert new row
							elt_row.insert(
								elt_row.begin() + col, "no_element"
							);
						}
						elt_row[col] = elt_name;
						return std::pair<int, int>(row, col);
					} else {
						elt_row.push_back(elt_name);
						return std::pair<int, int>(row, elt_row.size() - 1);
					}
				} else {
					// by default widget gets it's element into fisrt available slot, starting from
					// top left corner, going to down right
					if (layout_grid.size() == 0)
						layout_grid.push_back(new_row);
					std::vector<std::string> & elt_row = layout_grid.back().cells;
					elt_row.push_back(elt_name);
					return std::pair<int, int>(elt_row.size() - 1, 0);
				}
			}
		};

		struct UI_elements_map {
			std::map<std::string, UI_element> elements;

			// convenience operator
			UI_element & operator[](const std::string& id){
				// TODO : add assert(elementExists)
				return elements[id];
			}

			void addElement(
				const std::string & elt_name,
				UI_ELEMENT_TYPE type,
				WIDGET_BASE * widRef = NULL,
				int layout_row = -1,
				int layout_col = -1,
				bool insert_row = false,
				bool overwrite_col = true
			){
				if (elements.find(elt_name) != elements.end()) {
					std::cout << "[ERROR] addElement: element '" << elt_name << "' already exists!" << std::endl;
					return;
				}

				std::pair<int, int> elt_pos(-1, -1);
				if (widRef){
					elt_pos = widRef->addElement(elt_name, layout_row, layout_col, insert_row, overwrite_col);
				} else {
					std::cout << "[WARNING] addElement: element '" << elt_name << "' added to UIMap, but no widget will display it! Make shure to add it to *some* widget.widget_elements later!" << std::endl;
				}

				UI_element element;
				element.type = type;
				if (element.type == UI_STRING_INPUT){
					element._data.strPtr = new std::string;
				} else if (element.type == UI_ITEMS_LIST) {
					element._data.usgPtr = new ui_string_group;
				} else if (element.type == UI_COLOR_PICKER){
					element.min_height = 200.f;
				} else if (element.type == UI_BUTTON || element.type == UI_BOOL){
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
				if (elt_pos.first != -1)
					elements[elt_name].layout_row = elt_pos.first;

				if (elt_pos.second != -1)
					elements[elt_name].layout_col = elt_pos.first;

			};

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

		struct WIDGET : public WIDGET_BASE {

			// widget's size and location on screen, in percentage with 0% 0% top-left
			region<float> screen_region;
			region_size<unsigned int> screen_size;

			bool hidden = false;

			// set this to "true" to force focus on this widget
			// in current frame, before displayWidgets call
			bool setFocus = false;

			// true, if this widget is currently active (was clicked on)
			bool focused = false;
			
			// true, if cursor is hovered over this widget
			bool hasCursor = false;

			// provide implementation with actual drawing calls
			virtual void callImmediateBackend(UI_elements_map & UIMap);
			virtual void allocateRow(unsigned int cols, float min_height);
			virtual void allocateEmptySpace(unsigned int fill_count = 1);
			virtual void allocateGroupStart(elements_group & group, float min_height);
			virtual void allocateGroupEnd();

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

			color_table style;
			image_props skinned_style;
			bool custom_style = true;
		private:
			bool initialized_in_backend = false;

			void callWidgetUI(UI_elements_map & UIMap) {
				bool curr_group_folded = false;
				std::vector<elements_group>::iterator curr_group;
				for (auto row : layout_grid){
					if (row.cells.size() == 0) continue;
					std::string & first_elt = row.cells[0];
					// check, if first element in row starts group - in such case don't allocate row
					curr_group = find_if(elements_groups.begin(), elements_groups.end(), [&first_elt ](elements_group & g){ return g.start == first_elt; });
					if (curr_group == elements_groups.end()){
						// TODO : allocate row only for visible elements
						allocateRow(row.cells.size(), row.min_height);
					}
					for (auto elt : row.cells){
						// check, if element starts drawing group
						curr_group = find_if(elements_groups.begin(), elements_groups.end(), [&elt](elements_group & g){ return g.start == elt; });
						if (curr_group != elements_groups.end()){
							allocateGroupStart(*curr_group, row.min_height);
							curr_group_folded = !curr_group->unfolded;
						}
						if (!curr_group_folded) {
							if (UIMap.elements.find(elt) != UIMap.elements.end() && !UIMap.elements[elt].hidden){
								UIMap.elements[elt].callUIfunction();
							} else {
								// TODO : seems like very strange behavior. Need to investigate
								if (UIMap.elements[elt].takeSpaceIfHidden)
									allocateEmptySpace();
								if (UIMap.elements[elt].type == UI_COLOR_PICKER)
									UIMap.elements[elt].color_picker_unwrapped = false;
							}
						}
						// also check, if element ends group
						curr_group = find_if(elements_groups.begin(), elements_groups.end(), [&elt](elements_group & g){ return g.end == elt; });
						if (curr_group != elements_groups.end()){
							if (!curr_group_folded)
								allocateGroupEnd();
							curr_group_folded = false;
						}
					}
				}
			};
		};

		struct TEXT_WIDGET : public WIDGET {

		};

		// Main UI's controlling memory struct - contains all possible memory maps and widgets,
		// adds new ones and removes old, makes calls to Immediate-Mode backends
		struct Manager {

			region_size<unsigned int> screen_size;

			// [win_id] = ui_map
			std::map<std::string, UI_elements_map> UIMaps;

			// widgets contain elements from specified maps
			// [win_id] = set of widgets
			// application may have several windows, each loading different UI
			std::map<std::string, std::map<std::string, WIDGET>> widgets;

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
			std::map<std::string, WIDGET*> prevFocusedWidgets = {};

			std::map<std::string, WIDGET*> hoveredWidgets = {};

			WIDGET & getWidget(const std::string & widget_name, const std::string & win_name = DEFAULT_WINDOW_NAME){
				return widgets[win_name][widget_name];
			};

			// use this for window callbacks
			void scroll(void * window, double xoff, double yoff);
			void mouse_move(void * window, double x, double y);
			void mouse_button(void * window, int button, int action, int mods );
			void char_input(void * window, unsigned int codepoint);
			void key_input(void * window, unsigned int codepoint, bool pressed = false);

			void initNewMap(const std::string & win_name){
				UI_elements_map map;
				if ( UIMaps.find(win_name) != UIMaps.end() ){
					UIMaps.erase(
						UIMaps.find(win_name)
					);
				}
				UIMaps[win_name] = map;
			};

			// adds new image to global vector of images. Returns index or recently added image
			// unsigned int addImage(unsigned int texID);

			WIDGET & addWidget(WIDGET & new_widget, const std::string & win_name = DEFAULT_WINDOW_NAME) {
				// can't add widgets for window without map
				if (UIMaps.find(win_name) == UIMaps.end()){
					initNewMap(win_name);
				}

				if (widgets.find(win_name) == widgets.end()){
					std::map<std::string, WIDGET> container;
					widgets[win_name] = container;
					prevFocusedWidgets[win_name] = NULL;
					hoveredWidgets[win_name] = NULL;
				}

				if (widgets[win_name].find(new_widget.name) != widgets[win_name].end()){
					// rewrite widget, if it's already exist
					widgets[win_name].erase(
						widgets[win_name].find(new_widget.name)
					);
				}
				widgets[win_name][new_widget.name] = new_widget;
				return widgets[win_name][new_widget.name];
			}

			void displayWidgets(std::string curr_ui_map = DEFAULT_WINDOW_NAME) {
				if (UIMaps.find(curr_ui_map) == UIMaps.end())
					return;

				if (widgets.find(curr_ui_map) == widgets.end())
					return;

				bool hadFocus = false;
				// find previously focused widget
				WIDGET * currFocusedWidget = NULL;
				for (auto widget = widgets[curr_ui_map].begin(); widget != widgets[curr_ui_map].end(); widget++) {
					if (widget->second.focused){
						currFocusedWidget = & widget->second;
						break;
					}
				}

				hoveredWidgets[curr_ui_map] = NULL;
				for (auto widget = widgets[curr_ui_map].begin(); widget != widgets[curr_ui_map].end(); widget++) {
					hadFocus = false;
					
					if ( currFocusedWidget == & widget->second ) 
						hadFocus = true;

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

					if (hadFocus && !widget->second.focused)
						prevFocusedWidgets[curr_ui_map] = & widget->second;
				}
			};
		};
	}
}
#endif
