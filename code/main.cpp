#include <stdio.h> // sprintf
#include <assert.h>

#include "glad\glad.h"
#include "GLFW\glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glad.c"

#define PI 3.14159265359f
#define TWO_PI (2.0f * PI)

#define MEMORY_KB(x) ((x) * 1024ull)
#define MEMORY_MB(x) MEMORY_KB((x) * 1024ull)
#define MEMORY_GB(x) MEMORY_MB((x) * 1024ull)
#define TO_RADIANS(deg) ((PI / 180.0f) * deg)

struct Vec3f
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
        };

        struct
        {
            float pitch;
            float roll;
            float yaw;
        };

        float v[3];
    };

    Vec3f(float xx = 0.0f, float yy = 0.0f, float zz = 0.0f) : x(xx), y(yy), z(zz) {}
};

struct Vec4f
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };

        float v[4];
    };

    Vec4f(float xx = 0.0f, float yy = 0.0f, float zz = 0.0f, float ww = 0.0f) : x(xx), y(yy), z(zz), w(ww) {}
};

struct Mat4x4f
{
    float m[4][4];

    Mat4x4f()
    {
        float *p = &m[0][0];
        for (int i = 0; i < 4 * 4; i++)
        {
            *p++ = 0.0f;
        }
    }
};

struct Mat3x3f
{
    float m[3][3];
    Mat3x3f()
    {
        float *p = &m[0][0];
        for (int i = 0; i < 3 * 3; i++)
        {
            *p++ = 0.0f;
        }
    }
};

Vec3f operator-(const Vec3f &a)
{
    return Vec3f(-a.x, -a.y, -a.z);
}

Vec3f operator+(const Vec3f &a, const Vec3f &b)
{
    return Vec3f(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3f operator-(const Vec3f &a, const Vec3f &b)
{
    return Vec3f(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3f operator*(const Vec3f &a, float f)
{
    return Vec3f(a.x * f, a.y * f, a.z * f);
}

Vec3f operator*(float f, const Vec3f &a)
{
    return Vec3f(a.x * f, a.y * f, a.z * f);
}

Vec3f operator/(const Vec3f &a, float f)
{
    return Vec3f(a.x / f, a.y / f, a.z / f);
}

inline float length(const Vec3f &v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline float length2(const Vec3f &v)
{
    return (v.x * v.x + v.y * v.y + v.z * v.z);
}

inline float dot(const Vec3f &a, const Vec3f &b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

inline Vec3f cross(const Vec3f &a, const Vec3f &b)
{
    Vec3f result;

    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;

    return (result);
}

inline Vec3f normalize(const Vec3f &v)
{
    Vec3f result;

    float len = length(v);
    if (len > 0.0001f)
    {
        result.x = v.x / len;
        result.y = v.y / len;
        result.z = v.z / len;
    }

    return (result);
}

Mat3x3f mat3x3f_identity(void)
{
    Mat3x3f result;

    result.m[0][0] = 1.0f;
    result.m[1][1] = 1.0f;
    result.m[2][2] = 1.0f;

    return (result);
}

Vec4f vec4f_mul(const Vec4f &v, const Mat4x4f &m)
{
    Vec4f result;

    for (int j = 0; j < 4; j++)
    {
        for (int k = 0; k < 4; k++)
        {
            result.v[j] += m.m[k][j] * v.v[k];
        }
    }

    return (result);
}

Vec3f vec3f_mul(const Vec3f &v, const Mat3x3f &m)
{
    Vec3f result;

    for (int j = 0; j < 3; j++)
    {
        for (int k = 0; k < 3; k++)
        {
            result.v[j] += v.v[k] * m.m[k][j];
        }
    }

    return (result);
}

Mat4x4f mat4x4f_mul(const Mat4x4f &m0, const Mat4x4f &m1)
{
    Mat4x4f result;

    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int k = 0; k < 4; k++)
            {
                result.m[j][i] += m0.m[j][k] * m1.m[k][i];
            }
        }
    }

    return (result);
}

Mat4x4f mat4x4f_transpose(const Mat4x4f &m)
{
    Mat4x4f result;

    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            result.m[i][j] = m.m[j][i];
        }
    }

    return (result);
}

Mat4x4f mat4x4f_identity(void)
{
    Mat4x4f result;

    result.m[0][0] = 1.0f;
    result.m[1][1] = 1.0f;
    result.m[2][2] = 1.0f;
    result.m[3][3] = 1.0f;

    return (result);
}

Mat4x4f mat4x4f_rotate_x(const Mat4x4f &m, float angle)
{
    Mat4x4f result;

    result.m[0][0] = 1.0f;
    result.m[1][1] = cosf(angle);
    result.m[2][2] = cosf(angle);
    result.m[3][3] = 1.0f;

    result.m[1][2] = -sinf(angle);
    result.m[2][1] = sinf(angle);

    return mat4x4f_mul(m, mat4x4f_transpose(result));
}

Mat4x4f mat4x4f_rotate_y(const Mat4x4f &m, float angle)
{
    Mat4x4f result;

    result.m[0][0] = cosf(angle);
    result.m[1][1] = 1.0f;
    result.m[2][2] = cosf(angle);
    result.m[3][3] = 1.0f;

    result.m[0][2] = sinf(angle);
    result.m[2][0] = -sinf(angle);

    return mat4x4f_mul(m, mat4x4f_transpose(result));
}

Mat4x4f mat4x4f_translate(const Mat4x4f &m, const Vec3f &v)
{
    Mat4x4f result = mat4x4f_identity();

    result.m[3][0] = v.x;
    result.m[3][1] = v.y;
    result.m[3][2] = v.z;

    return mat4x4f_mul(m, result);
}

Mat4x4f mat4x4f_perspective(float fov, float aspect_ration, float n, float f)
{
    Mat4x4f result;

    float ctg = 1.0f / tanf(fov * (PI / 360.0f));

    result.m[0][0] = ctg / aspect_ration;
    result.m[1][1] = ctg;
    result.m[2][3] = -1.0f;
    result.m[2][2] = (n + f) / (n - f);
    result.m[3][2] = (2.0f * n * f) / (n - f);

    return (result);
}

void mat3x3f_set_col(Mat3x3f &m, const Vec3f &v, int col)
{
    m.m[0][col] = v.x;
    m.m[1][col] = v.y;
    m.m[2][col] = v.z;
}

void mat3x3f_set_row(Mat3x3f &m, const Vec3f &v, int row)
{
    m.m[row][0] = v.x;
    m.m[row][1] = v.y;
    m.m[row][2] = v.z;
}

Mat3x3f mat3x3f_transpose(const Mat3x3f &m)
{
    Mat3x3f result;

    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            result.m[i][j] = m.m[j][i];
        }
    }

    return (result);
}

