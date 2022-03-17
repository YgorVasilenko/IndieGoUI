#ifndef INDIEGO_UI_H_
#define INDIEGO_UI_H_

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

namespace IndieGo {
	namespace UI {
enum select_method {
	LIST_SELECT, RADIO_SELECT, BUTTON_SELECT
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
	UI_FLOAT,
	UI_INT,
	UI_UINT,
	UI_BOOL,
	UI_STRING_INPUT,
	UI_BUTTON,
	UI_BUTTON_SWITCH,
	UI_ITEMS_LIST
};

struct UI_element {
    union ui_data {
        bool b;
        float f;
        int i;
        unsigned int ui;
        ui_string_group * usgPtr;
        std::string * strPtr;
    } _data;

	// Nuklear does not have options for buttons usage, 
	// so this is a workaround
	bool hovered_by_keys = false;
	bool selected_by_keys = false;

    UI_ELEMENT_TYPE type;	
	std::string label = "";
	float min_height = 25.f;

	bool hidden = false;

	// default implementation could be overrided
	virtual void callUIfunction();
};
struct grid_row {
	std::vector<std::string> cells;
	float min_height = 25.f;
};

// defines memory items, that will be used by UI_elements_map
struct WIDGET_BASE {
	// widgets provide place on screen to display ui elements
	std::vector<std::string> widget_elements;
	std::string name = "";

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
	void addElement(const std::string & elt_name, int row = -1, int col = -1, bool insert_row = false, bool overwrite_col = true, float min_height = 25.f){
		// Guard against widgets elements doubling
		if (std::find(widget_elements.begin(), widget_elements.end(), elt_name) != widget_elements.end()){
			std::cout << "[ERROR] addElement: element '" << elt_name << "' already exists in widget " << name << std::endl;
			return;
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
			} else {
				elt_row.push_back(elt_name);
			}
		} else {
			// by default widget gets it's element into fisrt available slot, starting from
			// top left corner, going to down right
			if (layout_grid.size() == 0)
				layout_grid.push_back(new_row);
			std::vector<std::string> & elt_row = layout_grid.back().cells;
			elt_row.push_back(elt_name);
		}
	}
};

struct UI_elements_map {
    std::map<std::string, UI_element> elements;

	// convenience operator
	UI_element & operator[](const std::string& id){
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

		if (widRef){
			widRef->addElement(elt_name, layout_row, layout_col, insert_row, overwrite_col);
		} else {
			std::cout << "[WARNING] addElement: element '" << elt_name << "' added to UIMap, but no widget will display it! Make shure to add it to *some* widget.widget_elements later!" << std::endl;
		}

		UI_element element;
		element.type = type;
		if (element.type == UI_STRING_INPUT){
			element._data.strPtr = new std::string;
		} else if (element.type == UI_ITEMS_LIST) {
			element._data.usgPtr = new ui_string_group;
		}
		elements[elt_name] = element;
	};

    // template<typename T>
	// void setData(const std::string & name, T & data){
	// 	if (elements.find(name) == elements.end()) {
    //     	std::cout << "[ERROR] setData: no element with name " << name << std::endl;
    //     	return;
	// 	}
	// 	if constexpr (std::is_same<T, bool>::value){
	// 		if (elements[name].type != UI_BOOL){
	// 			std::cout << "[ERROR] setData: element with name " << name << " is not bool!" <<std::endl;
	// 		} else {
	// 			elements[name]._data.b = data;
	// 		}
	// 	} else if constexpr (std::is_same<T, float>::value){
	// 		if (elements[name].type != UI_FLOAT){
	// 			std::cout << "[ERROR] setData: element with name " << name << " is not float!" <<std::endl;
	// 		} else {
	// 			elements[name]._data.f = data;
	// 		}
	// 	} else if constexpr (std::is_same<T, int>::value){
	// 		if (elements[name].type != UI_INT){
	// 			std::cout << "[ERROR] setData: element with name " << name << " is not int!" <<std::endl;
	// 		} else {
	// 			elements[name]._data.i = data;
	// 		}
	// 	} else if constexpr (std::is_same<T, unsigned int>::value){
	// 		if (elements[name].type != UI_INT){
	// 			std::cout << "[ERROR] setData: element with name " << name << " is not unsigned int!" <<std::endl;
	// 		} else {
	// 			elements[name]._data.ui = data;
	// 		}
	// 	} else {
	// 		std::cout << "[ERROR] setData: cant set data for element " << name << std::endl;
	// 	}
	// };

