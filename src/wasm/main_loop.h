#ifndef GL_DEMO_SCREEN_MAIN_LOOP
#define GL_DEMO_SCREEN_MAIN_LOOP

#include "egl_thread_context.h"
#include "gl_demo_screen.h"
#include "macros.h"

class MainLoop {
 public:
  MainLoop();
  ~MainLoop();

  static void StartLoop();

 private:
  static void Loop();
  void LoopInternal();

  EglThreadContext thread_context_;
  GlDemoScreen demo_screen_;
  int canvas_width_ = 0;
  int canvas_height_ = 0;

  DISALLOW_COPY_AND_ASSIGN(MainLoop);
};

#endif // GL_DEMO_SCREEN_MAIN_LOOP
