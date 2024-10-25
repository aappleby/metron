
  var Module = typeof Module != 'undefined' ? Module : {};

  if (!Module.expectedDataFileDownloads) {
    Module.expectedDataFileDownloads = 0;
  }

  Module.expectedDataFileDownloads++;
  (() => {
    // Do not attempt to redownload the virtual filesystem data when in a pthread or a Wasm Worker context.
    var isPthread = typeof ENVIRONMENT_IS_PTHREAD != 'undefined' && ENVIRONMENT_IS_PTHREAD;
    var isWasmWorker = typeof ENVIRONMENT_IS_WASM_WORKER != 'undefined' && ENVIRONMENT_IS_WASM_WORKER;
    if (isPthread || isWasmWorker) return;
    function loadPackage(metadata) {

      var PACKAGE_PATH = '';
      if (typeof window === 'object') {
        PACKAGE_PATH = window['encodeURIComponent'](window.location.pathname.toString().substring(0, window.location.pathname.toString().lastIndexOf('/')) + '/');
      } else if (typeof process === 'undefined' && typeof location !== 'undefined') {
        // web worker
        PACKAGE_PATH = encodeURIComponent(location.pathname.toString().substring(0, location.pathname.toString().lastIndexOf('/')) + '/');
      }
      var PACKAGE_NAME = '/home/aappleby/repos/metron/docs/demo/examples.data';
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
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples", "picorv32", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
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
          }          Module['removeRunDependency']('datafile_/home/aappleby/repos/metron/docs/demo/examples.data');

      };
      Module['addRunDependency']('datafile_/home/aappleby/repos/metron/docs/demo/examples.data');

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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 368}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 368, "end": 20210}, {"filename": "/examples/gb_spu/MetroBoySPU2.sv.d", "start": 20210, "end": 20242}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20242, "end": 20805}, {"filename": "/examples/ibex/README.md", "start": 20805, "end": 20856}, {"filename": "/examples/ibex/ibex.hancho", "start": 20856, "end": 20856}, {"filename": "/examples/ibex/ibex_alu.h", "start": 20856, "end": 22495}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22495, "end": 34680}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34680, "end": 49172}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49172, "end": 65249}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65249, "end": 67834}, {"filename": "/examples/j1/dpram.h", "start": 67834, "end": 68295}, {"filename": "/examples/j1/dpram.sv.d", "start": 68295, "end": 68313}, {"filename": "/examples/j1/j1.h", "start": 68313, "end": 72604}, {"filename": "/examples/j1/j1.hancho", "start": 72604, "end": 73224}, {"filename": "/examples/j1/j1.sv.d", "start": 73224, "end": 73247}, {"filename": "/examples/picorv32/picorv32.h", "start": 73247, "end": 168451}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168451, "end": 168451}, {"filename": "/examples/pong/README.md", "start": 168451, "end": 168511}, {"filename": "/examples/pong/pong.h", "start": 168511, "end": 172502}, {"filename": "/examples/pong/pong.hancho", "start": 172502, "end": 172995}, {"filename": "/examples/pong/pong.sv.d", "start": 172995, "end": 173011}, {"filename": "/examples/pong/reference/README.md", "start": 173011, "end": 173071}, {"filename": "/examples/rvsimple/README.md", "start": 173071, "end": 173150}, {"filename": "/examples/rvsimple/adder.h", "start": 173150, "end": 173657}, {"filename": "/examples/rvsimple/alu.h", "start": 173657, "end": 175125}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175125, "end": 177737}, {"filename": "/examples/rvsimple/config.h", "start": 177737, "end": 178959}, {"filename": "/examples/rvsimple/constants.h", "start": 178959, "end": 184685}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 184685, "end": 185802}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 185802, "end": 187742}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 187742, "end": 188982}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 188982, "end": 190233}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190233, "end": 190912}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 190912, "end": 191883}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 191883, "end": 194008}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194008, "end": 194779}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 194779, "end": 195466}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 195466, "end": 196289}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 196289, "end": 197295}, {"filename": "/examples/rvsimple/regfile.h", "start": 197295, "end": 198273}, {"filename": "/examples/rvsimple/register.h", "start": 198273, "end": 198969}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 198969, "end": 202019}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 202019, "end": 205208}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 205208, "end": 210789}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 210789, "end": 213289}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 213289, "end": 218027}, {"filename": "/examples/rvsimple/toplevel.h", "start": 218027, "end": 220022}, {"filename": "/examples/scratch.h", "start": 220022, "end": 220390}, {"filename": "/examples/tutorial/adder.h", "start": 220390, "end": 220570}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 220570, "end": 221558}, {"filename": "/examples/tutorial/blockram.h", "start": 221558, "end": 222090}, {"filename": "/examples/tutorial/checksum.h", "start": 222090, "end": 222832}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 222832, "end": 223239}, {"filename": "/examples/tutorial/counter.h", "start": 223239, "end": 223390}, {"filename": "/examples/tutorial/declaration_order.h", "start": 223390, "end": 224216}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 224216, "end": 225175}, {"filename": "/examples/tutorial/nonblocking.h", "start": 225175, "end": 225301}, {"filename": "/examples/tutorial/submodules.h", "start": 225301, "end": 226401}, {"filename": "/examples/tutorial/templates.h", "start": 226401, "end": 226884}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 226884, "end": 226884}, {"filename": "/examples/tutorial/tutorial2.h", "start": 226884, "end": 226952}, {"filename": "/examples/tutorial/tutorial3.h", "start": 226952, "end": 226993}, {"filename": "/examples/tutorial/tutorial4.h", "start": 226993, "end": 227034}, {"filename": "/examples/tutorial/tutorial5.h", "start": 227034, "end": 227075}, {"filename": "/examples/tutorial/vga.h", "start": 227075, "end": 228255}, {"filename": "/examples/uart/README.md", "start": 228255, "end": 228499}, {"filename": "/examples/uart/uart.hancho", "start": 228499, "end": 231518}, {"filename": "/examples/uart/uart_hello.h", "start": 231518, "end": 234186}, {"filename": "/examples/uart/uart_rx.h", "start": 234186, "end": 236877}, {"filename": "/examples/uart/uart_top.h", "start": 236877, "end": 238729}, {"filename": "/examples/uart/uart_tx.h", "start": 238729, "end": 241778}, {"filename": "/tests/metron/fail/README.md", "start": 241778, "end": 241975}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 241975, "end": 242282}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 242282, "end": 242537}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 242537, "end": 242757}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 242757, "end": 243243}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 243243, "end": 243480}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 243480, "end": 243897}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 243897, "end": 244363}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 244363, "end": 244681}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 244681, "end": 245133}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 245133, "end": 245390}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 245390, "end": 245672}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 245672, "end": 245908}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 245908, "end": 246138}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 246138, "end": 246507}, {"filename": "/tests/metron/pass/README.md", "start": 246507, "end": 246588}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 246588, "end": 248089}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 248089, "end": 248506}, {"filename": "/tests/metron/pass/basic_function.h", "start": 248506, "end": 248758}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 248758, "end": 249085}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 249085, "end": 249390}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 249390, "end": 250009}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 250009, "end": 250265}, {"filename": "/tests/metron/pass/basic_output.h", "start": 250265, "end": 250537}, {"filename": "/tests/metron/pass/basic_param.h", "start": 250537, "end": 250806}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 250806, "end": 251007}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 251007, "end": 251194}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 251194, "end": 251425}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 251425, "end": 251652}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 251652, "end": 251968}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 251968, "end": 252333}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 252333, "end": 252719}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 252719, "end": 253208}, {"filename": "/tests/metron/pass/basic_task.h", "start": 253208, "end": 253543}, {"filename": "/tests/metron/pass/basic_template.h", "start": 253543, "end": 254045}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 254045, "end": 260025}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 260025, "end": 260459}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 260459, "end": 261125}, {"filename": "/tests/metron/pass/bitfields.h", "start": 261125, "end": 262331}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 262331, "end": 263148}, {"filename": "/tests/metron/pass/builtins.h", "start": 263148, "end": 263486}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 263486, "end": 263803}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 263803, "end": 264388}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 264388, "end": 265243}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 265243, "end": 265721}, {"filename": "/tests/metron/pass/counter.h", "start": 265721, "end": 266368}, {"filename": "/tests/metron/pass/defines.h", "start": 266368, "end": 266689}, {"filename": "/tests/metron/pass/dontcare.h", "start": 266689, "end": 266976}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 266976, "end": 268371}, {"filename": "/tests/metron/pass/for_loops.h", "start": 268371, "end": 268698}, {"filename": "/tests/metron/pass/good_order.h", "start": 268698, "end": 268901}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 268901, "end": 269322}, {"filename": "/tests/metron/pass/include_guards.h", "start": 269322, "end": 269519}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 269519, "end": 269710}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 269710, "end": 269887}, {"filename": "/tests/metron/pass/init_chain.h", "start": 269887, "end": 270608}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 270608, "end": 270900}, {"filename": "/tests/metron/pass/input_signals.h", "start": 270900, "end": 271573}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 271573, "end": 271758}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 271758, "end": 272071}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 272071, "end": 272392}, {"filename": "/tests/metron/pass/minimal.h", "start": 272392, "end": 272572}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 272572, "end": 272948}, {"filename": "/tests/metron/pass/namespaces.h", "start": 272948, "end": 273329}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 273329, "end": 273790}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 273790, "end": 274363}, {"filename": "/tests/metron/pass/noconvert.h", "start": 274363, "end": 274693}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 274693, "end": 275046}, {"filename": "/tests/metron/pass/oneliners.h", "start": 275046, "end": 275320}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 275320, "end": 275807}, {"filename": "/tests/metron/pass/preproc.h", "start": 275807, "end": 276135}, {"filename": "/tests/metron/pass/private_getter.h", "start": 276135, "end": 276387}, {"filename": "/tests/metron/pass/self_reference.h", "start": 276387, "end": 276592}, {"filename": "/tests/metron/pass/slice.h", "start": 276592, "end": 277099}, {"filename": "/tests/metron/pass/structs.h", "start": 277099, "end": 277553}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 277553, "end": 278098}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 278098, "end": 280718}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 280718, "end": 281512}, {"filename": "/tests/metron/pass/tock_task.h", "start": 281512, "end": 281964}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 281964, "end": 282136}, {"filename": "/tests/metron/pass/unions.h", "start": 282136, "end": 282342}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 282342, "end": 283011}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 283011, "end": 283680}], "remote_package_size": 283680});

  })();
