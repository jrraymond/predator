#version 330 core
uniform mat4 model ;
uniform mat4 proj ;
uniform mat4 view ;
in vec3 position ;
in vec3 color ;
out vec3 Color ;
void main() {
    Color = color ;
    gl_Position = proj * view * model * vec4(position, 1.0) ;
}
