#version 330 core

layout(location=0) in vec3 pos_model ;
layout(location=1) in vec3 color_in ;
layout(location=2) in vec3 normal_model ;

uniform mat4 model ;
uniform mat4 proj ;
uniform mat4 view ;
uniform vec3 light_model ;
uniform vec3 eye_model ;

out vec3 color_v_to_f ;
out vec3 normal_world ;
out vec3 pt_to_eye ;
out vec3 pt_to_light ;

void main() {

    vec4 light_world = model * vec4(light_model, 1.0) ;
    vec4 eye_world = model * vec4(eye_model, 1.0) ;
    vec4 pos_world = model * vec4(pos_model, 1.0) ;

    gl_Position = proj * view * pos_world ;

    normal_world = normalize(vec3(model * vec4(normal_model, 0.0))) ; //does not work with scale
    pt_to_light = normalize(vec3(light_world - pos_world)) ;
    pt_to_eye = normalize(vec3(eye_world - pos_world)) ;

    color_v_to_f = color_in ; //+ color_in * dot(l,vec4(normal,1.0));
}
