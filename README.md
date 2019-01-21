# WASM GlDemoScreen

This is a simple WebAssembly and OpenGL ES demo that shows a grid of tiles
moving across the screen. Some files are copied and modified from the
[Chromium remoting](https://cs.chromium.org/chromium/src/remoting/) project.

[Click here](https://equinox1993.github.io/WASM-GlDemoScreen/out/gl_demo_screen.html)
to see the live demo.

## Build and Run

You will need
[emsdk](https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html) 
to build the webpage. To build the files, simply run `./build.sh` on the
project root. This will generate `gl_demo_screen.*` files to `./out`.

If you have emsdk installed, you can run `emrun out/gl_demo_screen.html` (with
`--no_browser` if necessary) to start the web server and launch the webpage.
You can also use any web server of your choice to serve the webpage. 
