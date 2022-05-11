
function encode(s, len, mem, offset) {
  let buf = new Uint8Array(mem.buffer);
  for (let i = 0; i < len; i++) {
    buf[offset++] = s.charCodeAt(i);
  }
  buf[offset] = 0;
}

function decode(mem, offset, len) {
  let buf = new Uint8Array(mem.buffer);
  let s = "";
  for (let i = 0; i < len; i++) {
    s += String.fromCharCode(buf[offset++]);
  }
  return s;
}

let memory;
let bundle;

async function main() {
  memory = new WebAssembly.Memory({initial:1000, maximum:1000});
  WasiStub.memory = memory;
  const wasi_env = {
    env: { memory },
    wasi_snapshot_preview1: WasiStub,
  };

  bundle = await WebAssembly.instantiateStreaming(fetch("./bin/test.wasm"), wasi_env);
  document.getElementById("convert").onclick = () => convert();
}

function convert() {
  let log = document.getElementById("log");
  log.innerText = "";

  console.log("convert");
  let src = document.getElementById("src");
  let dst = document.getElementById("dst");
  const ptr = bundle.instance.exports.malloc(1024);
  console.log(src.innerText);
  encode(src.innerText, src.innerText.length, memory, ptr);
  bundle.instance.exports.metron_test(ptr, src.innerText.length);
  bundle.instance.exports.free(ptr);
}



//convert();
//
/*
document.getElementById("src").addEventListener("input", function() {
  convert();
}, false);
convert();
*/
