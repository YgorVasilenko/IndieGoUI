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

#define NK_IMPLEMENTATION
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_STANDARD_IO

#include "backends/Nuklear/nuklear.h"

#include <os/glad/glad.h>
#include <os/GLFW/glfw3.h>

#include <IndieGoUI.h>
#include <string>
#include <regex>
#include <filesystem>
namespace fs = std::filesystem;

enum nk_glfw_init_state{
    NK_GLFW3_DEFAULT=0,
    NK_GLFW3_INSTALL_CALLBACKS
};

#ifndef NK_GLFW_TEXT_MAX
#define NK_GLFW_TEXT_MAX 256
#endif

struct nk_glfw_device {
    struct nk_buffer cmds;
    struct nk_draw_null_texture null;
    GLuint vbo, vao, ebo;
    GLuint prog;
    GLuint vert_shdr;
    GLuint frag_shdr;
    GLint attrib_pos;
    GLint attrib_uv;
    GLint attrib_col;
    GLint uniform_tex;
    GLint uniform_proj;
    GLuint font_tex;
};

struct nk_glfw {
    GLFWwindow * win;
    int width, height;
    int display_width, display_height;
    struct nk_glfw_device ogl;
    struct nk_context ctx;
    // struct nk_font_atlas atlas;
    struct nk_vec2 fb_scale;
    unsigned int text[NK_GLFW_TEXT_MAX];
    int text_len;
    bool backspace;
    bool arrow_left;
    bool arrow_right;
    struct nk_vec2 scroll;
    double last_button_click;
    int is_double_click_down;
    struct nk_vec2 double_click_pos;
};

struct nk_glfw_vertex {
    float position[2];
    float uv[2];
    nk_byte col[4];
    float idx;
};

#include <backends/Nuklear/shader_codes.h>
// #ifdef __APPLE__
//   #define NK_SHADER_VERSION "#version 150\n"
// #else
//   #define NK_SHADER_VERSION "#version 300 es\n"
// #endif

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

// font_name = font_size : <fontPtr>
std::map<std::string, std::map<float, struct nk_font *>> backend_loaded_fonts;

// texture_iid = vector<sub_images>
std::map<unsigned int, std::vector<std::pair<struct nk_image, IndieGo::UI::region<float>>>> images;

// Use winID to store window's context. If window destroyed and re-initializes
// context should be same for respective winID
std::map<std::string, struct nk_glfw*> glfw_storage;

std::map<std::string, struct nk_font> fonts;

// TODO - struct for several atlases loading
struct nk_font_atlas atlas;

struct nk_vec2 cursor_pos;

nk_context* ctx = NULL;

NK_API bool disableMouse = false;

void nk_glfw3_device_upload_atlas(struct nk_glfw* glfw, const void *image, int width, int height) {
    struct nk_glfw_device *dev = &glfw->ogl;
    glDeleteTextures(1, &dev->font_tex);
    glGenTextures(1, &dev->font_tex);
    glBindTexture(GL_TEXTURE_2D, dev->font_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, image);
}

void nk_glfw3_clipboard_paste(nk_handle usr, struct nk_text_edit *edit) {
    struct nk_glfw* glfw = (nk_glfw*) usr.ptr;
    const char *text = glfwGetClipboardString(glfw->win);
    if (text) nk_textedit_paste(edit, text, nk_strlen(text));
    (void)usr;
}

void nk_glfw3_clipboard_copy(nk_handle usr, const char *text, int len) {
    char *str = 0;
    if (!len) return;
    str = (char*)malloc((size_t)len+1);
    if (!str) return;
    memcpy(str, text, (size_t)len);
    str[len] = '\0';
    struct nk_glfw* glfw = (nk_glfw*)usr.ptr;
    glfwSetClipboardString(glfw->win, str);
    free(str);
}