    // template<typename T>
	// T & getData(const std::string & name) {
	// 	if constexpr (std::is_same<T, bool>::value){
	// 		return elements[name]._data.b;
	// 	} else if constexpr (std::is_same<T, float>::value){
	// 		return elements[name]._data.f;
	// 	} else if constexpr (std::is_same<T, int>::value){
	// 		return elements[name]._data.i;
	// 	} else if constexpr (std::is_same<T, unsigned int>::value){
	// 		return elements[name]._data.ui;
	// 	}
	// 	std::cout << "[ERROR] getData: can't return no data for element " << name << std::endl;
	// 	T no_data;
	// 	return no_data;
	// };

    // template<typename T>
	// T * getDataPtr(const std::string & name){
	// 	if constexpr (std::is_same<T, std::string>::value){
	// 		return elements[name]._data.strPtr;
	// 	} else if constexpr (std::is_same<T, ui_string_group>::value){
	// 		return elements[name]._data.usgPtr;
	// 	}
	// 	std::cout << "[ERROR] getDataPtr: cant return NULL ptr for element " << name << std::endl;       
	// 	return NULL;
	// };

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

struct region { short x, y, w, h; };
struct region_size { short w, h; };
struct color { unsigned char r, g, b, a; };
struct color_table { color elements[28] = {}; };


struct WIDGET : public WIDGET_BASE {
	// widget's size and location on screen
	region screen_region;

	bool hidden = false;

	// provide implementation with actual drawing calls
	virtual void callImmediateBackend(UI_elements_map & UIMap);
	virtual void allocateRow(unsigned int cols, float min_height);
	virtual void allocateEmptySpace(unsigned int fill_count = 1);

	// Translate to Immediate-Mode flags
	bool border = false;
	bool minimizable = true;
	bool title = true;
	bool movable = true;
	bool scalable = true;

private:
	void callWidgetUI(UI_elements_map & UIMap) {
		for (auto row : layout_grid){
			allocateRow(row.cells.size(), row.min_height);	
			for (auto elt : row.cells){
				if (UIMap.elements.find(elt) != UIMap.elements.end() && !UIMap.elements[elt].hidden){
					UIMap.elements[elt].callUIfunction();
				} else {
					allocateEmptySpace();
				}
			}
		}
	};
};

struct TEXT_WIDGET : public WIDGET {

};

// Main UI's controlling memory struct - contains all possible memory maps and widgets,
// adds new ones and removes old, makes calls to Immediate-Mode backends
struct IndieGoUI {
	// [win_id] = ui_map
	std::map<std::string, UI_elements_map> UIMaps;

	// widgets contain elements from specified maps
	// [win_id] = set of widgets
	// application may have several windows, each loading different UI
	std::map<std::string, std::map<std::string, WIDGET>> widgets;

	// provide init functions in backend renderer module
	void init(
		void * initData = NULL // different backends may require different init data, so keep this as void pointer
	);

	void drawFrameStart();
	void drawFrameEnd();

	// use this for window callbacks
	void scroll(double xoff, double yoff);
	void mouse_move(double x, double y);
	void mouse_button( int button, int action, int mods );
	void char_input(unsigned int codepoint);
	void key_input(unsigned int codepoint, bool pressed = false);

	void initNewMap(const std::string & win_name){
		UI_elements_map map;
		if ( UIMaps.find(win_name) != UIMaps.end() ){
			UIMaps.erase(
				UIMaps.find(win_name)
			);
		}
		UIMaps[win_name] = map;
	};

	void addWidget(const std::string & win_name, WIDGET & new_widget){
		// can't add widgets for window without map
		if (UIMaps.find(win_name) == UIMaps.end()){
			initNewMap(win_name);
		}

		if (widgets.find(win_name) == widgets.end()){
			std::map<std::string, WIDGET> container;
			widgets[win_name] = container;
		}

		if (widgets[win_name].find(new_widget.name) != widgets[win_name].end()){
			// rewrite widget, if it's already exist
			widgets[win_name].erase(
				widgets[win_name].find(new_widget.name)
			);
		}
		widgets[win_name][new_widget.name] = new_widget;
	}

	void displayWidgets(std::string curr_ui_map){
		if (UIMaps.find(curr_ui_map) == UIMaps.end())
			return;
		
		if (widgets.find(curr_ui_map) == widgets.end())
			return;

		for (auto widget : widgets[curr_ui_map]){
			if (!widget.second.hidden){
				widget.second.callImmediateBackend(UIMaps[curr_ui_map]);
			}
		}
	};
};
	}
}
#endif