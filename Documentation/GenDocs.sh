#!/bin/sh

# Clean up the old stuff
if [[ -e "Metrics/" ]]; then rm -rf Metrics/; fi
if [[ -e "Doxygen/" ]]; then rm -rf Doxygen/; fi

# Generate CCCC docs
cccc ../Base/*.cpp ../Base/*.h ../Base/*.hpp ../Render/*.cpp ../Render/*.h ../Render/*.hpp ../Engine/*.cpp ../Engine/*.h ../Engine/*.hpp ../Mountainhome/*.cpp ../Mountainhome/*.h ../Mountainhome/*.hpp
mv .cccc Metrics

# Generate Doxygen docs
doxygen Doxygen.cfg
