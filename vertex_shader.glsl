#version 330 core
layout(location=0) in vec3 position ;
layout(location=1) in vec3 color_in ;
layout(location=2) in vec3 normal ;

uniform mat4 model ;
uniform mat4 proj ;
uniform mat4 view ;
uniform vec3 ambient ;
uniform vec3 light ;

out vec3 color_v_to_f ;
void main() {
    gl_Position = proj * view * model * vec4(position, 1.0) ;
    vec4 light_pos = model * vec4(light, 1.0) ;
    vec4 l = normalize(light_pos - gl_Position) ;
    color_v_to_f = color_in * ambient + color_in * dot(l,vec4(normal,1.0));
}
