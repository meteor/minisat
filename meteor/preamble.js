// (start of meteor/preamble.js)
cMinisat = function () {
  var module = {};
// Put emscripten in "node mode" but don't give it the real `process`
// object.  It does have access to `console`.
var require = function () {};
var process = {
  argv: ['node', 'minisat'],
  on: function () {},
  stdout: {
    write: function (str) {
      console.log("MINISAT-out:", str.replace(/\n$/, ''));
    }
  },
  stderr: {
    write: function (str) {
      console.log("MINISAT-err:", str.replace(/\n$/, ''));
    }
  }
};
var window = 0;
// (end of meteor/preamble.js)
