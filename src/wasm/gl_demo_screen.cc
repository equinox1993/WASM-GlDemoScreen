// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gl_demo_screen.h"

#include <algorithm>

namespace {

const GLfloat kSquare[] = {-1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0};

const GLchar* kFragmentShaderSource =
    "precision mediump float;"
    "void main() {"
    "  gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);"
    "} ";

const GLchar* kVertexShaderSource =
    "precision mediump float;"
    "attribute vec4 a_position;"
    "void main() {"
    "  gl_Position = a_position;"
    "}";

const GLchar* a_position = "a_position";

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
  // Create and compile vertex shader.
  vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader_, 1, &kVertexShaderSource, nullptr);
  glCompileShader(vertex_shader_);

  // Create and compile fragment shader.
  fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader_, 1, &kFragmentShaderSource, nullptr);
  glCompileShader(fragment_shader_);

  // Create and link program.
  program_ = glCreateProgram();
  glAttachShader(program_, vertex_shader_);
  glAttachShader(program_, fragment_shader_);
  glLinkProgram(program_);

  buffer_ = CreateBuffer(kSquare, sizeof(kSquare));
}

GlDemoScreen::~GlDemoScreen() {
  glDeleteBuffers(1, &buffer_);
  glDeleteProgram(program_);
  glDeleteShader(vertex_shader_);
  glDeleteShader(fragment_shader_);
}

void GlDemoScreen::Draw(int width, int height) {
  square_size_++;
  if (square_size_ > std::min(width, height)) {
    square_size_ = 1;
  }

  // Set the viewport.
  glViewport(0, 0, width, height);

  // Clear.
  glClearColor(0, 1, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  // Use program.
  glUseProgram(program_);

  // Send geometry to vertex shader.
  GLuint aPosition = glGetAttribLocation(program_, a_position);

  glBindBuffer(GL_ARRAY_BUFFER, buffer_);
  glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(aPosition);

  int skip = 0;
  for (int i = 0; i < width; i += square_size_) {
    if (skip == square_size_) {
      skip = 0;
    } else {
      skip = square_size_;
    }
    for (int j = skip; j < height; j += square_size_ * 2) {
      glViewport(i, j, square_size_, square_size_);

      // Draw.
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
  }

  glDisableVertexAttribArray(aPosition);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
