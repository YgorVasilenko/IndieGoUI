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

extern std::string getSkinPropName(IMAGE_SKIN_ELEMENT prop);

void processSkinning(int prev_selected_crop = -1) {
    UI_elements_map & UIMap = GUI.UIMaps[winID];

    if (UIMap["e load skin image"]._data.b || UIMap["w load skin image"]._data.b) {
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


    // SKINNING IMAGE DRAWING
    // ------------------------------------------------------------
    if (UIMap["w skin image scale"]._data.f > 10000.f)
        UIMap["w skin image scale"]._data.f = 10000.f;

    if (UIMap["w skin image scale"]._data.f < 0.f)
        UIMap["w skin image scale"]._data.f = 0.f;

    if (UIMap["w skin image x"]._data.f > 1000.f)
        UIMap["w skin image x"]._data.f = 1000.f;

    if (UIMap["w skin image x"]._data.f < -1000.f)
        UIMap["w skin image x"]._data.f = -1000.f;

    if (UIMap["w skin image y"]._data.f > 1000.f)
        UIMap["w skin image y"]._data.f = 1000.f;

    if (UIMap["w skin image y"]._data.f < -1000.f)
        UIMap["w skin image y"]._data.f = -1000.f;

    ui_string_group & skin_crops_list = *UIMap["w skin crops list"]._data.usgPtr;
    ui_string_group & widgets_list = *UIMap["widgets list"]._data.usgPtr;
    ui_string_group & w_skinning_props_list = *UIMap["w skinning properties"]._data.usgPtr;
    ui_string_group & elements_list = *UIMap["elements list"]._data.usgPtr;

    if (skin_crops_list.selected_element != prev_selected_crop) {
        // update displayed crop to newly selected crop
        UIMap["w crop x"]._data.f = skin_crops[skin_crops_list.getSelected()].second.x * UI_FLT_VAL_SCALE;
        UIMap["w crop y"]._data.f = skin_crops[skin_crops_list.getSelected()].second.y * UI_FLT_VAL_SCALE;
        UIMap["w crop w"]._data.f = skin_crops[skin_crops_list.getSelected()].second.w * UI_FLT_VAL_SCALE;
        UIMap["w crop h"]._data.f = skin_crops[skin_crops_list.getSelected()].second.h * UI_FLT_VAL_SCALE;
    }

    if (UIMap["w display skin image"]._data.b) {
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
    // ------------------------------------------------------------

    if (w_skinning_props_list.selected_element != -1)
        UIMap["w selected prop"].label = "selected property: " + getSkinPropName((IMAGE_SKIN_ELEMENT)w_skinning_props_list.selected_element);

    if (widgets_list.selected_element != -1)
        UIMap["w selected widget"].label = "selected widget: " + widgets_list.getSelected();

    if (UIMap["w new crop"]._data.b) {
        std::string new_crop_name = *UIMap["w new crop name"]._data.strPtr;
        if (new_crop_name.size() > 0 && skin_crops.find(new_crop_name) == skin_crops.end()) {
            region<float> crop;
            crop.x = UIMap["w crop x"]._data.f / UI_FLT_VAL_SCALE;
            crop.y = UIMap["w crop y"]._data.f / UI_FLT_VAL_SCALE;
            crop.w = UIMap["w crop w"]._data.f / UI_FLT_VAL_SCALE;
            crop.h = UIMap["w crop h"]._data.f / UI_FLT_VAL_SCALE;

            TexData td = Manager::load_image(UIMap["w skin image path"].label, true);
            skin_crops[new_crop_name] = { td, crop };
            skin_crops_list.elements.push_back(new_crop_name);
        }
    }

    if (
        skin_crops_list.selected_element != -1 
        && widgets_list.selected_element != -1 
        && w_skinning_props_list.selected_element != -1
        && UIMap["w apply crop"]._data.b
    ) {
        WIDGET & w = GUI.getWidget(widgets_list.getSelected(), winID);
        TexData td =  skin_crops[skin_crops_list.getSelected()].first;
        region<float> crop = skin_crops[skin_crops_list.getSelected()].second;
        td.texID = skinningShader.skin_tex_id;
        td.w = GUI.skin_img_size.w;
        td.h = GUI.skin_img_size.h;
        w.useSkinImage(
            td.texID,
            td.w,
            td.h,
            crop,
            (IMAGE_SKIN_ELEMENT)w_skinning_props_list.selected_element
        );
    }

    if (
        elements_list.selected_element != -1
        && skin_crops_list.selected_element != -1 
        && UIMap[elements_list.getSelected()].type == UI_BUTTON
        && UIMap["init button img"]._data.b
    ) {
        TexData td =  skin_crops[skin_crops_list.getSelected()].first;
        region<float> crop = skin_crops[skin_crops_list.getSelected()].second;
        UIMap[elements_list.getSelected()].initImage(
            td.texID,
            td.w,
            td.h,
            crop
        );
    }

    if (elements_list.selected_element != -1
        && skin_crops_list.selected_element != -1 
        && UIMap[elements_list.getSelected()].type == UI_IMAGE
        && UIMap["crop to img"]._data.b
    ) {
        TexData td =  skin_crops[skin_crops_list.getSelected()].first;
        region<float> crop = skin_crops[skin_crops_list.getSelected()].second;
        UIMap[elements_list.getSelected()].initImage(
            td.texID,
            td.w,
            td.h,
            crop
        );
    }
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