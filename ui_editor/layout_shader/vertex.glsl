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

#version 420 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 size;
layout (location = 2) in vec4 color_;

out vec2 sizes;
out vec4 color;

void main() {
    gl_Position = vec4(position, 0.f, 1.f);
    sizes = size;
    color = color_;
}