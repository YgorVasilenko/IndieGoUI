#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <vector>

struct Shader {

    unsigned int ID = 0;
    bool skinning = false;

    void load(const char* vertexPath, const char* fragmentPath, const char* geometryPath = 0);

    // activate the shader
    // ------------------------------------------------------------------------
    void use();

    unsigned load_errors_count = 0;
    std::string loadShaderFile(const char* path);

    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type);
private:
    void setUniforms();
    void setAttributes();
};
#endif