
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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 348}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 348, "end": 20190}, {"filename": "/examples/gb_spu/MetroBoySPU2.sv.d", "start": 20190, "end": 20222}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20222, "end": 20793}, {"filename": "/examples/ibex/README.md", "start": 20793, "end": 20844}, {"filename": "/examples/ibex/ibex.hancho", "start": 20844, "end": 20844}, {"filename": "/examples/ibex/ibex_alu.h", "start": 20844, "end": 22483}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22483, "end": 34668}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34668, "end": 49160}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49160, "end": 65237}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65237, "end": 67822}, {"filename": "/examples/j1/dpram.h", "start": 67822, "end": 68283}, {"filename": "/examples/j1/dpram.sv.d", "start": 68283, "end": 68301}, {"filename": "/examples/j1/j1.h", "start": 68301, "end": 72592}, {"filename": "/examples/j1/j1.hancho", "start": 72592, "end": 73205}, {"filename": "/examples/j1/j1.sv.d", "start": 73205, "end": 73228}, {"filename": "/examples/j1/main.d", "start": 73228, "end": 73311}, {"filename": "/examples/picorv32/picorv32.h", "start": 73311, "end": 168515}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168515, "end": 168515}, {"filename": "/examples/pong/README.md", "start": 168515, "end": 168575}, {"filename": "/examples/pong/main.d", "start": 168575, "end": 168797}, {"filename": "/examples/pong/pong.h", "start": 168797, "end": 172788}, {"filename": "/examples/pong/pong.hancho", "start": 172788, "end": 173267}, {"filename": "/examples/pong/pong.sv.d", "start": 173267, "end": 173283}, {"filename": "/examples/pong/reference/README.md", "start": 173283, "end": 173343}, {"filename": "/examples/rvsimple/README.md", "start": 173343, "end": 173422}, {"filename": "/examples/rvsimple/adder.h", "start": 173422, "end": 173929}, {"filename": "/examples/rvsimple/alu.h", "start": 173929, "end": 175397}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175397, "end": 178009}, {"filename": "/examples/rvsimple/config.h", "start": 178009, "end": 179231}, {"filename": "/examples/rvsimple/constants.h", "start": 179231, "end": 184957}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 184957, "end": 186074}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 186074, "end": 188014}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 188014, "end": 189254}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 189254, "end": 190505}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190505, "end": 191184}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 191184, "end": 192155}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 192155, "end": 194280}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194280, "end": 195051}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 195051, "end": 195738}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 195738, "end": 196561}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 196561, "end": 197567}, {"filename": "/examples/rvsimple/regfile.h", "start": 197567, "end": 198545}, {"filename": "/examples/rvsimple/register.h", "start": 198545, "end": 199241}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 199241, "end": 202291}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 202291, "end": 205368}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 205368, "end": 210949}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 210949, "end": 213449}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 213449, "end": 218187}, {"filename": "/examples/rvsimple/toplevel.h", "start": 218187, "end": 220182}, {"filename": "/examples/scratch.h", "start": 220182, "end": 220550}, {"filename": "/examples/tutorial/adder.h", "start": 220550, "end": 220730}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 220730, "end": 221718}, {"filename": "/examples/tutorial/blockram.h", "start": 221718, "end": 222250}, {"filename": "/examples/tutorial/checksum.h", "start": 222250, "end": 222992}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 222992, "end": 223399}, {"filename": "/examples/tutorial/counter.h", "start": 223399, "end": 223550}, {"filename": "/examples/tutorial/declaration_order.h", "start": 223550, "end": 224376}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 224376, "end": 225335}, {"filename": "/examples/tutorial/nonblocking.h", "start": 225335, "end": 225461}, {"filename": "/examples/tutorial/submodules.h", "start": 225461, "end": 226561}, {"filename": "/examples/tutorial/templates.h", "start": 226561, "end": 227044}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 227044, "end": 227044}, {"filename": "/examples/tutorial/tutorial2.h", "start": 227044, "end": 227112}, {"filename": "/examples/tutorial/tutorial3.h", "start": 227112, "end": 227153}, {"filename": "/examples/tutorial/tutorial4.h", "start": 227153, "end": 227194}, {"filename": "/examples/tutorial/tutorial5.h", "start": 227194, "end": 227235}, {"filename": "/examples/tutorial/vga.h", "start": 227235, "end": 228415}, {"filename": "/examples/uart/README.md", "start": 228415, "end": 228659}, {"filename": "/examples/uart/main.d", "start": 228659, "end": 229136}, {"filename": "/examples/uart/uart.hancho", "start": 229136, "end": 231946}, {"filename": "/examples/uart/uart_hello.h", "start": 231946, "end": 234614}, {"filename": "/examples/uart/uart_rx.h", "start": 234614, "end": 237305}, {"filename": "/examples/uart/uart_top.h", "start": 237305, "end": 239157}, {"filename": "/examples/uart/uart_tx.h", "start": 239157, "end": 242206}, {"filename": "/tests/metron/fail/README.md", "start": 242206, "end": 242403}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 242403, "end": 242710}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 242710, "end": 242965}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 242965, "end": 243185}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 243185, "end": 243671}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 243671, "end": 243908}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 243908, "end": 244325}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 244325, "end": 244791}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 244791, "end": 245109}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 245109, "end": 245561}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 245561, "end": 245818}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 245818, "end": 246100}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 246100, "end": 246336}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 246336, "end": 246566}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 246566, "end": 246935}, {"filename": "/tests/metron/pass/README.md", "start": 246935, "end": 247016}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 247016, "end": 248517}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 248517, "end": 248934}, {"filename": "/tests/metron/pass/basic_function.h", "start": 248934, "end": 249186}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 249186, "end": 249513}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 249513, "end": 249818}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 249818, "end": 250437}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 250437, "end": 250693}, {"filename": "/tests/metron/pass/basic_output.h", "start": 250693, "end": 250965}, {"filename": "/tests/metron/pass/basic_param.h", "start": 250965, "end": 251234}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 251234, "end": 251435}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 251435, "end": 251622}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 251622, "end": 251853}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 251853, "end": 252080}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 252080, "end": 252396}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 252396, "end": 252761}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 252761, "end": 253147}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 253147, "end": 253636}, {"filename": "/tests/metron/pass/basic_task.h", "start": 253636, "end": 253971}, {"filename": "/tests/metron/pass/basic_template.h", "start": 253971, "end": 254473}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 254473, "end": 260453}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 260453, "end": 260887}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 260887, "end": 261553}, {"filename": "/tests/metron/pass/bitfields.h", "start": 261553, "end": 262759}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 262759, "end": 263576}, {"filename": "/tests/metron/pass/builtins.h", "start": 263576, "end": 263914}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 263914, "end": 264231}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 264231, "end": 264816}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 264816, "end": 265671}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 265671, "end": 266149}, {"filename": "/tests/metron/pass/counter.h", "start": 266149, "end": 266796}, {"filename": "/tests/metron/pass/defines.h", "start": 266796, "end": 267117}, {"filename": "/tests/metron/pass/dontcare.h", "start": 267117, "end": 267404}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 267404, "end": 268799}, {"filename": "/tests/metron/pass/for_loops.h", "start": 268799, "end": 269126}, {"filename": "/tests/metron/pass/good_order.h", "start": 269126, "end": 269329}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 269329, "end": 269750}, {"filename": "/tests/metron/pass/include_guards.h", "start": 269750, "end": 269947}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 269947, "end": 270138}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 270138, "end": 270315}, {"filename": "/tests/metron/pass/init_chain.h", "start": 270315, "end": 271036}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 271036, "end": 271328}, {"filename": "/tests/metron/pass/input_signals.h", "start": 271328, "end": 272001}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 272001, "end": 272186}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 272186, "end": 272499}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 272499, "end": 272820}, {"filename": "/tests/metron/pass/minimal.h", "start": 272820, "end": 273000}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 273000, "end": 273376}, {"filename": "/tests/metron/pass/namespaces.h", "start": 273376, "end": 273757}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 273757, "end": 274218}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 274218, "end": 274791}, {"filename": "/tests/metron/pass/noconvert.h", "start": 274791, "end": 275121}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 275121, "end": 275474}, {"filename": "/tests/metron/pass/oneliners.h", "start": 275474, "end": 275748}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 275748, "end": 276235}, {"filename": "/tests/metron/pass/preproc.h", "start": 276235, "end": 276563}, {"filename": "/tests/metron/pass/private_getter.h", "start": 276563, "end": 276815}, {"filename": "/tests/metron/pass/self_reference.h", "start": 276815, "end": 277020}, {"filename": "/tests/metron/pass/slice.h", "start": 277020, "end": 277527}, {"filename": "/tests/metron/pass/structs.h", "start": 277527, "end": 277981}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 277981, "end": 278526}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 278526, "end": 281146}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 281146, "end": 281940}, {"filename": "/tests/metron/pass/tock_task.h", "start": 281940, "end": 282392}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 282392, "end": 282564}, {"filename": "/tests/metron/pass/unions.h", "start": 282564, "end": 282770}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 282770, "end": 283439}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 283439, "end": 284108}], "remote_package_size": 284108});

  })();
