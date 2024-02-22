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
out vec4 FragColor;

in vec4 fragColor;

// "Discard" squares:
// - main
uniform vec4 main_square;
// - element properties
uniform vec4 elements_square;
// - skinning
uniform vec4 skinning_square;
// - fonts
uniform vec4 fonts_square;

void main() {
    // TODO : get coords from editor UI widgets, discard if fall to them
    if (
        gl_FragCoord.x > main_square.x && gl_FragCoord.x < main_square.z
        && gl_FragCoord.y > main_square.y && gl_FragCoord.y < main_square.w
    )
        discard;

    if (
        gl_FragCoord.x > elements_square.x && gl_FragCoord.x < elements_square.z
        && gl_FragCoord.y > elements_square.y && gl_FragCoord.y < elements_square.w
    )
        discard;

    if (
        gl_FragCoord.x > skinning_square.x && gl_FragCoord.x < skinning_square.z
        && gl_FragCoord.y > skinning_square.y && gl_FragCoord.y < skinning_square.w
    )
        discard;
    
    if (
        gl_FragCoord.x > fonts_square.x && gl_FragCoord.x < fonts_square.z
        && gl_FragCoord.y > fonts_square.y && gl_FragCoord.y < fonts_square.w
    )
        discard;

    FragColor = fragColor;
    FragColor.a = 1.f;
}