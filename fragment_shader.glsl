#version 330 core

in vec3 color_v_to_f ;
in vec3 normal_world ;
in vec3 pt_to_eye ;
in vec3 pt_to_light ;

uniform vec3 ambient ;
uniform float spec_coefficient ;

void main() {
    vec3 ambient_c = color_v_to_f * ambient ;

    float d = dot(normal_world,pt_to_light) ;
    vec3 diffuse_c = clamp(color_v_to_f * d, 0, 1) ;

    vec3 reflected = reflect(-pt_to_light, normal_world) ;
    float s = dot(pt_to_eye, reflected) ;
    s = pow(s, spec_coefficient) ;
    vec3 specular_c = clamp(color_v_to_f * s, 0, 1) ;

    gl_FragColor = vec4(ambient_c + diffuse_c + specular_c, 1.0f) ;
}
