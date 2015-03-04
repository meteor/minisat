### Building MiniSat with Emscripten for the Meteor logic-solver

1. Install Emscripten.  On a Mac with Brew, `brew install emscripten` will do it.
   (Last tested with emscripten 1.28.0.)
2. Check that you have `emmake`, `emcc`, and `node` in your path.  If you don't
   have `node` in your path, you can locally modify the script in the next step
   where it says `NODE=node`.  (Node is only used to run a quick sanity check
   after compiling.)
2. In the repo directory, run `meteor/make-emscripten.sh`.
3. That's it.  If everything worked, the build product is `build/minisat.js`.

Notes:

* The "native" part of logic-solver is in `meteor/logic-solver.cc`.  If you add
  functions to it, you need to also list them in `make-emscripten.sh`.

* There's heap allocation instrumentation, hackily implemented, that doesn't
  actually run unless you put the compiler in debug mode (see comment in the
  make script about the -g flag).

* You want to leave minification on (no -g flag), because unlike
  client-side code served by Meteor (which is minified anyway), Node
  runs this code directly.  Minification actually helps the JS compile
  time quite a bit!

