#version 330 core

in vec4 coord ;
out vec2 texcoord ;

void main() {A
    gl_Position = vec4(coord.xy, 0, 1) ;
    texcoord = coord.zw ;
}