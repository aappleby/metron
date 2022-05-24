console.log("tutorial.js module load @ " + performance.now());

import { CodeJar } from "../assets/codejar.min.js";

import Module from "../app/metron.js"

let metron_mod = null;
let stdout = "";
let stderr = "";

const highlight = (editor) => {
  editor.textContent = editor.textContent
  hljs.highlightElement(editor)
}

//------------------------------------------------------------------------------

function swap_ext(filename, new_ext) {
  let base_name = filename.split(".").slice(0, -1).join(".");
  return base_name + new_ext;
}

//------------------------------------------------------------------------------

class SourcePane {
  constructor(pane_div) {
    this.pane_div = pane_div;
    this.header_bar = pane_div.querySelector(".header_bar");
    this.code_jar = new CodeJar(pane_div.querySelector(".code_jar"), highlight, {tab:"  "});
  }
};

//------------------------------------------------------------------------------

class Tutorial {
  constructor(div) {
    this.name = name;
    this.div = div;

    this.src_pane = new SourcePane(div.querySelector("#c_panel"));
    this.dst_pane = new SourcePane(div.querySelector("#v_panel"));
    this.compile_timeout = null;

    this.c_filename = this.src_pane.pane_div.querySelector(".filename");
    this.v_filename = this.dst_pane.pane_div.querySelector(".filename");

    this.c_filename.innerText = this.div.id;
    this.v_filename.innerText = swap_ext(this.div.id, ".sv");

    this.src_pane.code_jar.onUpdate(() => {
      this.queue_conversion();
    });

    this.c_filename.addEventListener("input", () => {
      this.v_filename.innerText = swap_ext(this.c_filename.innerText, ".sv");
      this.reload();
      this.queue_conversion();
    });

    this.reload();
    this.convert();
  }

  reload() {
    let src_contents = null;
    let dst_contents = null;

    try {
      src_contents = new TextDecoder().decode(metron_mod.FS.readFile(this.c_filename.innerText));
    } catch {}

    try {
      dst_contents = new TextDecoder().decode(metron_mod.FS.readFile(this.v_filename.innerText));
    } catch {}

    this.src_pane.code_jar.updateCode(src_contents);
    this.dst_pane.code_jar.updateCode(dst_contents);
  }

  queue_conversion() {
    this.dst_pane.header_bar.style.backgroundColor = "#333";
    clearTimeout(this.compile_timeout);
    this.compile_timeout = setTimeout(() => {
      this.convert();
    }, 200);
  }

  convert() {
    // Write the potentially modified source back to the filesystem
    let blob = new TextEncoder().encode(this.src_pane.code_jar.toString());
    metron_mod.FS.writeFile(this.c_filename.innerText, blob);

    // Run Metron
    var path = this.c_filename.innerText.split("/");
    var root = path.slice(0, -1).join("/");
    var file = path[path.length - 1];

    stdout = "";
    stderr = "";

    let args = ["-s", "-r", root, file];
    let ret = metron_mod.callMain(args);
    let cmdline = "metron " + args.join(" ");
    console.log(cmdline);

    // Read the output file back to the dst pane, or clear if no file.
    if (ret == 0) {
      try {
        let verilog = new TextDecoder().decode(metron_mod.FS.readFile(this.v_filename.innerText));
        this.dst_pane.code_jar.updateCode(verilog);
        this.dst_pane.header_bar.style.backgroundColor = "#353";
      }
      catch {
        this.dst_pane.code_jar.updateCode("<could not load output file>");
        this.dst_pane.header_bar.style.backgroundColor = "#553";
      }
    }
    else {
      this.dst_pane.code_jar.updateCode(stderr);
      this.dst_pane.header_bar.style.backgroundColor = "#533";
    }
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

let tutorials = [];
let code_jars = [];

function main(mod) {
  console.log("main(mod) @ " + performance.now());
  metron_mod = mod;

  /*
  let src_paths = [];
  dump_fs(mod.FS, ".", src_paths);
  src_paths.sort();
  console.log(src_paths);
  */

  let tut_divs = document.querySelectorAll(".live_code");
  for (let i = 0; i < tut_divs.length; i++) {
    let tut_div = tut_divs[i];
    let tut_obj = new Tutorial(tut_div);
    tutorials.push(tut_obj);
  }

  let code_divs = document.querySelectorAll(".code_jar");
  for (let i = 0; i < code_divs.length; i++) {
    let code_div = code_divs[i];
    let code_jar = new CodeJar(code_div, highlight, {tab:"  "});
    code_jars.push(code_jar);
  }
}

//------------------------------------------------------------------------------
