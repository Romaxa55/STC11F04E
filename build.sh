#!/usr/bin/env bash
mkdir -p Release
sdcc -mmcs51 --model-small --code-loc 0x0000 --xram-loc 0x0000 -I/opt/homebrew/Cellar/sdcc/4.4.0/share/sdcc/include/mcs51 main.c -o release/main.ihx