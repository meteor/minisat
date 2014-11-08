/****************************************************************************************[Dimacs.h]
Copyright (c) 2003-2006, Niklas Een, Niklas Sorensson
Copyright (c) 2007-2010, Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#ifndef Minisat_Dimacs_h
#define Minisat_Dimacs_h

#include <stdio.h>

#include "minisat/utils/ParseUtils.h"
#include "minisat/core/SolverTypes.h"

namespace Minisat {

//=================================================================================================
// DIMACS Parser:

template<class B, class Solver>
static bool readClause(B& in, Solver& S, vec<Lit>& lits) {
    int     parsed_lit, var;
    bool    error = false;
    lits.clear();
    for (;;){
        parsed_lit = parseInt(in, error);
        if (parsed_lit == 0 || error) break;
        var = abs(parsed_lit)-1;
        while (var >= S.nVars()) S.newVar();
        lits.push( (parsed_lit > 0) ? mkLit(var) : ~mkLit(var) );
    }
    return !error;
}

template<class B, class Solver>
static bool parse_DIMACS_main(B& in, Solver& S, bool strictp = false) {
    vec<Lit> lits;
    int vars    = 0;
    int clauses = 0;
    int cnt     = 0;
    bool error  = false;
    for (;;){
        skipWhitespace(in);
        if (*in == EOF || error) break;
        else if (*in == 'p'){
            if (eagerMatch(in, "p cnf")){
	        vars    = parseInt(in, error);
	        clauses = parseInt(in, error);
                // SATRACE'06 hack
                // if (clauses > 4000000)
                //     S.eliminate(true);
            }else{
                printf("PARSE ERROR! Unexpected char: %c\n", *in), error = true;
            }
        } else if (*in == 'c' || *in == 'p')
            skipLine(in);
        else{
            cnt++;
            error = !readClause(in, S, lits);
            S.addClause_(lits); }
    }
    if (strictp && cnt != clauses)
      printf("PARSE ERROR! DIMACS header mismatch: wrong number of clauses\n"), error = true;
    return !error;
}

// Inserts problem into solver.
//
template<class Solver>
static void parse_DIMACS(FILE* input_stream, Solver& S, bool strictp = false) {
    StreamBuffer in(input_stream);
    parse_DIMACS_main(in, S, strictp); }

template<class Solver>
  static bool parse_DIMACS_string(unsigned char* input, int size, Solver& S, bool strictp = false) {
  StreamBufferString in(input, size);
  return parse_DIMACS_main(in, S, strictp); }

//=================================================================================================
}

#endif
