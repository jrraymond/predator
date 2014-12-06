#version 330 core
in vec3 color_v_to_f ;
in vec3 normal_world ;
in vec3 lp ;
uniform vec3 ambient ;

void main() {
    vec3 ambient_component = color_v_to_f * ambient ;
    vec3 diffuse_component = color_v_to_f * dot(normal_world, lp) ;
    gl_FragColor = vec4(ambient_component, 1.0f) ;
}