void mat4x4f_set_row(Mat4x4f &m, const Vec4f &v, int row)
{
    m.m[row][0] = v.x;
    m.m[row][1] = v.y;
    m.m[row][2] = v.z;
    m.m[row][3] = v.w;
}

Mat4x4f mat4x4f_lookat(const Vec3f &from, const Vec3f &to, const Vec3f &up)
{
    Vec3f z_dir = normalize(from - to);
    Vec3f x_dir = normalize(cross(up, z_dir));
    Vec3f y_dir = normalize(cross(z_dir, x_dir));

    Mat4x4f result;

    result.m[0][0] = x_dir.x;
    result.m[1][0] = x_dir.y;
    result.m[2][0] = x_dir.z;
    result.m[3][0] = dot(-from, x_dir);

    result.m[0][1] = y_dir.x;
    result.m[1][1] = y_dir.y;
    result.m[2][1] = y_dir.z;
    result.m[3][1] = dot(-from, y_dir);

    result.m[0][2] = z_dir.x;
    result.m[1][2] = z_dir.y;
    result.m[2][2] = z_dir.z;
    result.m[3][2] = dot(-from, z_dir);

    result.m[3][3] = 1.0f;
    
    return (result);
}

struct Button
{
    int is_pressed;
    int was_pressed;
};

struct Game_input
{
    float aspect_ratio;
    float dt;

    float mouse_dx;
    float mouse_dy;
    
    union
    {
        struct
        {
            Button mleft;
            Button mright;

            Button w;
            Button a;
            Button s;
            Button d;

            Button enter;
            Button space;
            Button lshift;
        };

        Button buttons[9];
    };
};

struct Game_memory
{
    int is_initialized;

    uint64_t permanent_mem_size;
    void *permanent_mem;

    uint64_t transient_mem_size;
    void *transient_mem;
};

#define CHUNK_DIM_LOG2 4
#define CHUNK_DIM (1 << 4)

struct Chunk
{
    int x;
    int y;
    int z;
    uint8_t blocks[CHUNK_DIM * CHUNK_DIM * CHUNK_DIM];
};

