import { CodeJar } from "https://cdn.jsdelivr.net/npm/codejar@3.6.0/codejar.min.js";

import Module from "../app/metron.js"

let metron_mod = null;
let stdout = "";
let stderr = "";

//------------------------------------------------------------------------------

const highlight = editor => {
  editor.textContent = editor.textContent
  hljs.highlightElement(editor)
}

class SourcePane {
  constructor(pane_div, filename) {
    this.div = pane_div;
    this.filename = filename;
    this.div_bar = pane_div.querySelector(".bar");
    this.div_bar_text = this.div_bar.querySelector(".bar_text");
    this.div_jar = pane_div.querySelector(".jar");
    this.code_jar = new CodeJar(this.div_jar, highlight, {tab:"  "});
  }

  set_filename(new_name) {
    this.filename = new_name;
    this.div_bar_text.innerText = new_name;
  }
};

class Tutorial {
  constructor(name, div, src_filename, dst_filename) {
    this.name = name;
    this.div = div;
    this.src_filename = src_filename;
    this.dst_filename = dst_filename;
    this.src_pane = new SourcePane(div.querySelector(".src"), src_filename);
    this.dst_pane = new SourcePane(div.querySelector(".dst"), dst_filename);
    this.compile_timeout = null;

    this.src_jar = this.src_pane.code_jar;
    this.dst_jar = this.dst_pane.code_jar;

    let src_contents = null;
    let dst_contents = null;

    try {
      src_contents = new TextDecoder().decode(metron_mod.FS.readFile(src_filename));
    } catch {}

    try {
      dst_contents = new TextDecoder().decode(metron_mod.FS.readFile(dst_filename));
    } catch {}

    this.src_pane.code_jar.updateCode(src_contents);
    this.dst_pane.code_jar.updateCode(dst_contents);


    this.src_pane.code_jar.onUpdate(() => this.queue_conversion());

    convert(this);
  }

  get source() {
    return this.src_pane.code_jar.toString();
  }

  set verilog(src) {
    this.dst_pane.code_jar.updateCode(src);
  }

  queue_conversion() {
    this.dst_pane.div_bar.style.backgroundColor = "#333";
    clearTimeout(this.compile_timeout);
    this.compile_timeout = setTimeout(() => {
      convert(this);
    }, 200);
  }

  on_convert_ok() {
    this.verilog = new TextDecoder().decode(metron_mod.FS.readFile(this.dst_filename));
    this.dst_pane.div_bar.style.backgroundColor = "#353";
  }

  on_convert_err() {
    this.verilog = stderr;
    this.dst_pane.div_bar.style.backgroundColor = "#533";
  }
}

//------------------------------------------------------------------------------
// Set up Emscripten module to run main() when it's ready.

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

function convert(tutorial) {
  // Write the potentially modified source back to the filesystem
  let blob = new TextEncoder().encode(tutorial.source);
  metron_mod.FS.writeFile(tutorial.src_filename, blob);

  // Change dst pane title & delete any existing output file
  try {
    metron_mod.FS.unlink(tutorial.dst_filename);
  }
  catch {
  }

  // Run Metron
  var path = tutorial.src_filename.split("/");
  var root = path.slice(0, path.length - 1).join("/");
  var file = path[path.length - 1];

  stdout = "";
  stderr = "";

  let ret = metron_mod.callMain(["-s", "-r", root, file]);

  // Read the output file back to the dst pane, or clear if no file.
  if (ret == 0) {
    tutorial.on_convert_ok();
  }
  else {
    tutorial.on_convert_err();
  }

}

//------------------------------------------------------------------------------

let tutorials = [];

function main(mod) {
  console.log("main(mod)");
  metron_mod = mod;

  let tut_index = 1

  let tut_div = document.getElementById(`tut${tut_index}`);

  if (tut_div) {
    let tut1_obj = new Tutorial(
      `Tutorial ${tut_index}`,
      tut_div,
      `./examples/tutorial/tutorial${tut_index}.h`,
      `./examples/tutorial/tutorial${tut_index}.sv`
    );
    tutorials.push(tut1_obj);
  }
}
