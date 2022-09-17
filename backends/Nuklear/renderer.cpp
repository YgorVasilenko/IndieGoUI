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
    struct nk_vec2 scroll;
    double last_button_click;
    int is_double_click_down;
    struct nk_vec2 double_click_pos;
};

struct nk_glfw_vertex {
    float position[2];
    float uv[2];
    nk_byte col[4];
};

#ifdef __APPLE__
  #define NK_SHADER_VERSION "#version 150\n"
#else
  #define NK_SHADER_VERSION "#version 300 es\n"
#endif

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

NK_API void nk_glfw3_device_create(struct nk_glfw* glfw) {
    GLint status;

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

void Manager::scroll(void * window, double xoff, double yoff) {
    std::string glfw_win = *reinterpret_cast<std::string*>(window);
    struct nk_glfw * glfw = glfw_storage[glfw_win];

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
    //GLFWwindow * glfw_win = reinterpret_cast<GLFWwindow*>(window);
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

void UI_element::callUIfunction(float x, float y, float widget_w, float widget_h) {
    /*if (custom_style)
        nk_style_from_table(ctx, (struct nk_color*)style.elements);*/

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
            x,
            y,
            widget_w * width,
            widget_h * height
        )
    );
    
    nk_layout_row_dynamic(ctx, widget_h * height, 1);

    std::string full_name;
    nk_bool nk_val;
    static const float ratio[] = { 100, 120 };
    float dbgVal;
    if (type == UI_BOOL) {
        // TODO : add skinning
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
        // TODO : add skinning
        full_name = "#" + label + ":";
        nk_property_float(ctx, full_name.c_str(), -300000.0f, &_data.f, 300000.0f, 1, 0.5f);
        return;
    }

    if (type == UI_INT) {
        // TODO : add skinning
        full_name = "#" + label + ":";
        nk_property_int(ctx, full_name.c_str(), -1024, &_data.i, 1024, 1, 0.5f);
        return;
    }

    if (type == UI_UINT) {
        // TODO : add skinning
        full_name = "#" + label + ":";
        nk_property_int(ctx, full_name.c_str(), 0, &_data.i, 2040, 1, 0.5f);
        return;
    }

    if (type == UI_STRING_INPUT) {
        // TODO : add skinning
        std::string& stringRef = *_data.strPtr;
        stringToText(stringRef);
        nk_edit_string(ctx, NK_EDIT_SIMPLE, text, &text_len, 512, nk_filter_default);
        textToString(stringRef);
        return;
    }

    if (type == UI_BUTTON) {
        // TODO : add skinning
        ctx->style.button.border = border;
        ctx->style.button.rounding = rounding;
        ctx->style.button.padding = nk_vec2(padding.h, padding.w);

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
        // TODO : add skinning
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
        // TODO : add skinning
        if (_data.i != -1)
            nk_image(ctx, images[_data.i].back().first);
    }

    if (type == UI_STRING_LABEL) {
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
        nk_label_wrap(ctx, label.c_str());
    }
    
    if (type == UI_PROGRESS) {
        if (skinned_style.props[normal].first != -1) {
            ctx->style.progress.normal = nk_style_item_image(
                images[skinned_style.props[normal].first][skinned_style.props[normal].second].first
            );
        }
        if (skinned_style.props[hover].first != -1) {
            ctx->style.progress.hover = nk_style_item_image(
                images[skinned_style.props[hover].first][skinned_style.props[hover].second].first
            );
        }
        if (skinned_style.props[active].first != -1) {
            ctx->style.progress.active = nk_style_item_image(
                images[skinned_style.props[active].first][skinned_style.props[active].second].first
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

void UI_element::initImage(unsigned int texID, std::string path) {
    if (type != UI_IMAGE) {
        // TODO : add error message
        return;
    }
    if (images.find(texID) == images.end()) {
        // images[texID].push_back(nk_image_id((int)texID));
    }
    _data.i = texID;
    label = path;
}

void UI_element::useSkinImage(
	unsigned int texID,
	unsigned short w,
	unsigned short h,
	region<float> crop,
	IMAGE_SKIN_ELEMENT elt
) {
    images[texID].push_back(
        // nk_subimage_id(texID, w,h, nk_rect(crop.x, crop.y, crop.w, crop.h))
        std::pair<struct nk_image, region<float>> { nk_subimage_id(texID, w,h, nk_rect(crop.x, crop.y, crop.w, crop.h)), crop }
    );
    skinned_style.props[elt].first = texID;
    skinned_style.props[elt].second = images[texID].size() - 1;
}

IndieGo::UI::region<float> UI_element::getImgCrop(IndieGo::UI::IMAGE_SKIN_ELEMENT elt) {
    int idx = skinned_style.props[elt].first;
    if (idx == -1) return { 0.f, 0.f, 0.f, 0.f };
    return images[ skinned_style.props[elt].first ][ skinned_style.props[elt].second ].second;
}

bool test_img_loaded = false;
extern unsigned int load_image(const char *filename);
struct nk_image test_i;
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

    if (custom_style)
        nk_style_from_table(ctx, (struct nk_color*)style.elements);
    else
        nk_style_default(ctx);
    
    if (setFocus) {
        nk_window_set_focus(ctx, name.c_str());
        setFocus = false;
    }

    // TODO : prior to drawing, check if window's size changed
    // if so, try fitting widget to new size as much as possible
    float x = screen_size.w * screen_region.x;
    float y = screen_size.h * screen_region.y;
    float w = screen_size.w * screen_region.w;
    float h = screen_size.h * screen_region.h;

    if (initialized_in_backend) {
        nk_window_set_bounds(ctx, name.c_str(), nk_rect(x, y, w, h));
    }

    // check various styling possibilities
    // Background
    if (skinned_style.props[background].first != -1) {
        ctx->style.window.fixed_background = nk_style_item_image(
            images[skinned_style.props[background].first][skinned_style.props[background].second].first
        );
    }

    // Header
    if (skinned_style.props[normal].first != -1) {
        ctx->style.window.header.normal = nk_style_item_image(
            images[skinned_style.props[normal].first][skinned_style.props[normal].second].first
        );
    }
    if (skinned_style.props[hover].first != -1) {
        ctx->style.window.header.hover = nk_style_item_image(
            images[skinned_style.props[hover].first][skinned_style.props[hover].second].first
        );
    }
    if (skinned_style.props[active].first != -1) {
        ctx->style.window.header.active = nk_style_item_image(
            images[skinned_style.props[active].first][skinned_style.props[active].second].first
        );
    }

    ctx->style.window.spacing = nk_vec2(spacing.h, spacing.w);
    ctx->style.window.padding = nk_vec2(padding.h, padding.w);
    ctx->style.window.border = border_size;

    if (font != "None") {
        nk_style_set_font(
            ctx,
            &backend_loaded_fonts[font][font_size]->handle
        );
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
        initialized_in_backend = true;
        focused = nk_window_has_focus(ctx);
        hasCursor = nk_window_is_hovered(ctx);
        callWidgetUI(UIMap);
        // if (backgroundImage) {
        //     nk_layout_row_dynamic(ctx, (float)screen_region.h, 1);
        //     nk_image(ctx, images[img_idx]);
        // }
    }
    if (movable) {
        // update screen_region with current bounds, if those are changed by user
        screen_region.x = ctx->current->bounds.x / screen_size.w;
        screen_region.y = ctx->current->bounds.y / screen_size.h;
        screen_region.w = ctx->current->bounds.w / screen_size.w;
        screen_region.h = ctx->current->bounds.h / screen_size.h;
    }
    nk_end(ctx);
}

void WIDGET::useSkinImage(
	unsigned int texID,
	unsigned short w,
	unsigned short h,
	region<float> crop,
	IMAGE_SKIN_ELEMENT elt
) {
    images[texID].push_back(
        // std::make_pair<struct nk_image, region<float>>(
          std::pair<struct nk_image, region<float>> { nk_subimage_id(texID, w,h, nk_rect(crop.x, crop.y, crop.w, crop.h)), crop }
       //  )
    );
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
void WIDGET::allocateRow(unsigned int cols, float min_height, bool in_pixels) {
    float height = in_pixels ? min_height : screen_size.h * screen_region.h * min_height;
    nk_layout_space_begin(
        ctx, 
        NK_STATIC, 
        height,
        64
    );
    // float height = in_pixels ? min_height : screen_size.h * screen_region.h * min_height;
    // nk_layout_row_dynamic(
    //     ctx, 
    //     // first get size of widget in pixels, then get size of row
    //     height,
    //     cols
    // );
}

void WIDGET::endRow() {
    nk_layout_space_end(ctx);
}

// void WIDGET::allocateEmptySpace(unsigned int fill_count){
//     nk_spacing(ctx, fill_count);
// }

// void WIDGET::allocateGroupStart(elements_group & g, float min_height){
//     g.unfolded = nk_tree_push(ctx, NK_TREE_TAB, g.name.c_str(), NK_MINIMIZED);
//     if (g.unfolded)
//         nk_layout_row_dynamic(ctx, min_height, g.row_items_count);
// }

// void WIDGET::allocateGroupEnd(){
//     nk_tree_pop(ctx);
// }

#define NK_GLFW_GL3_MOUSE_GRABBING

void Manager::drawFrameStart(std::string & winID) {
    struct nk_glfw * glfw = glfw_storage[winID];

    int i;
    double x, y;
    // get context's address for current window
    ctx = &glfw->ctx;

    //struct nk_context *ctx = &glfw->ctx;
    struct GLFWwindow *win = glfw->win;
    
    // Update current window
    glfw->win = win;

    glfwGetWindowSize(win, &glfw->width, &glfw->height);
    glfwGetFramebufferSize(win, &glfw->display_width, &glfw->display_height);
    glfw->fb_scale.x = (float)glfw->display_width/(float)glfw->width;
    glfw->fb_scale.y = (float)glfw->display_height/(float)glfw->height;

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

    nk_input_key(ctx, NK_KEY_UP, glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_DOWN, glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_TEXT_START, glfwGetKey(win, GLFW_KEY_HOME) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_TEXT_END, glfwGetKey(win, GLFW_KEY_END) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_SCROLL_START, glfwGetKey(win, GLFW_KEY_HOME) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_SCROLL_END, glfwGetKey(win, GLFW_KEY_END) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_SCROLL_DOWN, glfwGetKey(win, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_SCROLL_UP, glfwGetKey(win, GLFW_KEY_PAGE_UP) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_SHIFT, glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS||
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
    } else {
        nk_input_key(ctx, NK_KEY_LEFT, glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_RIGHT, glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS);
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
    glfw->scroll = nk_vec2(0,0);

    // set "default global" font
    if (main_font != "None") {
        nk_style_set_font(
            &glfw->ctx, 
            &backend_loaded_fonts[main_font][main_font_size]->handle
        );
    }
}

void Manager::drawFrameEnd(std::string & winID) {
    struct nk_glfw * glfw = glfw_storage[winID];

    nk_glfw3_render(glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

void Manager::init(std::string winID, void * initData){
    // NuklearInitData * initPtr = reinterpret_cast<NuklearInitData*>(initData);
    prepareUIRenderer((GLFWwindow*)initData, winID);
}

void Manager::addWindow(std::string winID, void * initData) {
    prepareUIRenderer((GLFWwindow*)initData, winID);
}

void Manager::removeWindow(std::string winID, void * winData) {

}

void * img_data = NULL;
void Manager::loadFont(std::string path, const std::string & winID, float font_size) {

    std::string font_name = fs::path(path).stem().string();
    if (std::find(loaded_fonts[font_name].sizes.begin(), loaded_fonts[font_name].sizes.end(), font_size) != loaded_fonts[font_name].sizes.end())
        return;

    const void *image; int w, h;
    struct nk_font_config cfg = nk_font_config(0);
    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);

    loaded_fonts[ font_name ].sizes.push_back(font_size);
    char * pd = getenv("PROJECT_DIR");
    std::string project_dir = "";
    if (pd) {
        project_dir = pd;
        //if (path.find(project_dir) != std::string::npos)
        loaded_fonts[fs::path(path).stem().string()].path = path.substr(
            project_dir.size(), path.size()
        );
        /*else
            loaded_fonts[fs::path(path).stem().string()].path = path;*/
    } else {
        loaded_fonts[ fs::path(path).stem().string() ].path = path;
    }

    backend_loaded_fonts.clear();

    for (auto font : loaded_fonts) {
        for (auto font_size : font.second.sizes) {
            backend_loaded_fonts[font.first][font_size] = nk_font_atlas_add_from_file(&atlas, (project_dir + font.second.path).c_str(), font_size, &cfg);
        }
    }

    image = nk_font_atlas_bake(&atlas, &w, &h, NK_FONT_ATLAS_RGBA32);

    nk_glfw3_device_upload_atlas(glfw_storage[winID], image, w, h);
    nk_font_atlas_end(&atlas, nk_handle_id((int)glfw_storage[winID]->ogl.font_tex), &glfw_storage[winID]->ogl.null);
    // nk_init_default(&glfw_storage[winID]->ctx, &backend_loaded_fonts[font_name][font_size]->handle);
    if (main_font == "None") {
        main_font = font_name;
        main_font_size = font_size;
    }
}