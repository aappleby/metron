
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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 368}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 368, "end": 20210}, {"filename": "/examples/gb_spu/MetroBoySPU2.sv.d", "start": 20210, "end": 20242}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20242, "end": 20805}, {"filename": "/examples/ibex/README.md", "start": 20805, "end": 20856}, {"filename": "/examples/ibex/ibex.hancho", "start": 20856, "end": 20856}, {"filename": "/examples/ibex/ibex_alu.h", "start": 20856, "end": 22495}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22495, "end": 34680}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34680, "end": 49172}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49172, "end": 65249}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65249, "end": 67834}, {"filename": "/examples/j1/dpram.h", "start": 67834, "end": 68295}, {"filename": "/examples/j1/dpram.sv.d", "start": 68295, "end": 68313}, {"filename": "/examples/j1/j1.h", "start": 68313, "end": 72604}, {"filename": "/examples/j1/j1.hancho", "start": 72604, "end": 73224}, {"filename": "/examples/j1/j1.sv.d", "start": 73224, "end": 73247}, {"filename": "/examples/j1/main.d", "start": 73247, "end": 73330}, {"filename": "/examples/picorv32/picorv32.h", "start": 73330, "end": 168534}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168534, "end": 168534}, {"filename": "/examples/pong/README.md", "start": 168534, "end": 168594}, {"filename": "/examples/pong/main.d", "start": 168594, "end": 168816}, {"filename": "/examples/pong/pong.h", "start": 168816, "end": 172807}, {"filename": "/examples/pong/pong.hancho", "start": 172807, "end": 173300}, {"filename": "/examples/pong/pong.sv.d", "start": 173300, "end": 173316}, {"filename": "/examples/pong/reference/README.md", "start": 173316, "end": 173376}, {"filename": "/examples/rvsimple/README.md", "start": 173376, "end": 173455}, {"filename": "/examples/rvsimple/adder.h", "start": 173455, "end": 173962}, {"filename": "/examples/rvsimple/alu.h", "start": 173962, "end": 175430}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175430, "end": 178042}, {"filename": "/examples/rvsimple/config.h", "start": 178042, "end": 179264}, {"filename": "/examples/rvsimple/constants.h", "start": 179264, "end": 184990}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 184990, "end": 186107}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 186107, "end": 188047}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 188047, "end": 189287}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 189287, "end": 190538}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190538, "end": 191217}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 191217, "end": 192188}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 192188, "end": 194313}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194313, "end": 195084}, {"filename": "/examples/rvsimple/main.d", "start": 195084, "end": 197632}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 197632, "end": 198319}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 198319, "end": 199142}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 199142, "end": 200148}, {"filename": "/examples/rvsimple/regfile.h", "start": 200148, "end": 201126}, {"filename": "/examples/rvsimple/register.h", "start": 201126, "end": 201822}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 201822, "end": 204872}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 204872, "end": 208060}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 208060, "end": 213641}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 213641, "end": 216141}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 216141, "end": 220879}, {"filename": "/examples/rvsimple/toplevel.h", "start": 220879, "end": 222874}, {"filename": "/examples/scratch.h", "start": 222874, "end": 223242}, {"filename": "/examples/tutorial/adder.h", "start": 223242, "end": 223422}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 223422, "end": 224410}, {"filename": "/examples/tutorial/blockram.h", "start": 224410, "end": 224942}, {"filename": "/examples/tutorial/checksum.h", "start": 224942, "end": 225684}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 225684, "end": 226091}, {"filename": "/examples/tutorial/counter.h", "start": 226091, "end": 226242}, {"filename": "/examples/tutorial/declaration_order.h", "start": 226242, "end": 227068}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 227068, "end": 228027}, {"filename": "/examples/tutorial/nonblocking.h", "start": 228027, "end": 228153}, {"filename": "/examples/tutorial/submodules.h", "start": 228153, "end": 229253}, {"filename": "/examples/tutorial/templates.h", "start": 229253, "end": 229736}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 229736, "end": 229736}, {"filename": "/examples/tutorial/tutorial2.h", "start": 229736, "end": 229804}, {"filename": "/examples/tutorial/tutorial3.h", "start": 229804, "end": 229845}, {"filename": "/examples/tutorial/tutorial4.h", "start": 229845, "end": 229886}, {"filename": "/examples/tutorial/tutorial5.h", "start": 229886, "end": 229927}, {"filename": "/examples/tutorial/vga.h", "start": 229927, "end": 231107}, {"filename": "/examples/uart/README.md", "start": 231107, "end": 231351}, {"filename": "/examples/uart/main.d", "start": 231351, "end": 231828}, {"filename": "/examples/uart/uart.hancho", "start": 231828, "end": 234745}, {"filename": "/examples/uart/uart_hello.h", "start": 234745, "end": 237413}, {"filename": "/examples/uart/uart_rx.h", "start": 237413, "end": 240104}, {"filename": "/examples/uart/uart_top.h", "start": 240104, "end": 241956}, {"filename": "/examples/uart/uart_tx.h", "start": 241956, "end": 245005}, {"filename": "/tests/metron/fail/README.md", "start": 245005, "end": 245202}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 245202, "end": 245509}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 245509, "end": 245764}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 245764, "end": 245984}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 245984, "end": 246470}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 246470, "end": 246707}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 246707, "end": 247124}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 247124, "end": 247590}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 247590, "end": 247908}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 247908, "end": 248360}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 248360, "end": 248617}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 248617, "end": 248899}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 248899, "end": 249135}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 249135, "end": 249365}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 249365, "end": 249734}, {"filename": "/tests/metron/pass/README.md", "start": 249734, "end": 249815}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 249815, "end": 251316}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 251316, "end": 251733}, {"filename": "/tests/metron/pass/basic_function.h", "start": 251733, "end": 251985}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 251985, "end": 252312}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 252312, "end": 252617}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 252617, "end": 253236}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 253236, "end": 253492}, {"filename": "/tests/metron/pass/basic_output.h", "start": 253492, "end": 253764}, {"filename": "/tests/metron/pass/basic_param.h", "start": 253764, "end": 254033}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 254033, "end": 254234}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 254234, "end": 254421}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 254421, "end": 254652}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 254652, "end": 254879}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 254879, "end": 255195}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 255195, "end": 255560}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 255560, "end": 255946}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 255946, "end": 256435}, {"filename": "/tests/metron/pass/basic_task.h", "start": 256435, "end": 256770}, {"filename": "/tests/metron/pass/basic_template.h", "start": 256770, "end": 257272}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 257272, "end": 263252}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 263252, "end": 263686}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 263686, "end": 264352}, {"filename": "/tests/metron/pass/bitfields.h", "start": 264352, "end": 265558}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 265558, "end": 266375}, {"filename": "/tests/metron/pass/builtins.h", "start": 266375, "end": 266713}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 266713, "end": 267030}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 267030, "end": 267615}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 267615, "end": 268470}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 268470, "end": 268948}, {"filename": "/tests/metron/pass/counter.h", "start": 268948, "end": 269595}, {"filename": "/tests/metron/pass/defines.h", "start": 269595, "end": 269916}, {"filename": "/tests/metron/pass/dontcare.h", "start": 269916, "end": 270203}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 270203, "end": 271598}, {"filename": "/tests/metron/pass/for_loops.h", "start": 271598, "end": 271925}, {"filename": "/tests/metron/pass/good_order.h", "start": 271925, "end": 272128}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 272128, "end": 272549}, {"filename": "/tests/metron/pass/include_guards.h", "start": 272549, "end": 272746}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 272746, "end": 272937}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 272937, "end": 273114}, {"filename": "/tests/metron/pass/init_chain.h", "start": 273114, "end": 273835}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 273835, "end": 274127}, {"filename": "/tests/metron/pass/input_signals.h", "start": 274127, "end": 274800}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 274800, "end": 274985}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 274985, "end": 275298}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 275298, "end": 275619}, {"filename": "/tests/metron/pass/minimal.h", "start": 275619, "end": 275799}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 275799, "end": 276175}, {"filename": "/tests/metron/pass/namespaces.h", "start": 276175, "end": 276556}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 276556, "end": 277017}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 277017, "end": 277590}, {"filename": "/tests/metron/pass/noconvert.h", "start": 277590, "end": 277920}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 277920, "end": 278273}, {"filename": "/tests/metron/pass/oneliners.h", "start": 278273, "end": 278547}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 278547, "end": 279034}, {"filename": "/tests/metron/pass/preproc.h", "start": 279034, "end": 279362}, {"filename": "/tests/metron/pass/private_getter.h", "start": 279362, "end": 279614}, {"filename": "/tests/metron/pass/self_reference.h", "start": 279614, "end": 279819}, {"filename": "/tests/metron/pass/slice.h", "start": 279819, "end": 280326}, {"filename": "/tests/metron/pass/structs.h", "start": 280326, "end": 280780}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 280780, "end": 281325}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 281325, "end": 283945}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 283945, "end": 284739}, {"filename": "/tests/metron/pass/tock_task.h", "start": 284739, "end": 285191}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 285191, "end": 285363}, {"filename": "/tests/metron/pass/unions.h", "start": 285363, "end": 285569}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 285569, "end": 286238}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 286238, "end": 286907}], "remote_package_size": 286907});

  })();
