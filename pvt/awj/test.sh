#!/bin/bash

gcc -Wall main.c ../../ruby/lib/libruby-static.a -o main
./main
