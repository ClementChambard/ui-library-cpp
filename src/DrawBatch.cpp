#include "DrawBatch.hpp"
#include <iostream>
#include <vector>
#include <GL/glew.h>

struct Shader {
    u32 program_id;
    i32 uniform_loc;
    void init(const char* vtx, i32 vtx_s, const char* fgr, i32 fgr_s);
    ~Shader() { glUseProgram(0); glDeleteProgram(program_id); }
};

u32 make_shader(const char* s, i32 s_s, u32 ty) {
    u32 shdr = glCreateShader(ty);
    glShaderSource(shdr, 1, &s, &s_s);
    glCompileShader(shdr);
    i32 success = 0;
    glGetShaderiv(shdr, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        i32 max_len = 0;
        glGetShaderiv(shdr, GL_INFO_LOG_LENGTH, &max_len);
        pstr log = new char[max_len];
        glGetShaderInfoLog(shdr, max_len, &max_len, log);
        glDeleteShader(shdr);
        std::cerr << "Shader " << ty << " failed to compile: " << log << "\n";
        delete[] log;
        exit(1);
    }
    return shdr;
}

void Shader::init(const char* vtx, i32 vtx_s, const char* fgr, i32 fgr_s) {
    u32 vtx_shdr = make_shader(vtx, vtx_s, GL_VERTEX_SHADER);
    u32 fgr_shdr = make_shader(fgr, fgr_s, GL_FRAGMENT_SHADER);
    program_id = glCreateProgram();
    glAttachShader(program_id, vtx_shdr);
    glAttachShader(program_id, fgr_shdr);
    glLinkProgram(program_id);
    glValidateProgram(program_id);
    glDetachShader(program_id, vtx_shdr);
    glDetachShader(program_id, fgr_shdr);
    glDeleteShader(vtx_shdr);
    glDeleteShader(fgr_shdr);
    glUseProgram(program_id);
    uniform_loc = glGetUniformLocation(program_id, "the_matrix");
}

const char vtx_shdr[] = R"shdr(#version 460
in vec3 in_pos;
in vec4 in_col;
uniform mat4 the_matrix;
out vec4 frag_col;
void main() {
    gl_Position = the_matrix * vec4(in_pos, 1.0);
    frag_col = in_col;
})shdr";
const char fgr_shdr[] = R"shdr(#version 460
in vec4 frag_col;
out vec4 output_color;
void main() {
    output_color = frag_col;
})shdr";

static constexpr usize VERTEX_STRIDE = 4 * sizeof(u32);

struct vertex_t { f32 x, y, z; Color c; };

static_assert(sizeof(vertex_t) == VERTEX_STRIDE);

struct DrawBatchState {
    DrawBatchState() {
        glGenVertexArrays(1, &vao_id);
        glGenBuffers(1, &vbo_id);
        shdr.init(vtx_shdr, sizeof(vtx_shdr), fgr_shdr, sizeof(fgr_shdr));
        glBindVertexArray(vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (void*)0);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, VERTEX_STRIDE, (void*)(3 * sizeof(f32)));
    }
    ~DrawBatchState() {

    }
    u32 vao_id;
    u32 vbo_id;
    Shader shdr;
    std::vector<vertex_t> vertex;
};

DrawBatch::DrawBatch() {
    DrawBatchState* s = new DrawBatchState;
    state = s;
}

DrawBatch::~DrawBatch() {
    DrawBatchState* s = reinterpret_cast<DrawBatchState*>(state);
    delete s;
}

void DrawBatch::update_wnd_size(Size s) {
    DrawBatchState* st = reinterpret_cast<DrawBatchState*>(state);
    f32 matrix[] = {
        2.f / s.w,  0.f      ,  0.f   , -1.f,
        0.f      , -2.f / s.h,  0.f   ,  1.f,
        0.f      ,  0.f      , -0.001f,  0.f,
        0.f      ,  0.f      ,  0.f   ,  1.f
    };
    glUniformMatrix4fv(st->shdr.uniform_loc, 1, GL_TRUE, matrix);
}

void DrawBatch::draw_rectangle(f32 x1, f32 x2, f32 y1, f32 y2, f32 z, Color c) {
    auto *s = reinterpret_cast<DrawBatchState*>(state);
    s->vertex.push_back({x1, y1, z, c});
    s->vertex.push_back({x2, y1, z, c});
    s->vertex.push_back({x2, y2, z, c});
    s->vertex.push_back({x1, y1, z, c});
    s->vertex.push_back({x2, y2, z, c});
    s->vertex.push_back({x1, y2, z, c});
}

void DrawBatch::submit() {
    auto *s = reinterpret_cast<DrawBatchState*>(state);
    glBindBuffer(GL_ARRAY_BUFFER, s->vbo_id);
    usize data_size = s->vertex.size() * sizeof(s->vertex[0]);
    glBufferData(GL_ARRAY_BUFFER, data_size, s->vertex.data(), GL_DYNAMIC_DRAW);
    glBindVertexArray(s->vao_id);
    glDrawArrays(GL_TRIANGLES, 0, s->vertex.size());
    s->vertex.clear();
}
