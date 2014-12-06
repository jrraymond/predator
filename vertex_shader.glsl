#version 330 core
layout(location=0) in vec3 position ;
layout(location=1) in vec3 color_in ;
uniform mat4 model ;
uniform mat4 proj ;
uniform mat4 view ;

out vec3 color_v_to_f ;
void main() {
    color_v_to_f = color_in ;
    gl_Position = proj * view * model * vec4(position, 1.0) ;
}
