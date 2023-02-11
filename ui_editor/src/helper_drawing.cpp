#include <filesystem>
#include <Shader.h>
#include <editor_structs.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cassert>

namespace fs = std::filesystem;


unsigned int VAO = 0;
unsigned int VBO = 0;
unsigned int EBO = 0;

Shader layoutShader;
Shader skinningShader;
extern std::string home_dir;

void loadShader(const std::string & name) {
    std::string vertex = "", geometry = "", fragment = "";
    std::string load_path = home_dir + "/../../" + name;
    for (auto f : fs::directory_iterator(load_path)) {
        if ( f.symlink_status().type() == fs::file_type::regular ) {
            // filter non-glsl files
            if (fs::path(f).extension() != ".glsl") continue;
            // check name
            if (fs::path(f).filename() == "vertex.glsl"){
                vertex = fs::path(f).string();
            }
            if (fs::path(f).filename() == "geometry.glsl"){
                geometry = fs::path(f).string();
            }
            if (fs::path(f).filename() == "fragment.glsl"){
                fragment = fs::path(f).string();
            }
        }
    }
    if (name == "layout_shader") {
        layoutShader.load(vertex.c_str(), fragment.c_str(), geometry.c_str());
        layoutShader.updateProj();
    } else {
        skinningShader.load(vertex.c_str(), fragment.c_str(), geometry.c_str());
        skinningShader.skinning = true;
        skinningShader.updateProj();
    }
};

void initBuffers() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Opengl blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void drawLayout(LayoutRect element) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

	layoutShader.use();
    
	glBufferData( GL_ARRAY_BUFFER, sizeof(LayoutRect), &element, GL_DYNAMIC_DRAW );
	glDrawArrays( GL_POINTS, 0, 1 );
}

void drawSkinImage(LayoutRect element) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

	skinningShader.use();
    
	glBufferData( GL_ARRAY_BUFFER, sizeof(LayoutRect), &element, GL_DYNAMIC_DRAW );
    GLuint err = glGetError();
	glDrawArrays( GL_POINTS, 0, 1 );
    err = glGetError();
    assert(err == 0);
}