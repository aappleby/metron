
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
Module['FS_createPath']("/tests", "metron", true, true);
Module['FS_createPath']("/tests/metron", "fail", true, true);
Module['FS_createPath']("/tests/metron", "pass", true, true);

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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/picorv32/picorv32.h", "start": 71623, "end": 166827}, {"filename": "/examples/pong/README.md", "start": 166827, "end": 166887}, {"filename": "/examples/pong/metron/pong.h", "start": 166887, "end": 170878}, {"filename": "/examples/pong/reference/README.md", "start": 170878, "end": 170938}, {"filename": "/examples/rvsimple/README.md", "start": 170938, "end": 171017}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 171017, "end": 171524}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 171524, "end": 172992}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 172992, "end": 175604}, {"filename": "/examples/rvsimple/metron/config.h", "start": 175604, "end": 176826}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 176826, "end": 182552}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 182552, "end": 183669}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 183669, "end": 185609}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 185609, "end": 186849}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 186849, "end": 188100}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 188100, "end": 188779}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 188779, "end": 189750}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 189750, "end": 191875}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 191875, "end": 192646}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 192646, "end": 193333}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 193333, "end": 194156}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 194156, "end": 195162}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 195162, "end": 196140}, {"filename": "/examples/rvsimple/metron/register.h", "start": 196140, "end": 196836}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 196836, "end": 199886}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 199886, "end": 205467}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 205467, "end": 207967}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 207967, "end": 212705}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 212705, "end": 214700}, {"filename": "/examples/scratch.h", "start": 214700, "end": 215068}, {"filename": "/examples/tutorial/adder.h", "start": 215068, "end": 215248}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 215248, "end": 216236}, {"filename": "/examples/tutorial/blockram.h", "start": 216236, "end": 216764}, {"filename": "/examples/tutorial/checksum.h", "start": 216764, "end": 217506}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 217506, "end": 217913}, {"filename": "/examples/tutorial/counter.h", "start": 217913, "end": 218064}, {"filename": "/examples/tutorial/declaration_order.h", "start": 218064, "end": 218890}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 218890, "end": 220318}, {"filename": "/examples/tutorial/nonblocking.h", "start": 220318, "end": 220444}, {"filename": "/examples/tutorial/submodules.h", "start": 220444, "end": 221563}, {"filename": "/examples/tutorial/templates.h", "start": 221563, "end": 222046}, {"filename": "/examples/tutorial/tutorial2.h", "start": 222046, "end": 222114}, {"filename": "/examples/tutorial/tutorial3.h", "start": 222114, "end": 222155}, {"filename": "/examples/tutorial/tutorial4.h", "start": 222155, "end": 222196}, {"filename": "/examples/tutorial/tutorial5.h", "start": 222196, "end": 222237}, {"filename": "/examples/tutorial/vga.h", "start": 222237, "end": 223417}, {"filename": "/examples/uart/README.md", "start": 223417, "end": 223661}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 223661, "end": 226299}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 226299, "end": 228990}, {"filename": "/examples/uart/metron/uart_top.h", "start": 228990, "end": 230778}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 230778, "end": 233835}, {"filename": "/tests/metron/fail/README.md", "start": 233835, "end": 234032}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 234032, "end": 234339}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 234339, "end": 234594}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 234594, "end": 234814}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 234814, "end": 235300}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 235300, "end": 235537}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 235537, "end": 235954}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 235954, "end": 236420}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 236420, "end": 236738}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 236738, "end": 237190}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 237190, "end": 237447}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 237447, "end": 237729}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 237729, "end": 237965}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 237965, "end": 238195}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 238195, "end": 238564}, {"filename": "/tests/metron/pass/README.md", "start": 238564, "end": 238645}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 238645, "end": 240146}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 240146, "end": 240563}, {"filename": "/tests/metron/pass/basic_function.h", "start": 240563, "end": 240815}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 240815, "end": 241142}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 241142, "end": 241447}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 241447, "end": 242066}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 242066, "end": 242322}, {"filename": "/tests/metron/pass/basic_output.h", "start": 242322, "end": 242594}, {"filename": "/tests/metron/pass/basic_param.h", "start": 242594, "end": 242863}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 242863, "end": 243064}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 243064, "end": 243251}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 243251, "end": 243482}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 243482, "end": 243709}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 243709, "end": 244025}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 244025, "end": 244390}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 244390, "end": 244776}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 244776, "end": 245265}, {"filename": "/tests/metron/pass/basic_task.h", "start": 245265, "end": 245600}, {"filename": "/tests/metron/pass/basic_template.h", "start": 245600, "end": 246102}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 246102, "end": 252082}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 252082, "end": 252516}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 252516, "end": 253182}, {"filename": "/tests/metron/pass/bitfields.h", "start": 253182, "end": 254388}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 254388, "end": 255205}, {"filename": "/tests/metron/pass/builtins.h", "start": 255205, "end": 255543}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 255543, "end": 255860}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 255860, "end": 256445}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 256445, "end": 257308}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 257308, "end": 257780}, {"filename": "/tests/metron/pass/counter.h", "start": 257780, "end": 258427}, {"filename": "/tests/metron/pass/defines.h", "start": 258427, "end": 258748}, {"filename": "/tests/metron/pass/dontcare.h", "start": 258748, "end": 259035}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 259035, "end": 260430}, {"filename": "/tests/metron/pass/for_loops.h", "start": 260430, "end": 260757}, {"filename": "/tests/metron/pass/good_order.h", "start": 260757, "end": 260960}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 260960, "end": 261381}, {"filename": "/tests/metron/pass/include_guards.h", "start": 261381, "end": 261578}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 261578, "end": 261769}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 261769, "end": 261946}, {"filename": "/tests/metron/pass/init_chain.h", "start": 261946, "end": 262667}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 262667, "end": 262959}, {"filename": "/tests/metron/pass/input_signals.h", "start": 262959, "end": 263755}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 263755, "end": 263940}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 263940, "end": 264253}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 264253, "end": 264574}, {"filename": "/tests/metron/pass/minimal.h", "start": 264574, "end": 264754}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 264754, "end": 265130}, {"filename": "/tests/metron/pass/namespaces.h", "start": 265130, "end": 265511}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 265511, "end": 265972}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 265972, "end": 266545}, {"filename": "/tests/metron/pass/noconvert.h", "start": 266545, "end": 266875}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 266875, "end": 267228}, {"filename": "/tests/metron/pass/oneliners.h", "start": 267228, "end": 267502}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 267502, "end": 267989}, {"filename": "/tests/metron/pass/preproc.h", "start": 267989, "end": 268317}, {"filename": "/tests/metron/pass/private_getter.h", "start": 268317, "end": 268569}, {"filename": "/tests/metron/pass/self_reference.h", "start": 268569, "end": 268774}, {"filename": "/tests/metron/pass/slice.h", "start": 268774, "end": 269281}, {"filename": "/tests/metron/pass/structs.h", "start": 269281, "end": 269735}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 269735, "end": 270280}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 270280, "end": 272900}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 272900, "end": 273694}, {"filename": "/tests/metron/pass/tock_task.h", "start": 273694, "end": 274146}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 274146, "end": 274318}, {"filename": "/tests/metron/pass/unions.h", "start": 274318, "end": 274524}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 274524, "end": 275193}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 275193, "end": 275862}], "remote_package_size": 275862});

  })();
