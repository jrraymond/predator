#version 330 core
uniform mat4 model ;
uniform mat4 proj ;
uniform mat4 view ;
layout(location=0) in vec3 grid_pt ;
layout(location=1) in vec3 color_in ;
out vec3 color_v_to_f ;
void main() {
    gl_Position = proj * view * model * vec4(grid_pt, 1.0) ;
    color_v_to_f = color_in ;
}
