#include "main_loop.h"

#include <emscripten.h>
#include <emscripten/html5.h>

#include <iostream>

namespace {

static MainLoop* g_main_loop = nullptr;

}

MainLoop::MainLoop() {
  thread_context_.BindToWindow(0);
}

MainLoop::~MainLoop() {}

// static
void MainLoop::StartLoop() {
  assert(!g_main_loop);
  g_main_loop = new MainLoop();
  emscripten_set_main_loop(&MainLoop::Loop, 0, true);
}

// static
void MainLoop::Loop() {
  assert(g_main_loop);
  g_main_loop->LoopInternal();
}

void MainLoop::LoopInternal() {
  int width, height;
  emscripten_get_canvas_element_size("#canvas", &width, &height);
  if (width != canvas_width_ || height != canvas_height_) {
    std::cout << "New canvas size: " << width << " x " << height << std::endl;
    canvas_width_ = width;
    canvas_height_ = height;
  }
  demo_screen_.Draw(canvas_width_, canvas_height_);
  thread_context_.SwapBuffers();
}
