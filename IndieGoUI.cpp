// various functions for working with ui
#include <IndieGoUI.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
namespace fs = std::filesystem;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#if !defined NO_SERIALIZATION !defined NO_UI_SERIALIZATION
#include <IndieGoUI.pb.h>
#endif



using namespace IndieGo::UI;

extern Manager GUI;

// [path] = texData
std::map<std::string, TexData> loaded_textures;

void WIDGET_BASE::updateRowHeight(unsigned int row, float newHeight) {
    if (!uiMapPtr) 
        // TODO : print warning
        return;
	layout_grid[row].min_height = newHeight;
	for (auto cell : layout_grid[row].cells) {
		for (auto elt : cell.elements) {
			(*uiMapPtr).elements[elt].height = newHeight / cell.elements.size();
		}
	}
}

void WIDGET_BASE::updateColWidth(unsigned int row, unsigned int col, float newWidth) {
    if (!uiMapPtr) 
        // TODO : print warning
        return;
        
    layout_grid[row].cells[col].min_width = newWidth;
	for (auto elt : layout_grid[row].cells[col].elements) {
		(*uiMapPtr).elements[elt].width = newWidth;
	}
}

void createNewWidget(
    std::string newWidName, 
    region<float> screen_region,
    bool bordered,
    bool titled,
    bool minimizable,
    bool scalable,
    bool movable,
    const std::string & winID
) {
    WIDGET newWidget;

    // initialize new widget
    newWidget.screen_region = screen_region;
    newWidget.name = newWidName;

    newWidget.border = bordered;
    newWidget.title = titled;
    newWidget.minimizable = minimizable;
    newWidget.scalable = scalable;
    newWidget.movable = movable;

    WIDGET & widget = GUI.addWidget(newWidget, winID);
}

// void useBackgroundImage(std::string widID, std::string winID, unsigned int texID) {
//    /* WIDGET & w = GUI.getWidget(widID, winID);
//     w.img_idx = GUI.addImage(texID);
//     w.backgroundImage = true;*/
// }

extern ELT_PUSH_OPT push_opt;
void addElement(
    std::string widID, 
    std::string winID, 
    std::string elt_name, 
    UI_ELEMENT_TYPE type
) {
    if (elt_name.size() == 0)
        return;

    // get widget
    WIDGET& w = GUI.getWidget( widID, winID );

    // check if such element already exists, in which case don't add it
    if (std::find(w.widget_elements.begin(), w.widget_elements.end(), elt_name) != w.widget_elements.end())
        // TODO : add error message
        return;
    
    // get UIMap
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    // add element to widget
    UIMap.addElement(elt_name, type, &w, push_opt);
}

// helper function lo load image through stbi
// in other engine parts ImageLoader will do that
TexData Manager::load_image(std::string path, bool useProjectDir) {
    char* pd = getenv("PROJECT_DIR");
    std::string project_dir = "";
    if (useProjectDir && pd) {
        project_dir = pd;
        if (path.size() > project_dir.size()) {
            path = path.substr(
                project_dir.size(), path.size()
            );
        }
    }

    if (loaded_textures.find(path) != loaded_textures.end()) {
        return loaded_textures[path];
    }
    // unsigned int tex;
    TexData& td = loaded_textures[path];
    unsigned char *data = stbi_load(
        useProjectDir ? (project_dir + path).c_str() : path.c_str(), 
        &td.w, 
        &td.h, 
        &td.n, 
        0
    );
    
    if (!data) {
        std::cout << "[ERROR] failed to load image " << path << std::endl;
        td.texID = UINT_MAX;
        return td;
    }

    glGenTextures(1, &td.texID);
    glBindTexture(GL_TEXTURE_2D, td.texID);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, td.w, td.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    
    return td;
}

