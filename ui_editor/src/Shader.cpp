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

#include <Shader.h>
#include <sstream>
#include <fstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cassert>

#include <iostream>

void Shader::load(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::cout << "Vertex shader path: " + std::string(vertexPath) << std::endl;
    std::string vertexCode = loadShaderFile(vertexPath);

    std::cout << "Fragment shader path: " + std::string(fragmentPath) << std::endl;
    std::string fragmentCode = loadShaderFile(fragmentPath);
    std::string geometryCode;
    if (geometryPath != nullptr){
        std::cout << "Geometry shader path: " + std::string(geometryPath) << std::endl;
        geometryCode = loadShaderFile(geometryPath);
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // if geometry shader is given, compile geometry shader
    unsigned int geometry;
    if (geometryPath != nullptr) {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);

    if (geometryPath != nullptr)
        glAttachShader(ID, geometry);

    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    if (geometryPath != nullptr)
        glDeleteShader(geometry);
}

// activate the shader
// ------------------------------------------------------------------------
void Shader::use() {   

    GLenum shader_error = glGetError();
    glUseProgram(ID);
    shader_error = glGetError();

    if (skinning) {
        // todo : use skinning texture
        glActiveTexture(GL_TEXTURE0);
        shader_error = glGetError();
		glBindTexture(GL_TEXTURE_2D, skin_tex_id);
    }

    assert(shader_error == 0);
    setUniforms();
    setAttributes();
}

// ------------------------------------------------------------------------
void Shader::setInt(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

// ------------------------------------------------------------------------
void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

// ------------------------------------------------------------------------
void Shader::setMat4(const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setUniforms() {
    if (skinning) {
        setInt("skin_image", 0);
        setFloat("scale", scale);
    }
    setMat4("projection", orthogonal_proj);
}

void Shader::setAttributes() {
    // 4 floats for position and size of each layout element
    // position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // sizes
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // color/tex_coords
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

std::string Shader::loadShaderFile(const char* path) {
        // 1. retrieve the vertex/fragment source code from filePath
    std::string code = "";
    std::ifstream ShaderFile;

    // ensure ifstream objects can throw exceptions:
    ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open file
        ShaderFile.open(path);
        std::stringstream ShaderStream;

        // read file's buffer contents into streams
        ShaderStream << ShaderFile.rdbuf();
        // close file handlers
        ShaderFile.close();

        // convert stream into string
        code = ShaderStream.str();
    } catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        load_errors_count++;
    }
    return code;
}

    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
void Shader::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " + type << std::endl;
            std::cout << std::string(infoLog) << std::endl;
            std::cout << " -- --------------------------------------------------- -- " << std::endl;
            load_errors_count++;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " + type << std::endl;
            std::cout << std::string(infoLog) << std::endl;
            std::cout << " -- --------------------------------------------------- -- " << std::endl;
            load_errors_count++;
        }
    }
}