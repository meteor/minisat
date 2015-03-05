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

  // The number of variables allocated, not including the
  // dummy "0" variable we give to MiniSat.
  int NumVars = 0;

  // One global solver.  We instantiate the entire C environment
  // per JavaScript solver object.  Only call createTheSolver()
  // once (per C environment).
  SimpSolver *SOLVER;
  void createTheSolver() {
    SOLVER = new SimpSolver();
    // Turn off variable elimination, which is problematic for
    // incremental solving.
    SOLVER->eliminate(/*turn_off_elim = */true);
    SOLVER->newVar(); // create dummy "0" variable
  }

  // Make sure enough variables have been allocated to hold
  // "v".
  void ensureVar(int v) {
    // For example, suppose v is 4 and NumVars is 3.
    // That means MiniSat has allocated space for vars
    // 0, 1, 2, and 3 (where "0" is a dummy var).
    // The loop body is run once to allocate room for var 4.
    while (v > NumVars) {
      SOLVER->newVar();
      NumVars++;
    }
  }
  
  // terms is 0-terminated.  Negative numbers are negative
  // terms.  1, 2, and 3 mean variables 1, 2, and 3.
  // -1, -2, and -3 mean "not 1", "not 2", and "not 3".
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
      vars.push(mkLit(Var(v), isNot));
      terms++;
    }
    return SOLVER->addClause(vars);
  }

  bool solve() {
    return SOLVER->solve();
  }

  // Returns a pointer to a sequence of bytes representing
  // the solution values of the variables, starting with
  // variable 1 (that is, the first byte is variable 1,
  // the second byte is variable 2, etc.).  A value of
  // 0 means "true", and anything else means "false".
  uint8_t * getSolution() {
    // model is a vec<lbool>, which has a cast operator
    // that gives you a lbool* (a pointer to the data
    // array).  lbool is a "lifted boolean" that can be
    // true, false, or undefined (0, 1, or 2).  It's a
    // class containing a uint8 field, but in memory it
    // just looks like a uint8.
    //
    // MiniSat does not distinguish between false and
    // undefined in solutions (I think it sets undefined
    // variables to false).
    //
    // Adding 1 to the pointer skips the dummy "0" variable.
    return (uint8_t *)((lbool *)SOLVER->model) + 1;
  }

  int getNumVars() {
    return NumVars;
  }

  // solve while temporarily assuming variable v is true.
  bool solveAssuming(int v) {
    vec<Lit> assumps;
    assumps.push(mkLit(Var(v)));
    return SOLVER->solve(assumps,
                         /*do_sump = */true,
                         /*turn_off_simp = */false);
  }
  
  // mark a Var as false, forever.
  void retireVar(int v) {
    SOLVER->addClause(mkLit(Var(v), true));
  }

  int getConflictClauseSize() {
    return SOLVER->conflict.size();
  }

  // The "conflict clause" feature of MiniSat is not what it sounds
  // like, unfortunately -- it doesn't help explain conflicts.
  // It only tells us which assumption vars are to blame for a failed
  // solveAssuming (and we only ever pass one var).
  // We keep this function around in case we discover a use for it.
  //
  // Returns a pointer to an int[] of length getConflictClauseSize().
  int * getConflictClause() {
    // conflict is an IntSet<Lit>, which we can cast
    // to an array of int, where each int is 2*v + s where
    // v is the variable number and s is 1 for "not".
    return (int *)((Lit *)*const_cast<vec<Lit> *>(&SOLVER->conflict.toVec()));
  }
};
