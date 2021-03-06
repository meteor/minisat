#!/bin/bash -e

EMMAKE=emmake
EMCC=emcc
NODE=node

echo "Building"

mkdir -p build
rm -rf build/*
$EMMAKE make r
pushd build/release/bin
ln -s minisat minisat.bc
# flags reference: http://kripken.github.io/emscripten-site/docs/tools_reference/emcc.html
# Add -g to make the output pretty instead of mangled.
$EMCC -O3 --memory-init-file 0 --pre-js ../../../meteor/preamble.js --post-js ../../../meteor/postamble.js -s EXPORTED_FUNCTIONS='["_yo", "_unyo", "_createTheSolver", "_addClause", "_solve", "_getSolution", "_getNumVars", "_ensureVar", "_solveAssuming", "_retireVar", "_getConflictClauseSize", "_getConflictClause"]' -s TOTAL_MEMORY=67108864 -s NO_BROWSER=1 -s NO_FILESYSTEM=1 -s NO_DYNAMIC_EXECUTION=0 minisat.bc -o minisat.js
popd

( echo "// This file is generated by the meteor/minisat repo." ; \
    echo "// See that repo's README for instructions for building it." ; \
    sed -e '/^\/\/ EMSCRIPTEN_END_FUNCS/r meteor/midamble.js' \
    build/release/bin/minisat.js ) >build/minisat.js

echo "Testing"
$NODE meteor/test.js
