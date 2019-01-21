// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gl_demo_screen.h"

#include <algorithm>
#include <iostream>
#include <vector>

namespace {

const GLfloat kVertexData[] = {0, 0, 0, 1, 1, 0, 1, 1};
const GLfloat kOffsetIncrement = 5.f;

const GLchar* kVertexShaderSource = R"(
  precision mediump float;

  // This matrix translates normalized texture coordinates
  // ([0, 1] starting at upper-left corner) to the view coordinates
  // ([-1, 1] starting at the center of the screen).
  // Note that the matrix is defined in column-major order.
  const mat3 c_texToView = mat3(2, 0, 0,
                                0, -2, 0,
                                -1, 1, 0);

  attribute vec2 a_position;
  varying vec2 v_position;

  void main() {
    // Pass the original position to fragment shader.
    v_position = a_position;

    // Transform texture coordinates to view coordinates
    vec3 trans_position = c_texToView * vec3(a_position, 1.0);

    // Add projection component 1
    gl_Position = vec4(trans_position, 1.0);
  }
)";

const GLchar* kFragmentShaderSource = R"(
  precision mediump float;

  const vec2 c_tileSize = vec2(200, 200);
  varying vec2 v_position;
  uniform vec2 u_viewSize;
  uniform vec2 u_offset;

  void main() {
    vec2 tile_position =
        floor((v_position * u_viewSize + u_offset) / c_tileSize);
    vec2 mod_map = tile_position - 2.0 * floor(tile_position / 2.0);
    bool isGreen = mod_map.x == mod_map.y;
    gl_FragColor =
        isGreen ? vec4(0.0, 1.0, 0.0, 1.0) : vec4(1.0, 1.0, 0.0, 1.0);
  }
)";

GLuint CreateAndCompileShader(GLenum type, const GLchar* source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  GLint is_compiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
  if(is_compiled == GL_FALSE) {
    GLint max_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

    // The maxLength includes the NULL character
    std::vector<GLchar> error_log(max_length);
    glGetShaderInfoLog(shader, max_length, &max_length, &error_log[0]);

    // Provide the infolog in whatever manor you deem best.
    std::cerr << error_log.data() << std::endl;

    // Exit with failure.
    glDeleteShader(shader); // Don't leak the shader.
    return 0;
  }
  return shader;
}

GLuint CreateBuffer(const void* data, int size) {
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return buffer;
}

}  // namespace

// This is a demo screen that can be added to the renderer to test the drawable
// integration. This will draw an expanding checkerboard pattern to the screen.
GlDemoScreen::GlDemoScreen() {
  // Create and compile shaders.
  vertex_shader_ =
      CreateAndCompileShader(GL_VERTEX_SHADER, kVertexShaderSource);
  fragment_shader_ =
      CreateAndCompileShader(GL_FRAGMENT_SHADER, kFragmentShaderSource);

  // Create and link program.
  program_ = glCreateProgram();
  glAttachShader(program_, vertex_shader_);
  glAttachShader(program_, fragment_shader_);
  glLinkProgram(program_);

  buffer_ = CreateBuffer(kVertexData, sizeof(kVertexData));
}

GlDemoScreen::~GlDemoScreen() {
  glDeleteBuffers(1, &buffer_);
  glDeleteProgram(program_);
  glDeleteShader(vertex_shader_);
  glDeleteShader(fragment_shader_);
}

void GlDemoScreen::Draw(float width, float height) {
  glViewport(0, 0, width, height);

  // Clear.
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  // Use program.
  glUseProgram(program_);

  // Send uniform info to shaders.
  GLuint view_size_loc = glGetUniformLocation(program_, "u_viewSize");
  GLfloat view_size[] = {width, height};
  glUniform2fv(view_size_loc, 1, view_size);

  GLuint offset_loc = glGetUniformLocation(program_, "u_offset");
  GLfloat offset[] = {offset_xy_, offset_xy_};
  glUniform2fv(offset_loc, 1, offset);
  
  // Increment offset for future animations.
  offset_xy_ += kOffsetIncrement;
  if (offset_xy_ >= width * height) {
    offset_xy_ -= width * height;
  }

  // Send geometry to vertex shader.
  glBindBuffer(GL_ARRAY_BUFFER, buffer_);
  GLuint position_loc = glGetAttribLocation(program_, "a_position");
  glVertexAttribPointer(position_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(position_loc);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glDisableVertexAttribArray(position_loc);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
