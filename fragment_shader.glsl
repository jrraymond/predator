#version 330 core
in vec3 color_v_to_f ;

void main() {
    gl_FragColor = vec4(color_v_to_f, 1.0f) ;
}