NK_API void nk_glfw3_device_create(struct nk_glfw* glfw) {
    GLint status;

    shader_codes shaders;
    struct nk_glfw_device* dev = &glfw->ogl;
    nk_buffer_init_default(&dev->cmds);
    dev->prog = glCreateProgram();
    dev->vert_shdr = glCreateShader(GL_VERTEX_SHADER);
    dev->frag_shdr = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(dev->vert_shdr, 1, &shaders.vertex_shader, 0);
    glShaderSource(dev->frag_shdr, 1, &shaders.fragment_shader, 0);
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
    GLint attrib_idx = glGetAttribLocation(dev->prog, "Index");

    {
        /* buffer setup */
        GLsizei vs = sizeof(struct nk_glfw_vertex);
        size_t vp = offsetof(struct nk_glfw_vertex, position);
        size_t vt = offsetof(struct nk_glfw_vertex, uv);
        size_t vc = offsetof(struct nk_glfw_vertex, col);
        size_t vi = offsetof(struct nk_glfw_vertex, idx);

        glGenBuffers(1, &dev->vbo);
        glGenBuffers(1, &dev->ebo);
        glGenVertexArrays(1, &dev->vao);

        glBindVertexArray(dev->vao);
        glBindBuffer(GL_ARRAY_BUFFER, dev->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

        glEnableVertexAttribArray((GLuint)dev->attrib_pos);
        glEnableVertexAttribArray((GLuint)dev->attrib_uv);
        glEnableVertexAttribArray((GLuint)dev->attrib_col);
        glEnableVertexAttribArray((GLuint)attrib_idx);

        glVertexAttribPointer((GLuint)dev->attrib_pos, 2, GL_FLOAT, GL_FALSE, vs, (void*)vp);
        glVertexAttribPointer((GLuint)dev->attrib_uv, 2, GL_FLOAT, GL_FALSE, vs, (void*)vt);
        glVertexAttribPointer((GLuint)dev->attrib_col, 4, GL_UNSIGNED_BYTE, GL_TRUE, vs, (void*)vc);
        glVertexAttribPointer((GLuint)attrib_idx, 1, GL_FLOAT, GL_FALSE, vs, (void*)vi);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Leagcy functions for loading default font
// ----------------------------------------------------------
void nk_glfw3_font_stash_begin(struct nk_glfw* glfw) {
    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);
    // *atlas = &glfw->atlas;
}

void nk_glfw3_font_stash_end(struct nk_glfw* glfw) {
    const void* image; int w, h;
    image = nk_font_atlas_bake(&atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
    nk_glfw3_device_upload_atlas(glfw, image, w, h);
    nk_font_atlas_end(&atlas, nk_handle_id((int)glfw->ogl.font_tex), &glfw->ogl.null);
    if (&atlas.default_font)
        nk_style_set_font(&glfw->ctx, &atlas.default_font->handle);
}
// ----------------------------------------------------------
using namespace IndieGo::UI;
void (*Manager::custom_ui_uniforms)(void*) = 0;
void * Manager::uniforms_data_ptr = NULL;
int Manager::draw_idx = 0;

float apply_indices[50] = { 
    9, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
int last_apply_idx = -1;

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

    // set apply_indices array
    for (int i = 0; i < 50; i++) {
        std::string name = "apply_indices[" + std::to_string(i) + "]";
        glUniform1f(glGetUniformLocation(dev->prog, name.c_str()), apply_indices[i]);
    }

    if (Manager::custom_ui_uniforms) {
        Manager::custom_ui_uniforms(
            Manager::uniforms_data_ptr
        );
    }

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
        unsigned int times_call = 0;
        nk_draw_foreach(cmd, &glfw->ctx, &dev->cmds)
        {
            if (!cmd->elem_count) continue;
            times_call++;
            glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);
            glScissor(
                (GLint)(cmd->clip_rect.x * glfw->fb_scale.x),
                (GLint)((glfw->height - (GLint)(cmd->clip_rect.y + cmd->clip_rect.h)) * glfw->fb_scale.y),
                (GLint)(cmd->clip_rect.w * glfw->fb_scale.x),
                (GLint)(cmd->clip_rect.h * glfw->fb_scale.y));
            // TODO : insert additional calls for effects here?
            glDrawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count, GL_UNSIGNED_SHORT, offset);
            offset += cmd->elem_count;
        }
        nk_clear(&glfw->ctx);
        nk_buffer_clear(&dev->cmds);
    }

    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);
}

void prepareUIRenderer(GLFWwindow* window, std::string & winID) {
    if (glfw_storage.find(winID) != glfw_storage.end()) {
        // get rid of old context data before creating new window
        nk_free(&glfw_storage[winID]->ctx);
        delete glfw_storage[winID];
    }

    glfw_storage[winID] = new nk_glfw();
    
    struct nk_glfw * glfw = glfw_storage[winID];

	glfwSetWindowUserPointer(window, glfw);
	glfw->win = window;

	nk_init_default(&glfw->ctx, 0);

	glfw->ctx.clip.copy = nk_glfw3_clipboard_copy;
	glfw->ctx.clip.paste = nk_glfw3_clipboard_paste;
	glfw->ctx.clip.userdata = nk_handle_ptr(0);
	glfw->last_button_click = 0;

    nk_glfw3_device_create(glfw);

	glfw->is_double_click_down = nk_false;
	glfw->double_click_pos = nk_vec2(0, 0);

    nk_glfw3_font_stash_begin(glfw);
    nk_glfw3_font_stash_end(glfw);
}


using namespace IndieGo::UI;
void (*Manager::buttonClickCallback)(void*) = NULL;

void Manager::scroll(void * window, double xoff, double yoff) {
    std::string glfw_win = *reinterpret_cast<std::string*>(window);
    struct nk_glfw* glfw = glfw_storage[glfw_win];
    if (!guiHasCursor(glfw_win)) {
        glfw->scroll.x += 0.f;
        glfw->scroll.y += 0.f;
        return;
    }
    
    (void)xoff;
    glfw->scroll.x += (float)xoff;
    glfw->scroll.y += (float)yoff;
}

void Manager::mouse_move(void * window, double x, double y) {

    cursor_pos.x = x;
    cursor_pos.y = y;
}

#ifndef NK_GLFW_DOUBLE_CLICK_LO
#define NK_GLFW_DOUBLE_CLICK_LO 0.02
#endif
#ifndef NK_GLFW_DOUBLE_CLICK_HI
#define NK_GLFW_DOUBLE_CLICK_HI 0.2
#endif