void Manager::serialize(const std::string & winID, const std::string & path, const std::vector<std::string> & skipWidgets) {
#if !defined NO_SERIALIZATION !defined NO_UI_SERIALIZATION
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    ui_serialization::SerializedUI serialized_ui;
    for (auto widget : GUI.widgets[winID]) {
        if (std::find(skipWidgets.begin(), skipWidgets.end(), widget.first) != skipWidgets.end())
            continue;

        ui_serialization::Widget * w = serialized_ui.add_widgets();

        w->mutable_widget()->set_name(widget.first);
        
        // width, height, location on screen
        w->mutable_widget()->mutable_size_loc()->set_h(widget.second.screen_region.h);
        w->mutable_widget()->mutable_size_loc()->set_w(widget.second.screen_region.w);
        w->mutable_widget()->mutable_size_loc()->set_x(widget.second.screen_region.x);
        w->mutable_widget()->mutable_size_loc()->set_y(widget.second.screen_region.y);

        // various behavior/display flags
        w->mutable_widget()->set_bordered(widget.second.border);
        w->mutable_widget()->set_titled(widget.second.title);
        w->mutable_widget()->set_minimizable(widget.second.minimizable);
        w->mutable_widget()->set_scalable(widget.second.scalable);
        w->mutable_widget()->set_movable(widget.second.movable);

        // special props
        w->mutable_widget()->set_border_size(widget.second.border_size);
        w->mutable_widget()->mutable_padding()->set_x(widget.second.padding.w);
        w->mutable_widget()->mutable_padding()->set_y(widget.second.padding.h);
        w->mutable_widget()->mutable_spacing()->set_x(widget.second.spacing.w);
        w->mutable_widget()->mutable_spacing()->set_y(widget.second.spacing.h);

        // global widget font
        w->mutable_widget()->mutable_font()->set_name(widget.second.font);
        w->mutable_widget()->mutable_font()->set_size(widget.second.font_size);

        // layout sizes
        for (auto g_row : widget.second.layout_grid) {
            ui_serialization::LayoutRow * r = w->mutable_widget()->add_rows();
            r->set_height(g_row.min_height);
            for (auto col : g_row.cells) {
                r->add_cols_widths(col.min_width);
            }
        }

        // skinned props
        for (auto prop : widget.second.skinned_style.props) {
            if (prop.second.first != -1) {
                ui_serialization::SkinnedProperty * sp = w->mutable_widget()->add_skinned_props();
                sp->set_prop_type((unsigned int)prop.first);

                region<float> crop = widget.second.getImgCrop(prop.first);
                sp->mutable_crop()->set_h(crop.h);
                sp->mutable_crop()->set_w(crop.w);
                sp->mutable_crop()->set_x(crop.x);
                sp->mutable_crop()->set_y(crop.y);
            }
        }

        // color styled props
        for (int i = 0; i < 28; i++){
            ui_serialization::StyleColor * sc = w->mutable_widget()->add_styled_props();
            sc->set_r(widget.second.style.elements[i].r);
            sc->set_g(widget.second.style.elements[i].g);
            sc->set_b(widget.second.style.elements[i].b);
            sc->set_a(widget.second.style.elements[i].a);
        }

        // elements related to widget
        for (auto elt_name : widget.second.widget_elements) {
            ui_serialization::Element * e = w->add_elements();
            e->set_name(elt_name);
            e->set_widget_name(widget.first);
            e->set_type((unsigned int)UIMap[elt_name].type);

            e->set_border(UIMap[elt_name].border);
            e->set_rounding(UIMap[elt_name].rounding);
            e->mutable_padding()->set_x(UIMap[elt_name].padding.w);
            e->mutable_padding()->set_y(UIMap[elt_name].padding.h);

            // individual component's used font data
            e->mutable_font()->set_name(UIMap[elt_name].font);
            e->mutable_font()->set_size(UIMap[elt_name].font_size);

            e->set_label(UIMap[elt_name].label);
            e->set_text_align((unsigned int)UIMap[elt_name].text_align);

            bool add_to_new_row = false;
            for (auto elt_group : widget.second.elements_groups) {
                if (elt_group.start == elt_name) {
                    add_to_new_row = true;
                    break;
                }
            }

            e->set_elt_push_opt((unsigned int)UIMap[elt_name].push_opt);
            e->set_width(UIMap[elt_name].width);
            e->set_height(UIMap[elt_name].height);

            // skinned props
            for (auto prop : UIMap[elt_name].skinned_style.props) {
                if (prop.second.first != -1) {
                    ui_serialization::SkinnedProperty * sp = e->add_skinned_props();
                    sp->set_prop_type((unsigned int)prop.first);

                    region<float> crop = UIMap[elt_name].getImgCrop(prop.first);
                    sp->mutable_crop()->set_h(crop.h);
                    sp->mutable_crop()->set_w(crop.w);
                    sp->mutable_crop()->set_x(crop.x);
                    sp->mutable_crop()->set_y(crop.y);
                }
            }

            // color styled props
            for (int i = 0; i < 28; i++) {
                ui_serialization::StyleColor * sc = e->add_styled_props();
                sc->set_r(widget.second.style.elements[i].r);
                sc->set_g(widget.second.style.elements[i].g);
                sc->set_b(widget.second.style.elements[i].b);
                sc->set_a(widget.second.style.elements[i].a);
            }
        }
    }

    for (auto font : loaded_fonts) {
        // skip "default" font serialization
        if (fs::path(font.first).stem().string() == main_font) continue;
        for (auto size : font.second.sizes) {
            ui_serialization::Font * f = serialized_ui.add_fonts();
            f->set_name(font.second.path);
            f->set_size(size);
        }
    }

    unsigned int i = 0;
    for (auto image : loaded_textures) {
        serialized_ui.add_images(image.first);
        if (image.first == skinning_image) {
            serialized_ui.set_skinning_image_idx(i);
        }
        i++;
    }
    std::ofstream file(path, std::ios::binary);
    serialized_ui.SerializeToOstream(&file);
    file.close();
#endif
}

