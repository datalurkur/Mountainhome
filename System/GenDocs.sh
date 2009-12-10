#!/bin/sh

cccc *.cpp *.h *hpp
mv .cccc docs/metrics
doxygen doxygen.cfg