void Manager::mouse_button(void * window, int button, int action, int mods){
    std::string glfw_win = *reinterpret_cast<std::string*>(window);
    struct nk_glfw * glfw = glfw_storage[glfw_win];

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

void Manager::char_input(void * window, unsigned int codepoint) {
    std::string glfw_win = *reinterpret_cast<std::string*>(window);
    struct nk_glfw * glfw = glfw_storage[glfw_win];

    if (glfw->text_len < NK_GLFW_TEXT_MAX) {
        glfw->text[glfw->text_len++] = codepoint;
    }
}

void Manager::key_input(void * window, unsigned int codepoint, bool pressed) {
    std::string glfw_win = *reinterpret_cast<std::string*>(window);
    struct nk_glfw * glfw = glfw_storage[glfw_win];

    if (codepoint == GLFW_KEY_BACKSPACE && pressed) {
        if (!glfw->backspace) {
            glfw->backspace = true;
        }
    }

    if (codepoint == GLFW_KEY_RIGHT && pressed) {
        if (!glfw->arrow_right) {
            glfw->arrow_right = true;
        }
    }

    if (codepoint == GLFW_KEY_LEFT && pressed) {
        if (!glfw->arrow_left) {
            glfw->arrow_left = true;
        }
    }
}

// Memory for string input storage
char text[512] = {};
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

std::map<std::string, int> debug_array;
void UI_element::callUIfunction(float x, float y, float space_w, float space_h) {
    // debug_array[label] = Manager::draw_idx;
    /*ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
    debug_array[label] = Manager::draw_idx;*/
    // Manager::draw_idx++;
    if (apply_custom_shader) {
        last_apply_idx++;
        apply_indices[last_apply_idx] = Manager::draw_idx;
    }

    if (font != "None") {
        nk_style_set_font(
            ctx,
            &backend_loaded_fonts[font][font_size]->handle
        );
    }

    // prepare space for element
    nk_layout_space_push(
        ctx, 
        nk_rect(
            x + space_w * width * padding.w, // left border
            y + space_h * height * padding.h,
            space_w * width - (space_w * width * padding.w * 2.f), // right border
            space_h * height - (space_h * height * padding.h * 2.f)
        )
    );

    std::string full_name;
    nk_bool nk_val;
    static const float ratio[] = { 100, 120 };
    float dbgVal;
    if (type == UI_BOOL) {
        ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        debug_array[label] = Manager::draw_idx;
        Manager::draw_idx++;
        // TODO : add skinning
        /*struct nk_key_selector ks;
        if (hovered_by_keys) {
            ks.focused = true;
        }*/
        nk_val = _data.b;
        //nk_checkbox_label(ctx, label.c_str(), &nk_val, &ks);
        nk_checkbox_label(ctx, label.c_str(), &nk_val);
        _data.b = nk_val;
        // return;
    }

    if (type == UI_FLOAT) {
        ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        debug_array[label] = Manager::draw_idx;
        Manager::draw_idx++;
        // TODO : add skinning
        full_name = "#" + label + ":";
        nk_property_float(ctx, full_name.c_str(), -300000.0f, &_data.f, 300000.0f, 1, flt_px_incr);
        // return;
    }

    if (type == UI_INT) {
        ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        debug_array[label] = Manager::draw_idx;
        Manager::draw_idx++;
        // TODO : add skinning
        full_name = "#" + label + ":";
        nk_property_int(ctx, full_name.c_str(), -1024, &_data.i, 1024, 1, 0.5f);
        // return;
    }

    if (type == UI_UINT) {
        ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        debug_array[label] = Manager::draw_idx;
        Manager::draw_idx++;
        // TODO : add skinning
        full_name = "#" + label + ":";
        nk_property_int(ctx, full_name.c_str(), 0, &_data.i, 2040, 1, 0.5f);
        // return;
    }

    if (type == UI_STRING_INPUT) {
        ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        debug_array[label] = Manager::draw_idx;
        Manager::draw_idx++;
        // TODO : add skinning
        std::string& stringRef = *_data.strPtr;
        stringToText(stringRef);
        // ctx->style.edit.normal
        nk_edit_string(ctx, NK_EDIT_SIMPLE, text, &text_len, 512, nk_filter_default);
        textToString(stringRef);
        // return;
    }

    if (type == UI_BUTTON) {
        ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        debug_array[label] = Manager::draw_idx;
        Manager::draw_idx++;
        ctx->style.button.border = border;
        ctx->style.button.rounding = rounding;
        if (skinned_style.props[button_normal].first != -1) {
            ctx->style.button.normal = nk_style_item_image(
                images[skinned_style.props[button_normal].first][skinned_style.props[button_normal].second].first
            );
        }
        if (skinned_style.props[button_hover].first != -1) {
            ctx->style.button.hover = nk_style_item_image(
                images[skinned_style.props[button_hover].first][skinned_style.props[button_hover].second].first
            );
        }
        if (skinned_style.props[button_active].first != -1) {
            ctx->style.button.active = nk_style_item_image(
                images[skinned_style.props[button_active].first][skinned_style.props[button_active].second].first
            );
        }
        /*if (skinned_style.props[cursor_normal].first != -1) {
            ctx->style.button. = nk_style_item_image(
                images[skinned_style.props[cursor_normal].first][skinned_style.props[cursor_normal].second].first
            );
        }*/
        /*if (skinned_style.props[cursor_hover].first != -1) {
            ctx->style.button.c = nk_style_item_image(
                images[skinned_style.props[cursor_hover].first][skinned_style.props[cursor_hover].second].first
            );
        }*/
        /*if (skinned_style.props[cursor_active].first != -1) {
            ctx->style.progress.cursor_active = nk_style_item_image(
                images[skinned_style.props[cursor_active].first][skinned_style.props[cursor_active].second].first
            );
        }*/
        // struct nk_rect bounds = nk_widget_bounds(ctx);
        if (ui_button_image != -1) {
            // button_state = nk_button_image(ctx, images[ui_button_image][cropId].first);
            _data.b = nk_button_image(ctx, images[ui_button_image][cropId].first);
        } else {
            // button_state = nk_button_label(ctx, label.c_str());
            _data.b = nk_button_label(ctx, label.c_str());
        }
        nk_bool button_hovered = nk_widget_is_hovered(ctx);
        if (button_hovered) {
            isHovered = true;
        } else {
            isHovered = false;
        }

        isHovered = nk_widget_is_hovered(ctx);
        if (tooltip_display) {
            struct nk_rect bounds = nk_widget_bounds(ctx);
            if (nk_input_is_mouse_hovering_rect(&ctx->input, bounds)) {
                // color_table tooltip_style = style;
                // tooltip_style.elements[UI_COLOR_TEXT].r = 255;
                // tooltip_style.elements[UI_COLOR_TEXT].g = 255;
                // tooltip_style.elements[UI_COLOR_TEXT].b = 255;
                nk_style_from_table(ctx, (struct nk_color*)tooltip_style.elements);
                nk_tooltip(ctx, tooltip_text.c_str());
                nk_style_from_table(ctx, (struct nk_color*)style.elements);
            }
        }

        if (disabled) {
            rmb_click = false;
            _data.b = false;
        }

        if (_data.b && Manager::buttonClickCallback) {
            Manager::buttonClickCallback(NULL);
        }

        if (selected_by_keys)
            _data.b = true;

        if (nk_widget_is_mouse_clicked(ctx, NK_BUTTON_RIGHT)) {
            rmb_click = true;
        } else {
            rmb_click = false;
        }

        if (_data.b) {
            // evoke callbacks
            unsigned int cbIdx = 0;
            for (auto callback : activeCallbacks) {
                callback(activeDatas[cbIdx]);
                cbIdx++;
            }
        }
    }

    // nk_button_image_label
    if (type == UI_EMPTY) {
        nk_spacing(ctx, 1);

        if (tooltip_display) {
            struct nk_rect bounds = nk_widget_bounds(ctx);
            if (nk_input_is_mouse_hovering_rect(&ctx->input, bounds)) {
                // color_table tooltip_style = style;
                // tooltip_style.elements[UI_COLOR_TEXT].r = 255;
                // tooltip_style.elements[UI_COLOR_TEXT].g = 255;
                // tooltip_style.elements[UI_COLOR_TEXT].b = 255;
                nk_style_from_table(ctx, (struct nk_color*)tooltip_style.elements);
                nk_tooltip(ctx, tooltip_text.c_str());
                nk_style_from_table(ctx, (struct nk_color*)style.elements);
            }
        }
    }

    if (type == UI_BUTTON_SWITCH) {
        ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        debug_array[label] = Manager::draw_idx;
        Manager::draw_idx++;

        if (skinned_style.props[button_normal].first != -1) {
            ctx->style.button.normal = nk_style_item_image(
                images[skinned_style.props[button_normal].first][skinned_style.props[button_normal].second].first
            );
        }
        if (skinned_style.props[button_hover].first != -1) {
            ctx->style.button.hover = nk_style_item_image(
                images[skinned_style.props[button_hover].first][skinned_style.props[button_hover].second].first
            );
        }
        if (skinned_style.props[button_active].first != -1) {
            ctx->style.button.active = nk_style_item_image(
                images[skinned_style.props[button_active].first][skinned_style.props[button_active].second].first
            );
        }

        if (_data.b) {
            // use "active" skin for normal and hover properties
            if (skinned_style.props[button_active].first != -1) {
                ctx->style.button.normal = nk_style_item_image(
                    images[skinned_style.props[button_active].first][skinned_style.props[button_active].second].first
                );
                ctx->style.button.hover = nk_style_item_image(
                    images[skinned_style.props[button_active].first][skinned_style.props[button_active].second].first
                );
            } else {
                // default styling options
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
                ctx->style.button = button;
            }
            if (nk_button_label(ctx, label.c_str()))
                _data.b = false;
        } else if (nk_button_label(ctx, label.c_str()))
            _data.b = true;
        else
            _data.b = false;
        // return;
    }

    if (type == UI_DROPDOWN) {
        // allocate memory for items from uiStringGroup
        // const char **lst = reinterpret_cast<const char **>( _data.usgPtr->getCharArrays() );
        // _data.usgPtr->selected_element = nk_combo(ctx, lst, NK_LEN(lst), _data.usgPtr->selected_element, 25, nk_vec2(200, 200));
        // _data.usgPtr->disposeCharsArray();
    }

    if (type == UI_COLOR_PICKER) {
        // TODO : add skinning
        struct nk_colorf & curr_widget_color = *reinterpret_cast<nk_colorf*>(&_data.c);
        if (nk_combo_begin_color(ctx, nk_rgb_cf(curr_widget_color), nk_vec2(nk_widget_width(ctx), 400))) {
            color_picker_unwrapped = true;
            nk_layout_row_dynamic(ctx, 120, 1);
            curr_widget_color = nk_color_picker(ctx, curr_widget_color, NK_RGBA);
            nk_layout_row_dynamic(ctx, 25, 1);
            curr_widget_color.r = nk_propertyf(ctx, "#R:", 0, curr_widget_color.r, 1.0f, 0.01f, 0.005f);
            curr_widget_color.g = nk_propertyf(ctx, "#G:", 0, curr_widget_color.g, 1.0f, 0.01f, 0.005f);
            curr_widget_color.b = nk_propertyf(ctx, "#B:", 0, curr_widget_color.b, 1.0f, 0.01f, 0.005f);
            curr_widget_color.a = nk_propertyf(ctx, "#A:", 0, curr_widget_color.a, 1.0f, 0.01f, 0.005f);
            nk_combo_end(ctx);
        } else {
            color_picker_unwrapped = false;
        }
    }
    
    if (type == UI_IMAGE) {
        ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        debug_array[label] = Manager::draw_idx;
        Manager::draw_idx++;

        if (_data.i != -1)
            nk_image(ctx, images[_data.i][cropId].first);
    }

    if (type == UI_STRING_LABEL) {
        ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        debug_array[label] = Manager::draw_idx;
        Manager::draw_idx++;

        nk_flags align;
        switch(text_align){
          case LEFT:
            align = NK_TEXT_LEFT;
            break;
          case CENTER:
            align = NK_TEXT_CENTERED;
            break;
          case RIGHT:
            align = NK_TEXT_RIGHT;
            break;
        }
        nk_label(ctx, label.c_str(), align);
    }

    if (type == UI_STRING_TEXT) {
        ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        debug_array[label] = Manager::draw_idx;
        Manager::draw_idx++;

        nk_label_wrap(ctx, label.c_str());
    }
    
    if (type == UI_PROGRESS) {
        ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        debug_array[label] = Manager::draw_idx;
        Manager::draw_idx++;
        if (use_custom_element_style) {
            ctx->style.progress.cursor_normal = nk_style_item_color(
                *(struct nk_color*)&style.elements[COLOR_ELEMENTS::UI_COLOR_SLIDER_CURSOR]
            );
            ctx->style.progress.cursor_hover = nk_style_item_color(
                *(struct nk_color*)&style.elements[COLOR_ELEMENTS::UI_COLOR_SLIDER_CURSOR_HOVER]
            );
            ctx->style.progress.cursor_active = nk_style_item_color(
                *(struct nk_color*)&style.elements[COLOR_ELEMENTS::UI_COLOR_SLIDER_CURSOR_ACTIVE]
            );
        }

        if (skinned_style.props[progress_normal].first != -1) {
            ctx->style.progress.normal = nk_style_item_image(
                images[skinned_style.props[progress_normal].first][skinned_style.props[progress_normal].second].first
            );
        }
        if (skinned_style.props[progress_hover].first != -1) {
            ctx->style.progress.hover = nk_style_item_image(
                images[skinned_style.props[progress_hover].first][skinned_style.props[progress_hover].second].first
            );
        }
        if (skinned_style.props[progress_active].first != -1) {
            ctx->style.progress.active = nk_style_item_image(
                images[skinned_style.props[progress_active].first][skinned_style.props[progress_active].second].first
            );
        }
        if (skinned_style.props[cursor_normal].first != -1) {
            ctx->style.progress.cursor_normal = nk_style_item_image(
                images[skinned_style.props[cursor_normal].first][skinned_style.props[cursor_normal].second].first
            );
        }
        if (skinned_style.props[cursor_hover].first != -1) {
            ctx->style.progress.cursor_hover = nk_style_item_image(
                images[skinned_style.props[cursor_hover].first][skinned_style.props[cursor_hover].second].first
            );
        }
        if (skinned_style.props[cursor_active].first != -1) {
            ctx->style.progress.cursor_active = nk_style_item_image(
                images[skinned_style.props[cursor_active].first][skinned_style.props[cursor_active].second].first
            );
        }

        ctx->style.progress.border = border;
        ctx->style.progress.rounding = rounding;
        ctx->style.progress.padding = nk_vec2(padding.h, padding.w);

        nk_size curr = _data.ui;
        nk_progress(ctx, &curr, 100, modifyable_progress_bar);
        _data.ui = curr;
    }

    if (type == UI_ITEMS_LIST) {
        ctx->current->buffer.curr_cmd_idx = Manager::draw_idx;
        debug_array[label] = Manager::draw_idx;
        Manager::draw_idx++;

        // TODO : add skinning
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
            nk_flags align;
            switch(text_align){
            case LEFT:
                align = NK_TEXT_LEFT;
                break;
            case CENTER:
                align = NK_TEXT_CENTERED;
                break;
            case RIGHT:
                align = NK_TEXT_RIGHT;
                break;
            }
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
                    nk_layout_row_dynamic(ctx, 20, 1);
                    nk_selectable_label(ctx, uiGroupRef.elements[i].c_str(), align, &selected);
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
            /*struct nk_key_selector ks;
            ks.focused = true;*/
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
                    }
                    //} else {
                    //    //nk_button_label(ctx, it->c_str(), uiGroupRef.focused_by_key_element == i ? &ks : 0);
                    //    nk_button_label(ctx, it->c_str(), uiGroupRef.focused_by_key_element == i ? &ks : 0);
                    //}
                    ctx->style.button = button;
                //} else if (nk_button_label(ctx, it->c_str(), uiGroupRef.focused_by_key_element == i ? &ks : 0 )) {
                }
                else if (nk_button_label(ctx, it->c_str())) {
                    if ( uiGroupRef.selected_element != i )
                        uiGroupRef.selection_switch = true;
                    uiGroupRef.selected_element = i;
                }
                i++;
            }
        }
    }

    struct nk_rect wid_rect = nk_widget_bounds(ctx);
    layout_border.x = wid_rect.x; 
    layout_border.y = wid_rect.y;
    layout_border.w = wid_rect.w;
    layout_border.h = wid_rect.h;
}

