  import { CodeJar } from "../assets/codejar.min.js";

import Module from "../app/metron.js"

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
// Set up Emscripten module to run main() when it's ready.

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
const dst_jar = CodeJar(dst_text, highlight, {tab:"  "});

//------------------------------------------------------------------------------
// Set up Terminal

var term = new Terminal({
  fontSize: 14,
  fontFamily: "monospace",
  convertEol: true,
  theme: {
    background: '#1E1E1E'
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

  // Load all the filenames in our filesystem.
  let src_paths = [];
  dump_fs(mod.FS, ".", src_paths);
  src_paths.sort();

  // Put all those names into the selector widget.
  src_selector.innerHTML = "";
  for (let i = 0; i < src_paths.length; i++) {
    var opt = document.createElement('option');
    opt.value = src_paths[i];
    opt.innerHTML = src_paths[i];
    src_selector.appendChild(opt);
  }

  // Load scratch.h by default.
  var initial_file = "./examples/scratch.h";
  src_selector.selectedIndex = src_paths.indexOf(initial_file);
  src_jar.updateCode(new TextDecoder().decode(mod.FS.readFile(initial_file)));

  // When the user changes the selected file, load and convert it (updateCode
  // doesn't trigger onUpdate)
  src_selector.addEventListener('change', function(event) {
    var selector_name = src_selector.options[src_selector.selectedIndex].value;
    src_jar.updateCode(new TextDecoder().decode(mod.FS.readFile(selector_name)));
    convert(mod);
  });

  // Re-convert whenever the user edits the source pane or changes an option.
  src_jar.onUpdate(() => convert(mod));
  opt_help.addEventListener("change", () => convert(mod));
  opt_echo.addEventListener("change", () => convert(mod));
  opt_dump.addEventListener("change", () => convert(mod));
  opt_quiet.addEventListener("change", () => convert(mod));
  opt_verbose.addEventListener("change", () => convert(mod));
  opt_save.addEventListener("change", () => convert(mod));

  // Start with save mode on and trigger convert so we generate scratch.sv.
  opt_save.checked = true;
  convert(mod);
}

//------------------------------------------------------------------------------
// Convert the selected .h file to Verilog and update the panes.

function convert(mod) {
  var selector_name = src_selector.options[src_selector.selectedIndex].value;

  // Write the potentially modified source back to the filesystem
  let blob = new TextEncoder().encode(src_jar.toString());
  mod.FS.writeFile(selector_name, blob);

  // Abort conversion if file doesn't end with ".h"
  if (!selector_name.endsWith(".h")) return;

  // Change dst pane title & delete any existing output file
  var file_out = selector_name.substr(0, selector_name.lastIndexOf('.')) + ".sv";
  dst_title.innerText = file_out;
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
  if (opt_echo.checked) args.push("-e");
  if (opt_dump.checked) args.push("--dump");
  if (opt_quiet.checked) args.push("-q");
  if (opt_verbose.checked) args.push("-v");
  if (opt_save.checked) args.push("-s");
  args.push("-r");
  args.push(root);
  args.push(file);
  if (opt_help.checked) args = ["-h"];

  term.clear();
  log_cmdline.innerText = "metron " + args.join(" ");
  mod.callMain(args);

  // Read the output file back to the dst pane, or clear if no file.
  try {
    dst_jar.updateCode(new TextDecoder().decode(mod.FS.readFile(file_out)));
  }
  catch {
    dst_jar.updateCode("");
  }
}

//------------------------------------------------------------------------------
