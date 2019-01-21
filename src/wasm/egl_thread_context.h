// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GL_DEMO_SCREEN_EGL_THREAD_CONTEXT_H_
#define GL_DEMO_SCREEN_EGL_THREAD_CONTEXT_H_

#include <EGL/egl.h>

#include "macros.h"

// Establishes an EGL-OpenGL|ES 2 (if 3 is not supported) or 3 (backward
// compatible with 2) context on current thread. Must be constructed, used, and
// deleted on single thread (i.e. the display thread). Each thread can have no
// more than one EglThreadContext.
class EglThreadContext {
 public:
  enum class GlVersion {
    UNKNOWN = 0,
    ES_2 = 2,
    ES_3 = 3
  };

  EglThreadContext();
  ~EglThreadContext();

  // Creates a surface on the given window and binds the context to the surface.
  // EGLNativeWindowType is platform specific. E.g. ANativeWindow* on Android.
  void BindToWindow(EGLNativeWindowType window);
  void UnbindFromWindow();

  // Returns true if the context is bound to a window (i.e. current surface is
  // not NULL).
  bool IsWindowBound() const;

  // Posts EGL surface buffer to the window being bound.
  // Returns true if the buffer is successfully swapped.
  bool SwapBuffers();

  // Returns the current OpenGL ES client version of the EGL context.
  GlVersion client_version() const {
    return client_version_;
  }

 private:
  // Creates an EGLContext with given |renderable_type| and |client_version|.
  // |renderable_type| and |client_version| must match with each other.
  // E.g. renderable_type = EGL_OPENGL_ES3_BIT,
  //      client_version = CLIENT_VERSION_ES_3.
  bool CreateContextWithClientVersion(int renderable_type,
                                      GlVersion client_version);

  EGLDisplay display_ = EGL_NO_DISPLAY;
  EGLConfig config_ = nullptr;
  EGLSurface surface_ = EGL_NO_SURFACE;
  EGLContext context_ = EGL_NO_CONTEXT;
  GlVersion client_version_ = GlVersion::UNKNOWN;

  DISALLOW_COPY_AND_ASSIGN(EglThreadContext);
};

#endif  // GL_DEMO_SCREEN_EGL_THREAD_CONTEXT_H_