void UI_element::initImage(unsigned int texID, unsigned int w, unsigned int h, region<float> crop) {
    if (type != UI_IMAGE && type != UI_BUTTON) {
        // TODO : add error message
        return;
    }

    Manager::addImage(
        texID,
        w,
        h,
        crop
    );

    if (type == UI_IMAGE)
        _data.i = texID;
    else
        ui_button_image = texID;
    cropId = images[texID].size() - 1;
}

void UI_element::useSkinImage(
	unsigned int texID,
	unsigned short w,
	unsigned short h,
	region<float> crop,
	IMAGE_SKIN_ELEMENT elt
) {
    Manager::addImage(texID, w, h, crop);
    skinned_style.props[elt].first = texID;
    skinned_style.props[elt].second = images[texID].size() - 1;
}

IndieGo::UI::region<float> UI_element::getImgCrop(IndieGo::UI::IMAGE_SKIN_ELEMENT elt) {
    int idx = skinned_style.props[elt].first;
    if (idx == -1) return { 0.f, 0.f, 0.f, 0.f };
    return images[ skinned_style.props[elt].first ][ skinned_style.props[elt].second ].second;
}

extern Manager GUI;

//--------------------------------------------------------
//
//            Widget display function. May use
//            different Immadiate-Mode Ui libs
//
//-------------------------------------------------------
void WIDGET::callImmediateBackend(UI_elements_map & UIMap){
    nk_flags flags = 0;
    header_height = 0.f;
    if (border)
        flags = flags | NK_WINDOW_BORDER;

    if (minimizable)
        flags = flags | NK_WINDOW_MINIMIZABLE;

    if (title) {
        flags = flags | NK_WINDOW_TITLE;
    }

    if (movable)
        flags = flags | NK_WINDOW_MOVABLE;

    if (scalable)
        flags = flags | NK_WINDOW_SCALABLE;

    if (!has_scrollbar)
        flags = flags | NK_WINDOW_NO_SCROLLBAR;

    if (forceNoFocus)
        flags = flags | NK_WINDOW_NO_INPUT | NK_WINDOW_BACKGROUND;

    if (custom_style)
        nk_style_from_table(ctx, (struct nk_color*)style.elements);
    else
        nk_style_default(ctx);

    // check various styling possibilities
    // Background
    if (skinned_style.props[background].first != -1) {
        ctx->style.window.fixed_background = nk_style_item_image(
            images[skinned_style.props[background].first][skinned_style.props[background].second].first
        );
    }

    // Header
    // if (skinned_style.props[progress_normal].first != -1) {
    //     ctx->style.window.header.normal = nk_style_item_image(
    //         images[skinned_style.props[normal].first][skinned_style.props[normal].second].first
    //     );
    // }
    // if (skinned_style.props[hover].first != -1) {
    //     ctx->style.window.header.hover = nk_style_item_image(
    //         images[skinned_style.props[hover].first][skinned_style.props[hover].second].first
    //     );
    // }
    // if (skinned_style.props[active].first != -1) {
    //     ctx->style.window.header.active = nk_style_item_image(
    //         images[skinned_style.props[active].first][skinned_style.props[active].second].first
    //     );
    // }

    ctx->style.window.spacing = nk_vec2(spacing.h, spacing.w);
    ctx->style.window.padding = nk_vec2(padding.h, padding.w);
    ctx->style.window.border = border_size;
    
    if (font != "None") {
        nk_style_set_font(
            ctx,
            &backend_loaded_fonts[font][font_size]->handle
        );
    } else {
        // use main font, if available
        if (GUI.main_font != "None") {
            nk_style_set_font(
                ctx,
                &backend_loaded_fonts[GUI.main_font][GUI.main_font_size]->handle
            );
        }
    }
    ctx->draw_idx = Manager::draw_idx;
    if (apply_custom_shader) {
        last_apply_idx++;
        apply_indices[last_apply_idx] = Manager::draw_idx;
    }

    if (
        nk_begin(
            ctx,
            name.c_str(),
            nk_rect(
                screen_size.w * screen_region.x,
                screen_size.h * screen_region.y,
                screen_size.w * screen_region.w,
                screen_size.h * screen_region.h
            ),
            flags 
        )
    ) {
        if (setFocus) {
            nk_window_set_focus(ctx, name.c_str());
            setFocus = false;
        }
        
        if (has_scrollbar && updateScrollPosReq) {
            unsigned int X, Y;
            nk_window_get_scroll(ctx, &X, &Y);
            if (scroll_offsets.w != -1) {
                /*unsigned int upd = (100.f / (float)scroll_offsets.w) * (screen_region.x - screen_region.w) * screen_size.w;
                upd += screen_region.x * screen_size.w;
                nk_window_set_scroll(ctx, upd, Y);*/
            }

            if (scroll_offsets.h != -1) {
                float scroll_percent = ((float)scroll_offsets.h / 100.f);
                float offset_region_from_widget_persent = scroll_percent * screen_region.h;
                float final_scroll = ( offset_region_from_widget_persent + screen_region.y) * screen_size.h + screen_size.h * screen_region.h;
                unsigned int upd = final_scroll;
                nk_window_set_scroll(ctx, X, upd);
                
            }
            updateScrollPosReq = false;
        }
        
        if (title || movable || minimizable)
            header_height = ctx->current->layout->header_height;
        
        Manager::draw_idx++;
        callWidgetUI(UIMap);
        minimized = false;
    } else {
        minimized = true;
    }
    if (has_scrollbar) {
        unsigned int X, Y;
        nk_window_get_scroll(ctx, &X, &Y);
        scroll_offsets.h = Y;
        scroll_offsets.w = X;
    }

    focused = nk_window_has_focus(ctx);
    hasCursor = nk_window_is_hovered(ctx);

    if (movable || scalable) {
        // update screen_region with current bounds, if those are changed by user
        struct nk_rect updBounds = nk_window_get_bounds(ctx);
        screen_region.x = updBounds.x / screen_size.w;
        screen_region.y = updBounds.y / screen_size.h;
        screen_region.w = updBounds.w / screen_size.w;
        screen_region.h = updBounds.h / screen_size.h;
    }
    nk_end(ctx);
}