struct World
{
    int num_of_chunks;
    Chunk chunks[4];
};

struct Game_state
{
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint shader_program;

    GLuint vao;
    GLuint tex;

    Vec3f cam_pos;
    Vec3f cam_view_dir;
    Vec3f cam_move_dir;
    Vec3f cam_up;
    Vec3f cam_rot;

    World world;
};

struct Raycast_result
{
    bool collision;
    float last_t;
    Chunk *chunk;

    int i;
    int j;
    int k;
    
    int last_i;
    int last_j;
    int last_k;
};

Raycast_result raycast(World *world, Vec3f pos, Vec3f view_dir)
{
    Raycast_result result = {};

    bool collision = false;
    float last_t = 0.0f;
    
    float tile_dim = 1.0f;
    float tolerance = 0.001f;

    float start_x = pos.x;
    float start_y = pos.y;
    float start_z = pos.z;

    float max_ray_len = 3.0f;
    Vec3f end_point = pos + max_ray_len * view_dir;
    float end_x = end_point.x;
    float end_y = end_point.y;
    float end_z = end_point.z;

    int i = (int)floorf(start_x / tile_dim);
    int j = (int)floorf(start_y / tile_dim);
    int k = (int)floorf(start_z / tile_dim);

    int i_end = (int)floorf(end_x / tile_dim);
    int j_end = (int)floorf(end_y / tile_dim);
    int k_end = (int)floorf(end_z / tile_dim);

    int di = (start_x < end_x) ? 1 : ((start_x > end_x) ? -1 : 0);
    int dj = (start_y < end_y) ? 1 : ((start_y > end_y) ? -1 : 0);
    int dk = (start_z < end_z) ? 1 : ((start_z > end_z) ? -1 : 0);

    float min_x = tile_dim * floorf(start_x / tile_dim);
    float max_x = min_x + tile_dim;
    float t_x = INFINITY;
    if (fabs(end_x - start_x) > tolerance)
    {
        t_x = ((start_x < end_x) ? (max_x - start_x) : (start_x - min_x)) / fabsf(end_x - start_x);
    }

    float min_y = tile_dim * floorf(start_y / tile_dim);
    float max_y = min_y + tile_dim;
    float t_y = INFINITY;
    if (fabs(end_y - start_y) > tolerance)
    {
        t_y = ((start_y < end_y) ? (max_y - start_y) : (start_y - min_y)) / fabsf(end_y - start_y);
    }

    float min_z = tile_dim * floorf(start_z / tile_dim);
    float max_z = min_z + tile_dim;
    float t_z = INFINITY;
    if (fabs(end_z - start_z) > tolerance)
    {
        t_z = ((start_z < end_z) ? (max_z - start_z) : (start_z - min_z)) / fabsf(end_z - start_z);
    }

    float dt_x = INFINITY;
    if (fabs(end_x - start_x) > tolerance)
    {
        dt_x = tile_dim / fabsf(end_x - start_x);
    }

    float dt_y = INFINITY;
    if (fabs(end_y - start_y) > tolerance)
    {
        dt_y = tile_dim / fabsf(end_y - start_y);
    }

    float dt_z = INFINITY;
    if (fabs(end_z - start_z) > tolerance)
    {
        dt_z = tile_dim / fabsf(end_z - start_z);
    }

    Chunk *chunk = 0;
    int last_di = 0;
    int last_dj = 0;
    int last_dk = 0;
    for (;;)
    {
        int chunk_x = i >> CHUNK_DIM_LOG2;
        int chunk_y = j >> CHUNK_DIM_LOG2;
        int chunk_z = k >> CHUNK_DIM_LOG2;
        for (int idx = 0; idx < world->num_of_chunks; idx++)
        {
            Chunk *c = &world->chunks[idx];
            if ((chunk_x == c->x) && (chunk_y == c->y) && (chunk_z == c->z))
            {
                int mask = ~((~1) << (CHUNK_DIM_LOG2 - 1));
                int block_x = i & mask;
                int block_y = j & mask;
                int block_z = k & mask;

                if (c->blocks[CHUNK_DIM * CHUNK_DIM * block_z + CHUNK_DIM * block_y + block_x])
                {
                    chunk = c;
                    collision = true;
                    goto end_loop;
                }
            }
        }

        if (i == i_end && j == j_end && k == k_end)
        {
            break;
        }

        if (t_x <= t_y && t_x <= t_z)
        {
            last_di = di;
            last_dj = 0;
            last_dk = 0;

            last_t = t_x;
            t_x += dt_x;
            i += di;
        }
        else if (t_y <= t_x && t_y <= t_z)
        {
            last_di = 0;
            last_dj = dj;
            last_dk = 0;

            last_t = t_y;
            t_y += dt_y;
            j += dj;
        }
        else
        {
            last_di = 0;
            last_dj = 0;
            last_dk = dk;

            last_t = t_z;
            t_z += dt_z;
            k += dk;
        }
    }

end_loop:
    float dx2 = (end_x - start_x) * (end_x - start_x);
    float dy2 = (end_y - start_y) * (end_y - start_y);
    float dz2 = (end_z - start_z) * (end_z - start_z);
    last_t *= sqrtf(dx2 + dy2 + dz2);

    result.collision = collision;
    result.i = i;
    result.j = j;
    result.k = k;
    result.last_i = i - last_di;
    result.last_j = j - last_dj;
    result.last_k = k - last_dk;
    result.last_t = last_t;
    result.chunk = chunk;

    return (result);
}

