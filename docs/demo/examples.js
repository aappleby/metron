
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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 368}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 368, "end": 20210}, {"filename": "/examples/gb_spu/MetroBoySPU2.sv.d", "start": 20210, "end": 20242}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20242, "end": 20805}, {"filename": "/examples/ibex/README.md", "start": 20805, "end": 20856}, {"filename": "/examples/ibex/ibex.hancho", "start": 20856, "end": 20856}, {"filename": "/examples/ibex/ibex_alu.h", "start": 20856, "end": 22495}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22495, "end": 34680}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34680, "end": 49172}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49172, "end": 65249}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65249, "end": 67834}, {"filename": "/examples/j1/dpram.h", "start": 67834, "end": 68295}, {"filename": "/examples/j1/dpram.sv.d", "start": 68295, "end": 68313}, {"filename": "/examples/j1/j1.h", "start": 68313, "end": 72604}, {"filename": "/examples/j1/j1.hancho", "start": 72604, "end": 73224}, {"filename": "/examples/j1/j1.sv.d", "start": 73224, "end": 73247}, {"filename": "/examples/j1/main.d", "start": 73247, "end": 73330}, {"filename": "/examples/picorv32/picorv32.h", "start": 73330, "end": 168534}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168534, "end": 168534}, {"filename": "/examples/pong/README.md", "start": 168534, "end": 168594}, {"filename": "/examples/pong/main.d", "start": 168594, "end": 168816}, {"filename": "/examples/pong/pong.h", "start": 168816, "end": 172807}, {"filename": "/examples/pong/pong.hancho", "start": 172807, "end": 173300}, {"filename": "/examples/pong/pong.sv.d", "start": 173300, "end": 173316}, {"filename": "/examples/pong/reference/README.md", "start": 173316, "end": 173376}, {"filename": "/examples/rvsimple/README.md", "start": 173376, "end": 173455}, {"filename": "/examples/rvsimple/adder.h", "start": 173455, "end": 173962}, {"filename": "/examples/rvsimple/alu.h", "start": 173962, "end": 175430}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175430, "end": 178042}, {"filename": "/examples/rvsimple/config.h", "start": 178042, "end": 179264}, {"filename": "/examples/rvsimple/constants.h", "start": 179264, "end": 184990}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 184990, "end": 186107}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 186107, "end": 188047}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 188047, "end": 189287}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 189287, "end": 190538}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190538, "end": 191217}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 191217, "end": 192188}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 192188, "end": 194313}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194313, "end": 195084}, {"filename": "/examples/rvsimple/main.d", "start": 195084, "end": 197632}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 197632, "end": 198319}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 198319, "end": 199142}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 199142, "end": 200148}, {"filename": "/examples/rvsimple/regfile.h", "start": 200148, "end": 201126}, {"filename": "/examples/rvsimple/register.h", "start": 201126, "end": 201822}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 201822, "end": 204872}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 204872, "end": 208061}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 208061, "end": 213642}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 213642, "end": 216142}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 216142, "end": 220880}, {"filename": "/examples/rvsimple/toplevel.h", "start": 220880, "end": 222875}, {"filename": "/examples/scratch.h", "start": 222875, "end": 223243}, {"filename": "/examples/tutorial/adder.h", "start": 223243, "end": 223423}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 223423, "end": 224411}, {"filename": "/examples/tutorial/blockram.h", "start": 224411, "end": 224943}, {"filename": "/examples/tutorial/checksum.h", "start": 224943, "end": 225685}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 225685, "end": 226092}, {"filename": "/examples/tutorial/counter.h", "start": 226092, "end": 226243}, {"filename": "/examples/tutorial/declaration_order.h", "start": 226243, "end": 227069}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 227069, "end": 228028}, {"filename": "/examples/tutorial/nonblocking.h", "start": 228028, "end": 228154}, {"filename": "/examples/tutorial/submodules.h", "start": 228154, "end": 229254}, {"filename": "/examples/tutorial/templates.h", "start": 229254, "end": 229737}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 229737, "end": 229737}, {"filename": "/examples/tutorial/tutorial2.h", "start": 229737, "end": 229805}, {"filename": "/examples/tutorial/tutorial3.h", "start": 229805, "end": 229846}, {"filename": "/examples/tutorial/tutorial4.h", "start": 229846, "end": 229887}, {"filename": "/examples/tutorial/tutorial5.h", "start": 229887, "end": 229928}, {"filename": "/examples/tutorial/vga.h", "start": 229928, "end": 231108}, {"filename": "/examples/uart/README.md", "start": 231108, "end": 231352}, {"filename": "/examples/uart/main.d", "start": 231352, "end": 231829}, {"filename": "/examples/uart/uart.hancho", "start": 231829, "end": 234747}, {"filename": "/examples/uart/uart_hello.h", "start": 234747, "end": 237415}, {"filename": "/examples/uart/uart_rx.h", "start": 237415, "end": 240106}, {"filename": "/examples/uart/uart_top.h", "start": 240106, "end": 241958}, {"filename": "/examples/uart/uart_tx.h", "start": 241958, "end": 245007}, {"filename": "/tests/metron/fail/README.md", "start": 245007, "end": 245204}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 245204, "end": 245511}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 245511, "end": 245766}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 245766, "end": 245986}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 245986, "end": 246472}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 246472, "end": 246709}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 246709, "end": 247126}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 247126, "end": 247592}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 247592, "end": 247910}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 247910, "end": 248362}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 248362, "end": 248619}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 248619, "end": 248901}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 248901, "end": 249137}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 249137, "end": 249367}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 249367, "end": 249736}, {"filename": "/tests/metron/pass/README.md", "start": 249736, "end": 249817}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 249817, "end": 251318}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 251318, "end": 251735}, {"filename": "/tests/metron/pass/basic_function.h", "start": 251735, "end": 251987}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 251987, "end": 252314}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 252314, "end": 252619}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 252619, "end": 253238}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 253238, "end": 253494}, {"filename": "/tests/metron/pass/basic_output.h", "start": 253494, "end": 253766}, {"filename": "/tests/metron/pass/basic_param.h", "start": 253766, "end": 254035}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 254035, "end": 254236}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 254236, "end": 254423}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 254423, "end": 254654}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 254654, "end": 254881}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 254881, "end": 255197}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 255197, "end": 255562}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 255562, "end": 255948}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 255948, "end": 256437}, {"filename": "/tests/metron/pass/basic_task.h", "start": 256437, "end": 256772}, {"filename": "/tests/metron/pass/basic_template.h", "start": 256772, "end": 257274}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 257274, "end": 263254}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 263254, "end": 263688}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 263688, "end": 264354}, {"filename": "/tests/metron/pass/bitfields.h", "start": 264354, "end": 265560}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 265560, "end": 266377}, {"filename": "/tests/metron/pass/builtins.h", "start": 266377, "end": 266715}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 266715, "end": 267032}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 267032, "end": 267617}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 267617, "end": 268472}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 268472, "end": 268950}, {"filename": "/tests/metron/pass/counter.h", "start": 268950, "end": 269597}, {"filename": "/tests/metron/pass/defines.h", "start": 269597, "end": 269918}, {"filename": "/tests/metron/pass/dontcare.h", "start": 269918, "end": 270205}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 270205, "end": 271600}, {"filename": "/tests/metron/pass/for_loops.h", "start": 271600, "end": 271927}, {"filename": "/tests/metron/pass/good_order.h", "start": 271927, "end": 272130}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 272130, "end": 272551}, {"filename": "/tests/metron/pass/include_guards.h", "start": 272551, "end": 272748}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 272748, "end": 272939}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 272939, "end": 273116}, {"filename": "/tests/metron/pass/init_chain.h", "start": 273116, "end": 273837}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 273837, "end": 274129}, {"filename": "/tests/metron/pass/input_signals.h", "start": 274129, "end": 274802}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 274802, "end": 274987}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 274987, "end": 275300}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 275300, "end": 275621}, {"filename": "/tests/metron/pass/minimal.h", "start": 275621, "end": 275801}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 275801, "end": 276177}, {"filename": "/tests/metron/pass/namespaces.h", "start": 276177, "end": 276558}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 276558, "end": 277019}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 277019, "end": 277592}, {"filename": "/tests/metron/pass/noconvert.h", "start": 277592, "end": 277922}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 277922, "end": 278275}, {"filename": "/tests/metron/pass/oneliners.h", "start": 278275, "end": 278549}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 278549, "end": 279036}, {"filename": "/tests/metron/pass/preproc.h", "start": 279036, "end": 279364}, {"filename": "/tests/metron/pass/private_getter.h", "start": 279364, "end": 279616}, {"filename": "/tests/metron/pass/self_reference.h", "start": 279616, "end": 279821}, {"filename": "/tests/metron/pass/slice.h", "start": 279821, "end": 280328}, {"filename": "/tests/metron/pass/structs.h", "start": 280328, "end": 280782}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 280782, "end": 281327}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 281327, "end": 283947}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 283947, "end": 284741}, {"filename": "/tests/metron/pass/tock_task.h", "start": 284741, "end": 285193}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 285193, "end": 285365}, {"filename": "/tests/metron/pass/unions.h", "start": 285365, "end": 285571}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 285571, "end": 286240}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 286240, "end": 286909}], "remote_package_size": 286909});

  })();
