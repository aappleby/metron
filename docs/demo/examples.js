
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
Module['FS_createPath']("/examples/gb_spu", "build", true, true);
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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 348}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 348, "end": 20190}, {"filename": "/examples/gb_spu/MetroBoySPU2.sv.d", "start": 20190, "end": 20222}, {"filename": "/examples/gb_spu/build/MetroBoySPU2.a", "start": 20222, "end": 20230}, {"filename": "/examples/gb_spu/build/MetroBoySPU2.sv.d", "start": 20230, "end": 20268}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20268, "end": 20839}, {"filename": "/examples/ibex/README.md", "start": 20839, "end": 20890}, {"filename": "/examples/ibex/ibex.hancho", "start": 20890, "end": 20890}, {"filename": "/examples/ibex/ibex_alu.h", "start": 20890, "end": 22529}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22529, "end": 34714}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34714, "end": 49206}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49206, "end": 65283}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65283, "end": 67868}, {"filename": "/examples/j1/dpram.h", "start": 67868, "end": 68329}, {"filename": "/examples/j1/dpram.sv.d", "start": 68329, "end": 68347}, {"filename": "/examples/j1/j1.h", "start": 68347, "end": 72638}, {"filename": "/examples/j1/j1.hancho", "start": 72638, "end": 73251}, {"filename": "/examples/j1/j1.sv.d", "start": 73251, "end": 73274}, {"filename": "/examples/j1/main.d", "start": 73274, "end": 73357}, {"filename": "/examples/picorv32/picorv32.h", "start": 73357, "end": 168561}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168561, "end": 168561}, {"filename": "/examples/pong/README.md", "start": 168561, "end": 168621}, {"filename": "/examples/pong/main.d", "start": 168621, "end": 168843}, {"filename": "/examples/pong/pong.h", "start": 168843, "end": 172834}, {"filename": "/examples/pong/pong.hancho", "start": 172834, "end": 173313}, {"filename": "/examples/pong/pong.sv.d", "start": 173313, "end": 173329}, {"filename": "/examples/pong/reference/README.md", "start": 173329, "end": 173389}, {"filename": "/examples/rvsimple/README.md", "start": 173389, "end": 173468}, {"filename": "/examples/rvsimple/adder.h", "start": 173468, "end": 173975}, {"filename": "/examples/rvsimple/alu.h", "start": 173975, "end": 175443}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175443, "end": 178055}, {"filename": "/examples/rvsimple/config.h", "start": 178055, "end": 179277}, {"filename": "/examples/rvsimple/constants.h", "start": 179277, "end": 185003}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 185003, "end": 186120}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 186120, "end": 188060}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 188060, "end": 189300}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 189300, "end": 190551}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190551, "end": 191230}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 191230, "end": 192201}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 192201, "end": 194326}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194326, "end": 195097}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 195097, "end": 195784}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 195784, "end": 196607}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 196607, "end": 197613}, {"filename": "/examples/rvsimple/regfile.h", "start": 197613, "end": 198591}, {"filename": "/examples/rvsimple/register.h", "start": 198591, "end": 199287}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 199287, "end": 202337}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 202337, "end": 205414}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 205414, "end": 210995}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 210995, "end": 213495}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 213495, "end": 218233}, {"filename": "/examples/rvsimple/toplevel.h", "start": 218233, "end": 220228}, {"filename": "/examples/scratch.h", "start": 220228, "end": 220596}, {"filename": "/examples/tutorial/adder.h", "start": 220596, "end": 220776}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 220776, "end": 221764}, {"filename": "/examples/tutorial/blockram.h", "start": 221764, "end": 222296}, {"filename": "/examples/tutorial/checksum.h", "start": 222296, "end": 223038}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 223038, "end": 223445}, {"filename": "/examples/tutorial/counter.h", "start": 223445, "end": 223596}, {"filename": "/examples/tutorial/declaration_order.h", "start": 223596, "end": 224422}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 224422, "end": 225381}, {"filename": "/examples/tutorial/nonblocking.h", "start": 225381, "end": 225507}, {"filename": "/examples/tutorial/submodules.h", "start": 225507, "end": 226607}, {"filename": "/examples/tutorial/templates.h", "start": 226607, "end": 227090}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 227090, "end": 227090}, {"filename": "/examples/tutorial/tutorial2.h", "start": 227090, "end": 227158}, {"filename": "/examples/tutorial/tutorial3.h", "start": 227158, "end": 227199}, {"filename": "/examples/tutorial/tutorial4.h", "start": 227199, "end": 227240}, {"filename": "/examples/tutorial/tutorial5.h", "start": 227240, "end": 227281}, {"filename": "/examples/tutorial/vga.h", "start": 227281, "end": 228461}, {"filename": "/examples/uart/README.md", "start": 228461, "end": 228705}, {"filename": "/examples/uart/main.d", "start": 228705, "end": 229182}, {"filename": "/examples/uart/uart.hancho", "start": 229182, "end": 231992}, {"filename": "/examples/uart/uart_hello.h", "start": 231992, "end": 234660}, {"filename": "/examples/uart/uart_rx.h", "start": 234660, "end": 237351}, {"filename": "/examples/uart/uart_top.h", "start": 237351, "end": 239203}, {"filename": "/examples/uart/uart_tx.h", "start": 239203, "end": 242252}, {"filename": "/tests/metron/fail/README.md", "start": 242252, "end": 242449}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 242449, "end": 242756}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 242756, "end": 243011}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 243011, "end": 243231}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 243231, "end": 243717}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 243717, "end": 243954}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 243954, "end": 244371}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 244371, "end": 244837}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 244837, "end": 245155}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 245155, "end": 245607}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 245607, "end": 245864}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 245864, "end": 246146}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 246146, "end": 246382}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 246382, "end": 246612}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 246612, "end": 246981}, {"filename": "/tests/metron/pass/README.md", "start": 246981, "end": 247062}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 247062, "end": 248563}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 248563, "end": 248980}, {"filename": "/tests/metron/pass/basic_function.h", "start": 248980, "end": 249232}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 249232, "end": 249559}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 249559, "end": 249864}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 249864, "end": 250483}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 250483, "end": 250739}, {"filename": "/tests/metron/pass/basic_output.h", "start": 250739, "end": 251011}, {"filename": "/tests/metron/pass/basic_param.h", "start": 251011, "end": 251280}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 251280, "end": 251481}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 251481, "end": 251668}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 251668, "end": 251899}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 251899, "end": 252126}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 252126, "end": 252442}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 252442, "end": 252807}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 252807, "end": 253193}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 253193, "end": 253682}, {"filename": "/tests/metron/pass/basic_task.h", "start": 253682, "end": 254017}, {"filename": "/tests/metron/pass/basic_template.h", "start": 254017, "end": 254519}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 254519, "end": 260499}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 260499, "end": 260933}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 260933, "end": 261599}, {"filename": "/tests/metron/pass/bitfields.h", "start": 261599, "end": 262805}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 262805, "end": 263622}, {"filename": "/tests/metron/pass/builtins.h", "start": 263622, "end": 263960}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 263960, "end": 264277}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 264277, "end": 264862}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 264862, "end": 265717}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 265717, "end": 266195}, {"filename": "/tests/metron/pass/counter.h", "start": 266195, "end": 266842}, {"filename": "/tests/metron/pass/defines.h", "start": 266842, "end": 267163}, {"filename": "/tests/metron/pass/dontcare.h", "start": 267163, "end": 267450}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 267450, "end": 268845}, {"filename": "/tests/metron/pass/for_loops.h", "start": 268845, "end": 269172}, {"filename": "/tests/metron/pass/good_order.h", "start": 269172, "end": 269375}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 269375, "end": 269796}, {"filename": "/tests/metron/pass/include_guards.h", "start": 269796, "end": 269993}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 269993, "end": 270184}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 270184, "end": 270361}, {"filename": "/tests/metron/pass/init_chain.h", "start": 270361, "end": 271082}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 271082, "end": 271374}, {"filename": "/tests/metron/pass/input_signals.h", "start": 271374, "end": 272047}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 272047, "end": 272232}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 272232, "end": 272545}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 272545, "end": 272866}, {"filename": "/tests/metron/pass/minimal.h", "start": 272866, "end": 273046}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 273046, "end": 273422}, {"filename": "/tests/metron/pass/namespaces.h", "start": 273422, "end": 273803}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 273803, "end": 274264}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 274264, "end": 274837}, {"filename": "/tests/metron/pass/noconvert.h", "start": 274837, "end": 275167}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 275167, "end": 275520}, {"filename": "/tests/metron/pass/oneliners.h", "start": 275520, "end": 275794}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 275794, "end": 276281}, {"filename": "/tests/metron/pass/preproc.h", "start": 276281, "end": 276609}, {"filename": "/tests/metron/pass/private_getter.h", "start": 276609, "end": 276861}, {"filename": "/tests/metron/pass/self_reference.h", "start": 276861, "end": 277066}, {"filename": "/tests/metron/pass/slice.h", "start": 277066, "end": 277573}, {"filename": "/tests/metron/pass/structs.h", "start": 277573, "end": 278027}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 278027, "end": 278572}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 278572, "end": 281192}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 281192, "end": 281986}, {"filename": "/tests/metron/pass/tock_task.h", "start": 281986, "end": 282438}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 282438, "end": 282610}, {"filename": "/tests/metron/pass/unions.h", "start": 282610, "end": 282816}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 282816, "end": 283485}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 283485, "end": 284154}], "remote_package_size": 284154});

  })();