const char *vertex_shader_src = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aVertexPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "uniform mat4 u_projection;\n"
    "uniform mat4 u_view;\n"
    "uniform mat4 u_model;\n"
    "out vec2 tex_coord;"
    "void main() {\n"
    "   tex_coord = aTexCoord;\n"
    "   gl_Position = u_projection * u_view * u_model * vec4(aVertexPos, 1.0f);\n"
    "}\n\0";

const char *fragment_shader_src =
    "#version 330 core\n"
    "uniform vec3 u_color;\n"
    "uniform sampler2D tex0;\n"
    "in vec2 tex_coord;\n"
    "out vec4 frag_color;\n"
    "void main() {\n"
    "   frag_color = texture(tex0, tex_coord);\n"
    "}\n\0";

void game_update_and_render(Game_input *input, Game_memory *memory)
{
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    assert(sizeof(Game_state) <= memory->permanent_mem_size);
    Game_state *state = (Game_state *)memory->permanent_mem;
    if (!memory->is_initialized)
    {
        memory->is_initialized = 1;

        /* init chunk (0, 0) */ {
            Chunk *c = &state->world.chunks[0];
            c->x = 0;
            c->y = 0;
            c->z = 0;

            int i = 0;
            for (int y = 0; y < 4; y++)
            {
                for (int z = 0; z < CHUNK_DIM; z++)
                {
                    for (int x = 0; x < CHUNK_DIM; x++)
                    {
                        c->blocks[CHUNK_DIM * CHUNK_DIM * z + CHUNK_DIM * y + x] = 1;
                        i++;
                    }
                }
            }
        }

        /* init chunk (-1, -1) */ {
            Chunk *c = &state->world.chunks[1];
            c->x = -1;
            c->y = 0;
            c->z = -1;

            int i = 0;
            for (int y = 0; y < 6; y++)
            {
                for (int z = 0; z < CHUNK_DIM; z++)
                {
                    for (int x = 0; x < CHUNK_DIM; x++)
                    {
                        if (i % 3 == 0)
                            c->blocks[CHUNK_DIM * CHUNK_DIM * z + CHUNK_DIM * y + x] = 1;
                        i++;
                    }
                }
            }
        }

        state->world.num_of_chunks = 2;
        // TODO(max): add assert

        state->cam_pos = Vec3f(0, 20, 0);
        state->cam_up = Vec3f(0, 1, 0);

        state->cam_rot.pitch = 0.0f;
        state->cam_rot.roll = 0.0f;
        state->cam_rot.yaw = -90.0f;

        state->cam_view_dir.x = cosf(TO_RADIANS(state->cam_rot.yaw)) * cosf(TO_RADIANS(state->cam_rot.pitch));
        state->cam_view_dir.y = sinf(TO_RADIANS(state->cam_rot.pitch));
        state->cam_view_dir.z = sinf(TO_RADIANS(state->cam_rot.yaw)) * cosf(TO_RADIANS(state->cam_rot.pitch));

        state->cam_move_dir.x = state->cam_view_dir.x;
        state->cam_move_dir.y = 0.0f;
        state->cam_move_dir.z = state->cam_view_dir.z;

        state->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(state->vertex_shader, 1, &vertex_shader_src, 0);
        glCompileShader(state->vertex_shader);

        GLint success = 0;

        glGetShaderiv(state->vertex_shader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE)
        {
            char log_buf[256] = {};
            glGetShaderInfoLog(state->vertex_shader, sizeof(log_buf), 0, log_buf);
            OutputDebugString(log_buf);
            assert(0);
        }

        state->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(state->fragment_shader, 1, &fragment_shader_src, 0);
        glCompileShader(state->fragment_shader);

        glGetShaderiv(state->fragment_shader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE)
        {
            char log_buf[256] = {};
            glGetShaderInfoLog(state->fragment_shader, sizeof(log_buf), 0, log_buf);
            OutputDebugString(log_buf);
            assert(0);
        }

        state->shader_program = glCreateProgram();
        glAttachShader(state->shader_program, state->vertex_shader);
        glAttachShader(state->shader_program, state->fragment_shader);
        glLinkProgram(state->shader_program);

        glGetProgramiv(state->shader_program, GL_LINK_STATUS, &success);
        if (success == GL_FALSE)
        {
            char log_buf[256] = {};
            glGetProgramInfoLog(state->shader_program, sizeof(log_buf), 0, log_buf);
            
            glDeleteProgram(state->shader_program);
            glDeleteShader(state->vertex_shader);
            glDeleteShader(state->fragment_shader);

            OutputDebugString(log_buf);
            assert(0);
        }


        int tex_width = 0;
        int tex_height = 0;
        int num_of_channels = 0;
        uint8_t *texture_data = stbi_load("..\\code\\wall.jpg", &tex_width, &tex_height, &num_of_channels, 0);

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);

        stbi_image_free(texture_data);

        state->tex = texture;


        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        state->vao = vao;
    }

    /* logic update */
    {
        state->cam_rot.pitch += input->mouse_dy;
        state->cam_rot.yaw += input->mouse_dx;

        if (state->cam_rot.pitch > 89.0f)
        {
            state->cam_rot.pitch = 89.0f;
        }
        if (state->cam_rot.pitch < -89.0f)
        {
            state->cam_rot.pitch = -89.0f;
        }

        Vec3f view_dir;
        view_dir.x = cosf(TO_RADIANS(state->cam_rot.yaw)) * cosf(TO_RADIANS(state->cam_rot.pitch));
        view_dir.y = sinf(TO_RADIANS(state->cam_rot.pitch));
        view_dir.z = sinf(TO_RADIANS(state->cam_rot.yaw)) * cosf(TO_RADIANS(state->cam_rot.pitch));
        state->cam_view_dir = normalize(view_dir);

        Vec3f move_dir;
        move_dir = state->cam_view_dir;
        move_dir.y = 0.0f;
        state->cam_move_dir = normalize(move_dir);

        float cam_speed = 5.0f * input->dt;
        if (input->w.is_pressed)
        {
            state->cam_pos = state->cam_pos + state->cam_move_dir * cam_speed;
        }
        if (input->s.is_pressed)
        {
            state->cam_pos = state->cam_pos + state->cam_move_dir * -cam_speed;
        }
        
        Vec3f right = cross(state->cam_move_dir, state->cam_up);
        if (input->d.is_pressed)
        {
            state->cam_pos = state->cam_pos + right * cam_speed;
        }
        if (input->a.is_pressed)
        {
            state->cam_pos = state->cam_pos + right * -cam_speed;
        }

        if (input->space.is_pressed)
        {
            state->cam_pos = state->cam_pos + Vec3f(0, cam_speed, 0);
        }
        if (input->lshift.is_pressed)
        {
            state->cam_pos = state->cam_pos + Vec3f(0, -cam_speed, 0);
        }

        if (input->mleft.is_pressed)
        {
            Raycast_result rc = raycast(&state->world, state->cam_pos, state->cam_view_dir);
            if (rc.collision == true)
            {
                int mask = ~((~1) << (CHUNK_DIM_LOG2 - 1));
                int block_x = rc.i & mask;
                int block_y = rc.j & mask;
                int block_z = rc.k & mask;

                rc.chunk->blocks[CHUNK_DIM * CHUNK_DIM * block_z + CHUNK_DIM * block_y + block_x] = 0;
            }
        }
    }
    
    /* rendering */
    {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0, 1, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(state->shader_program);

        Mat4x4f projection = mat4x4f_perspective(90.0f, input->aspect_ratio, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(state->shader_program, "u_projection"), 1, GL_FALSE, &projection.m[0][0]);

        Mat4x4f view = mat4x4f_lookat(state->cam_pos, state->cam_pos + state->cam_view_dir, state->cam_up);
        glUniformMatrix4fv(glGetUniformLocation(state->shader_program, "u_view"), 1, GL_FALSE, &view.m[0][0]);

        for (int chunk_id = 0; chunk_id < state->world.num_of_chunks; chunk_id++)
        {
            float block_offset = 0.5f;
            Chunk *c = &state->world.chunks[chunk_id];
            Vec3f chunk_offset(
                (float)(c->x * CHUNK_DIM) + block_offset,
                (float)(c->y * CHUNK_DIM) + block_offset,
                (float)(c->z * CHUNK_DIM) + block_offset);
            
            for (int y = 0; y < CHUNK_DIM; y++)
            {
                for (int z = 0; z < CHUNK_DIM; z++)
                {
                    for (int x = 0; x < CHUNK_DIM; x++)
                    {
                        if (c->blocks[z * CHUNK_DIM * CHUNK_DIM + y * CHUNK_DIM + x])
                        {
                            Mat4x4f model = mat4x4f_identity();
                            model = mat4x4f_translate(model, Vec3f((float)x, (float)y, (float)z) + chunk_offset);
                            glUniformMatrix4fv(glGetUniformLocation(state->shader_program, "u_model"), 1, GL_FALSE, &model.m[0][0]);

                            float gray = 0.7f;
                            glUniform3f(glGetUniformLocation(state->shader_program, "u_color"), gray, gray, gray);

                            glBindTexture(GL_TEXTURE_2D, state->tex);
                            glBindVertexArray(state->vao);
                            glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / (5 * sizeof(float)));
                        }
                    }
                }
            }
        }
    }
}