void Manager::deserialize(const std::string & winID, const std::string & path) {
#if !defined NO_SERIALIZATION !defined NO_UI_SERIALIZATION
    ui_serialization::SerializedUI serialized_ui;
    std::ifstream file(path, std::ios::binary);
    if (!serialized_ui.ParseFromIstream(&file)) {
        std::cout << "[WORLD::ERROR] Failed to parse ui from: " << path << std::endl;
        return;
    } else {
        std::cout << "[WORLD::INFO] Loading ui from: " + path << std::endl;
    }
    unsigned int idx = 0;
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    
    // load images first, because ui uses them
    
    for (unsigned int i = 0; i < serialized_ui.images_size(); i++) {
        Manager::load_image(serialized_ui.images(i));
        if (serialized_ui.has_skinning_image_idx() && i == serialized_ui.skinning_image_idx()) {
            skinning_image = serialized_ui.images(i);
        }
    }

    // load ui widgets
    for (int i = 0; i < serialized_ui.widgets_size(); i++) {
        const ui_serialization::Widget & w = serialized_ui.widgets(i);

        // region
        region<float> screen_region;
        screen_region.h = w.widget().size_loc().h();
        screen_region.w = w.widget().size_loc().w();
        screen_region.x = w.widget().size_loc().x();
        screen_region.y = w.widget().size_loc().y();
        
        createNewWidget(
            w.widget().name(),
            screen_region,
            w.widget().bordered(),
            w.widget().titled(),
            w.widget().minimizable(),
            w.widget().scalable(),
            w.widget().movable(),
            winID
        );

        // styling
        WIDGET & added_w = GUI.getWidget(w.widget().name(), winID);

        // font
        added_w.font = w.widget().font().name();
        added_w.font_size = w.widget().font().size();

        // special props
        added_w.border_size = w.widget().border_size();
        added_w.padding.h = w.widget().padding().y();
        added_w.padding.w = w.widget().padding().x();
        added_w.spacing.h = w.widget().spacing().y();
        added_w.spacing.w = w.widget().spacing().x();

        // skinned props
        for (int j = 0; j < w.widget().skinned_props_size(); j++) {
            const ui_serialization::SkinnedProperty & sp = w.widget().skinned_props(j);
            region<float> crop_region;
            crop_region.h = sp.crop().h();
            crop_region.w = sp.crop().w();
            crop_region.y = sp.crop().y();
            crop_region.x = sp.crop().x();
            TexData td = Manager::load_image(skinning_image);
            added_w.useSkinImage(
                td.texID,
                td.w,
                td.h,
                crop_region,
                (IMAGE_SKIN_ELEMENT)sp.prop_type()
            );
        }

        // color styled props
        for (int j = 0; j < 28; j++) {
            const ui_serialization::StyleColor & sc = w.widget().styled_props(j);
            added_w.style.elements[j].r = sc.r();
            added_w.style.elements[j].g = sc.g();
            added_w.style.elements[j].b = sc.b();
            added_w.style.elements[j].a = sc.a();
        }
        // add elements to related widget
        for (int j = 0; j < w.elements_size(); j++) {
            const ui_serialization::Element & e = w.elements(j);
            push_opt = (ELT_PUSH_OPT)e.elt_push_opt();
            addElement(
                w.widget().name(),
                winID,
                e.name(),
                (UI_ELEMENT_TYPE)e.type()
            );
            
            UIMap[e.name()].border = e.border();
            UIMap[e.name()].rounding = e.rounding();
            UIMap[e.name()].padding.h = e.padding().y();
            UIMap[e.name()].padding.w = e.padding().x();

            // individual component's font
            UIMap[e.name()].font = e.font().name();
            UIMap[e.name()].font_size = e.font().size();
            
            UIMap[e.name()].label = e.label();
            UIMap[e.name()].text_align = (TEXT_ALIGN)e.text_align();

            // skinned props
            for (int k = 0; k < e.skinned_props_size(); k++) {
                const ui_serialization::SkinnedProperty & sp = e.skinned_props(j);
                region<float> crop_region;
                crop_region.h = sp.crop().h();
                crop_region.w = sp.crop().w();
                crop_region.y = sp.crop().y();
                crop_region.x = sp.crop().x();
                TexData td = Manager::load_image(skinning_image);
                UIMap[e.name()].useSkinImage(
                    td.texID,
                    td.w,
                    td.h,
                    crop_region,
                    (IMAGE_SKIN_ELEMENT)sp.prop_type()
                );
            }
            // color styled props
            for (int k = 0; k < 28; k++) {
                const ui_serialization::StyleColor & sc = e.styled_props(k);
                UIMap[e.name()].style.elements[k].r = sc.r();
                UIMap[e.name()].style.elements[k].g = sc.g();
                UIMap[e.name()].style.elements[k].b = sc.b();
                UIMap[e.name()].style.elements[k].a = sc.a();
            }
        }

        // load layout sizes
        for (int j = 0; j < w.widget().rows_size(); j++) {
            const ui_serialization::LayoutRow & r = w.widget().rows(j);
            added_w.updateRowHeight(j, r.height());
            for (int k = 0; k < r.cols_widths_size(); k++) {
                added_w.updateColWidth(j, k, r.cols_widths(k));
            }
        }

        // walk over saved elements list again to customize sizes after automated layout update
        for (int j = 0; j < w.elements_size(); j++) {
            const ui_serialization::Element & e = w.elements(j);
            UIMap[e.name()].height = e.height();
            UIMap[e.name()].width = e.width();
        }
    }

    // load used fonts
    for (int i = 0; i < serialized_ui.fonts_size(); i++) {
        const ui_serialization::Font & f = serialized_ui.fonts(i);
        loadFont(f.name(), winID, f.size(), true, false);
    }
#endif
}