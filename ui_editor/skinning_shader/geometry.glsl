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

#version 410 core
layout (points) in;
layout (triangle_strip, max_vertices = 6) out;


// width and height of sprite
in vec2 sizes[];
in vec4 texture[];

out vec4 fragColor;
out vec2 texCoords;

uniform mat4 projection;

vec4 buf;

void main() { 
    // First triangle
    // top-left
    buf.x = -sizes[0].x / 2;
    buf.y = sizes[0].y / 2;
    buf.z = 0.f; 
    buf.w = 1.f;
    buf = projection * buf;
    gl_Position =  gl_in[0].gl_Position + buf; 
    gl_Position.w = 1.f;
    // tex coord for BOTTOM-left
    texCoords.x = -texture[0].x;
    texCoords.y = texture[0].w;
    EmitVertex();

    // bottom-left
    buf.x = -sizes[0].x / 2;
    buf.y = -sizes[0].y / 2;
    buf.z = 0.f; 
    buf.w = 1.f;
    buf = projection * buf;
    gl_Position = gl_in[0].gl_Position + buf; 
    gl_Position.w = 1.f;
    // text coord for TOP-left
    texCoords.x = -texture[0].x;
    texCoords.y = texture[0].y;
    EmitVertex();

    // bottom-right
    buf.x = sizes[0].x / 2;
    buf.y = -sizes[0].y / 2;
    buf.z = 0.f; 
    buf.w = 1.f;
    buf = projection * buf;
    gl_Position = gl_in[0].gl_Position + buf; 
    gl_Position.w = 1.f;
    // tex coord for TOP-right
    texCoords.x = texture[0].z;
    texCoords.y = texture[0].y;
    EmitVertex();
    EndPrimitive();

    // Second triangle
    // bottom-right
    buf.x = sizes[0].x / 2;
    buf.y = -sizes[0].y / 2;
    buf.z = 0.f; 
    buf.w = 1.f;
    buf = projection * buf;
    gl_Position = gl_in[0].gl_Position + buf; 
    gl_Position.w = 1.f;
    // tex coord for TOP-right
    texCoords.x = texture[0].z;
    texCoords.y = texture[0].y;
    EmitVertex();

    // top-right
    buf.x = sizes[0].x / 2;
    buf.y = sizes[0].y / 2;
    buf.z = 0.f; 
    buf.w = 1.f;
    buf = projection * buf;
    gl_Position = gl_in[0].gl_Position + buf; 
    gl_Position.w = 1.f;
    // tex coord for BOTTOM-right
    texCoords.x = texture[0].z;
    texCoords.y = texture[0].w;
    EmitVertex();

    // top-left
    buf.x = -sizes[0].x / 2;
    buf.y = sizes[0].y / 2;
    buf.z = 0.f; 
    buf.w = 1.f;
    buf = projection * buf;
    gl_Position = gl_in[0].gl_Position + buf; 
    gl_Position.w = 1.f;
    // coord for BOTTOM-left
    texCoords.x = -texture[0].x;
    texCoords.y = texture[0].w;
    EmitVertex();
    EndPrimitive();
}  