// data structures for skinning
#include <map>
#include <IndieGoUI.h>
#include <Shader.h>
#include <editor_structs.h>
#include <editor_data.pb.h>
#include <fstream>

using namespace IndieGo::UI;

std::map<std::string, std::pair<TexData, region<float>>> skin_crops;

extern Manager GUI;
extern std::string winID;
extern std::vector<std::string> getPaths(
    bool single_item = false,
    bool select_folders = false,
    std::string start_folder = "None"
);
extern Shader skinningShader;

// Global skinning image draw data
// ---------------------------------------
LayoutRect skin_img_rect;
LayoutRect crop_img_rect;
// ---------------------------------------

void drawLayout(LayoutRect element);
void drawSkinImage(LayoutRect element);

void displaySkinImage() {
    UI_elements_map & UIMap = GUI.UIMaps[winID];

    // SKINNING IMAGE DRAWING
    // ------------------------------------------------------------
    skinningShader.scale = UIMap["w skin image scale"]._data.f / 1000.f;
    skin_img_rect.x = UIMap["w skin image x"]._data.f / 1000.f;
    skin_img_rect.y = UIMap["w skin image y"]._data.f / 1000.f;
    drawSkinImage(skin_img_rect);

    crop_img_rect.x = UIMap["w crop x"]._data.f / UI_FLT_VAL_SCALE;
    crop_img_rect.y = UIMap["w crop y"]._data.f / UI_FLT_VAL_SCALE;
    crop_img_rect.width = UIMap["w crop w"]._data.f / UI_FLT_VAL_SCALE;
    crop_img_rect.height = UIMap["w crop h"]._data.f / UI_FLT_VAL_SCALE;

    LayoutRect draw_rect = crop_img_rect;

    draw_rect.width = skin_img_rect.width * crop_img_rect.width;
    draw_rect.height = skin_img_rect.height * crop_img_rect.height;

    draw_rect.x = skin_img_rect.x 
        - ((skin_img_rect.width * skinningShader.scale) / 2.f) 
        + (skin_img_rect.width * skinningShader.scale) * crop_img_rect.x;

    draw_rect.x += (draw_rect.width * skinningShader.scale) / 2.f;

    draw_rect.y = skin_img_rect.y 
        + ((skin_img_rect.height * skinningShader.scale) / 2.f) 
        - (skin_img_rect.height * skinningShader.scale) * crop_img_rect.y;

    draw_rect.y -= (draw_rect.height * skinningShader.scale) / 2.f;

    draw_rect.width *= skinningShader.scale;
    draw_rect.height *= skinningShader.scale;
    draw_rect.alpha = 0.f;
    drawLayout(draw_rect);
}

void serializeCropsData(const std::string & path) {
    editor_serialization::SerializedCropsData serialized_crops;
    for (auto skin_crop : skin_crops) {
        editor_serialization::CropsData * cd = serialized_crops.add_data();

        cd->set_crop_name(skin_crop.first);

        cd->mutable_crop()->set_x(skin_crop.second.second.x);
        cd->mutable_crop()->set_y(skin_crop.second.second.y);
        cd->mutable_crop()->set_w(skin_crop.second.second.w);
        cd->mutable_crop()->set_h(skin_crop.second.second.h);
    }

    std::ofstream file(path, std::ios::binary);
    serialized_crops.SerializeToOstream(&file);
    file.close();
}

void deserializeCropsData(const std::string & path) {
    if (GUI.skinning_image == "None")
        return;
    
    TexData td = Manager::load_image(GUI.skinning_image);

    editor_serialization::SerializedCropsData serialized_crops;
    std::ifstream file(path, std::ios::binary);
    if (!serialized_crops.ParseFromIstream(&file)) {
        std::cout << "[WORLD::ERROR] Failed to parse crops from: " << path << std::endl;
        return;
    } else {
        std::cout << "[WORLD::INFO] Loading crops from: " + path << std::endl;
    }

    skin_crops.clear();
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    ui_string_group & skin_crops_list = *UIMap["w skin crops list"]._data.usgPtr;
    UIMap["w skin image path"].label = "skin image path: " + GUI.skinning_image;

    skin_crops_list.elements.clear();
    skin_crops_list.unselect();

    for (int i = 0; i < serialized_crops.data_size(); i++) {
        const editor_serialization::CropsData & cd = serialized_crops.data(i);
        region<float> crop;
        crop.x = cd.crop().x();
        crop.y = cd.crop().y();
        crop.w = cd.crop().w();
        crop.h = cd.crop().h();
        skin_crops[cd.crop_name()] = { td, crop };
        skin_crops_list.elements.push_back(cd.crop_name());
    }

}