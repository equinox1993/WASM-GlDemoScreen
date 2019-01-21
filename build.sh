#!/bin/bash

em++ --std=c++11 --shell-file src/shell.html --emrun \
     -o out/gl_demo_screen.html src/wasm/*.cc