void Manager::addImage(
    unsigned int texID,
    unsigned short w,
	unsigned short h,
	region<float> crop
) {

    images[texID].push_back(
        std::pair<struct nk_image, region<float>> { 
            nk_subimage_id(
                texID, 
                w,
                h, 
                nk_rect(
                    crop.x * w, 
                    crop.y * h, 
                    crop.w * w, 
                    crop.h * h
                )
            ), 
            crop 
        }
    );
}

void WIDGET::useSkinImage(
	unsigned int texID,
	unsigned short w,
	unsigned short h,
	region<float> crop,
	IMAGE_SKIN_ELEMENT elt
) {
    Manager::addImage(texID, w, h, crop);
    skinned_style.props[elt].first = texID;
    skinned_style.props[elt].second = images[texID].size() - 1;
}

IndieGo::UI::region<float> WIDGET::getImgCrop(IndieGo::UI::IMAGE_SKIN_ELEMENT elt) {
    int idx = skinned_style.props[elt].first;
    if (idx == -1) return { 0.f, 0.f, 0.f, 0.f };
    return images[ skinned_style.props[elt].first ][ skinned_style.props[elt].second ].second;
}

// TODO : calculate elements count in row
// so far - 64 == max
float WIDGET::allocateRow(unsigned int cols, float min_height, bool in_pixels) {
    float height = in_pixels ? min_height : screen_size.h * screen_region.h * min_height;
    nk_layout_space_begin(
        ctx, 
        NK_STATIC, 
        height,
        64
    );
    struct nk_rect total_space = nk_layout_space_bounds(ctx);
    // std::cout << total_space.h << std::endl;
    return total_space.h;
}

