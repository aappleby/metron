
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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 368}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 368, "end": 20210}, {"filename": "/examples/gb_spu/MetroBoySPU2.sv.d", "start": 20210, "end": 20242}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20242, "end": 20821}, {"filename": "/examples/ibex/README.md", "start": 20821, "end": 20872}, {"filename": "/examples/ibex/ibex.hancho", "start": 20872, "end": 20872}, {"filename": "/examples/ibex/ibex_alu.h", "start": 20872, "end": 22511}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22511, "end": 34696}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34696, "end": 49188}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49188, "end": 65265}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65265, "end": 67850}, {"filename": "/examples/j1/dpram.h", "start": 67850, "end": 68311}, {"filename": "/examples/j1/dpram.sv.d", "start": 68311, "end": 68329}, {"filename": "/examples/j1/j1.h", "start": 68329, "end": 72620}, {"filename": "/examples/j1/j1.hancho", "start": 72620, "end": 73346}, {"filename": "/examples/j1/j1.sv.d", "start": 73346, "end": 73369}, {"filename": "/examples/picorv32/picorv32.h", "start": 73369, "end": 168573}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168573, "end": 168573}, {"filename": "/examples/pong/README.md", "start": 168573, "end": 168633}, {"filename": "/examples/pong/pong.h", "start": 168633, "end": 172624}, {"filename": "/examples/pong/pong.hancho", "start": 172624, "end": 173154}, {"filename": "/examples/pong/pong.sv.d", "start": 173154, "end": 173170}, {"filename": "/examples/pong/reference/README.md", "start": 173170, "end": 173230}, {"filename": "/examples/rvsimple/README.md", "start": 173230, "end": 173309}, {"filename": "/examples/rvsimple/adder.h", "start": 173309, "end": 173816}, {"filename": "/examples/rvsimple/alu.h", "start": 173816, "end": 175284}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175284, "end": 177896}, {"filename": "/examples/rvsimple/config.h", "start": 177896, "end": 179118}, {"filename": "/examples/rvsimple/constants.h", "start": 179118, "end": 184844}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 184844, "end": 185961}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 185961, "end": 187901}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 187901, "end": 189141}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 189141, "end": 190392}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190392, "end": 191071}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 191071, "end": 192042}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 192042, "end": 194167}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194167, "end": 194938}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 194938, "end": 195625}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 195625, "end": 196448}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 196448, "end": 197454}, {"filename": "/examples/rvsimple/regfile.h", "start": 197454, "end": 198432}, {"filename": "/examples/rvsimple/register.h", "start": 198432, "end": 199128}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 199128, "end": 202178}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 202178, "end": 205157}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 205157, "end": 210738}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 210738, "end": 213238}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 213238, "end": 217976}, {"filename": "/examples/rvsimple/toplevel.h", "start": 217976, "end": 219971}, {"filename": "/examples/scratch.h", "start": 219971, "end": 220339}, {"filename": "/examples/tutorial/adder.h", "start": 220339, "end": 220519}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 220519, "end": 221507}, {"filename": "/examples/tutorial/blockram.h", "start": 221507, "end": 222039}, {"filename": "/examples/tutorial/checksum.h", "start": 222039, "end": 222781}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 222781, "end": 223188}, {"filename": "/examples/tutorial/counter.h", "start": 223188, "end": 223339}, {"filename": "/examples/tutorial/declaration_order.h", "start": 223339, "end": 224165}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 224165, "end": 225124}, {"filename": "/examples/tutorial/nonblocking.h", "start": 225124, "end": 225250}, {"filename": "/examples/tutorial/submodules.h", "start": 225250, "end": 226350}, {"filename": "/examples/tutorial/templates.h", "start": 226350, "end": 226833}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 226833, "end": 226833}, {"filename": "/examples/tutorial/tutorial2.h", "start": 226833, "end": 226901}, {"filename": "/examples/tutorial/tutorial3.h", "start": 226901, "end": 226942}, {"filename": "/examples/tutorial/tutorial4.h", "start": 226942, "end": 226983}, {"filename": "/examples/tutorial/tutorial5.h", "start": 226983, "end": 227024}, {"filename": "/examples/tutorial/vga.h", "start": 227024, "end": 228204}, {"filename": "/examples/uart/README.md", "start": 228204, "end": 228448}, {"filename": "/examples/uart/uart.hancho", "start": 228448, "end": 231394}, {"filename": "/examples/uart/uart_hello.h", "start": 231394, "end": 234062}, {"filename": "/examples/uart/uart_rx.h", "start": 234062, "end": 236753}, {"filename": "/examples/uart/uart_top.h", "start": 236753, "end": 238605}, {"filename": "/examples/uart/uart_tx.h", "start": 238605, "end": 241654}, {"filename": "/tests/metron/fail/README.md", "start": 241654, "end": 241851}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 241851, "end": 242158}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 242158, "end": 242413}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 242413, "end": 242633}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 242633, "end": 243119}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 243119, "end": 243356}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 243356, "end": 243773}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 243773, "end": 244239}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 244239, "end": 244557}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 244557, "end": 245009}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 245009, "end": 245266}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 245266, "end": 245548}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 245548, "end": 245784}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 245784, "end": 246014}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 246014, "end": 246383}, {"filename": "/tests/metron/pass/README.md", "start": 246383, "end": 246464}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 246464, "end": 247965}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 247965, "end": 248382}, {"filename": "/tests/metron/pass/basic_function.h", "start": 248382, "end": 248634}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 248634, "end": 248961}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 248961, "end": 249266}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 249266, "end": 249885}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 249885, "end": 250141}, {"filename": "/tests/metron/pass/basic_output.h", "start": 250141, "end": 250413}, {"filename": "/tests/metron/pass/basic_param.h", "start": 250413, "end": 250682}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 250682, "end": 250883}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 250883, "end": 251070}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 251070, "end": 251301}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 251301, "end": 251528}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 251528, "end": 251844}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 251844, "end": 252209}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 252209, "end": 252595}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 252595, "end": 253084}, {"filename": "/tests/metron/pass/basic_task.h", "start": 253084, "end": 253419}, {"filename": "/tests/metron/pass/basic_template.h", "start": 253419, "end": 253921}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 253921, "end": 259901}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 259901, "end": 260335}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 260335, "end": 261001}, {"filename": "/tests/metron/pass/bitfields.h", "start": 261001, "end": 262207}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 262207, "end": 263024}, {"filename": "/tests/metron/pass/builtins.h", "start": 263024, "end": 263362}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 263362, "end": 263679}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 263679, "end": 264264}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 264264, "end": 265119}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 265119, "end": 265597}, {"filename": "/tests/metron/pass/counter.h", "start": 265597, "end": 266244}, {"filename": "/tests/metron/pass/defines.h", "start": 266244, "end": 266565}, {"filename": "/tests/metron/pass/dontcare.h", "start": 266565, "end": 266852}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 266852, "end": 268247}, {"filename": "/tests/metron/pass/for_loops.h", "start": 268247, "end": 268574}, {"filename": "/tests/metron/pass/good_order.h", "start": 268574, "end": 268777}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 268777, "end": 269198}, {"filename": "/tests/metron/pass/include_guards.h", "start": 269198, "end": 269395}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 269395, "end": 269586}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 269586, "end": 269763}, {"filename": "/tests/metron/pass/init_chain.h", "start": 269763, "end": 270484}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 270484, "end": 270776}, {"filename": "/tests/metron/pass/input_signals.h", "start": 270776, "end": 271449}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 271449, "end": 271634}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 271634, "end": 271947}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 271947, "end": 272268}, {"filename": "/tests/metron/pass/minimal.h", "start": 272268, "end": 272448}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 272448, "end": 272824}, {"filename": "/tests/metron/pass/namespaces.h", "start": 272824, "end": 273205}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 273205, "end": 273666}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 273666, "end": 274239}, {"filename": "/tests/metron/pass/noconvert.h", "start": 274239, "end": 274569}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 274569, "end": 274922}, {"filename": "/tests/metron/pass/oneliners.h", "start": 274922, "end": 275196}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 275196, "end": 275683}, {"filename": "/tests/metron/pass/preproc.h", "start": 275683, "end": 276011}, {"filename": "/tests/metron/pass/private_getter.h", "start": 276011, "end": 276263}, {"filename": "/tests/metron/pass/self_reference.h", "start": 276263, "end": 276468}, {"filename": "/tests/metron/pass/slice.h", "start": 276468, "end": 276975}, {"filename": "/tests/metron/pass/structs.h", "start": 276975, "end": 277429}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 277429, "end": 277974}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 277974, "end": 280594}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 280594, "end": 281388}, {"filename": "/tests/metron/pass/tock_task.h", "start": 281388, "end": 281840}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 281840, "end": 282012}, {"filename": "/tests/metron/pass/unions.h", "start": 282012, "end": 282218}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 282218, "end": 282887}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 282887, "end": 283556}], "remote_package_size": 283556});

  })();
