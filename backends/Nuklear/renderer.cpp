#define NK_IMPLEMENTATION
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_INCLUDE_STANDARD_IO

#include "backends/Nuklear/nuklear.h"
#include "backends/Nuklear/nuklear_glfw_gl3.h"

#include <IndieGoUI.h>
#include <string>
#include <regex>

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

std::map<std::string, struct nk_font *> loaded_fonts;

struct nk_glfw * glfw = NULL;

// TODO - struct for several atlases loading
struct nk_font_atlas * atlas = NULL;

struct nk_vec2 cursor_pos;

nk_context* ctx = NULL;

NK_API bool disableMouse = false;

NK_API void nk_glfw3_render(struct nk_glfw* glfw, enum nk_anti_aliasing AA, int max_vertex_buffer, int max_element_buffer) {

    struct nk_glfw_device* dev = &glfw->ogl;
    struct nk_buffer vbuf, ebuf;
    GLfloat ortho[4][4] = {
        {2.0f, 0.0f, 0.0f, 0.0f},
        {0.0f,-2.0f, 0.0f, 0.0f},
        {0.0f, 0.0f,-1.0f, 0.0f},
        {-1.0f,1.0f, 0.0f, 1.0f},
    };
    ortho[0][0] /= (GLfloat)glfw->width;
    ortho[1][1] /= (GLfloat)glfw->height;

    /* setup global state */
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* setup program */
    glUseProgram(dev->prog);
    glUniform1i(dev->uniform_tex, 0);
    glUniformMatrix4fv(dev->uniform_proj, 1, GL_FALSE, &ortho[0][0]);
    glViewport(0, 0, (GLsizei)glfw->display_width, (GLsizei)glfw->display_height);
    {
        /* convert from command queue into draw list and draw to screen */
        const struct nk_draw_command* cmd;
        void* vertices, * elements;
        const nk_draw_index* offset = NULL;

        /* allocate vertex and element buffer */
        glBindVertexArray(dev->vao);
        glBindBuffer(GL_ARRAY_BUFFER, dev->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

        glBufferData(GL_ARRAY_BUFFER, max_vertex_buffer, NULL, GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_element_buffer, NULL, GL_STREAM_DRAW);

        /* load draw vertices & elements directly into vertex + element buffer */
        vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        elements = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
        {
            /* fill convert configuration */
            struct nk_convert_config config;
            static const struct nk_draw_vertex_layout_element vertex_layout[] = {
                {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_glfw_vertex, position)},
                {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_glfw_vertex, uv)},
                {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct nk_glfw_vertex, col)},
                {NK_VERTEX_LAYOUT_END}
            };
            NK_MEMSET(&config, 0, sizeof(config));
            config.vertex_layout = vertex_layout;
            config.vertex_size = sizeof(struct nk_glfw_vertex);
            config.vertex_alignment = NK_ALIGNOF(struct nk_glfw_vertex);
            config.null = dev->null;
            config.circle_segment_count = 22;
            config.curve_segment_count = 22;
            config.arc_segment_count = 22;
            config.global_alpha = 1.0f;
            config.shape_AA = AA;
            config.line_AA = AA;

            /* setup buffers to load vertices and elements */
            nk_buffer_init_fixed(&vbuf, vertices, (size_t)max_vertex_buffer);
            nk_buffer_init_fixed(&ebuf, elements, (size_t)max_element_buffer);
            nk_convert(&glfw->ctx, &dev->cmds, &vbuf, &ebuf, &config);
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

        /* iterate over and execute each draw command */
        nk_draw_foreach(cmd, &glfw->ctx, &dev->cmds)
        {
            if (!cmd->elem_count) continue;
            glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);
            glScissor(
                (GLint)(cmd->clip_rect.x * glfw->fb_scale.x),
                (GLint)((glfw->height - (GLint)(cmd->clip_rect.y + cmd->clip_rect.h)) * glfw->fb_scale.y),
                (GLint)(cmd->clip_rect.w * glfw->fb_scale.x),
                (GLint)(cmd->clip_rect.h * glfw->fb_scale.y));
            glDrawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count, GL_UNSIGNED_SHORT, offset);
            offset += cmd->elem_count;
        }
        nk_clear(&glfw->ctx);
        nk_buffer_clear(&dev->cmds);
    }

    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);
}

