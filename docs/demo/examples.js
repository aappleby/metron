
  var Module = typeof Module !== 'undefined' ? Module : {};

  if (!Module.expectedDataFileDownloads) {
    Module.expectedDataFileDownloads = 0;
  }

  Module.expectedDataFileDownloads++;
  (function() {
    // Do not attempt to redownload the virtual filesystem data when in a pthread or a Wasm Worker context.
    if (Module['ENVIRONMENT_IS_PTHREAD'] || Module['$ww']) return;
    var loadPackage = function(metadata) {

      var PACKAGE_PATH = '';
      if (typeof window === 'object') {
        PACKAGE_PATH = window['encodeURIComponent'](window.location.pathname.toString().substring(0, window.location.pathname.toString().lastIndexOf('/')) + '/');
      } else if (typeof process === 'undefined' && typeof location !== 'undefined') {
        // web worker
        PACKAGE_PATH = encodeURIComponent(location.pathname.toString().substring(0, location.pathname.toString().lastIndexOf('/')) + '/');
      }
      var PACKAGE_NAME = 'docs/demo/examples.data';
      var REMOTE_PACKAGE_BASE = 'examples.data';
      if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
        Module['locateFile'] = Module['locateFilePackage'];
        err('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
      }
      var REMOTE_PACKAGE_NAME = Module['locateFile'] ? Module['locateFile'](REMOTE_PACKAGE_BASE, '') : REMOTE_PACKAGE_BASE;
var REMOTE_PACKAGE_SIZE = metadata['remote_package_size'];

      function fetchRemotePackage(packageName, packageSize, callback, errback) {
        
        var xhr = new XMLHttpRequest();
        xhr.open('GET', packageName, true);
        xhr.responseType = 'arraybuffer';
        xhr.onprogress = function(event) {
          var url = packageName;
          var size = packageSize;
          if (event.total) size = event.total;
          if (event.loaded) {
            if (!xhr.addedTotal) {
              xhr.addedTotal = true;
              if (!Module.dataFileDownloads) Module.dataFileDownloads = {};
              Module.dataFileDownloads[url] = {
                loaded: event.loaded,
                total: size
              };
            } else {
              Module.dataFileDownloads[url].loaded = event.loaded;
            }
            var total = 0;
            var loaded = 0;
            var num = 0;
            for (var download in Module.dataFileDownloads) {
            var data = Module.dataFileDownloads[download];
              total += data.total;
              loaded += data.loaded;
              num++;
            }
            total = Math.ceil(total * Module.expectedDataFileDownloads/num);
            if (Module['setStatus']) Module['setStatus'](`Downloading data... (${loaded}/${total})`);
          } else if (!Module.dataFileDownloads) {
            if (Module['setStatus']) Module['setStatus']('Downloading data...');
          }
        };
        xhr.onerror = function(event) {
          throw new Error("NetworkError for: " + packageName);
        }
        xhr.onload = function(event) {
          if (xhr.status == 200 || xhr.status == 304 || xhr.status == 206 || (xhr.status == 0 && xhr.response)) { // file URLs can return 0
            var packageData = xhr.response;
            callback(packageData);
          } else {
            throw new Error(xhr.statusText + " : " + xhr.responseURL);
          }
        };
        xhr.send(null);
      };

      function handleError(error) {
        console.error('package error:', error);
      };

      var fetchedCallback = null;
      var fetched = Module['getPreloadedPackage'] ? Module['getPreloadedPackage'](REMOTE_PACKAGE_NAME, REMOTE_PACKAGE_SIZE) : null;

      if (!fetched) fetchRemotePackage(REMOTE_PACKAGE_NAME, REMOTE_PACKAGE_SIZE, function(data) {
        if (fetchedCallback) {
          fetchedCallback(data);
          fetchedCallback = null;
        } else {
          fetched = data;
        }
      }, handleError);

    function runWithFS() {

      function assert(check, msg) {
        if (!check) throw msg + new Error().stack;
      }
Module['FS_createPath']("/", "examples", true, true);
Module['FS_createPath']("/examples", "gb_spu", true, true);
Module['FS_createPath']("/examples/gb_spu", "metron", true, true);
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples/j1", "metron", true, true);
Module['FS_createPath']("/examples", "picorv32", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/examples/uart", "metron", true, true);
Module['FS_createPath']("/", "tests", true, true);
Module['FS_createPath']("/tests", "lockstep", true, true);
Module['FS_createPath']("/tests", "metron", true, true);
Module['FS_createPath']("/tests/metron", "fail", true, true);
Module['FS_createPath']("/tests/metron", "generated", true, true);
Module['FS_createPath']("/tests/metron", "pass", true, true);
Module['FS_createPath']("/tests", "risc-v", true, true);
Module['FS_createPath']("/tests/risc-v", "instructions", true, true);
Module['FS_createPath']("/tests", "tools", true, true);
Module['FS_createPath']("/tests/tools", "icarus", true, true);
Module['FS_createPath']("/tests/tools/icarus", "fail", true, true);
Module['FS_createPath']("/tests/tools/icarus", "pass", true, true);
Module['FS_createPath']("/tests/tools", "pass", true, true);
Module['FS_createPath']("/tests/tools", "verilator", true, true);
Module['FS_createPath']("/tests/tools/verilator", "fail", true, true);
Module['FS_createPath']("/tests/tools/verilator", "pass", true, true);
Module['FS_createPath']("/tests/tools", "yosys", true, true);
Module['FS_createPath']("/tests/tools/yosys", "fail", true, true);
Module['FS_createPath']("/tests/tools/yosys", "pass", true, true);

      /** @constructor */
      function DataRequest(start, end, audio) {
        this.start = start;
        this.end = end;
        this.audio = audio;
      }
      DataRequest.prototype = {
        requests: {},
        open: function(mode, name) {
          this.name = name;
          this.requests[name] = this;
          Module['addRunDependency'](`fp ${this.name}`);
        },
        send: function() {},
        onload: function() {
          var byteArray = this.byteArray.subarray(this.start, this.end);
          this.finish(byteArray);
        },
        finish: function(byteArray) {
          var that = this;
          // canOwn this data in the filesystem, it is a slide into the heap that will never change
          Module['FS_createDataFile'](this.name, null, byteArray, true, true, true);
          Module['removeRunDependency'](`fp ${that.name}`);
          this.requests[this.name] = null;
        }
      };

      var files = metadata['files'];
      for (var i = 0; i < files.length; ++i) {
        new DataRequest(files[i]['start'], files[i]['end'], files[i]['audio'] || 0).open('GET', files[i]['filename']);
      }

      function processPackageData(arrayBuffer) {
        assert(arrayBuffer, 'Loading data file failed.');
        assert(arrayBuffer.constructor.name === ArrayBuffer.name, 'bad input to processPackageData');
        var byteArray = new Uint8Array(arrayBuffer);
        var curr;
        // Reuse the bytearray from the XHR as the source for file reads.
          DataRequest.prototype.byteArray = byteArray;
          var files = metadata['files'];
          for (var i = 0; i < files.length; ++i) {
            DataRequest.prototype.requests[files[i].filename].onload();
          }          Module['removeRunDependency']('datafile_docs/demo/examples.data');

      };
      Module['addRunDependency']('datafile_docs/demo/examples.data');

      if (!Module.preloadResults) Module.preloadResults = {};

      Module.preloadResults[PACKAGE_NAME] = {fromCache: false};
      if (fetched) {
        processPackageData(fetched);
        fetched = null;
      } else {
        fetchedCallback = processPackageData;
      }

    }
    if (Module['calledRun']) {
      runWithFS();
    } else {
      if (!Module['preRun']) Module['preRun'] = [];
      Module["preRun"].push(runWithFS); // FS is not initialized yet, wait for it
    }

    }
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/picorv32/picorv32.h", "start": 71623, "end": 166827}, {"filename": "/examples/pong/README.md", "start": 166827, "end": 166887}, {"filename": "/examples/pong/metron/pong.h", "start": 166887, "end": 170878}, {"filename": "/examples/pong/reference/README.md", "start": 170878, "end": 170938}, {"filename": "/examples/rvsimple/README.md", "start": 170938, "end": 171017}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 171017, "end": 171524}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 171524, "end": 172992}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 172992, "end": 175604}, {"filename": "/examples/rvsimple/metron/config.h", "start": 175604, "end": 176826}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 176826, "end": 182552}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 182552, "end": 183669}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 183669, "end": 185609}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 185609, "end": 186849}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 186849, "end": 188100}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 188100, "end": 188779}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 188779, "end": 189750}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 189750, "end": 191875}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 191875, "end": 192646}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 192646, "end": 193333}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 193333, "end": 194156}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 194156, "end": 195162}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 195162, "end": 196140}, {"filename": "/examples/rvsimple/metron/register.h", "start": 196140, "end": 196836}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 196836, "end": 199886}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 199886, "end": 205467}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 205467, "end": 207967}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 207967, "end": 212705}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 212705, "end": 214700}, {"filename": "/examples/scratch.h", "start": 214700, "end": 215068}, {"filename": "/examples/tutorial/adder.h", "start": 215068, "end": 215248}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 215248, "end": 216236}, {"filename": "/examples/tutorial/blockram.h", "start": 216236, "end": 216768}, {"filename": "/examples/tutorial/checksum.h", "start": 216768, "end": 217510}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 217510, "end": 217917}, {"filename": "/examples/tutorial/counter.h", "start": 217917, "end": 218068}, {"filename": "/examples/tutorial/declaration_order.h", "start": 218068, "end": 218894}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 218894, "end": 219853}, {"filename": "/examples/tutorial/nonblocking.h", "start": 219853, "end": 219979}, {"filename": "/examples/tutorial/submodules.h", "start": 219979, "end": 221079}, {"filename": "/examples/tutorial/templates.h", "start": 221079, "end": 221562}, {"filename": "/examples/tutorial/tutorial2.h", "start": 221562, "end": 221630}, {"filename": "/examples/tutorial/tutorial3.h", "start": 221630, "end": 221671}, {"filename": "/examples/tutorial/tutorial4.h", "start": 221671, "end": 221712}, {"filename": "/examples/tutorial/tutorial5.h", "start": 221712, "end": 221753}, {"filename": "/examples/tutorial/vga.h", "start": 221753, "end": 222933}, {"filename": "/examples/uart/README.md", "start": 222933, "end": 223177}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 223177, "end": 225815}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 225815, "end": 228506}, {"filename": "/examples/uart/metron/uart_top.h", "start": 228506, "end": 230294}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 230294, "end": 233351}, {"filename": "/tests/lockstep/README.md", "start": 233351, "end": 233462}, {"filename": "/tests/lockstep/counter.h", "start": 233462, "end": 233805}, {"filename": "/tests/lockstep/funcs_and_tasks.h", "start": 233805, "end": 234607}, {"filename": "/tests/lockstep/lfsr.h", "start": 234607, "end": 235097}, {"filename": "/tests/lockstep/lockstep_bad.h", "start": 235097, "end": 235596}, {"filename": "/tests/lockstep/timeout_bad.h", "start": 235596, "end": 236025}, {"filename": "/tests/metron/fail/README.md", "start": 236025, "end": 236222}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 236222, "end": 236529}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 236529, "end": 236784}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 236784, "end": 237004}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 237004, "end": 237490}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 237490, "end": 237727}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 237727, "end": 238144}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 238144, "end": 238610}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 238610, "end": 238928}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 238928, "end": 239380}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 239380, "end": 239637}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 239637, "end": 239919}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 239919, "end": 240155}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 240155, "end": 240385}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 240385, "end": 240754}, {"filename": "/tests/metron/generated/.gitignore", "start": 240754, "end": 240758}, {"filename": "/tests/metron/generated/all_func_types.sv.d", "start": 240758, "end": 240835}, {"filename": "/tests/metron/generated/basic_constructor.sv.d", "start": 240835, "end": 240918}, {"filename": "/tests/metron/generated/basic_function.sv.d", "start": 240918, "end": 240995}, {"filename": "/tests/metron/generated/basic_increment.sv.d", "start": 240995, "end": 241074}, {"filename": "/tests/metron/generated/basic_inputs.sv.d", "start": 241074, "end": 241147}, {"filename": "/tests/metron/generated/basic_literals.sv.d", "start": 241147, "end": 241224}, {"filename": "/tests/metron/generated/basic_localparam.sv.d", "start": 241224, "end": 241305}, {"filename": "/tests/metron/generated/basic_output.sv.d", "start": 241305, "end": 241378}, {"filename": "/tests/metron/generated/basic_param.sv.d", "start": 241378, "end": 241449}, {"filename": "/tests/metron/generated/basic_public_reg.sv.d", "start": 241449, "end": 241530}, {"filename": "/tests/metron/generated/basic_public_sig.sv.d", "start": 241530, "end": 241611}, {"filename": "/tests/metron/generated/basic_reg_rww.sv.d", "start": 241611, "end": 241686}, {"filename": "/tests/metron/generated/basic_sig_wwr.sv.d", "start": 241686, "end": 241761}, {"filename": "/tests/metron/generated/basic_submod.sv.d", "start": 241761, "end": 241834}, {"filename": "/tests/metron/generated/basic_submod_param.sv.d", "start": 241834, "end": 241919}, {"filename": "/tests/metron/generated/basic_submod_public_reg.sv.d", "start": 241919, "end": 242014}, {"filename": "/tests/metron/generated/basic_switch.sv.d", "start": 242014, "end": 242087}, {"filename": "/tests/metron/generated/basic_task.sv.d", "start": 242087, "end": 242156}, {"filename": "/tests/metron/generated/basic_template.sv.d", "start": 242156, "end": 242233}, {"filename": "/tests/metron/generated/bit_casts.sv.d", "start": 242233, "end": 242300}, {"filename": "/tests/metron/generated/bit_concat.sv.d", "start": 242300, "end": 242369}, {"filename": "/tests/metron/generated/bit_dup.sv.d", "start": 242369, "end": 242432}, {"filename": "/tests/metron/generated/bitfields.sv.d", "start": 242432, "end": 242499}, {"filename": "/tests/metron/generated/both_tock_and_tick_use_tasks_and_funcs.sv.d", "start": 242499, "end": 242624}, {"filename": "/tests/metron/generated/builtins.sv.d", "start": 242624, "end": 242689}, {"filename": "/tests/metron/generated/call_tick_from_tock.sv.d", "start": 242689, "end": 242776}, {"filename": "/tests/metron/generated/case_with_fallthrough.sv.d", "start": 242776, "end": 242867}, {"filename": "/tests/metron/generated/constructor_arg_passing.sv.d", "start": 242867, "end": 242962}, {"filename": "/tests/metron/generated/constructor_args.sv.d", "start": 242962, "end": 243043}, {"filename": "/tests/metron/generated/counter.sv.d", "start": 243043, "end": 243106}, {"filename": "/tests/metron/generated/defines.sv.d", "start": 243106, "end": 243169}, {"filename": "/tests/metron/generated/dontcare.sv.d", "start": 243169, "end": 243234}, {"filename": "/tests/metron/generated/enum_simple.sv.d", "start": 243234, "end": 243305}, {"filename": "/tests/metron/generated/for_loops.sv.d", "start": 243305, "end": 243372}, {"filename": "/tests/metron/generated/good_order.sv.d", "start": 243372, "end": 243441}, {"filename": "/tests/metron/generated/if_with_compound.sv.d", "start": 243441, "end": 243522}, {"filename": "/tests/metron/generated/include_guards.sv.d", "start": 243522, "end": 243599}, {"filename": "/tests/metron/generated/include_test_module.sv.d", "start": 243599, "end": 243729}, {"filename": "/tests/metron/generated/include_test_submod.sv.d", "start": 243729, "end": 243816}, {"filename": "/tests/metron/generated/init_chain.sv.d", "start": 243816, "end": 243885}, {"filename": "/tests/metron/generated/initialize_struct_to_zero.sv.d", "start": 243885, "end": 243984}, {"filename": "/tests/metron/generated/input_signals.sv.d", "start": 243984, "end": 244059}, {"filename": "/tests/metron/generated/local_localparam.sv.d", "start": 244059, "end": 244140}, {"filename": "/tests/metron/generated/magic_comments.sv.d", "start": 244140, "end": 244217}, {"filename": "/tests/metron/generated/matching_port_and_arg_names.sv.d", "start": 244217, "end": 244320}, {"filename": "/tests/metron/generated/minimal.sv.d", "start": 244320, "end": 244383}, {"filename": "/tests/metron/generated/multi_tick.sv.d", "start": 244383, "end": 244452}, {"filename": "/tests/metron/generated/namespaces.sv.d", "start": 244452, "end": 244521}, {"filename": "/tests/metron/generated/nested_structs.sv.d", "start": 244521, "end": 244598}, {"filename": "/tests/metron/generated/nested_submod_calls.sv.d", "start": 244598, "end": 244685}, {"filename": "/tests/metron/generated/noconvert.sv.d", "start": 244685, "end": 244752}, {"filename": "/tests/metron/generated/nonblocking_assign_to_struct_union.sv.d", "start": 244752, "end": 244869}, {"filename": "/tests/metron/generated/oneliners.sv.d", "start": 244869, "end": 244936}, {"filename": "/tests/metron/generated/plus_equals.sv.d", "start": 244936, "end": 245007}, {"filename": "/tests/metron/generated/preproc.sv.d", "start": 245007, "end": 245070}, {"filename": "/tests/metron/generated/private_getter.sv.d", "start": 245070, "end": 245147}, {"filename": "/tests/metron/generated/self_reference.sv.d", "start": 245147, "end": 245224}, {"filename": "/tests/metron/generated/slice.sv.d", "start": 245224, "end": 245283}, {"filename": "/tests/metron/generated/structs.sv.d", "start": 245283, "end": 245346}, {"filename": "/tests/metron/generated/structs_as_args.sv.d", "start": 245346, "end": 245425}, {"filename": "/tests/metron/generated/structs_as_ports.sv.d", "start": 245425, "end": 245506}, {"filename": "/tests/metron/generated/submod_bindings.sv.d", "start": 245506, "end": 245585}, {"filename": "/tests/metron/generated/tock_task.sv.d", "start": 245585, "end": 245652}, {"filename": "/tests/metron/generated/trivial_adder.sv.d", "start": 245652, "end": 245727}, {"filename": "/tests/metron/generated/unions.sv.d", "start": 245727, "end": 245788}, {"filename": "/tests/metron/generated/utf8-mod.bom.sv.d", "start": 245788, "end": 245861}, {"filename": "/tests/metron/generated/utf8-mod.sv.d", "start": 245861, "end": 245926}, {"filename": "/tests/metron/pass/README.md", "start": 245926, "end": 246007}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 246007, "end": 247508}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 247508, "end": 247925}, {"filename": "/tests/metron/pass/basic_function.h", "start": 247925, "end": 248177}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 248177, "end": 248504}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 248504, "end": 248809}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 248809, "end": 249428}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 249428, "end": 249684}, {"filename": "/tests/metron/pass/basic_output.h", "start": 249684, "end": 249956}, {"filename": "/tests/metron/pass/basic_param.h", "start": 249956, "end": 250225}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 250225, "end": 250426}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 250426, "end": 250613}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 250613, "end": 250844}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 250844, "end": 251071}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 251071, "end": 251387}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 251387, "end": 251752}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 251752, "end": 252138}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 252138, "end": 252627}, {"filename": "/tests/metron/pass/basic_task.h", "start": 252627, "end": 252962}, {"filename": "/tests/metron/pass/basic_template.h", "start": 252962, "end": 253464}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 253464, "end": 259444}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 259444, "end": 259878}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 259878, "end": 260544}, {"filename": "/tests/metron/pass/bitfields.h", "start": 260544, "end": 261750}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 261750, "end": 262567}, {"filename": "/tests/metron/pass/builtins.h", "start": 262567, "end": 262905}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 262905, "end": 263222}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 263222, "end": 263807}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 263807, "end": 264670}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 264670, "end": 265142}, {"filename": "/tests/metron/pass/counter.h", "start": 265142, "end": 265789}, {"filename": "/tests/metron/pass/defines.h", "start": 265789, "end": 266110}, {"filename": "/tests/metron/pass/dontcare.h", "start": 266110, "end": 266397}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 266397, "end": 267792}, {"filename": "/tests/metron/pass/for_loops.h", "start": 267792, "end": 268119}, {"filename": "/tests/metron/pass/good_order.h", "start": 268119, "end": 268322}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 268322, "end": 268743}, {"filename": "/tests/metron/pass/include_guards.h", "start": 268743, "end": 268940}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 268940, "end": 269131}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 269131, "end": 269308}, {"filename": "/tests/metron/pass/init_chain.h", "start": 269308, "end": 270029}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 270029, "end": 270321}, {"filename": "/tests/metron/pass/input_signals.h", "start": 270321, "end": 270994}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 270994, "end": 271179}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 271179, "end": 271492}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 271492, "end": 271813}, {"filename": "/tests/metron/pass/minimal.h", "start": 271813, "end": 271993}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 271993, "end": 272369}, {"filename": "/tests/metron/pass/namespaces.h", "start": 272369, "end": 272750}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 272750, "end": 273211}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 273211, "end": 273784}, {"filename": "/tests/metron/pass/noconvert.h", "start": 273784, "end": 274114}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 274114, "end": 274467}, {"filename": "/tests/metron/pass/oneliners.h", "start": 274467, "end": 274741}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 274741, "end": 275228}, {"filename": "/tests/metron/pass/preproc.h", "start": 275228, "end": 275556}, {"filename": "/tests/metron/pass/private_getter.h", "start": 275556, "end": 275808}, {"filename": "/tests/metron/pass/self_reference.h", "start": 275808, "end": 276013}, {"filename": "/tests/metron/pass/slice.h", "start": 276013, "end": 276520}, {"filename": "/tests/metron/pass/structs.h", "start": 276520, "end": 276974}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 276974, "end": 277519}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 277519, "end": 280139}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 280139, "end": 280933}, {"filename": "/tests/metron/pass/tock_task.h", "start": 280933, "end": 281385}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 281385, "end": 281557}, {"filename": "/tests/metron/pass/unions.h", "start": 281557, "end": 281763}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 281763, "end": 282432}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 282432, "end": 283101}, {"filename": "/tests/risc-v/instructions/.gitignore", "start": 283101, "end": 283106}, {"filename": "/tests/risc-v/instructions/LICENSE", "start": 283106, "end": 284508}, {"filename": "/tests/risc-v/instructions/README", "start": 284508, "end": 284579}, {"filename": "/tests/risc-v/instructions/add.S", "start": 284579, "end": 287724}, {"filename": "/tests/risc-v/instructions/add.code.vh", "start": 287724, "end": 291102}, {"filename": "/tests/risc-v/instructions/add.data.vh", "start": 291102, "end": 291140}, {"filename": "/tests/risc-v/instructions/addi.S", "start": 291140, "end": 293485}, {"filename": "/tests/risc-v/instructions/addi.code.vh", "start": 293485, "end": 295303}, {"filename": "/tests/risc-v/instructions/addi.data.vh", "start": 295303, "end": 295341}, {"filename": "/tests/risc-v/instructions/and.S", "start": 295341, "end": 297974}, {"filename": "/tests/risc-v/instructions/and.code.vh", "start": 297974, "end": 301252}, {"filename": "/tests/risc-v/instructions/and.data.vh", "start": 301252, "end": 301290}, {"filename": "/tests/risc-v/instructions/andi.S", "start": 301290, "end": 302970}, {"filename": "/tests/risc-v/instructions/andi.code.vh", "start": 302970, "end": 304268}, {"filename": "/tests/risc-v/instructions/andi.data.vh", "start": 304268, "end": 304306}, {"filename": "/tests/risc-v/instructions/auipc.S", "start": 304306, "end": 304950}, {"filename": "/tests/risc-v/instructions/auipc.code.vh", "start": 304950, "end": 305262}, {"filename": "/tests/risc-v/instructions/auipc.data.vh", "start": 305262, "end": 305300}, {"filename": "/tests/risc-v/instructions/benchmark.S", "start": 305300, "end": 306269}, {"filename": "/tests/risc-v/instructions/benchmark.code.vh", "start": 306269, "end": 306472}, {"filename": "/tests/risc-v/instructions/benchmark.data.vh", "start": 306472, "end": 306620}, {"filename": "/tests/risc-v/instructions/beq.S", "start": 306620, "end": 308648}, {"filename": "/tests/risc-v/instructions/beq.code.vh", "start": 308648, "end": 310612}, {"filename": "/tests/risc-v/instructions/beq.data.vh", "start": 310612, "end": 310650}, {"filename": "/tests/risc-v/instructions/bge.S", "start": 310650, "end": 312799}, {"filename": "/tests/risc-v/instructions/bge.code.vh", "start": 312799, "end": 315009}, {"filename": "/tests/risc-v/instructions/bge.data.vh", "start": 315009, "end": 315047}, {"filename": "/tests/risc-v/instructions/bgeu.S", "start": 315047, "end": 317585}, {"filename": "/tests/risc-v/instructions/bgeu.code.vh", "start": 317585, "end": 319932}, {"filename": "/tests/risc-v/instructions/bgeu.data.vh", "start": 319932, "end": 319970}, {"filename": "/tests/risc-v/instructions/blt.S", "start": 319970, "end": 321998}, {"filename": "/tests/risc-v/instructions/blt.code.vh", "start": 321998, "end": 323962}, {"filename": "/tests/risc-v/instructions/blt.data.vh", "start": 323962, "end": 324000}, {"filename": "/tests/risc-v/instructions/bltu.S", "start": 324000, "end": 326366}, {"filename": "/tests/risc-v/instructions/bltu.code.vh", "start": 326366, "end": 328467}, {"filename": "/tests/risc-v/instructions/bltu.data.vh", "start": 328467, "end": 328505}, {"filename": "/tests/risc-v/instructions/bne.S", "start": 328505, "end": 330518}, {"filename": "/tests/risc-v/instructions/bne.code.vh", "start": 330518, "end": 332491}, {"filename": "/tests/risc-v/instructions/bne.data.vh", "start": 332491, "end": 332529}, {"filename": "/tests/risc-v/instructions/jal.S", "start": 332529, "end": 333616}, {"filename": "/tests/risc-v/instructions/jal.code.vh", "start": 333616, "end": 333965}, {"filename": "/tests/risc-v/instructions/jal.data.vh", "start": 333965, "end": 334003}, {"filename": "/tests/risc-v/instructions/jalr.S", "start": 334003, "end": 335439}, {"filename": "/tests/risc-v/instructions/jalr.code.vh", "start": 335439, "end": 336116}, {"filename": "/tests/risc-v/instructions/jalr.data.vh", "start": 336116, "end": 336154}, {"filename": "/tests/risc-v/instructions/lb.S", "start": 336154, "end": 338436}, {"filename": "/tests/risc-v/instructions/lb.code.vh", "start": 338436, "end": 340099}, {"filename": "/tests/risc-v/instructions/lb.data.vh", "start": 340099, "end": 340165}, {"filename": "/tests/risc-v/instructions/lbu.S", "start": 340165, "end": 342467}, {"filename": "/tests/risc-v/instructions/lbu.code.vh", "start": 342467, "end": 344130}, {"filename": "/tests/risc-v/instructions/lbu.data.vh", "start": 344130, "end": 344196}, {"filename": "/tests/risc-v/instructions/lh.S", "start": 344196, "end": 346486}, {"filename": "/tests/risc-v/instructions/lh.code.vh", "start": 346486, "end": 348231}, {"filename": "/tests/risc-v/instructions/lh.data.vh", "start": 348231, "end": 348297}, {"filename": "/tests/risc-v/instructions/lhu.S", "start": 348297, "end": 350607}, {"filename": "/tests/risc-v/instructions/lhu.code.vh", "start": 350607, "end": 352397}, {"filename": "/tests/risc-v/instructions/lhu.data.vh", "start": 352397, "end": 352463}, {"filename": "/tests/risc-v/instructions/link.ld", "start": 352463, "end": 353840}, {"filename": "/tests/risc-v/instructions/lui.S", "start": 353840, "end": 354705}, {"filename": "/tests/risc-v/instructions/lui.code.vh", "start": 354705, "end": 355081}, {"filename": "/tests/risc-v/instructions/lui.data.vh", "start": 355081, "end": 355119}, {"filename": "/tests/risc-v/instructions/lw.S", "start": 355119, "end": 357427}, {"filename": "/tests/risc-v/instructions/lw.code.vh", "start": 357427, "end": 359254}, {"filename": "/tests/risc-v/instructions/lw.data.vh", "start": 359254, "end": 359338}, {"filename": "/tests/risc-v/instructions/makefile", "start": 359338, "end": 359911}, {"filename": "/tests/risc-v/instructions/or.S", "start": 359911, "end": 362534}, {"filename": "/tests/risc-v/instructions/or.code.vh", "start": 362534, "end": 365839}, {"filename": "/tests/risc-v/instructions/or.data.vh", "start": 365839, "end": 365877}, {"filename": "/tests/risc-v/instructions/ori.S", "start": 365877, "end": 367706}, {"filename": "/tests/risc-v/instructions/ori.code.vh", "start": 367706, "end": 369077}, {"filename": "/tests/risc-v/instructions/ori.data.vh", "start": 369077, "end": 369115}, {"filename": "/tests/risc-v/instructions/riscv_test.h", "start": 369115, "end": 369644}, {"filename": "/tests/risc-v/instructions/sb.S", "start": 369644, "end": 372254}, {"filename": "/tests/risc-v/instructions/sb.code.vh", "start": 372254, "end": 375021}, {"filename": "/tests/risc-v/instructions/sb.data.vh", "start": 375021, "end": 375105}, {"filename": "/tests/risc-v/instructions/sh.S", "start": 375105, "end": 377747}, {"filename": "/tests/risc-v/instructions/sh.code.vh", "start": 377747, "end": 380851}, {"filename": "/tests/risc-v/instructions/sh.data.vh", "start": 380851, "end": 380953}, {"filename": "/tests/risc-v/instructions/simple.S", "start": 380953, "end": 381513}, {"filename": "/tests/risc-v/instructions/simple.code.vh", "start": 381513, "end": 381597}, {"filename": "/tests/risc-v/instructions/simple.data.vh", "start": 381597, "end": 381635}, {"filename": "/tests/risc-v/instructions/sll.S", "start": 381635, "end": 385864}, {"filename": "/tests/risc-v/instructions/sll.code.vh", "start": 385864, "end": 389534}, {"filename": "/tests/risc-v/instructions/sll.data.vh", "start": 389534, "end": 389572}, {"filename": "/tests/risc-v/instructions/slli.S", "start": 389572, "end": 392399}, {"filename": "/tests/risc-v/instructions/slli.code.vh", "start": 392399, "end": 394208}, {"filename": "/tests/risc-v/instructions/slli.data.vh", "start": 394208, "end": 394246}, {"filename": "/tests/risc-v/instructions/slt.S", "start": 394246, "end": 397195}, {"filename": "/tests/risc-v/instructions/slt.code.vh", "start": 397195, "end": 400509}, {"filename": "/tests/risc-v/instructions/slt.data.vh", "start": 400509, "end": 400547}, {"filename": "/tests/risc-v/instructions/slti.S", "start": 400547, "end": 402722}, {"filename": "/tests/risc-v/instructions/slti.code.vh", "start": 402722, "end": 404485}, {"filename": "/tests/risc-v/instructions/slti.data.vh", "start": 404485, "end": 404523}, {"filename": "/tests/risc-v/instructions/sltiu.S", "start": 404523, "end": 406724}, {"filename": "/tests/risc-v/instructions/sltiu.code.vh", "start": 406724, "end": 408487}, {"filename": "/tests/risc-v/instructions/sltiu.data.vh", "start": 408487, "end": 408525}, {"filename": "/tests/risc-v/instructions/sltu.S", "start": 408525, "end": 411273}, {"filename": "/tests/risc-v/instructions/sltu.code.vh", "start": 411273, "end": 414587}, {"filename": "/tests/risc-v/instructions/sltu.data.vh", "start": 414587, "end": 414625}, {"filename": "/tests/risc-v/instructions/sra.S", "start": 414625, "end": 418647}, {"filename": "/tests/risc-v/instructions/sra.code.vh", "start": 418647, "end": 422518}, {"filename": "/tests/risc-v/instructions/sra.data.vh", "start": 422518, "end": 422556}, {"filename": "/tests/risc-v/instructions/srai.S", "start": 422556, "end": 425153}, {"filename": "/tests/risc-v/instructions/srai.code.vh", "start": 425153, "end": 427089}, {"filename": "/tests/risc-v/instructions/srai.data.vh", "start": 427089, "end": 427127}, {"filename": "/tests/risc-v/instructions/srl.S", "start": 427127, "end": 430579}, {"filename": "/tests/risc-v/instructions/srl.code.vh", "start": 430579, "end": 434386}, {"filename": "/tests/risc-v/instructions/srl.data.vh", "start": 434386, "end": 434424}, {"filename": "/tests/risc-v/instructions/srli.S", "start": 434424, "end": 436600}, {"filename": "/tests/risc-v/instructions/srli.code.vh", "start": 436600, "end": 438482}, {"filename": "/tests/risc-v/instructions/srli.data.vh", "start": 438482, "end": 438520}, {"filename": "/tests/risc-v/instructions/sub.S", "start": 438520, "end": 441642}, {"filename": "/tests/risc-v/instructions/sub.code.vh", "start": 441642, "end": 444938}, {"filename": "/tests/risc-v/instructions/sub.data.vh", "start": 444938, "end": 444976}, {"filename": "/tests/risc-v/instructions/sw.S", "start": 444976, "end": 447656}, {"filename": "/tests/risc-v/instructions/sw.code.vh", "start": 447656, "end": 450797}, {"filename": "/tests/risc-v/instructions/sw.data.vh", "start": 450797, "end": 450945}, {"filename": "/tests/risc-v/instructions/test_macros.h", "start": 450945, "end": 467863}, {"filename": "/tests/risc-v/instructions/xor.S", "start": 467863, "end": 470514}, {"filename": "/tests/risc-v/instructions/xor.code.vh", "start": 470514, "end": 473810}, {"filename": "/tests/risc-v/instructions/xor.data.vh", "start": 473810, "end": 473848}, {"filename": "/tests/risc-v/instructions/xori.S", "start": 473848, "end": 475709}, {"filename": "/tests/risc-v/instructions/xori.code.vh", "start": 475709, "end": 477098}, {"filename": "/tests/risc-v/instructions/xori.data.vh", "start": 477098, "end": 477136}, {"filename": "/tests/tools/README.md", "start": 477136, "end": 477327}, {"filename": "/tests/tools/icarus/fail/README.md", "start": 477327, "end": 477327}, {"filename": "/tests/tools/icarus/pass/README.md", "start": 477327, "end": 477327}, {"filename": "/tests/tools/pass/README.md", "start": 477327, "end": 477377}, {"filename": "/tests/tools/verilator/fail/README.md", "start": 477377, "end": 477377}, {"filename": "/tests/tools/verilator/pass/README.md", "start": 477377, "end": 477377}, {"filename": "/tests/tools/yosys/fail/README.md", "start": 477377, "end": 477377}, {"filename": "/tests/tools/yosys/pass/README.md", "start": 477377, "end": 477377}], "remote_package_size": 477377});

  })();
