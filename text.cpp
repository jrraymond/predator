#include <glm/detail/type_vec.hpp>
#include "text.h"

bool Text::instantiated_ = false ;

Text::Text() {
    if (Text::instantiated_) {
        fprintf(stderr, "Text library already initialized") ;
    }
    FT_Error err = FT_Init_FreeType(&ft) ;
    if (err) {
        fprintf(stderr, "Failed to Initialize Free Type Library\n") ;
    }
    //load shader programs
    vertex_shader = glCreateShader(GL_VERTEX_SHADER) ;
    compile_shader("text_vertex_shader.glsl", vertex_shader) ;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER) ;
    compile_shader("text_fragment_shader.glsl", fragment_shader) ;
    shader_program = glCreateProgram() ;
    link_shader(shader_program, {vertex_shader, fragment_shader}) ;

    //glGenVertexArrays(1, &vao) ;
    texture_uniform = (GLuint) glGetUniformLocation(shader_program, "texture") ;
    color_uniform = (GLuint) glGetUniformLocation(shader_program, "color") ;
    glGenBuffers(1, &vbo) ;
    pos_attr = (GLuint) glGetAttribLocation(shader_program, "pos") ;

    instantiated_ = true ;
}
Text::~Text() {
    //glDeleteVertexArrays(1, &vao) ;
    glDeleteBuffers(1, &vbo) ;
    glDisableVertexAttribArray(pos_attr) ;
    glDeleteTextures(1, &texture) ;
    glDeleteProgram(shader_program) ;
    glDeleteShader(fragment_shader) ;
    glDeleteShader(vertex_shader) ;


    FT_Done_FreeType(ft) ;
    Text::instantiated_ = false ;
}
void Text::load_font_face(std::string f_name) {
    FT_Error err = FT_New_Face(ft, f_name.c_str(), 0, &face) ;
    if (err) {
        fprintf(stderr, "Failed to open font: %s\n", f_name.c_str()) ;
    }
    err = FT_Set_Char_Size(
        face,    /* handle to face object           */
        0,       /* char_width in 1/64th of points  */
        16*64,   /* char_height in 1/64th of points */
        300,     /* horizontal device resolution    */
        300 );   /* vertical device resolution      */
    if (err) {
        fprintf(stderr, "Error setting font face char size: %i\n", err) ;
    }
    err = FT_Set_Pixel_Sizes(face, 0, 16) ;
    if (err) {
        fprintf(stderr, "Error setting pixel size: %i\n", err) ;
    }
}
void Text::print_face_info() {
    fprintf(stdout,
            //"Num faces: %l\n"
            //"Face index: %l\n"
            "Family name: %s\n"
            "Style name: %s\n",
            //"Num glyphs: %l\n"
            //"Face flags: %l\n"
            //"Style flags: %l\n"
            //"Num Fixed Sizes: %l\n"
            //"Available sizes: %l\n",
            //"Num charmaps: %i\n",
            //face->num_faces,
            //face->face_index,
            face->family_name,
            face->style_name
            //face->num_glyphs,
            //face->face_flags,
            //face->style_flags,
            //face->num_fixed_sizes,
            //face->available_sizes
            //face->num_charmaps
            ) ;

}
/* Sets the pixel size, casts i to an unsigned int
 */
void Text::set_pixel_size(int i) {
    FT_Set_Pixel_Sizes(face, 0, (FT_UInt) i) ;
}

/* s string to render
*  x initial x position
*  y initial y position
*  sx, sy scale parameters should be chosen such that glyph pixel : screen pixel is 1:1
*/
void Text::render(std::string s, float x, float y, float sx, float sy) {
    GLfloat white_color[4] = {1.0f, 1.0f, 1.0f, 1.0f} ;
    //glUniform4fv(uniform_color, 1, &white_color) ;
    FT_GlyphSlot g ;
    g = face->glyph ;
    float x2, y2, w, h ;
    GLfloat box[4][4] ;
    FT_Error err ;
    GLuint tex;

    glUseProgram(shader_program) ;
    glClearColor(0,0,0,1) ;
    glClear(GL_COLOR_BUFFER_BIT) ;

    glEnable(GL_BLEND) ;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;

    glUniform4fv(color_uniform, 1, white_color) ;
    glBindBuffer(GL_ARRAY_BUFFER, vbo) ;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(texture_uniform, 0);

    /* We require 1 byte alignment when uploading texture data */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* Linear filtering usually looks best for text */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Set up the VBO for our vertex data */
    glEnableVertexAttribArray(pos_attr);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(pos_attr, 4, GL_FLOAT, GL_FALSE, 0, 0);

    for(auto c : s) {
        err = FT_Load_Char(face, c, FT_LOAD_RENDER) ;
        if(err) {
            fprintf(stderr, "Could not load character '%c'\n", c);
            continue ;
        }

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_R8,
            g->bitmap.width,
            g->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            g->bitmap.buffer
        ) ;

        x2 = x + g->bitmap_left * sx ;
        y2 = -y - g->bitmap_top * sy ;
        w = g->bitmap.width * sx ;
        h = g->bitmap.rows * sy ;

        box[0][0] =     x2 ; box[0][1] =     -y2 ; box[0][2] = 0 ; box[0][3] = 0 ;
        box[1][0] = x2 + w ; box[1][1] =     -y2 ; box[1][2] = 1 ; box[1][3] = 0 ;
        box[2][0] =     x2 ; box[2][1] = -y2 - h ; box[2][2] = 0 ; box[2][3] = 1 ;
        box[3][0] = x2 + w ; box[3][1] = -y2 - h ; box[3][2] = 1 ; box[3][3] = 1 ;

        glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW) ;
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4) ; //could this be done all at once for entire string

        x += (g->advance.x >> 6) * sx ;
        y += (g->advance.y >> 6) * sy ;
    }
    glDeleteTextures(1, &tex) ;
    glDisableVertexAttribArray(pos_attr) ;
}