NK_API void nk_glfw3_device_create(struct nk_glfw* glfw) {
    GLint status;
    static const GLchar* vertex_shader =
        NK_SHADER_VERSION
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 TexCoord;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main() {\n"
        "   Frag_UV = TexCoord;\n"
        "   Frag_Color = Color;\n"
        "   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
        "}\n";
    static const GLchar* fragment_shader =
        NK_SHADER_VERSION
        "precision mediump float;\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main(){\n"
        "   Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
        "}\n";

    struct nk_glfw_device* dev = &glfw->ogl;
    nk_buffer_init_default(&dev->cmds);
    dev->prog = glCreateProgram();
    dev->vert_shdr = glCreateShader(GL_VERTEX_SHADER);
    dev->frag_shdr = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(dev->vert_shdr, 1, &vertex_shader, 0);
    glShaderSource(dev->frag_shdr, 1, &fragment_shader, 0);
    glCompileShader(dev->vert_shdr);
    glCompileShader(dev->frag_shdr);
    glGetShaderiv(dev->vert_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glGetShaderiv(dev->frag_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glAttachShader(dev->prog, dev->vert_shdr);
    glAttachShader(dev->prog, dev->frag_shdr);
    glLinkProgram(dev->prog);
    glGetProgramiv(dev->prog, GL_LINK_STATUS, &status);
    assert(status == GL_TRUE);

    dev->uniform_tex = glGetUniformLocation(dev->prog, "Texture");
    dev->uniform_proj = glGetUniformLocation(dev->prog, "ProjMtx");
    dev->attrib_pos = glGetAttribLocation(dev->prog, "Position");
    dev->attrib_uv = glGetAttribLocation(dev->prog, "TexCoord");
    dev->attrib_col = glGetAttribLocation(dev->prog, "Color");

    {
        /* buffer setup */
        GLsizei vs = sizeof(struct nk_glfw_vertex);
        size_t vp = offsetof(struct nk_glfw_vertex, position);
        size_t vt = offsetof(struct nk_glfw_vertex, uv);
        size_t vc = offsetof(struct nk_glfw_vertex, col);

        glGenBuffers(1, &dev->vbo);
        glGenBuffers(1, &dev->ebo);
        glGenVertexArrays(1, &dev->vao);

        glBindVertexArray(dev->vao);
        glBindBuffer(GL_ARRAY_BUFFER, dev->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

        glEnableVertexAttribArray((GLuint)dev->attrib_pos);
        glEnableVertexAttribArray((GLuint)dev->attrib_uv);
        glEnableVertexAttribArray((GLuint)dev->attrib_col);

        glVertexAttribPointer((GLuint)dev->attrib_pos, 2, GL_FLOAT, GL_FALSE, vs, (void*)vp);
        glVertexAttribPointer((GLuint)dev->attrib_uv, 2, GL_FLOAT, GL_FALSE, vs, (void*)vt);
        glVertexAttribPointer((GLuint)dev->attrib_col, 4, GL_UNSIGNED_BYTE, GL_TRUE, vs, (void*)vc);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void prepareUIRenderer(GLFWwindow* window) {
	
	glfw = new nk_glfw();

	glfwSetWindowUserPointer(window, glfw);
	glfw->win = window;
	nk_init_default(&glfw->ctx, 0);

	glfw->ctx.clip.copy = nk_glfw3_clipboard_copy;
	glfw->ctx.clip.paste = nk_glfw3_clipboard_paste;
	glfw->ctx.clip.userdata = nk_handle_ptr(0);
	glfw->last_button_click = 0;

    // get context's address
    ctx = &glfw->ctx;

    nk_glfw3_device_create(glfw);

	glfw->is_double_click_down = nk_false;
	glfw->double_click_pos = nk_vec2(0, 0);

    nk_glfw3_font_stash_begin(glfw, &atlas);
    nk_glfw3_font_stash_end(glfw);
}

using namespace IndieGo::UI;

void IndieGoUI::scroll(double xoff, double yoff) {
    (void)xoff;
    glfw->scroll.x += (float)xoff;
    glfw->scroll.y += (float)yoff;
}

void IndieGoUI::mouse_move(double x, double y){
    cursor_pos.x = x;
    cursor_pos.y = y;
}

void IndieGoUI::mouse_button(int button, int action, int mods){
    if (disableMouse) return;
    double x, y;
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;
    if (action == GLFW_PRESS) {
        double dt = glfwGetTime() - glfw->last_button_click;
        if (dt > NK_GLFW_DOUBLE_CLICK_LO && dt < NK_GLFW_DOUBLE_CLICK_HI) {
            glfw->is_double_click_down = nk_true;
            glfw->double_click_pos = cursor_pos;
        }
        glfw->last_button_click = glfwGetTime();
    } else glfw->is_double_click_down = nk_false;
}

// unsigned int last_codepoint = 0;

void IndieGoUI::char_input(unsigned int codepoint){
    // if (last_codepoint != 0 && codepoint == last_codepoint)
    //     // guard against "press add" in each frame
    //     return;
    
    if (glfw->text_len < NK_GLFW_TEXT_MAX) {
        glfw->text[glfw->text_len++] = codepoint;
        // last_codepoint = codepoint;
    }
}

void IndieGoUI::key_input(unsigned int codepoint, bool pressed){
    if (codepoint == GLFW_KEY_BACKSPACE && pressed) {
        if (!glfw->backspace) {
            glfw->backspace = true;
        }
    }
}

// Memory for string input storage
char text[64] = {};
int text_len;

void stringToText(std::string & str) {
    for (int i = 0; i < str.length(); i++)
        text[i] = str[i];
    text_len = str.length();
}

void textToString(std::string & str) {
    str.clear();
    if (text_len > 0) {
        for (int i = 0; i < text_len; i++) {
            str.push_back(text[i]);
        }
    }
}

//--------------------------------------------------------
//
//            Core concept. 
// callUIfunction abstracts away different UI elements.
//
//-------------------------------------------------------

void UI_element::callUIfunction() {
    
    std::string full_name; 
    nk_bool nk_val;
    static const float ratio[] = { 100, 120 };
    float dbgVal;
    if (type == UI_BOOL) {
        struct nk_key_selector ks;
        if (hovered_by_keys) {
            ks.focused = true;
        }
        nk_val = _data.b;
        nk_checkbox_label(ctx, label.c_str(), &nk_val, &ks);
        _data.b = nk_val;
        return;
    }

    if (type == UI_FLOAT) {
        full_name = "#" + label + ":";
        nk_property_float(ctx, full_name.c_str(), -300000.0f, &_data.f, 300000.0f, 1, 0.5f);
        return;
    }

    if (type == UI_INT) {
        full_name = "#" + label + ":";
        nk_property_int(ctx, full_name.c_str(), -1024, &_data.i, 1024, 1, 0.5f);
        return;
    }

    if (type == UI_UINT) {
        full_name = "#" + label + ":";
        nk_property_int(ctx, full_name.c_str(), 0, &_data.i, 2040, 1, 0.5f);
        return;
    }

    if (type == UI_STRING_INPUT) {
        std::string& stringRef = *_data.strPtr;
        stringToText(stringRef);
        nk_edit_string(ctx, NK_EDIT_SIMPLE, text, &text_len, 64, nk_filter_default);
        textToString(stringRef);
        return;
    }

    if (type == UI_BUTTON) {
        struct nk_key_selector ks;
        if (hovered_by_keys) {
            ks.focused = true;
        }
        if (nk_button_label(ctx, label.c_str(), &ks))
            _data.b = true;
        else
            _data.b = false;

        if (selected_by_keys)
            _data.b = true;
        return;
    }

    if (type == UI_BUTTON_SWITCH) {
        if (_data.b) {
                struct nk_style_button button;
                button = ctx->style.button;
                ctx->style.button.normal = nk_style_item_color(nk_rgb(40, 40, 40));
                ctx->style.button.hover = nk_style_item_color(nk_rgb(40, 40, 40));
                ctx->style.button.active = nk_style_item_color(nk_rgb(40, 40, 40));
                ctx->style.button.border_color = nk_rgb(60, 60, 60);
                ctx->style.button.text_background = nk_rgb(60, 60, 60);
                ctx->style.button.text_normal = nk_rgb(60, 60, 60);
                ctx->style.button.text_hover = nk_rgb(60, 60, 60);
                ctx->style.button.text_active = nk_rgb(60, 60, 60);
                nk_button_label(ctx, label.c_str());
                ctx->style.button = button;
            } else if (nk_button_label(ctx, label.c_str()))
                _data.b = true;
            else
            _data.b = false;
        return;
    }


    if (type == UI_ITEMS_LIST) {
        ui_string_group& uiGroupRef = *_data.usgPtr;
        uiGroupRef.selection_switch = false;
        if (uiGroupRef.selectMethod == RADIO_SELECT) {
            nk_layout_row_dynamic(ctx, 25, 1);
            for (int i = 0; i < uiGroupRef.elements.size(); i++) {
                if (nk_option_label(ctx, uiGroupRef.elements[i].c_str(), i == uiGroupRef.selected_element))  {
                    if ( uiGroupRef.selected_element != i )
                        uiGroupRef.selection_switch = true;

                    uiGroupRef.selected_element = i;
                }
            }
        } else if (uiGroupRef.selectMethod == LIST_SELECT) {
            std::regex e(uiGroupRef.regrex_filter);
            if (nk_group_begin(ctx, label.c_str(), NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
                int selected = 0;
                for (int i = 0; i < uiGroupRef.elements.size(); i++) {
                    if (uiGroupRef.elements[i] == "") continue;
                    if (i == uiGroupRef.selected_element)
                        selected = 1;
                    else
                        selected = 0;
                    if (uiGroupRef.regrex_filter != "") {
                        if (!std::regex_match(uiGroupRef.elements[i], e))
                            continue;
                    }
                    // nk_layout_row_static(ctx, 18, 285, 1);
                    nk_layout_row_dynamic(ctx, 20, 1);
                    nk_selectable_label(ctx, uiGroupRef.elements[i].c_str(), NK_TEXT_CENTERED, &selected);
                    if (selected) {
                        if ( uiGroupRef.selected_element != i )
                            uiGroupRef.selection_switch = true;
                        uiGroupRef.selected_element = i;
                    }
                }
                nk_group_end(ctx);
            }
        } else if (uiGroupRef.selectMethod == BUTTON_SELECT) {
            bool useLabels = uiGroupRef.elements.size() == uiGroupRef.element_labels.size();
            int i = 0;
            struct nk_key_selector ks;
            ks.focused = true;
            for (auto it = uiGroupRef.elements.begin(); it != uiGroupRef.elements.end(); it++) {
                if (useLabels) {
                    nk_layout_row_dynamic(ctx, 25, 2);
                    nk_label(ctx, uiGroupRef.element_labels[i].c_str(), NK_TEXT_LEFT);
                } else {
                    nk_layout_row_dynamic(ctx, 25, 1);
                }
                if (i == uiGroupRef.selected_element) {
                    // display code for selected button
                    struct nk_style_button button;
                    button = ctx->style.button;
                    ctx->style.button.normal = nk_style_item_color(nk_rgb(40, 40, 40));
                    ctx->style.button.hover = nk_style_item_color(nk_rgb(40, 40, 40));
                    ctx->style.button.active = nk_style_item_color(nk_rgb(40, 40, 40));
                    ctx->style.button.border_color = nk_rgb(60, 60, 60);
                    ctx->style.button.text_background = nk_rgb(60, 60, 60);
                    ctx->style.button.text_normal = nk_rgb(60, 60, 60);
                    ctx->style.button.text_hover = nk_rgb(60, 60, 60);
                    ctx->style.button.text_active = nk_rgb(60, 60, 60);
                    if (uiGroupRef.use_selected_string) {
                        nk_button_label(ctx, uiGroupRef.selectedString.c_str());
                    } else {
                        nk_button_label(ctx, it->c_str(), uiGroupRef.focused_by_key_element == i ? &ks : 0);
                    }
                    ctx->style.button = button;
                } else if (nk_button_label(ctx, it->c_str(), uiGroupRef.focused_by_key_element == i ? &ks : 0 )) {
                    if ( uiGroupRef.selected_element != i )
                        uiGroupRef.selection_switch = true;
                    uiGroupRef.selected_element = i;
                }
                i++;
            }
        }
    }
}

//--------------------------------------------------------
//
//            Widget display function. May use 
//            different Immadiate-Mode Ui libs
//
//-------------------------------------------------------
void WIDGET::callImmediateBackend(UI_elements_map & UIMap){
    nk_flags flags = 0;
    if (border)
        flags = flags | NK_WINDOW_BORDER;

    if (minimizable)
        flags = flags | NK_WINDOW_MINIMIZABLE;

    if (title)
        flags = flags | NK_WINDOW_TITLE;

    if (movable)
        flags = flags | NK_WINDOW_MOVABLE;
    
    if (scalable)
        flags = flags | NK_WINDOW_SCALABLE;

    // TODO - add style customization
    nk_style_default(ctx);
    if (
        nk_begin(
            ctx, 
            name.c_str(), 
            nk_rect(
                (float)screen_region.x, 
                (float)screen_region.y,
                (float)screen_region.w,
                (float)screen_region.h
            ),
            flags
        )
    ) {
        callWidgetUI(UIMap);
    }
    nk_end(ctx);
}

void WIDGET::allocateRow(unsigned int cols, float min_height){
    nk_layout_row_dynamic(ctx, min_height, cols);
}

void WIDGET::allocateEmptySpace(unsigned int fill_count){
    nk_spacing(ctx, fill_count);
}

void IndieGoUI::drawFrameStart(){
    nk_glfw3_new_frame(glfw);
}

void IndieGoUI::drawFrameEnd(){
    nk_glfw3_render(glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

void IndieGoUI::init(void * initData){
    prepareUIRenderer((GLFWwindow*)initData);
}
