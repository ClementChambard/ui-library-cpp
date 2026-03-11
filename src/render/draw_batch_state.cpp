#include "draw_batch_state.hpp"
#include <GL/glew.h>
#include <iostream>

Shader::~Shader() { glDeleteProgram(program_id); }

static const char vtx_shdr[] = R"shdr(#version 460
in vec2 in_pos;
in vec4 in_col;
in vec2 in_uvs;
uniform mat4 the_matrix;
out vec4 frag_col;
out vec2 frag_uvs;
void main() {
    gl_Position = the_matrix * vec4(in_pos, 0.0, 1.0);
    frag_col = in_col;
    frag_uvs = in_uvs;
})shdr";
static const char fgr_shdr[] = R"shdr(#version 460
in vec4 frag_col;
in vec2 frag_uvs;
out vec4 output_color;
uniform sampler2D u_texture;
void main() {
    output_color = frag_col * texture(u_texture, frag_uvs);
})shdr";

DrawBatchState::DrawBatchState() {
  glCreateBuffers(1, &vbo_id);
  glCreateBuffers(1, &ibo_id);
  glCreateVertexArrays(1, &vao_id);
  glVertexArrayElementBuffer(vao_id, ibo_id);
  glVertexArrayVertexBuffer(vao_id, 0, vbo_id, 0, sizeof(vertex_t));
  glEnableVertexArrayAttrib(vao_id, 0);
  glEnableVertexArrayAttrib(vao_id, 1);
  glEnableVertexArrayAttrib(vao_id, 2);
  glVertexArrayAttribBinding(vao_id, 0, 0);
  glVertexArrayAttribBinding(vao_id, 1, 0);
  glVertexArrayAttribBinding(vao_id, 2, 0);
  glVertexArrayAttribFormat(vao_id, 0, 2, GL_FLOAT, GL_FALSE,
                            offsetof(vertex_t, pos));
  glVertexArrayAttribFormat(vao_id, 1, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                            offsetof(vertex_t, col));
  glVertexArrayAttribFormat(vao_id, 2, 2, GL_FLOAT, GL_FALSE,
                            offsetof(vertex_t, uv));
  shdr.init(vtx_shdr, sizeof(vtx_shdr), fgr_shdr, sizeof(fgr_shdr));

  // TODO: texture should be loaded with default font

  glCreateTextures(GL_TEXTURE_2D, 1, &default_tex);
  glTextureStorage2D(default_tex, 1, GL_RGBA8, 1, 1);
  u8 data[] = {255, 255, 255, 255};
  glTextureSubImage2D(default_tex, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                      data);
}

DrawBatchState::~DrawBatchState() {
  glDeleteBuffers(1, &vbo_id);
  glDeleteBuffers(1, &ibo_id);
  glDeleteVertexArrays(1, &vao_id);
  glDeleteTextures(1, &default_tex);
}

u32 make_shader(const char *s, i32 s_s, u32 ty) {
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

void DrawBatchState::use_texture(u32 tex) {
  glBindTextureUnit(0, tex);
  cur_tex = tex;
}

void Shader::init(const char *vtx, i32 vtx_s, const char *fgr, i32 fgr_s) {
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

u32 DrawBatchState::vertex(vertex_t v) {
  vertices.push_back(v);
  return vertices.size() - 1;
}

void DrawBatchState::index(u32 i) { indices.push_back(i); }

void DrawBatchState::triangle(vertex_t p1, vertex_t p2, vertex_t p3) {
  u32 i1 = vertex(p1), i2 = vertex(p2), i3 = vertex(p3);
  triangle(i1, i2, i3);
}

void DrawBatchState::triangle(u32 i1, u32 i2, u32 i3) {
  if (i1 == i2 || i2 == i3 || i1 == i3)
    return;
  index(i1), index(i2), index(i3);
}

void DrawBatchState::quad(u32 i1, u32 i2, u32 i3, u32 i4) {
  triangle(i1, i2, i3);
  triangle(i1, i3, i4);
}

void DrawBatchState::quad(vertex_t p1, vertex_t p2, vertex_t p3, vertex_t p4) {
  u32 i1 = vertex(p1), i2 = vertex(p2), i3 = vertex(p3), i4 = vertex(p4);
  quad(i1, i2, i3, i4);
}
