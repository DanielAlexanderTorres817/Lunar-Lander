#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "Entity.h"
#include <vector>
#include "cmath"
#include "Map.h"

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH = 640 * 1.5,
WINDOW_HEIGHT = 480 * 1.5;

constexpr float BG_RED = 0.9765625f,
BG_GREEN = 0.97265625f,
BG_BLUE = 0.9609375f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;
constexpr char SHIP_FILEPATH[] = "rocket.png";
constexpr char MAP_FILEPATH[] = "tilesheet.png";
constexpr char FONT_FILEPATH[] = "font1.png";




//--- BACKGROUND CYCLING VARS AND CONSTANTS HERE --//
constexpr char BG1_SPRITE[] = "bg1.png";
constexpr char BG2_SPRITE[] = "bg2.png";
constexpr char BG3_SPRITE[] = "bg3.png";
constexpr char BG4_SPRITE[] = "bg4.png";
constexpr char BG5_SPRITE[] = "bg5.png";
constexpr char BG6_SPRITE[] = "bg6.png";
constexpr char BG7_SPRITE[] = "bg7.png";
constexpr char BG8_SPRITE[] = "bg8.png";

GLuint g_bg1_texture_id;
GLuint g_bg2_texture_id;
GLuint g_bg3_texture_id;
GLuint g_bg4_texture_id;
GLuint g_bg5_texture_id;
GLuint g_bg6_texture_id;
GLuint g_bg7_texture_id;
GLuint g_bg8_texture_id;

GLuint g_bg_texture_ids[8];

GLuint g_font_texture_id;
constexpr int FONTBANK_SIZE = 16;

int g_background_idx = 0;
float g_bg_switch_time = 0.15f;
float g_prev_switch_time = 0.0f;




constexpr GLint NUMBER_OF_TEXTURES = 1,
LEVEL_OF_DETAIL = 0,
TEXTURE_BORDER = 0;

// ---- MAP VARIABLES ----//
unsigned int LEVEL_DATA[] =
{
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    40, 22, 22, 22, 22, 22, 22, 22, 22, 36, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    48, 22, 22, 22, 22, 22, 22, 20, 21, 36, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    56, 22, 22, 22, 22, 22, 22,  8, 10, 36, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
     0,  1, 21, 22, 22, 22, 22,  8, 28,  1,  1, 21, 20,  2, 22, 22, 22, 22, 22, 22,
     8,  9, 28, 21, 22, 22, 22,  8, 28, 28, 28, 28, 28, 10, 22, 22, 22, 22, 22, 51,
     8,  9, 28, 28, 21, 22, 22,  8, 28, 28, 28, 28, 28, 10, 22, 20, 21, 20,  1,  1,
     8,  9, 28, 28, 28,  2, 22,  8, 28, 28, 28, 28, 28, 10, 22,  8, 28, 28, 28, 28,
     8, 28, 28, 28, 28, 10, 22,  8, 28, 28, 28, 28, 28, 10, 22,  8, 28, 28, 28, 28,
};





// ————— STRUCTS AND ENUMS —————//
enum AppStatus { RUNNING, TERMINATED };
enum FilterType { NEAREST, LINEAR };
enum lastDirection { L, R, NONE};
enum playStatus {PAUSED, STARTED, WON, LOST};

struct GameState
{
    Entity* player;
    Map* map;
    
    
};

// ————— VARIABLES ————— //
GameState g_game_state;
lastDirection g_last_direction = NONE;
playStatus g_play_status = PAUSED;

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix, g_bg_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

void initialise();
void process_input();
void update();
void render();
void shutdown();



// ———— GENERAL FUNCTIONS ———— //
GLuint load_texture(const char* filepath, FilterType filterType)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER,
        GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        filterType == NEAREST ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
        filterType == NEAREST ? GL_NEAREST : GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}

