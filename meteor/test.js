console.time('Loading module');
var Module = require('../build/minisat.js')();
console.timeEnd('Loading module');

console.log(Module.MEMSTATS);
var ptr = Module._yo();
console.log(Module.MEMSTATS);
Module._unyo(ptr);
console.log(Module.MEMSTATS);
