
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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 348}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 348, "end": 20190}, {"filename": "/examples/gb_spu/MetroBoySPU2.sv.d", "start": 20190, "end": 20222}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20222, "end": 20793}, {"filename": "/examples/ibex/README.md", "start": 20793, "end": 20844}, {"filename": "/examples/ibex/ibex.hancho", "start": 20844, "end": 20844}, {"filename": "/examples/ibex/ibex_alu.h", "start": 20844, "end": 22483}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22483, "end": 34668}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34668, "end": 49160}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49160, "end": 65237}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65237, "end": 67822}, {"filename": "/examples/j1/dpram.h", "start": 67822, "end": 68283}, {"filename": "/examples/j1/dpram.sv.d", "start": 68283, "end": 68301}, {"filename": "/examples/j1/j1.h", "start": 68301, "end": 72592}, {"filename": "/examples/j1/j1.hancho", "start": 72592, "end": 73205}, {"filename": "/examples/j1/j1.sv.d", "start": 73205, "end": 73228}, {"filename": "/examples/j1/main.d", "start": 73228, "end": 73311}, {"filename": "/examples/picorv32/picorv32.h", "start": 73311, "end": 168515}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168515, "end": 168515}, {"filename": "/examples/pong/README.md", "start": 168515, "end": 168575}, {"filename": "/examples/pong/main.d", "start": 168575, "end": 168797}, {"filename": "/examples/pong/pong.h", "start": 168797, "end": 172788}, {"filename": "/examples/pong/pong.hancho", "start": 172788, "end": 173267}, {"filename": "/examples/pong/pong.sv.d", "start": 173267, "end": 173283}, {"filename": "/examples/pong/reference/README.md", "start": 173283, "end": 173343}, {"filename": "/examples/rvsimple/README.md", "start": 173343, "end": 173422}, {"filename": "/examples/rvsimple/adder.h", "start": 173422, "end": 173929}, {"filename": "/examples/rvsimple/alu.h", "start": 173929, "end": 175397}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175397, "end": 178009}, {"filename": "/examples/rvsimple/config.h", "start": 178009, "end": 179231}, {"filename": "/examples/rvsimple/constants.h", "start": 179231, "end": 184957}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 184957, "end": 186074}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 186074, "end": 188014}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 188014, "end": 189254}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 189254, "end": 190505}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190505, "end": 191184}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 191184, "end": 192155}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 192155, "end": 194280}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194280, "end": 195051}, {"filename": "/examples/rvsimple/main.d", "start": 195051, "end": 197599}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 197599, "end": 198286}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 198286, "end": 199109}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 199109, "end": 200115}, {"filename": "/examples/rvsimple/regfile.h", "start": 200115, "end": 201093}, {"filename": "/examples/rvsimple/register.h", "start": 201093, "end": 201789}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 201789, "end": 204839}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 204839, "end": 207911}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 207911, "end": 213492}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 213492, "end": 215992}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 215992, "end": 220730}, {"filename": "/examples/rvsimple/toplevel.h", "start": 220730, "end": 222725}, {"filename": "/examples/scratch.h", "start": 222725, "end": 223093}, {"filename": "/examples/tutorial/adder.h", "start": 223093, "end": 223273}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 223273, "end": 224261}, {"filename": "/examples/tutorial/blockram.h", "start": 224261, "end": 224793}, {"filename": "/examples/tutorial/checksum.h", "start": 224793, "end": 225535}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 225535, "end": 225942}, {"filename": "/examples/tutorial/counter.h", "start": 225942, "end": 226093}, {"filename": "/examples/tutorial/declaration_order.h", "start": 226093, "end": 226919}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 226919, "end": 227878}, {"filename": "/examples/tutorial/nonblocking.h", "start": 227878, "end": 228004}, {"filename": "/examples/tutorial/submodules.h", "start": 228004, "end": 229104}, {"filename": "/examples/tutorial/templates.h", "start": 229104, "end": 229587}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 229587, "end": 229587}, {"filename": "/examples/tutorial/tutorial2.h", "start": 229587, "end": 229655}, {"filename": "/examples/tutorial/tutorial3.h", "start": 229655, "end": 229696}, {"filename": "/examples/tutorial/tutorial4.h", "start": 229696, "end": 229737}, {"filename": "/examples/tutorial/tutorial5.h", "start": 229737, "end": 229778}, {"filename": "/examples/tutorial/vga.h", "start": 229778, "end": 230958}, {"filename": "/examples/uart/README.md", "start": 230958, "end": 231202}, {"filename": "/examples/uart/main.d", "start": 231202, "end": 231679}, {"filename": "/examples/uart/uart.hancho", "start": 231679, "end": 234489}, {"filename": "/examples/uart/uart_hello.h", "start": 234489, "end": 237157}, {"filename": "/examples/uart/uart_rx.h", "start": 237157, "end": 239848}, {"filename": "/examples/uart/uart_top.h", "start": 239848, "end": 241700}, {"filename": "/examples/uart/uart_tx.h", "start": 241700, "end": 244749}, {"filename": "/tests/metron/fail/README.md", "start": 244749, "end": 244946}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 244946, "end": 245253}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 245253, "end": 245508}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 245508, "end": 245728}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 245728, "end": 246214}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 246214, "end": 246451}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 246451, "end": 246868}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 246868, "end": 247334}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 247334, "end": 247652}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 247652, "end": 248104}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 248104, "end": 248361}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 248361, "end": 248643}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 248643, "end": 248879}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 248879, "end": 249109}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 249109, "end": 249478}, {"filename": "/tests/metron/pass/README.md", "start": 249478, "end": 249559}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 249559, "end": 251060}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 251060, "end": 251477}, {"filename": "/tests/metron/pass/basic_function.h", "start": 251477, "end": 251729}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 251729, "end": 252056}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 252056, "end": 252361}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 252361, "end": 252980}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 252980, "end": 253236}, {"filename": "/tests/metron/pass/basic_output.h", "start": 253236, "end": 253508}, {"filename": "/tests/metron/pass/basic_param.h", "start": 253508, "end": 253777}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 253777, "end": 253978}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 253978, "end": 254165}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 254165, "end": 254396}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 254396, "end": 254623}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 254623, "end": 254939}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 254939, "end": 255304}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 255304, "end": 255690}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 255690, "end": 256179}, {"filename": "/tests/metron/pass/basic_task.h", "start": 256179, "end": 256514}, {"filename": "/tests/metron/pass/basic_template.h", "start": 256514, "end": 257016}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 257016, "end": 262996}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 262996, "end": 263430}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 263430, "end": 264096}, {"filename": "/tests/metron/pass/bitfields.h", "start": 264096, "end": 265302}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 265302, "end": 266119}, {"filename": "/tests/metron/pass/builtins.h", "start": 266119, "end": 266457}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 266457, "end": 266774}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 266774, "end": 267359}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 267359, "end": 268214}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 268214, "end": 268692}, {"filename": "/tests/metron/pass/counter.h", "start": 268692, "end": 269339}, {"filename": "/tests/metron/pass/defines.h", "start": 269339, "end": 269660}, {"filename": "/tests/metron/pass/dontcare.h", "start": 269660, "end": 269947}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 269947, "end": 271342}, {"filename": "/tests/metron/pass/for_loops.h", "start": 271342, "end": 271669}, {"filename": "/tests/metron/pass/good_order.h", "start": 271669, "end": 271872}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 271872, "end": 272293}, {"filename": "/tests/metron/pass/include_guards.h", "start": 272293, "end": 272490}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 272490, "end": 272681}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 272681, "end": 272858}, {"filename": "/tests/metron/pass/init_chain.h", "start": 272858, "end": 273579}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 273579, "end": 273871}, {"filename": "/tests/metron/pass/input_signals.h", "start": 273871, "end": 274544}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 274544, "end": 274729}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 274729, "end": 275042}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 275042, "end": 275363}, {"filename": "/tests/metron/pass/minimal.h", "start": 275363, "end": 275543}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 275543, "end": 275919}, {"filename": "/tests/metron/pass/namespaces.h", "start": 275919, "end": 276300}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 276300, "end": 276761}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 276761, "end": 277334}, {"filename": "/tests/metron/pass/noconvert.h", "start": 277334, "end": 277664}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 277664, "end": 278017}, {"filename": "/tests/metron/pass/oneliners.h", "start": 278017, "end": 278291}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 278291, "end": 278778}, {"filename": "/tests/metron/pass/preproc.h", "start": 278778, "end": 279106}, {"filename": "/tests/metron/pass/private_getter.h", "start": 279106, "end": 279358}, {"filename": "/tests/metron/pass/self_reference.h", "start": 279358, "end": 279563}, {"filename": "/tests/metron/pass/slice.h", "start": 279563, "end": 280070}, {"filename": "/tests/metron/pass/structs.h", "start": 280070, "end": 280524}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 280524, "end": 281069}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 281069, "end": 283689}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 283689, "end": 284483}, {"filename": "/tests/metron/pass/tock_task.h", "start": 284483, "end": 284935}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 284935, "end": 285107}, {"filename": "/tests/metron/pass/unions.h", "start": 285107, "end": 285313}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 285313, "end": 285982}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 285982, "end": 286651}], "remote_package_size": 286651});

  })();
