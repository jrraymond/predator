#include <iostream>
#include <fstream>
#include <limits>
#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <thread>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <unistd.h>
#include "lib/shader_utils/shader_utils.h"
#include "lib/gl_text/text.h"
#include "object.h"
#include "Flock.h"
#include "Player.h"
#include "GameState.h"
#include "Physics/PhysicsEngine.h"
#include "Graphics/GraphicsEngine.h"
#include "GameConfig.h"

//TODO finish 3d grid using instancing
using std::vector ;

const FlockConfig default_flock_config { 5, 5, 0.5, 1, 100, 0.1, 0.1};
const GameConfig default_game_config { default_flock_config, 5, 10, 10, 10 };

void handle_input(GLFWwindow *window, Player *p, glm::mat4 *view_ptr, const GameConfig config);
double inline sec_to_ms(double seconds);

vector<PhysicsObject> spawn_boids(int max_x, int max_y, int max_z, int sz, int sz_range, int number);

int main() {
    // -------------------------------- INIT ------------------------------- //
    if (glfwInit() != GL_TRUE) {
        fprintf(stderr, "Failed to initialize GLFW\n") ;
        return -1 ;
    }
    // Create a rendering window with OpenGL 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3) ;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2) ;
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE) ;
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE) ;
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE) ;

    int window_h = 600, window_w = 800 ;
    GLFWwindow* window = glfwCreateWindow(window_w, window_h, "OpenGL", NULL, NULL) ;
    glfwMakeContextCurrent(window) ;

    glewExperimental = GL_TRUE ;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n") ;
        return -1 ;
    }
    Text text ("OpenSans-Regular.ttf") ;
    int params = -1 ;
    int x_max = 1000, y_max = 1000, z_max = 1000 ;
    int frames_rendered = 0 ;
    int physics_updates = 0 ;
    // Boid information
    int num_boids, boid_s, vert_s;
    num_boids = 50 ;
    // init game state
    GameState game_state ;
    game_state.config = default_game_config;
    game_state.camera.view = glm::lookAt(
        glm::vec3(10.0f, 10.0f, 10.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    game_state.camera.proj = (glm::mat4) glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

    game_state.player = Player { glm::vec3(10.0f, 10.0f, 10.0f)
                               , glm::vec3(0.0f, 0.0f, 0.0f)
                               , glm::vec3(0.0f, 0.0f, 0.0f)
                               , 4.0f ,-0.6f, 45.0f } ;
    game_state.boid_physics_objs = spawn_boids(10, 10, 10, 1, 0, num_boids) ;
    InvertedBox bounding_box (0,0,0,20,20,20,1,1,1,1) ;
    vector<InvertedBox> iboxes;
    iboxes.push_back(bounding_box) ;
    game_state.iboxes = std::move(iboxes);
    GraphicsEngine graphics_engine (game_state);
    // ----------------------------- RESOURCES ----------------------------- //
    //graphics_engine.add_shaders("vertex_shader.glsl","fragment_shader.glsl");



    double last_time = sec_to_ms(glfwGetTime()) ;
    double this_time, elapsed_time ;
    double lag_time = 0.0f ;
    double ms_per_update = 16.0f ;

    // ---------------------------- RENDERING ------------------------------ //
    //TODO update game loop to do fixed update with variable rendering
    while(!glfwWindowShouldClose(window)) {
        this_time = sec_to_ms(glfwGetTime()) ;
        elapsed_time = this_time - last_time;
        lag_time += elapsed_time ;
        last_time = this_time ;

        handle_input(window, &game_state.player, &game_state.camera.view, game_state.config);

        while (lag_time >= ms_per_update) {
            ++physics_updates ;
            game_state = update_physics(game_state, ms_per_update) ;
            lag_time -= ms_per_update ;
        }

        ++frames_rendered ;
        graphics_engine.render(game_state, lag_time / ms_per_update);
        glfwSwapBuffers(window);
        glfwPollEvents() ;
        if (frames_rendered % 1000 == 0) {
            std::cout << "Frames rendered: " << frames_rendered << "\t physics updates: " << physics_updates << std::endl ;
        }
    }
    // ---------------------------- CLEARING ------------------------------ //

    // ---------------------------- TERMINATE ----------------------------- //
    glfwTerminate() ;

    return EXIT_SUCCESS ;
}

void handle_input(GLFWwindow *window, Player *p, glm::mat4 *view_ptr, const GameConfig config) {
    float speed = 100 ;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { p->h_angle -= 0.1f ; }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { p->h_angle += 0.1f ; }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { p->v_angle -= 0.1f ; }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { p->v_angle += 0.1f ; }
    glm::vec3 dir = glm::vec3( cos(p->v_angle) * sin(p->h_angle)
                             , sin(p->v_angle)
                             , cos(p->v_angle) * cos(p->h_angle)) ;
    glm::vec3 right = glm::vec3( sin(p->h_angle - 3.14f/2.0f)
                               , 0
                               , cos(p->h_angle - 3.14f/2.0f)) ;
    glm::vec3 up = glm::cross(right, dir) ;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { p->acc += right; }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { p->acc -= right;  }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { p->acc -= dir; }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { p->acc += dir; }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { p->acc += up; }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { p->acc -= up; }
    if (glm::length(p->acc) > 0) { p->acc = glm::normalize(p->acc);}
    p->acc *= speed;
    std::cout << "3" << fromGLM(p->acc) << std::endl;
    *view_ptr = glm::lookAt(p->pos + dir * -config.camera_distance, p->pos, up) ;
    //std::cout << p->pos.z << "," << p->pos.y << "," << p->pos.z << "\n" ;
    //p->vel = dir;
}


double inline sec_to_ms(double seconds) {
    return seconds * 1000 ;
}


vector<PhysicsObject> spawn_boids(int max_x, int max_y, int max_z, int sz, int sz_range, int number) {
    vector<PhysicsObject> objs ;
    objs.reserve(number) ;
    std::random_device                  rand_dev ;
    std::mt19937                        generator(rand_dev()) ;
    std::uniform_int_distribution<int>  r_x(-max_x, max_x) ;
    std::uniform_int_distribution<int>  r_y(-max_y, max_y) ;
    std::uniform_int_distribution<int>  r_z(-max_z, max_z) ;
    int x, y, z ;
    V3 p, v;
    V3 a {0,0,0};
    V3 f {0,0,0};
    float mass;
    float radius;
    for (int i = 0 ; i < number ; ++i) {
        x = r_x(generator) ; y = r_y(generator) ; z = r_z(generator) ;
        p = V3{(float) x, (float) y, (float) z} ;
        v = V3{(float) rand() / RAND_MAX * 2 - 1,
               (float) rand() / RAND_MAX * 2 - 1,
               (float) rand() / RAND_MAX * 2 - 1} ;
        mass = sz + sz_range * ((float) rand() / RAND_MAX * 2 - 1);
        radius = mass;
        PhysicsObject obj {mass, p, v, a, f, radius};
        objs.push_back(obj) ;
        //obj.debug("spawned: ");
    }
    return objs ;
}
