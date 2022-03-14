#include <IndieGoUI.h>

void main() {
    std::cout << "UI_element size: " << sizeof(UI_element) << std::endl;
    std::cout << "UI_elements_map size: " << sizeof(UI_elements_map) << std::endl;
    UI_elements_map UI;
    bool bool_data = true;

    UI.addElement("test boolean", UI_BOOL);
    std::cout << "added element " << std::endl;
    UI.setData("test boolean", bool_data);
    std::cout << "setted value for element " << std::endl;
    bool_data = UI.getData<bool>("test boolean");

    std::cout << "test boolean value is: " << bool_data << std::endl; 
    bool & ref_data = UI.getData<bool>("test boolean");
    std::cout << "Set boolean value in UI_map through reference" << std::endl;
    ref_data = false;
    bool_data = UI.getData<bool>("test boolean");
    std::cout << "test boolean value is: " << bool_data << std::endl; 
}