void WIDGET::endRow() {
    nk_layout_space_end(ctx);
}

#define NK_GLFW_GL3_MOUSE_GRABBING

char uncode_to_char(unsigned int codepoint) {
    nk_rune u = codepoint;
    char c;
    nk_utf_encode(u, &c, 1);
    return c;
}

unsigned int char_to_uncode(char c) {
    nk_rune u;
    nk_utf_decode((const char*)&c, &u, 2);
    return (unsigned int)u;
}

void Manager::drawFrameStart(std::string & winID) {
    struct nk_glfw * glfw = glfw_storage[winID];

    int i;
    double x, y;
    // get context's address for current window
    ctx = &glfw->ctx;

    //struct nk_context *ctx = &glfw->ctx;
    struct GLFWwindow* win = glfw->win;

    // Update current window
    glfw->win = win;

    glfwGetWindowSize(win, &glfw->width, &glfw->height);
    glfwGetFramebufferSize(win, &glfw->display_width, &glfw->display_height);
    glfw->fb_scale.x = (float)glfw->display_width / (float)glfw->width;
    glfw->fb_scale.y = (float)glfw->display_height / (float)glfw->height;

    nk_input_begin(ctx);
    for (i = 0; i < glfw->text_len; ++i)
        nk_input_unicode(ctx, glfw->text[i]);

#ifdef NK_GLFW_GL3_MOUSE_GRABBING
    /* optional grabbing behavior */
    if (ctx->input.mouse.grab)
        glfwSetInputMode(glfw->win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    else if (ctx->input.mouse.ungrab)
        glfwSetInputMode(glfw->win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#endif

    nk_input_key(ctx, NK_KEY_DEL, glfwGetKey(win, GLFW_KEY_DELETE) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_ENTER, glfwGetKey(win, GLFW_KEY_ENTER) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_TAB, glfwGetKey(win, GLFW_KEY_TAB) == GLFW_PRESS);

    if (glfw->backspace) {
        glfw->backspace = false;
        nk_input_key(ctx, NK_KEY_BACKSPACE, true);
    }

    if (glfw->arrow_right) {
        glfw->arrow_right = false;
        nk_input_key(ctx, NK_KEY_RIGHT, true);
    }

    if (glfw->arrow_left) {
        glfw->arrow_left = false;
        nk_input_key(ctx, NK_KEY_LEFT, true);
    }

    nk_input_key(ctx, NK_KEY_UP, glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_DOWN, glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_TEXT_START, glfwGetKey(win, GLFW_KEY_HOME) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_TEXT_END, glfwGetKey(win, GLFW_KEY_END) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_SCROLL_START, glfwGetKey(win, GLFW_KEY_HOME) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_SCROLL_END, glfwGetKey(win, GLFW_KEY_END) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_SCROLL_DOWN, glfwGetKey(win, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_SCROLL_UP, glfwGetKey(win, GLFW_KEY_PAGE_UP) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_SHIFT, glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(win, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

    if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(win, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
        nk_input_key(ctx, NK_KEY_COPY, glfwGetKey(win, GLFW_KEY_C) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_PASTE, glfwGetKey(win, GLFW_KEY_V) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_CUT, glfwGetKey(win, GLFW_KEY_X) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_UNDO, glfwGetKey(win, GLFW_KEY_Z) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_REDO, glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_LINE_START, glfwGetKey(win, GLFW_KEY_B) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_LINE_END, glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS);
    }
    else {
        // nk_input_key(ctx, NK_KEY_LEFT, glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS);
        // nk_input_key(ctx, NK_KEY_RIGHT, glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_COPY, 0);
        nk_input_key(ctx, NK_KEY_PASTE, 0);
        nk_input_key(ctx, NK_KEY_CUT, 0);
        nk_input_key(ctx, NK_KEY_SHIFT, 0);
    }

    glfwGetCursorPos(win, &x, &y);
    nk_input_motion(ctx, (int)x, (int)y);
#ifdef NK_GLFW_GL3_MOUSE_GRABBING
    if (ctx->input.mouse.grabbed) {
        glfwSetCursorPos(glfw->win, ctx->input.mouse.prev.x, ctx->input.mouse.prev.y);
        ctx->input.mouse.pos.x = ctx->input.mouse.prev.x;
        ctx->input.mouse.pos.y = ctx->input.mouse.prev.y;
    }
#endif
    nk_input_button(ctx, NK_BUTTON_LEFT, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    nk_input_button(ctx, NK_BUTTON_MIDDLE, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
    nk_input_button(ctx, NK_BUTTON_RIGHT, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
    nk_input_button(ctx, NK_BUTTON_DOUBLE, (int)glfw->double_click_pos.x, (int)glfw->double_click_pos.y, glfw->is_double_click_down);
    nk_input_scroll(ctx, glfw->scroll);
    nk_input_end(&glfw->ctx);
    glfw->text_len = 0;
    glfw->scroll = nk_vec2(0, 0);

    // set "default global" font
    if (main_font != "None") {
        nk_style_set_font(
            &glfw->ctx,
            &backend_loaded_fonts[main_font][main_font_size]->handle
        );
    }

    // reset apply_indices
    for (int i = 0; i < 50; i++) {
        apply_indices[i] = -1;
    }
    last_apply_idx = -1;
}

void Manager::drawFrameEnd(std::string & winID) {
    struct nk_glfw * glfw = glfw_storage[winID];

    nk_glfw3_render(glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

void Manager::init(std::string winID, void * initData) {
    prepareUIRenderer((GLFWwindow*)initData, winID);
}

void Manager::addWindow(std::string winID, void * initData) {
    prepareUIRenderer((GLFWwindow*)initData, winID);
}

void Manager::removeWindow(std::string winID, void * winData) {

}

// std::string project_dir = "None";
void * img_data = NULL;

void Manager::loadFont(std::string path, const std::string & winID, float font_size, bool useProjectDir, bool cutProjDirFromPath) {
    std::string font_name = fs::path(path).stem().string();
    if (std::find(loaded_fonts[font_name].sizes.begin(), loaded_fonts[font_name].sizes.end(), font_size) != loaded_fonts[font_name].sizes.end())
        return;

    const void *image; int w, h;
    struct nk_font_config cfg = nk_font_config(0);
    cfg.range = nk_font_cyrillic_glyph_ranges();

    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);

    loaded_fonts[ font_name ].sizes.push_back(font_size);
    std::string pdir = project_dir;
    if (fs::exists(fs::path(project_dir)))
        pdir = project_dir;
    else
        pdir = "";

    if (fs::path(path).is_absolute() && pdir != "") {
        loaded_fonts[font_name].path = fs::relative(fs::path(path), fs::path(pdir)).string();
    } else {
        loaded_fonts[font_name].path = path;
    }

    backend_loaded_fonts.clear();

    bool mainFont = false;
    for (auto font : loaded_fonts) {
        mainFont = font.first == main_font;
        for (auto font_size : font.second.sizes) {
            if (mainFont) // never use PROJECT_DIR for main font loading
                backend_loaded_fonts[font.first][font_size] = nk_font_atlas_add_from_file(&atlas, font.second.path.c_str(), font_size, &cfg );
            else
                backend_loaded_fonts[font.first][font_size] = nk_font_atlas_add_from_file(
                    &atlas, 
                    fs::path(pdir).append(font.second.path).string().c_str(),
                    font_size, 
                    &cfg
                );
        }
    }

    image = nk_font_atlas_bake(&atlas, &w, &h, NK_FONT_ATLAS_RGBA32);

    nk_glfw3_device_upload_atlas(glfw_storage[winID], image, w, h);
    nk_font_atlas_end(&atlas, nk_handle_id((int)glfw_storage[winID]->ogl.font_tex), &glfw_storage[winID]->ogl.null);
    if (main_font == "None") {
        main_font = font_name;
        main_font_size = font_size;
    }
}