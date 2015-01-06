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

  // v is 1-based
  void ensureVar(int v) {
    while (v > NumVars) {
      SOLVER->newVar();
      NumVars++;
    }
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
      ensureVar(v);
      vars.push(mkLit(Var(v-1), isNot));
      terms++;
    }
    return SOLVER->addClause(vars);
  }

  bool solve() {
    return SOLVER->solve();
  }

  uint8_t * getSolution() {
    // model is a vec<lbool>, which has a cast operator
    // that gives you a lbool* (a pointer to the data
    // array).  lbool is a "lifted boolean" that can be
    // true, false, or undefined (0, 1, or 2).  It's a
    // class containing a uint8 field, but in memory it
    // just looks like a uint8.
    return (uint8_t *)((lbool *)SOLVER->model);
  }

  int getNumVars() {
    return NumVars;
  }

  // solve while temporarily assuming variable v is true.
  // v is 1-based.
  bool solveAssuming(int v) {
    vec<Lit> assumps;
    assumps.push(mkLit(Var(v-1)));
    return SOLVER->solve(assumps,
                         /*do_sump = */true,
                         /*turn_off_simp = */false);
  }
  
  // mark a Var as false, forever.
  // v is 1-based.
  void retireVar(int v) {
    SOLVER->addClause(mkLit(Var(v-1), true));
  }
};