//OLD LOAD TEXTURE FOR BG
GLuint old_load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}
void draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text,
    float font_size, float spacing, glm::vec3 position)
{
    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    // Instead of having a single pair of arrays, we'll have a series of pairs—one for
    // each character. Don't forget to include <vector>!
    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    // For every character...
    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset (i.e. their
        //    position relative to the whole sentence)
        int spritesheet_index = (int)text[i];  // ascii value of character
        float offset = (font_size + spacing) * i;

        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Inset the current pair in both vectors
        vertices.insert(vertices.end(), {
            offset + (-0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
            });
    }

    // 4. And render all of them using the pairs
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);

    program->set_model_matrix(model_matrix);
    glUseProgram(program->get_program_id());

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0,
        vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0,
        texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Lunar Lander",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr)
    {
        std::cerr << "Error: SDL window could not be created.\n";
        shutdown();
    }


#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // ————— PLAYER ————— //
    GLuint player_texture_id = load_texture(SHIP_FILEPATH, NEAREST);
    

    int player_walking_animation[4][4] =
    {
        { 1, 5, 9, 13 },  // for George to move to the left,
        { 3, 7, 11, 15 }, // for George to move to the right,
        { 2, 6, 10, 14 }, // for George to move upwards,
        { 0, 4, 8, 12 }   // for George to move downwards
    };

    g_game_state.player = new Entity(player_texture_id, 1.0f, 1.0f, 1.0f);

    
    g_game_state.player->set_position(glm::vec3(0.0f, 2.5f, 0.0f));
    

    // ————— MAP & TEXT ————— //
    GLuint map_texture_id = old_load_texture(MAP_FILEPATH);
    g_font_texture_id = old_load_texture(FONT_FILEPATH);
    g_game_state.map = new Map(20, 15, LEVEL_DATA, map_texture_id, 0.5f, 8, 8 );
    // ————— BACKGROUND ————— //
    g_bg_matrix = glm::mat4(1.0f);
    //now loading the animated pngs 
    g_bg_texture_ids[0] = old_load_texture(BG1_SPRITE);
    g_bg_texture_ids[1] = old_load_texture(BG2_SPRITE);
    g_bg_texture_ids[2] = old_load_texture(BG3_SPRITE);
    g_bg_texture_ids[3] = old_load_texture(BG4_SPRITE);
    g_bg_texture_ids[4] = old_load_texture(BG5_SPRITE);
    g_bg_texture_ids[5] = old_load_texture(BG6_SPRITE);
    g_bg_texture_ids[6] = old_load_texture(BG7_SPRITE);
    g_bg_texture_ids[7] = old_load_texture(BG8_SPRITE);

    g_bg_matrix = glm::translate(g_bg_matrix, glm::vec3(0.0f, 0.0f, -1.0f));
    g_bg_matrix = glm::scale(g_bg_matrix, glm::vec3(10.0f, 10.0f, 1.0f));

    // ————— GENERAL ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    g_game_state.player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q: g_app_status = TERMINATED;
            default:     break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    glm::vec3 accel = g_game_state.player->get_acceleration();
    glm::vec3 mov = g_game_state.player->get_movement();
    if (key_state[SDL_SCANCODE_LSHIFT]) {
        glm::vec3 acceleration = glm::vec3(0.f, -0.2f, 0.0f);
        g_game_state.player->set_acceleration(acceleration);
        g_play_status = STARTED;
        
    }
    
    if (key_state[SDL_SCANCODE_D])
    {
        mov.x = -0.5f;

        g_last_direction = L;
        if (accel.x <= 0.55f) {
            accel.x += 0.1085f;
        }
        g_game_state.player->set_movement(mov);
        
    }
    else if (key_state[SDL_SCANCODE_A])
    {
        g_game_state.player->move_right();
        mov.x = 0.5f;
        g_last_direction = R;
        if (accel.x >= -0.55f) {
            accel.x -= 0.0085f;
        }
                
        g_game_state.player->set_movement(mov);
    }
    if (!key_state[SDL_SCANCODE_D] && !key_state[SDL_SCANCODE_A]) {
        if (accel.x > 0) { accel.x -= 0.004f; }
        else { accel.x += 0.00f; }
    }
    else { mov.x = 0; g_game_state.player->set_movement(mov); }
    
    if (key_state[SDL_SCANCODE_SPACE]) {
        

        if (accel.y <= 0.75f) {
            accel.y += 0.01f;
        }

        if (g_last_direction == L) {
            if (accel.x <= 0.10f) {
                accel.x += 0.001f;
        }
        }

        g_game_state.player->set_acceleration(accel);
    }
    else {
        glm::vec3 accel = g_game_state.player->get_acceleration();

        if (accel.y >= -0.8f) {
            accel.y -= 0.05f;
        }

        if (accel.x > 0.0f) {
            accel.x -= 0.005f;
            if (accel.x < 0.0f) accel.x = 0.0f;
        }
        else if (accel.x < 0.0f) {
            accel.x += 0.005f;
            if (accel.x > 0.0f) accel.x = 0.0f;
        }

        g_game_state.player->set_acceleration(accel);
        

    }

    if (glm::length(g_game_state.player->get_movement()) > 1.0f) {
        g_game_state.player->normalise_movement();
    }
};


