
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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 368}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 368, "end": 20210}, {"filename": "/examples/gb_spu/MetroBoySPU2.sv.d", "start": 20210, "end": 20242}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20242, "end": 20805}, {"filename": "/examples/ibex/README.md", "start": 20805, "end": 20856}, {"filename": "/examples/ibex/ibex.hancho", "start": 20856, "end": 20856}, {"filename": "/examples/ibex/ibex_alu.h", "start": 20856, "end": 22495}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22495, "end": 34680}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34680, "end": 49172}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49172, "end": 65249}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65249, "end": 67834}, {"filename": "/examples/j1/dpram.h", "start": 67834, "end": 68295}, {"filename": "/examples/j1/dpram.sv.d", "start": 68295, "end": 68313}, {"filename": "/examples/j1/j1.h", "start": 68313, "end": 72604}, {"filename": "/examples/j1/j1.hancho", "start": 72604, "end": 73224}, {"filename": "/examples/j1/j1.sv.d", "start": 73224, "end": 73247}, {"filename": "/examples/picorv32/picorv32.h", "start": 73247, "end": 168451}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168451, "end": 168451}, {"filename": "/examples/pong/README.md", "start": 168451, "end": 168511}, {"filename": "/examples/pong/pong.h", "start": 168511, "end": 172502}, {"filename": "/examples/pong/pong.hancho", "start": 172502, "end": 172995}, {"filename": "/examples/pong/pong.sv.d", "start": 172995, "end": 173011}, {"filename": "/examples/pong/reference/README.md", "start": 173011, "end": 173071}, {"filename": "/examples/rvsimple/README.md", "start": 173071, "end": 173150}, {"filename": "/examples/rvsimple/adder.h", "start": 173150, "end": 173657}, {"filename": "/examples/rvsimple/alu.h", "start": 173657, "end": 175125}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175125, "end": 177737}, {"filename": "/examples/rvsimple/config.h", "start": 177737, "end": 178959}, {"filename": "/examples/rvsimple/constants.h", "start": 178959, "end": 184685}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 184685, "end": 185802}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 185802, "end": 187742}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 187742, "end": 188982}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 188982, "end": 190233}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190233, "end": 190912}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 190912, "end": 191883}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 191883, "end": 194008}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194008, "end": 194779}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 194779, "end": 195466}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 195466, "end": 196289}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 196289, "end": 197295}, {"filename": "/examples/rvsimple/regfile.h", "start": 197295, "end": 198273}, {"filename": "/examples/rvsimple/register.h", "start": 198273, "end": 198969}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 198969, "end": 202019}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 202019, "end": 204983}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 204983, "end": 210564}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 210564, "end": 213064}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 213064, "end": 217802}, {"filename": "/examples/rvsimple/toplevel.h", "start": 217802, "end": 219797}, {"filename": "/examples/scratch.h", "start": 219797, "end": 220165}, {"filename": "/examples/tutorial/adder.h", "start": 220165, "end": 220345}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 220345, "end": 221333}, {"filename": "/examples/tutorial/blockram.h", "start": 221333, "end": 221865}, {"filename": "/examples/tutorial/checksum.h", "start": 221865, "end": 222607}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 222607, "end": 223014}, {"filename": "/examples/tutorial/counter.h", "start": 223014, "end": 223165}, {"filename": "/examples/tutorial/declaration_order.h", "start": 223165, "end": 223991}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 223991, "end": 224950}, {"filename": "/examples/tutorial/nonblocking.h", "start": 224950, "end": 225076}, {"filename": "/examples/tutorial/submodules.h", "start": 225076, "end": 226176}, {"filename": "/examples/tutorial/templates.h", "start": 226176, "end": 226659}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 226659, "end": 226659}, {"filename": "/examples/tutorial/tutorial2.h", "start": 226659, "end": 226727}, {"filename": "/examples/tutorial/tutorial3.h", "start": 226727, "end": 226768}, {"filename": "/examples/tutorial/tutorial4.h", "start": 226768, "end": 226809}, {"filename": "/examples/tutorial/tutorial5.h", "start": 226809, "end": 226850}, {"filename": "/examples/tutorial/vga.h", "start": 226850, "end": 228030}, {"filename": "/examples/uart/README.md", "start": 228030, "end": 228274}, {"filename": "/examples/uart/uart.hancho", "start": 228274, "end": 231293}, {"filename": "/examples/uart/uart_hello.h", "start": 231293, "end": 233961}, {"filename": "/examples/uart/uart_rx.h", "start": 233961, "end": 236652}, {"filename": "/examples/uart/uart_top.h", "start": 236652, "end": 238504}, {"filename": "/examples/uart/uart_tx.h", "start": 238504, "end": 241553}, {"filename": "/tests/metron/fail/README.md", "start": 241553, "end": 241750}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 241750, "end": 242057}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 242057, "end": 242312}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 242312, "end": 242532}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 242532, "end": 243018}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 243018, "end": 243255}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 243255, "end": 243672}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 243672, "end": 244138}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 244138, "end": 244456}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 244456, "end": 244908}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 244908, "end": 245165}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 245165, "end": 245447}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 245447, "end": 245683}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 245683, "end": 245913}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 245913, "end": 246282}, {"filename": "/tests/metron/pass/README.md", "start": 246282, "end": 246363}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 246363, "end": 247864}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 247864, "end": 248281}, {"filename": "/tests/metron/pass/basic_function.h", "start": 248281, "end": 248533}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 248533, "end": 248860}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 248860, "end": 249165}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 249165, "end": 249784}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 249784, "end": 250040}, {"filename": "/tests/metron/pass/basic_output.h", "start": 250040, "end": 250312}, {"filename": "/tests/metron/pass/basic_param.h", "start": 250312, "end": 250581}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 250581, "end": 250782}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 250782, "end": 250969}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 250969, "end": 251200}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 251200, "end": 251427}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 251427, "end": 251743}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 251743, "end": 252108}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 252108, "end": 252494}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 252494, "end": 252983}, {"filename": "/tests/metron/pass/basic_task.h", "start": 252983, "end": 253318}, {"filename": "/tests/metron/pass/basic_template.h", "start": 253318, "end": 253820}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 253820, "end": 259800}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 259800, "end": 260234}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 260234, "end": 260900}, {"filename": "/tests/metron/pass/bitfields.h", "start": 260900, "end": 262106}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 262106, "end": 262923}, {"filename": "/tests/metron/pass/builtins.h", "start": 262923, "end": 263261}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 263261, "end": 263578}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 263578, "end": 264163}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 264163, "end": 265018}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 265018, "end": 265496}, {"filename": "/tests/metron/pass/counter.h", "start": 265496, "end": 266143}, {"filename": "/tests/metron/pass/defines.h", "start": 266143, "end": 266464}, {"filename": "/tests/metron/pass/dontcare.h", "start": 266464, "end": 266751}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 266751, "end": 268146}, {"filename": "/tests/metron/pass/for_loops.h", "start": 268146, "end": 268473}, {"filename": "/tests/metron/pass/good_order.h", "start": 268473, "end": 268676}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 268676, "end": 269097}, {"filename": "/tests/metron/pass/include_guards.h", "start": 269097, "end": 269294}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 269294, "end": 269485}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 269485, "end": 269662}, {"filename": "/tests/metron/pass/init_chain.h", "start": 269662, "end": 270383}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 270383, "end": 270675}, {"filename": "/tests/metron/pass/input_signals.h", "start": 270675, "end": 271348}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 271348, "end": 271533}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 271533, "end": 271846}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 271846, "end": 272167}, {"filename": "/tests/metron/pass/minimal.h", "start": 272167, "end": 272347}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 272347, "end": 272723}, {"filename": "/tests/metron/pass/namespaces.h", "start": 272723, "end": 273104}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 273104, "end": 273565}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 273565, "end": 274138}, {"filename": "/tests/metron/pass/noconvert.h", "start": 274138, "end": 274468}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 274468, "end": 274821}, {"filename": "/tests/metron/pass/oneliners.h", "start": 274821, "end": 275095}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 275095, "end": 275582}, {"filename": "/tests/metron/pass/preproc.h", "start": 275582, "end": 275910}, {"filename": "/tests/metron/pass/private_getter.h", "start": 275910, "end": 276162}, {"filename": "/tests/metron/pass/self_reference.h", "start": 276162, "end": 276367}, {"filename": "/tests/metron/pass/slice.h", "start": 276367, "end": 276874}, {"filename": "/tests/metron/pass/structs.h", "start": 276874, "end": 277328}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 277328, "end": 277873}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 277873, "end": 280493}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 280493, "end": 281287}, {"filename": "/tests/metron/pass/tock_task.h", "start": 281287, "end": 281739}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 281739, "end": 281911}, {"filename": "/tests/metron/pass/unions.h", "start": 281911, "end": 282117}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 282117, "end": 282786}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 282786, "end": 283455}], "remote_package_size": 283455});

  })();
