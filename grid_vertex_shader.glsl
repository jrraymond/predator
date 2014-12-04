#version 330 core
uniform mat4 model ;
uniform mat4 proj ;
uniform mat4 view ;
in vec3 grid_pt ;
void main() {
    gl_Position = proj * view * model * vec4(grid_pt, 1.0) ;
}
