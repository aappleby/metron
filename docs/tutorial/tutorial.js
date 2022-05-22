import { CodeJar } from "https://cdn.jsdelivr.net/npm/codejar@3.6.0/codejar.min.js";

var tut1_c = document.getElementById("tut1_c");
var tut1_v = document.getElementById("tut1_v");

//------------------------------------------------------------------------------
// Set up CodeJar

const highlight = editor => {
  editor.textContent = editor.textContent
  hljs.highlightElement(editor)
}

const tut1_c_jar = CodeJar(tut1_c, highlight, {tab:"  "});
const tut1_v_jar = CodeJar(tut1_v, highlight, {tab:"  "});


var tut1_c_src = String.raw`// Trivial adder example
class Adder {
public:
  int add(int a, int b) {
    return a + b;
  }
};`
tut1_c_jar.updateCode(tut1_c_src);

var tut1_v_src = String.raw`// Trivial adder example
module Adder
(
  input int a,
  input int b,
  output int add
);
  always_comb begin
    add = a + b;
  end
endmodule`
tut1_v_jar.updateCode(tut1_v_src);
