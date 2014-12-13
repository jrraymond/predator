#ifndef TEXT_H
#define TEXT_H
#include <string>
#include <ft2build.h>
#include <fts.h>
#include <GL/glew.h>
#include FT_FREETYPE_H
#include "shader_utils.h"
class Text {
private:
    FT_Library ft ;
    FT_Face face ;
    GLuint vertex_shader ;
    GLuint fragment_shader ;
    GLuint shader_program ;
    GLuint tex ;
    GLuint vbo ;
    int uniform_tex ;
    GLuint attribute_coord ;
    static bool instantiated_ ;
public:
    Text() ;
    void load_font_face(std::string f_name) ;
    void set_pixel_size(int i) ;
    void render(std::string s, float x, float y, float sx, float sy) ;
    ~Text() ;
} ;
#endif