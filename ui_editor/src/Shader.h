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

// Shader implementation taken from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_s.h

#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <vector>
#include <glm/glm.hpp>
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