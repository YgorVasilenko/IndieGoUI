// various functions for working with ui
#include <IndieGoUI.h>

#if !defined NO_SERIALIZATION !defined NO_UI_SERIALIZATION
#include <IndieGoUI.pb.h>
#endif

using namespace IndieGo::UI;

extern Manager GUI;

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

void useBackgroundImage(std::string widID, std::string winID, unsigned int texID) {
   /* WIDGET & w = GUI.getWidget(widID, winID);
    w.img_idx = GUI.addImage(texID);
    w.backgroundImage = true;*/
}

void addElement(
    std::string widID, 
    std::string winID, 
    std::string elt_name, 
    UI_ELEMENT_TYPE type,
    bool add_on_new_row
) {
    if (elt_name.size() == 0)
        return;

    // get widget
    WIDGET& w = GUI.getWidget( widID, winID );

    // check if such element already exists, in which case don't add it
    if (std::find(w.widget_elements.begin(), w.widget_elements.end(), elt_name) != w.widget_elements.end())
        // TODO : add error message
        return;

    // figure row to add element on
    int row = w.layout_grid.size() - 1;
    if (add_on_new_row || row == -1)
        row++;
    
    // get UIMap
    UI_elements_map & UIMap = GUI.UIMaps[winID];
    // add element to widget
    UIMap.addElement(elt_name, type, &w, row);

    // TODO : process special cases
    w.layout_grid[row].in_pixels = false;
    w.layout_grid[row].min_height = 0.25f;
}

extern std::string skinning_img_path = "";
extern unsigned int skinning_image_id = 0;
extern unsigned int si_w = 0;
extern unsigned int si_h = 0;

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

        // rows heights
        for (auto g_row : widget.second.layout_grid) {
            w->mutable_widget()->add_rows_heights(g_row.min_height);
        }

        // skinned props
        for (auto prop : widget.second.skinned_style.props) {
            if (prop.second.first != -1) {
                ui_serialization::SkinnedProperty * sp = w->mutable_widget()->add_skinned_props();
                sp->set_prop_type((unsigned int)prop.first);
                sp->mutable_img()->set_path(skinning_img_path);

                region<float> crop = widget.second.getImgCrop(prop.first);
                sp->mutable_img()->mutable_crop()->set_h(crop.h);
                sp->mutable_img()->mutable_crop()->set_w(crop.w);
                sp->mutable_img()->mutable_crop()->set_x(crop.x);
                sp->mutable_img()->mutable_crop()->set_y(crop.y);
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
            e->set_add_on_new_row(add_to_new_row);
            // skinned props
            for (auto prop : UIMap[elt_name].skinned_style.props) {
                if (prop.second.first != -1) {
                    ui_serialization::SkinnedProperty * sp = e->add_skinned_props();
                    sp->set_prop_type((unsigned int)prop.first);
                    sp->mutable_img()->set_path(skinning_img_path);

                    region<float> crop = UIMap[elt_name].getImgCrop(prop.first);
                    sp->mutable_img()->mutable_crop()->set_h(crop.h);
                    sp->mutable_img()->mutable_crop()->set_w(crop.w);
                    sp->mutable_img()->mutable_crop()->set_x(crop.x);
                    sp->mutable_img()->mutable_crop()->set_y(crop.y);
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
        for (auto size : font.second.sizes) {
            ui_serialization::Font * f = serialized_ui.add_fonts();
            f->set_name(font.second.path);
            f->set_size(size);
        }
    }

    std::ofstream file(path, std::ios::binary);
    serialized_ui.SerializeToOstream(&file);
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
            // TODO : check if image is loaded. Load, if not
            const ui_serialization::SkinnedProperty & sp = w.widget().skinned_props(j);
            region<float> crop_region;
            crop_region.h = sp.img().crop().h();
            crop_region.w = sp.img().crop().w();
            crop_region.y = sp.img().crop().y();
            crop_region.x = sp.img().crop().x();
            added_w.useSkinImage(
                skinning_image_id,
                si_w,
                si_h,
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
            addElement(
                w.widget().name(),
                winID,
                e.name(),
                (UI_ELEMENT_TYPE)e.type(),
                e.add_on_new_row()
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
                // TODO : check if image is loaded. Load, if not
                const ui_serialization::SkinnedProperty & sp = e.skinned_props(j);
                region<float> crop_region;
                crop_region.h = sp.img().crop().h();
                crop_region.w = sp.img().crop().w();
                crop_region.y = sp.img().crop().y();
                crop_region.x = sp.img().crop().x();
                UIMap[e.name()].useSkinImage(
                    skinning_image_id,
                    si_w,
                    si_h,
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
    }

    // load used fonts
    for (int i = 0; i < serialized_ui.fonts_size(); i++) {
        const ui_serialization::Font & f = serialized_ui.fonts(i);
        loadFont(f.name(), winID, f.size());
    }

#endif
}