void update()
{
    

    
        
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
        float delta_time = ticks - g_previous_ticks;
        g_previous_ticks = ticks;

        delta_time += g_accumulator;

        if (delta_time < FIXED_TIMESTEP)
        {
            g_accumulator = delta_time;
            return;
        }

        while (delta_time >= FIXED_TIMESTEP)
        {
            g_game_state.player->update(FIXED_TIMESTEP, g_game_state.player, NULL, 0,
                g_game_state.map);
            if (g_game_state.map->win_status == 'W') { g_play_status = WON; return; }
            if (g_game_state.map->win_status == 'L') { g_play_status = LOST; return; }
            delta_time -= FIXED_TIMESTEP;
        }

        g_accumulator = delta_time;



        // ————— BACKGROUND ————— //
        g_prev_switch_time += delta_time;

        if (g_prev_switch_time >= g_bg_switch_time) {
            g_background_idx += 1;
            g_background_idx %= 8;
            g_prev_switch_time = 0.0f;
        }
    
     
}

void draw_object(glm::mat4& g_object_model_matrix, GLuint& g_object_texture_id)
{
    g_shader_program.set_model_matrix(g_object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, g_object_texture_id);

    // Vertices
    float vertices[] =
    {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] =
    {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
}


void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
  

    // Render the background first
    draw_object(g_bg_matrix, g_bg_texture_ids[g_background_idx]);
    

    g_game_state.player->render(&g_shader_program);
    g_game_state.map->render(&g_shader_program);

    if (g_play_status == PAUSED) {
        draw_text(&g_shader_program, g_font_texture_id, "Press SPACE,[A],or [D]!", 0.3f, 0.0f,
            glm::vec3(-3.1f, 2.5f, 0.0f));
    }
    else if (g_play_status == WON) {
        draw_text(&g_shader_program, g_font_texture_id, "YOU WON!", 0.3f, 0.0f,
            glm::vec3(-1.1f, 2.5f, 0.0f));
    }
    else if (g_play_status == LOST) {
        draw_text(&g_shader_program, g_font_texture_id, "YOU LOST!", 0.3f, 0.0f,
            glm::vec3(-1.1f, 2.5f, 0.0f));
    }

    SDL_GL_SwapWindow(g_display_window);
}


void shutdown()
{
    SDL_Quit();
    delete g_game_state.player;
    delete g_game_state.map;
    
}


int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
        if (g_play_status == WON || g_play_status == LOST) {
            SDL_Delay(5000);
            g_app_status = TERMINATED;

        }
        
    }

    shutdown();
    return 0;
}