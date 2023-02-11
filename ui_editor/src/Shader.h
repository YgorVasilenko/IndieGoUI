#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <vector>
#include <glm/glm.hpp>
// #include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Shader {

    unsigned int ID = 0;
    bool skinning = false;
    unsigned int skin_tex_id = 0;
    float scale = 1.f;
    glm::mat4 orthogonal_proj = glm::mat4(0.f);

    int viewWidth = WIDTH;
    int viewHeight = HEIGHT;
    
    void updateProj() {
        orthogonal_proj = glm::ortho(-1 * ((float)(viewWidth) / (float)viewHeight), ((float)(viewWidth) / (float)viewHeight), -1.f, 1.f, -100.f, 100.f);
    }

    void load(const char* vertexPath, const char* fragmentPath, const char* geometryPath = 0);

    // activate the shader
    // ------------------------------------------------------------------------
    void use();

    unsigned load_errors_count = 0;
    std::string loadShaderFile(const char* path);

    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type);

    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value); 

    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value);

    // ---------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat);

private:
    void setUniforms();
    void setAttributes();
};
#endif