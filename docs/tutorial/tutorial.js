import { CodeJar } from "https://cdn.jsdelivr.net/npm/codejar@3.6.0/codejar.min.js";

import Module from "../app/metron.js"

var tut1_c = document.getElementById("tut1_c");
var tut1_v = document.getElementById("tut1_v");

let metron_mod = null;

//------------------------------------------------------------------------------
// Set up Emscripten module to run main() when it's ready.

let stdout = "";
let stderr = "";

var mod_options = window.Module;
mod_options.noInitialRun = true;
mod_options.print = function(text) {
  stdout = stdout + text + "\n";
}
mod_options.printErr = function(text) {
  stderr = stderr + text + "\n";
}
mod_options.onRuntimeInitialized = function() {
  console.log("mod_options.onRuntimeInitialized " + performance.now());
}

Module(mod_options).then(main);

//------------------------------------------------------------------------------
// Set up CodeJar

const highlight = editor => {
  editor.textContent = editor.textContent
  hljs.highlightElement(editor)
}

const tut1_c_jar = CodeJar(tut1_c, highlight, {tab:"  "});
const tut1_v_jar = CodeJar(tut1_v, highlight, {tab:"  "});

/*
let compile_timeout = null;
tut1_c_jar.onUpdate(() => {
  clearTimeout(compile_timeout);
  compile_timeout = setTimeout(() => {
    console.log("source updated"), 300)
  });
});

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
*/

//------------------------------------------------------------------------------

function dump_fs(FS, path, src_paths) {
  if (path == "./dev") return;
  if (path == "./tmp") return;
  if (path == "./home") return;
  if (path == "./proc") return;

  if (FS.isDir(FS.stat(path).mode)) {
    let names = FS.readdir(path);
    for (let i = 0; i < names.length; i++) {
      let name = names[i];
      if (name == "." || name == "..") continue;
      dump_fs(FS, path + "/" + name, src_paths);
    }
  }
  else {
    src_paths.push(path);
  }
}

//------------------------------------------------------------------------------

function convert(mod) {
  var selector_name = "./examples/tutorial/tutorial1.h";

  // Write the potentially modified source back to the filesystem
  let blob = new TextEncoder().encode(tut1_c_jar.toString());
  mod.FS.writeFile(selector_name, blob);

  // Abort conversion if file doesn't end with ".h"
  if (!selector_name.endsWith(".h")) return;

  // Change dst pane title & delete any existing output file
  var file_out = selector_name.substr(0, selector_name.lastIndexOf('.')) + ".sv";
  document.getElementById("tut1_v_bar_text").innerText = file_out;
  try {
    mod.FS.unlink(file_out);
  }
  catch {
  }

  // Run Metron
  var path = selector_name.split("/");
  var root = path.slice(0, path.length - 1).join("/");
  var file = path[path.length - 1];

  var args = [];
  //args.push("-q");
  args.push("-s");
  args.push("-r");
  args.push(root);
  args.push(file);

  //term.clear();
  //log_cmdline.innerText = "metron " + args.join(" ");
  console.log("metron " + args.join(" "));

  stdout = "";
  stderr = "";

  let ret = mod.callMain(args);

  // Read the output file back to the dst pane, or clear if no file.
  if (ret == 0) {
    //tut1_v_jar.updateCode(stdout);
    tut1_v_jar.updateCode(new TextDecoder().decode(mod.FS.readFile(file_out)));
    document.getElementById("tut1_v_bar").style.backgroundColor = "#353";
  }
  else {
    tut1_v_jar.updateCode(stderr);
    document.getElementById("tut1_v_bar").style.backgroundColor = "#533";
  }

}

//------------------------------------------------------------------------------

let compile_timeout = null;
let tut1_c_filename = "./examples/tutorial/tutorial1.h";
let tut1_v_filename = "./examples/tutorial/tutorial1.sv";


function main(mod) {
  console.log("main(mod)");
  metron_mod = mod;

  // Load all the filenames in our filesystem.
  let src_paths = [];
  dump_fs(mod.FS, ".", src_paths);
  src_paths.sort();

  console.log(src_paths);

  let tut1_c_bar_text = document.getElementById("tut1_c_bar_text");
  let tut1_c_bar = document.getElementById("tut1_c_bar");

  tut1_c_bar_text.innerText = tut1_c_filename;
  tut1_c_jar.updateCode(new TextDecoder().decode(mod.FS.readFile(tut1_c_filename)));

  convert(mod);

  tut1_c_jar.onUpdate(() => {
    tut1_c_bar.style.backgroundColor = "#335";
    clearTimeout(compile_timeout);
    compile_timeout = setTimeout(() => convert(mod), 200);
  });

  /*
  var args = [];
  if (1) args.push("-e");
  if (1) args.push("--dump");
  if (0) args.push("-q");
  if (1) args.push("-v");
  if (0) args.push("-s");
  args.push("-r");
  args.push("examples/tutorial");
  args.push("tutorial1.h");

  tut1_v.innerText = "";

  let ret = mod.callMain(args);
  console.log("ret was " + ret);

  if (ret == 0) {
    tut1_v_jar.updateCode(stdout);
    document.getElementById("tut1_v_bar").style.backgroundColor = "#353";
  }
  else {
    tut1_v_jar.updateCode(stderr);
    document.getElementById("tut1_v_bar").style.backgroundColor = "#533";
  }
  */

}
