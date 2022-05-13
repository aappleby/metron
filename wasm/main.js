var term = new Terminal({
  //rows:60,
  //cols:140,
  fontSize: 14,
  //fontFamily: "Roboto Mono", // xterm.js 3 acts weird if it has to load a font
  convertEol: true,
  theme: {
    background: '#333'
  },
  scrollback: 9999,
});

const fitAddon = new FitAddon();
term.loadAddon(fitAddon);
term.open(document.getElementById('log'));
fitAddon.fit();

window.addEventListener('resize', () => fitAddon.fit());

var Module = {};
Module.preRun = [];
Module.postRun = [];

var element = document.getElementById('output');
if (element) element.value = ''; // clear browser cache

Module.print = function (text) {
  term.write(text + "\n");
};

Module.setStatus = function (text) {
  term.write(`Emscripten status : ${text}\n`);
}

Module.totalDependencies = 0;

Module.onRuntimeInitialized = function () {
  console.log("init!\n");
}

window.onerror = function () {
  Module.setStatus('Exception thrown, see JavaScript console');
  Module.setStatus = function (text) {
    if (text) console.error('[post-exception status] ' + text);
  };
  term.write('Exception thrown, see JavaScript console');
};

Module.arguments = ["-v", "-e", "-c", "-r", "examples/uart/metron", "uart_top.h"];

Module.locateFile = function (path, prefix) {
  return "bin/" + path;
}

function main() {
  console.log("main()");
}

document.body.onload = () => main();

Module.noInitialRun = true;

document.getElementById("convert").onclick = () => {
  term.clear();
  document.getElementById("src").innerText = new TextDecoder().decode(FS.readFile("scratch.h"));
  //Module.callMain(["-v", "-e", "-c", "-r", "examples/uart/metron", "uart_top.h"]);
  Module.callMain(["-v", "-e", "-c", "scratch.h"]);
  document.getElementById("dst").innerText = new TextDecoder().decode(FS.readFile("scratch.sv"));

};
