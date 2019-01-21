// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "egl_thread_context.h"

#include <iostream>

#ifndef EGL_OPENGL_ES3_BIT
#define EGL_OPENGL_ES3_BIT 0
#endif

namespace remoting {

EglThreadContext::EglThreadContext() {
  display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (!display_ || !eglInitialize(display_, NULL, NULL)) {
    std::cerr << "Failed to initialize EGL display: " << eglGetError()
              << std::endl;
  }

  if (EGL_OPENGL_ES3_BIT &&
      CreateContextWithClientVersion(EGL_OPENGL_ES3_BIT, GlVersion::ES_3)) {
    client_version_ = GlVersion::ES_3;
  } else if (CreateContextWithClientVersion(EGL_OPENGL_ES2_BIT,
                                            GlVersion::ES_2)) {
    std::cout << "OpenGL ES 3 context not supported."
              << "Falled back to OpenGL ES 2" << std::endl;
    client_version_ = GlVersion::ES_2;
  } else {
    std::cerr << "Failed to create context: " << eglGetError() << std::endl;
  }
}

EglThreadContext::~EglThreadContext() {
  eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroyContext(display_, context_);
  if (surface_) {
    eglDestroySurface(display_, surface_);
  }
  eglTerminate(display_);
}

void EglThreadContext::BindToWindow(EGLNativeWindowType window) {
  if (surface_) {
    eglDestroySurface(display_, surface_);
    surface_ = EGL_NO_SURFACE;
  }
  if (window) {
    surface_ = eglCreateWindowSurface(display_, config_, window, NULL);
    if (!surface_) {
      std::cerr << "Failed to create window surface: " << eglGetError()
                << std::endl;
    }
  } else {
    surface_ = EGL_NO_SURFACE;
  }
  if (!eglMakeCurrent(display_, surface_, surface_, context_)) {
    std::cerr << "Failed to make current: " << eglGetError() << std::endl;
  }
}

bool EglThreadContext::IsWindowBound() const {
  return surface_ != EGL_NO_SURFACE;
}

bool EglThreadContext::SwapBuffers() {
  if (!IsWindowBound()) {
    return false;
  }
  if (!eglSwapBuffers(display_, surface_)) {
    // Not fatal since the surface may be destroyed on a different thread
    // earlier than the window is unbound. The context can still be reused
    // after rebinding to the right window.
    std::cerr << "Failed to swap buffer: " << eglGetError() << std::endl;
    return false;
  }
  return true;
}

bool EglThreadContext::CreateContextWithClientVersion(
    int renderable_type,
    GlVersion client_version) {
  EGLint config_attribs[] = {
     EGL_RED_SIZE, 8,
     EGL_GREEN_SIZE, 8,
     EGL_BLUE_SIZE, 8,
     EGL_RENDERABLE_TYPE, renderable_type,
     EGL_NONE
   };

   EGLint num_configs;
   if (!eglChooseConfig(display_, config_attribs, &config_, 1, &num_configs)) {
     std::cerr << "Failed to choose config: " << eglGetError() << std::endl;
     return false;
   }

  EGLint context_attribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, static_cast<int>(client_version),
    EGL_NONE
  };
  context_ = eglCreateContext(display_, config_, EGL_NO_CONTEXT,
                              context_attribs);
  return context_ != EGL_NO_CONTEXT;
}

}  // namespace remoting