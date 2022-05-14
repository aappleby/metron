"use strict";

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

const fitAddon = new FitAddon();
term.loadAddon(fitAddon);
term.open(log_text);
fitAddon.fit();

window.addEventListener('resize', () => fitAddon.fit());

//------------------------------------------------------------------------------
// Set up Emscripten module

var Module = {};

Module.print = function (text) {
  term.write(text + "\n");
};

Module.locateFile = function (path, prefix) {
  return "bin/" + path;
}

function dump_fs(path) {

  if (FS.isDir(FS.stat(path).mode)) {
    let names = FS.readdir(path);
    for (let i = 0; i < names.length; i++) {
      let name = names[i];
      if (name == "." || name == "..") continue;
      dump_fs(path + "/" + name);
    }
  }
  else {
    if (path.endsWith(".h")) {
      src_paths.push(path);
      var opt = document.createElement('option');
      opt.value = path;
      opt.innerHTML = path;
      src_selector.appendChild(opt);
    }
  }
}

Module.onRuntimeInitialized = function() {
  src_selector.innerHTML = "";
  dump_fs("examples");

  var initial_file = "examples/scratch.h";
  src_selector.selectedIndex = src_paths.indexOf(initial_file);
  src_text.innerText = new TextDecoder().decode(FS.readFile(initial_file));

  src_selector.addEventListener('change', function(event) {
    // Load the new file
    var selector_name = src_selector.options[src_selector.selectedIndex].value;
    src_text.innerText = new TextDecoder().decode(FS.readFile(selector_name));

    // And convert it
    convert();
  });

  // Re-convert whenever the user edits the source pane or changes an option.
  src_text.addEventListener("input", () => convert());
  opt_help.addEventListener("change", () => convert());
  opt_echo.addEventListener("change", () => convert());
  opt_dump.addEventListener("change", () => convert());
  opt_quiet.addEventListener("change", () => convert());
  opt_verbose.addEventListener("change", () => convert());
  opt_save.addEventListener("change", () => convert());

  opt_save.checked = true;

  convert();
  fitAddon.fit();
}

Module.noInitialRun = true;

//------------------------------------------------------------------------------

let src_paths = [];

function convert() {
  term.clear();
  //term.setOption('theme', { fontSize: 22 })
  //fitAddon.fit();

  dst_text.innerText = "";

  var selector_name = src_selector.options[src_selector.selectedIndex].value;

  // Write the potentially modified source back to the filesystem
  let blob = new TextEncoder().encode(src_text.innerText);
  FS.writeFile(selector_name, blob);

  // Delete any existing output file
  var file_out = selector_name.substr(0, selector_name.lastIndexOf('.')) + ".sv";

  dst_title.innerText = file_out;

  console.log("unlinking " + file_out);
  try {
    FS.unlink(file_out);
    console.log("unlink ok");
  }
  catch {
    console.log("unlink fail");
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

  Module.callMain(args);

  // Read the output file back to the dst pane
  try {
    dst_text.innerText = new TextDecoder().decode(FS.readFile(file_out));
  }
  catch {
  }
}

//------------------------------------------------------------------------------
