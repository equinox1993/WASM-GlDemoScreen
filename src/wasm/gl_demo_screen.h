// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GL_DEMO_SCREEN_GL_DEMO_SCREEN_H_
#define GL_DEMO_SCREEN_GL_DEMO_SCREEN_H_

#include <GLES3/gl3.h>

#include "macros.h"

// This class draws the desktop on the canvas.
class GlDemoScreen {
 public:
  GlDemoScreen();
  ~GlDemoScreen();

  void Draw(int width, int height);

 private:
  int square_size_ = 0;

  GLuint vertex_shader_;
  GLuint fragment_shader_;
  GLuint program_;
  GLuint buffer_;

  DISALLOW_COPY_AND_ASSIGN(GlDemoScreen);
};

#endif  // GL_DEMO_SCREEN_GL_DEMO_SCREEN_H_