int main(void)
{
    if (glfwInit() == GLFW_FALSE)
    {
        return (-1);
    }

    int window_width  = 1280;
    int window_height = 720;
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "This is awesome!", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return (-1);
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return (-1);
    }
    glfwSwapInterval(1);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

#define PERMANENT_MEM_SIZE MEMORY_KB(64)
#define TRANSIENT_MEM_SIZE MEMORY_GB(1)

    static uint8_t permanent_mem_blob[PERMANENT_MEM_SIZE];
    static uint8_t transient_mem_blob[TRANSIENT_MEM_SIZE];

    Game_memory game_memory = {};
    game_memory.permanent_mem_size = PERMANENT_MEM_SIZE;
    game_memory.permanent_mem = permanent_mem_blob;
    game_memory.transient_mem_size = TRANSIENT_MEM_SIZE;
    game_memory.transient_mem = transient_mem_blob;

    Game_input inputs[2] = {};
    Game_input *game_input = &inputs[0];
    Game_input *prev_game_input = &inputs[1];

    double curr_time = glfwGetTime();
    double prev_time = curr_time;

    double prev_mx = (float)window_width / 2.0f;
    double prev_my = (float)window_height / 2.0f;

    while (!glfwWindowShouldClose(window))
    {
        glfwGetFramebufferSize(window, &window_width, &window_height);
        glViewport(0, 0, window_width, window_height);

        game_input->aspect_ratio = float(window_width) / float(window_height);
        game_input->dt = float(curr_time - prev_time);

        double curr_mx;
        double curr_my;
        glfwGetCursorPos(window, &curr_mx, &curr_my);
        double sensitivity = 0.1;
        game_input->mouse_dx = (float)((curr_mx - prev_mx) * sensitivity);
        game_input->mouse_dy = (float)((prev_my - curr_my) * sensitivity);
        prev_mx = curr_mx;
        prev_my = curr_my;

        int num_of_buttons = sizeof(inputs[0].buttons) / sizeof(inputs[0].buttons[0]);
        for (int i = 0; i < num_of_buttons; i++)
        {
            game_input->buttons[i].is_pressed  = 0;
            game_input->buttons[i].was_pressed = 0;
            game_input->buttons[i].was_pressed = prev_game_input->buttons[i].is_pressed;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            game_input->mleft.is_pressed = 1;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            game_input->mright.is_pressed = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            game_input->w.is_pressed = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            game_input->a.is_pressed = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            game_input->s.is_pressed = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            game_input->d.is_pressed = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            game_input->enter.is_pressed = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            game_input->space.is_pressed = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            game_input->lshift.is_pressed = 1;
        }

        game_update_and_render(game_input, &game_memory);

        prev_time = curr_time;
        curr_time = glfwGetTime();

        glfwSwapBuffers(window);
        glfwPollEvents();

        Game_input *temp_input = game_input;
        game_input = prev_game_input;
        prev_game_input = temp_input;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return (0);
}