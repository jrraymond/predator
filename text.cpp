#include "text.h"

Text::Text() {
    if (Text::instantiated_) { fprintf(stderr, "Text library already initialized") ; }
    FT_Error r = FT_Init_FreeType(&ft) ;
    if (r) {
        fprintf(stderr, "Failed to Initialize Free Type Library\n") ;
    }
    //load shader programs
    vertex_shader = glCreateShader(GL_VERTEX_SHADER) ;
    compile_shader("text_vertex_shader.glsl", vertex_shader) ;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER) ;
    compile_shader("text_fragment_shader.glsl", fragment_shader) ;
    shader_program = glCreateProgram() ;
    link_shader(shader_program, {vertex_shader, fragment_shader}) ;

    glActiveTexture(GL_TEXTURE0) ;
    glGenTextures(1, &tex) ;
    glBindTexture(GL_TEXTURE_2D, tex) ;
    glUniform1i(uniform_tex, 0) ;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) ;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) ;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1) ;

    glGenBuffers(1, &vbo) ;
    glEnableVertexAttribArray(attribute_coord) ;
    glBindBuffer(GL_ARRAY_BUFFER, vbo) ;
    glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0) ;
    instantiated_ = true ;
}
Text::~Text() {
    instantiated_ = false ;
}
void Text::load_font_face(std::string f_name) {
    FT_Error r = FT_New_Face(ft, f_name.c_str(), 0, &face) ;
    if (r) {
        fprintf(stderr, "Failed to open font: %s\n", f_name) ;
    }
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
    glUseProgram(shader_program) ;
    FT_GlyphSlot g ;
    float x2, y2, w, h ;
    GLfloat box[4][4] ;
    for(auto c : s) {
        if(FT_Load_Char(face, c, FT_LOAD_RENDER)) { continue ; }

        g = face->glyph ;

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_ALPHA,
            g->bitmap.width,
            g->bitmap.rows,
            0,
            GL_ALPHA,
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
}
