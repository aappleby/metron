import { CodeJar } from "https://cdn.jsdelivr.net/npm/codejar@3.6.0/codejar.min.js";

import Module from "./metron.js"

var src_selector = document.getElementById("src_selector");
var src_controls = document.getElementById("src_controls");
var src_text = document.getElementById("src_text");

var opt_help = document.getElementById("opt_help");
var opt_echo = document.getElementById("opt_echo");
var opt_dump = document.getElementById("opt_dump");
var opt_quiet = document.getElementById("opt_quiet");
var opt_verbose = document.getElementById("opt_verbose");
var opt_save = document.getElementById("opt_save");

var dst_title = document.getElementById("dst_title");
var dst_text = document.getElementById("dst_text");

var log_text = document.getElementById("log_text");
var log_cmdline = document.getElementById("log_cmdline");

//------------------------------------------------------------------------------
// Set up Emscripten module

var mod_options = window.Module;

mod_options.noInitialRun = true;

mod_options.print = function(text) {
  term.write(text + "\n");
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


const src_jar = CodeJar(src_text, highlight, {tab:"  "});

// readOnly?
const dst_jar = CodeJar(dst_text, highlight, {tab:"  "});

//------------------------------------------------------------------------------
// Set up terminal

var term = new Terminal({
  fontSize: 14,
  //fontFamily: "Roboto Mono", // xterm.js 3 acts weird if it has to load a font
  fontFamily: "Consolas",
  convertEol: true,
  theme: {
    background: '#222'
  },
  scrollback: 9999,
});

const fitAddon = new FitAddon.FitAddon();
term.loadAddon(fitAddon);
term.open(log_text);
fitAddon.fit();

window.addEventListener('resize', () => {
  console.log("resize");
  fitAddon.fit();
});

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

function main(mod) {
  console.log("main() " + performance.now());
  let src_paths = [];
  dump_fs(mod.FS, ".", src_paths);
  //console.log(src_paths);

  src_paths.sort();

  src_selector.innerHTML = "";
  for (let i = 0; i < src_paths.length; i++) {
    var opt = document.createElement('option');
    opt.value = src_paths[i];
    opt.innerHTML = src_paths[i];
    src_selector.appendChild(opt);
  }

  var initial_file = "./examples/scratch.h";
  src_selector.selectedIndex = src_paths.indexOf(initial_file);
  src_jar.updateCode(new TextDecoder().decode(mod.FS.readFile(initial_file)));

  src_selector.addEventListener('change', function(event) {
    // Load the new file
    var selector_name = src_selector.options[src_selector.selectedIndex].value;
    src_jar.updateCode(new TextDecoder().decode(mod.FS.readFile(selector_name)));

    // And convert it
    convert(mod);
  });

  // Re-convert whenever the user edits the source pane or changes an option.
  //src_text.addEventListener("input", () => convert(mod));
  src_jar.onUpdate(() => convert(mod));

  opt_help.addEventListener("change", () => convert(mod));
  opt_echo.addEventListener("change", () => convert(mod));
  opt_dump.addEventListener("change", () => convert(mod));
  opt_quiet.addEventListener("change", () => convert(mod));
  opt_verbose.addEventListener("change", () => convert(mod));
  opt_save.addEventListener("change", () => convert(mod));

  opt_save.checked = true;

  convert(mod);
  fitAddon.fit();
}

//------------------------------------------------------------------------------

function convert(mod) {
  term.clear();
  //term.setOption('theme', { fontSize: 22 })
  //fitAddon.fit();

  dst_jar.updateCode("");

  var selector_name = src_selector.options[src_selector.selectedIndex].value;

  // Write the potentially modified source back to the filesystem
  let blob = new TextEncoder().encode(src_jar.toString());
  mod.FS.writeFile(selector_name, blob);

  // Abort conversion if file doesn't end with ".h"
  if (!selector_name.endsWith(".h")) return;

  // Delete any existing output file
  var file_out = selector_name.substr(0, selector_name.lastIndexOf('.')) + ".sv";

  dst_title.innerText = file_out;

  //console.log("unlinking " + file_out);
  try {
    FS.unlink(file_out);
    //console.log("unlink ok");
  }
  catch {
    //console.log("unlink fail");
  }

  // Run Metron
  var path = selector_name.split("/");
  var root = path.slice(0, path.length - 1).join("/");
  var file = path[path.length - 1];
  var file_out = selector_name.substr(0, selector_name.lastIndexOf('.')) + ".sv";

  var args = [];

  if (opt_echo.checked) args.push("-e");
  if (opt_dump.checked) args.push("--dump");
  if (opt_quiet.checked) args.push("-q");
  if (opt_verbose.checked) args.push("-v");
  if (opt_save.checked) args.push("-s");

  args.push("-r");
  args.push(root);
  args.push(file);

  if (opt_help.checked) args = ["-h"];

  log_cmdline.innerText = "metron " + args.join(" ");

  mod.callMain(args);

  // Read the output file back to the dst pane
  try {
    dst_jar.updateCode(new TextDecoder().decode(mod.FS.readFile(file_out)));
  }
  catch {
  }
}

//------------------------------------------------------------------------------
