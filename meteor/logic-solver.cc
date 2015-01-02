#include "../minisat/simp/SimpSolver.h"

using namespace Minisat;

// NOTE: These functions needed to be listed in "make-emscripten.sh" to
// be picked up by the compiler!

extern "C" {

  // Test functions for sanity-checking the build.
  SimpSolver *yo() {
    printf("YO!\n");
    // allocate a SimpSolver and don't free it
    SimpSolver*   a_solver = new SimpSolver();
    return a_solver;
  }
  void unyo(SimpSolver *s) {
    delete s;
  }

  // One global solver.  We instantiate the entire C environment
  // per JavaScript solver object.
  SimpSolver *SOLVER;
  int NumVars = 0;
  void createTheSolver() {
    SOLVER = new SimpSolver();
    // Turn off variable elimination, which is problematic for
    // incremental solving.
    SOLVER->eliminate(/*turn_off_elim = */true);
  }

  // terms is 0-terminated.  Negative numbers are negative
  // terms.  1, 2, and 3 mean variables 0, 1, and 2.
  // -1, -2, and -3 mean "not 0", "not 1", and "not 2".
  bool addClause(int *terms) {
    vec<Lit> vars;
    while (*terms) {
      int v = *terms;
      bool isNot = false;
      if (v < 0) {
        v = -v;
        isNot = true;
      }
      v--;
      while (v >= NumVars) {
        SOLVER->newVar();
        NumVars++;
      }
      vars.push(mkLit(Var(v), isNot));
      terms++;
    }
    return SOLVER->addClause(vars);
  }

  bool solve() {
    return SOLVER->solve();
  }
};
