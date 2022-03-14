#ifndef INDIEGO_UI_H_
#define INDIEGO_UI_H_

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

enum select_method {
	LIST_SELECT, RADIO_SELECT, BUTTON_SELECT
};

struct ui_string_group {
    std::vector<std::string> elements;

	// additional vector, used to add additional labels to elements
	std::vector<std::string> element_labels;

	int selected_element = -1;
	
	select_method selectMethod = LIST_SELECT;

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

    UI_ELEMENT_TYPE type;
};

struct UI_elements_map {
    std::map<std::string, UI_element> elements;

    void addElement(const std::string & name, UI_ELEMENT_TYPE type) {
		if (elements.find(name) != elements.end()) {
			std::cout << "[ERROR] addElement: element " << name << " already exists!" << std::endl;
			return;
		}
		UI_element element;
		element.type = type;
		if (element.type == UI_STRING_INPUT){
			element._data.strPtr = new std::string;
		} else if (element.type == UI_ITEMS_LIST) {
			element._data.usgPtr = new ui_string_group;
		}
		elements[name] = element;
	};

    template<typename T>
	void setData(const std::string & name, T & data){
		if (elements.find(name) == elements.end()) {
        	std::cout << "[ERROR] setData: no element with name " << name << std::endl;
        	return;
		}
		if constexpr (std::is_same_v<T, bool>){
			if (elements[name].type != UI_BOOL){
				std::cout << "[ERROR] setData: element with name " << name << " is not bool!" <<std::endl;
			} else {
				elements[name]._data.b = data;
			}
		} else if constexpr (std::is_same_v<T, float>){
			if (elements[name].type != UI_FLOAT){
				std::cout << "[ERROR] setData: element with name " << name << " is not float!" <<std::endl;
			} else {
				elements[name]._data.f = data;
			}
		} else if constexpr (std::is_same_v<T, int>){
			if (elements[name].type != UI_INT){
				std::cout << "[ERROR] setData: element with name " << name << " is not int!" <<std::endl;
			} else {
				elements[name]._data.i = data;
			}
		} else if constexpr (std::is_same_v<T, unsigned int>){
			if (elements[name].type != UI_INT){
				std::cout << "[ERROR] setData: element with name " << name << " is not unsigned int!" <<std::endl;
			} else {
				elements[name]._data.ui = data;
			}
		} else {
			std::cout << "[ERROR] setData: cant set data for element " << name << std::endl;
		}
	};

    template<typename T>
	T & getData(const std::string & name) {
		if constexpr (std::is_same_v<T, bool>){
			return elements[name]._data.b;
		} else if constexpr (std::is_same_v<T, float>){
			return elements[name]._data.f;
		} else if constexpr (std::is_same_v<T, int>){
			return elements[name]._data.i;
		} else if constexpr (std::is_same_v<T, unsigned int>){
			return elements[name]._data.ui;
		}
		std::cout << "[ERROR] getData: can't return no data for element " << name << std::endl;
		T no_data;
		return no_data;
	};

    template<typename T>
	T * getDataPtr(const std::string & name){
		if constexpr (std::is_same_v<T, std::string>){
			return elements[name]._data.strPtr;
		} else if constexpr (std::is_same_v<T, ui_string_group>){
			return elements[name]._data.usgPtr;
		}
		std::cout << "[ERROR] getDataPtr: cant return NULL ptr for element " << name << std::endl;       
		return NULL;
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

#endif