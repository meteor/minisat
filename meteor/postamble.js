// (start of meteor/postamble.js)
// Hook into `malloc` and `free` so that we can measure how much
// memory has been allocated.  If we're going to be creating instances
// of C++ objects in a "heap" that lives in a statically-allocated
// JavaScript array, we'd better understand how much of it we are
// using and make sure we aren't leaking it.
var origMalloc = Module._malloc;
var origFree = Module._free;

var MEMSTATS = {
  totalMemory: Module.HEAPU8.length,
  heapUsed: 0 // doesn't include stack
};
var MEMSTATS_DATA = {
  // Keep a mapping of pointers to their allocated sizes, so we know
  // how much memory is freed when the pointer is freed.
  pointerToSizeMap: {}, // ptr -> size of memory allocated at ptr
  getSizeOfPointer: function (ptr) {
    return MEMSTATS_DATA.pointerToSizeMap[ptr];
  }
};
Module.MEMSTATS = MEMSTATS;
Module.MEMSTATS_DATA = MEMSTATS_DATA;

var hookedMalloc = function (size) {
  var ptr = origMalloc(size);
  if (! ptr) {
    return 0;
  }

  MEMSTATS.heapUsed += size;
  MEMSTATS_DATA.pointerToSizeMap[ptr] = size;

  return ptr;
};

var hookedFree = function (ptr) {
  if (ptr) {
    MEMSTATS.heapUsed -= (MEMSTATS_DATA.getSizeOfPointer(ptr) || 0);
    delete MEMSTATS_DATA.pointerToSizeMap[ptr];
  }
  return origFree(ptr);
};

// Overwrite malloc and free in three scopes
Module._malloc = hookedMalloc;
Module._free = hookedFree;
_malloc = hookedMalloc;
_free = hookedFree;
var setInnerMalloc, setInnerFree; // assigned from the "midamble"
setInnerMalloc(hookedMalloc);
setInnerFree(hookedFree);

  return module.exports;
};
// Just so we can also run as a node module via
// `cMinisat = require('minisat.js')()`:
if (typeof module !== 'undefined') {
  module.exports = cMinisat;
}
// (end of meteor/postamble.js)
