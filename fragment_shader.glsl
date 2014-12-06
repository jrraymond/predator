#version 330 core
in vec3 color_v_to_f ;
uniform vec3 ambient ;

void main() {
    gl_FragColor = vec4(color_v_to_f * ambient, 1.0f) ;
}
