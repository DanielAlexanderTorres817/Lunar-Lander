/**
* Author: Daniel Torres
* Assignment: Lunar Lander
* Date due: 2024-10-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#include "Map.h"

Map::Map(int width, int height, unsigned int *level_data, GLuint texture_id, float tile_size, int tile_count_x, int tile_count_y) : m_width(width), m_height(height),
    m_level_data(level_data), m_texture_id(texture_id), m_tile_size(tile_size), m_tile_count_x(tile_count_x), m_tile_count_y(tile_count_y)
{
    build();
}

void Map::build()
{
    // Since this is a 2D map, we need a nested for-loop
    for(int y_coord = 0; y_coord < m_height; y_coord++)
    {
        for(int x_coord = 0; x_coord < m_width; x_coord++)
        {
            // Get the current tile
            int tile = m_level_data[y_coord * m_width + x_coord];
            
            // If the tile number is 0 i.e. not solid, skip to the next one
            if (tile == 22) continue;
            
            // Otherwise, calculate its UV-coordinates
            float u_coord = (float) (tile % m_tile_count_x) / (float) m_tile_count_x;
            float v_coord = (float) (tile / m_tile_count_x) / (float) m_tile_count_y;
            
            // And work out their dimensions and posititions
            float tile_width = 1.0f/ (float)  m_tile_count_x;
            float tile_height = 1.0f/ (float) m_tile_count_y;
            
            float x_offset = -(m_left_bound / 2); // From center of tile
            float y_offset =  (m_top_bound  / 2); // From center of tile
            
            // So we can store them inside our std::vectors
            m_vertices.insert(m_vertices.end(), {
                x_offset + (m_tile_size * x_coord),  y_offset +  -m_tile_size * y_coord,
                x_offset + (m_tile_size * x_coord),  y_offset + (-m_tile_size * y_coord) - m_tile_size,
                x_offset + (m_tile_size * x_coord) + m_tile_size, y_offset + (-m_tile_size * y_coord) - m_tile_size,
                x_offset + (m_tile_size * x_coord), y_offset + -m_tile_size * y_coord,
                x_offset + (m_tile_size * x_coord) + m_tile_size, y_offset + (-m_tile_size * y_coord) - m_tile_size,
                x_offset + (m_tile_size * x_coord) + m_tile_size, y_offset +  -m_tile_size * y_coord
            });
            
            m_texture_coordinates.insert(m_texture_coordinates.end(), {
                u_coord, v_coord,
                u_coord, v_coord + (tile_height),
                u_coord + tile_width, v_coord + (tile_height),
                u_coord, v_coord,
                u_coord + tile_width, v_coord + (tile_height),
                u_coord + tile_width, v_coord
            });
        }
    }
    
    // The bounds are dependent on the size of the tiles
    m_left_bound   = 0 - (m_tile_size / 2);
    m_right_bound  = (m_tile_size * m_width) - (m_tile_size / 2);
    m_top_bound    = 0 + (m_tile_size / 2);
    m_bottom_bound = -(m_tile_size * m_height) + (m_tile_size / 2);
}

void Map::render(ShaderProgram *program)
{
    glm::mat4 model_matrix = glm::mat4(1.0f);

    float left_bound_offset = -(m_width * m_tile_size) / 2.0f;
    float top_bound_offset = (m_height * m_tile_size) / 2.0f;
    model_matrix = glm::translate(model_matrix, glm::vec3(left_bound_offset, top_bound_offset, 0.0f));

    program->set_model_matrix(model_matrix);
    
    glUseProgram(program->get_program_id());
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, m_vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, m_texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    
    glDrawArrays(GL_TRIANGLES, 0, (int) m_vertices.size() / 2);
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool Map::is_solid(glm::vec3 position, float* penetration_x, float* penetration_y)

{
    float left_bound_offset = -(m_width * m_tile_size) / 2.0f;
    float top_bound_offset = (m_height * m_tile_size) / 2.0f;

    // Adjust position by the map's offset
    float adjusted_x = position.x - left_bound_offset;
    float adjusted_y = position.y - top_bound_offset;

    // If we are out of bounds, it is not solid
    if (adjusted_x < m_left_bound || adjusted_x > m_right_bound)  return false;
    if (adjusted_y > m_top_bound || adjusted_y < m_bottom_bound) return false;

    int tile_x = floor((adjusted_x + (m_tile_size / 2)) / m_tile_size);
    int tile_y = -(ceil(adjusted_y - (m_tile_size / 2)) / m_tile_size); // Our array counts up as Y goes down.

    // Check boundaries
    if (tile_x < 0 || tile_x >= m_width)  return false;
    if (tile_y < 0 || tile_y >= m_height) return false;

    // Check tile solidity
    int tile = m_level_data[tile_y * m_width + tile_x];
    if (tile == 22) return false;

    if (tile == 20 || tile == 21 || tile == 51 || tile == 56 || tile == 48 || tile == 40) {
        this->win_status = 'L';
        return true;

    }

    // Collision adjustments for penetration distances
    float tile_center_x = (tile_x * m_tile_size) + left_bound_offset;
    float tile_center_y = -(tile_y * m_tile_size) + top_bound_offset;

    /*if (tile == 20 || tile == 21) {
        *penetration_x = (m_tile_size / 2) - fabs(position.x - tile_center_x);
        *penetration_y = (m_tile_size / 2) - fabs(position.y - tile_center_y);
    }*/
    
    *penetration_x = (m_tile_size / 4) - fabs(position.x - tile_center_x);
    *penetration_y = (m_tile_size / 4) - fabs(position.y - tile_center_y);

    this->win_status = 'W';
    
   

    return true;
}

