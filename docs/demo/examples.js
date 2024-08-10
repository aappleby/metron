
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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 535}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 535, "end": 20377}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20377, "end": 21007}, {"filename": "/examples/ibex/README.md", "start": 21007, "end": 21058}, {"filename": "/examples/ibex/ibex.hancho", "start": 21058, "end": 21058}, {"filename": "/examples/ibex/ibex_alu.h", "start": 21058, "end": 22697}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22697, "end": 34882}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34882, "end": 49374}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49374, "end": 65451}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65451, "end": 68036}, {"filename": "/examples/j1/dpram.h", "start": 68036, "end": 68497}, {"filename": "/examples/j1/j1.h", "start": 68497, "end": 72788}, {"filename": "/examples/j1/j1.hancho", "start": 72788, "end": 73481}, {"filename": "/examples/picorv32/picorv32.h", "start": 73481, "end": 168685}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168685, "end": 168685}, {"filename": "/examples/pong/README.md", "start": 168685, "end": 168745}, {"filename": "/examples/pong/pong.h", "start": 168745, "end": 172736}, {"filename": "/examples/pong/pong.hancho", "start": 172736, "end": 173232}, {"filename": "/examples/pong/reference/README.md", "start": 173232, "end": 173292}, {"filename": "/examples/rvsimple/README.md", "start": 173292, "end": 173371}, {"filename": "/examples/rvsimple/adder.h", "start": 173371, "end": 173878}, {"filename": "/examples/rvsimple/alu.h", "start": 173878, "end": 175346}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175346, "end": 177958}, {"filename": "/examples/rvsimple/config.h", "start": 177958, "end": 179180}, {"filename": "/examples/rvsimple/constants.h", "start": 179180, "end": 184906}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 184906, "end": 186023}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 186023, "end": 187963}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 187963, "end": 189203}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 189203, "end": 190454}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190454, "end": 191133}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 191133, "end": 192104}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 192104, "end": 194229}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194229, "end": 195000}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 195000, "end": 195687}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 195687, "end": 196510}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 196510, "end": 197516}, {"filename": "/examples/rvsimple/regfile.h", "start": 197516, "end": 198494}, {"filename": "/examples/rvsimple/register.h", "start": 198494, "end": 199190}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 199190, "end": 202240}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 202240, "end": 205278}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 205278, "end": 210859}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 210859, "end": 213359}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 213359, "end": 218097}, {"filename": "/examples/rvsimple/toplevel.h", "start": 218097, "end": 220092}, {"filename": "/examples/scratch.h", "start": 220092, "end": 220460}, {"filename": "/examples/tutorial/adder.h", "start": 220460, "end": 220640}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 220640, "end": 221628}, {"filename": "/examples/tutorial/blockram.h", "start": 221628, "end": 222160}, {"filename": "/examples/tutorial/checksum.h", "start": 222160, "end": 222902}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 222902, "end": 223309}, {"filename": "/examples/tutorial/counter.h", "start": 223309, "end": 223460}, {"filename": "/examples/tutorial/declaration_order.h", "start": 223460, "end": 224286}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 224286, "end": 225245}, {"filename": "/examples/tutorial/nonblocking.h", "start": 225245, "end": 225371}, {"filename": "/examples/tutorial/submodules.h", "start": 225371, "end": 226471}, {"filename": "/examples/tutorial/templates.h", "start": 226471, "end": 226954}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 226954, "end": 226954}, {"filename": "/examples/tutorial/tutorial2.h", "start": 226954, "end": 227022}, {"filename": "/examples/tutorial/tutorial3.h", "start": 227022, "end": 227063}, {"filename": "/examples/tutorial/tutorial4.h", "start": 227063, "end": 227104}, {"filename": "/examples/tutorial/tutorial5.h", "start": 227104, "end": 227145}, {"filename": "/examples/tutorial/vga.h", "start": 227145, "end": 228325}, {"filename": "/examples/uart/README.md", "start": 228325, "end": 228569}, {"filename": "/examples/uart/uart.hancho", "start": 228569, "end": 231490}, {"filename": "/examples/uart/uart_hello.h", "start": 231490, "end": 234158}, {"filename": "/examples/uart/uart_rx.h", "start": 234158, "end": 236849}, {"filename": "/examples/uart/uart_top.h", "start": 236849, "end": 238701}, {"filename": "/examples/uart/uart_tx.h", "start": 238701, "end": 241750}, {"filename": "/tests/metron/fail/README.md", "start": 241750, "end": 241947}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 241947, "end": 242254}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 242254, "end": 242509}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 242509, "end": 242729}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 242729, "end": 243215}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 243215, "end": 243452}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 243452, "end": 243869}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 243869, "end": 244335}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 244335, "end": 244653}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 244653, "end": 245105}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 245105, "end": 245362}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 245362, "end": 245644}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 245644, "end": 245880}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 245880, "end": 246110}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 246110, "end": 246479}, {"filename": "/tests/metron/pass/README.md", "start": 246479, "end": 246560}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 246560, "end": 248061}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 248061, "end": 248478}, {"filename": "/tests/metron/pass/basic_function.h", "start": 248478, "end": 248730}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 248730, "end": 249057}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 249057, "end": 249362}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 249362, "end": 249981}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 249981, "end": 250237}, {"filename": "/tests/metron/pass/basic_output.h", "start": 250237, "end": 250509}, {"filename": "/tests/metron/pass/basic_param.h", "start": 250509, "end": 250778}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 250778, "end": 250979}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 250979, "end": 251166}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 251166, "end": 251397}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 251397, "end": 251624}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 251624, "end": 251940}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 251940, "end": 252305}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 252305, "end": 252691}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 252691, "end": 253180}, {"filename": "/tests/metron/pass/basic_task.h", "start": 253180, "end": 253515}, {"filename": "/tests/metron/pass/basic_template.h", "start": 253515, "end": 254017}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 254017, "end": 259997}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 259997, "end": 260431}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 260431, "end": 261097}, {"filename": "/tests/metron/pass/bitfields.h", "start": 261097, "end": 262303}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 262303, "end": 263120}, {"filename": "/tests/metron/pass/builtins.h", "start": 263120, "end": 263458}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 263458, "end": 263775}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 263775, "end": 264360}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 264360, "end": 265215}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 265215, "end": 265693}, {"filename": "/tests/metron/pass/counter.h", "start": 265693, "end": 266340}, {"filename": "/tests/metron/pass/defines.h", "start": 266340, "end": 266661}, {"filename": "/tests/metron/pass/dontcare.h", "start": 266661, "end": 266948}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 266948, "end": 268343}, {"filename": "/tests/metron/pass/for_loops.h", "start": 268343, "end": 268670}, {"filename": "/tests/metron/pass/good_order.h", "start": 268670, "end": 268873}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 268873, "end": 269294}, {"filename": "/tests/metron/pass/include_guards.h", "start": 269294, "end": 269491}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 269491, "end": 269682}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 269682, "end": 269859}, {"filename": "/tests/metron/pass/init_chain.h", "start": 269859, "end": 270580}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 270580, "end": 270872}, {"filename": "/tests/metron/pass/input_signals.h", "start": 270872, "end": 271545}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 271545, "end": 271730}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 271730, "end": 272043}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 272043, "end": 272364}, {"filename": "/tests/metron/pass/minimal.h", "start": 272364, "end": 272544}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 272544, "end": 272920}, {"filename": "/tests/metron/pass/namespaces.h", "start": 272920, "end": 273301}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 273301, "end": 273762}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 273762, "end": 274335}, {"filename": "/tests/metron/pass/noconvert.h", "start": 274335, "end": 274665}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 274665, "end": 275018}, {"filename": "/tests/metron/pass/oneliners.h", "start": 275018, "end": 275292}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 275292, "end": 275779}, {"filename": "/tests/metron/pass/preproc.h", "start": 275779, "end": 276107}, {"filename": "/tests/metron/pass/private_getter.h", "start": 276107, "end": 276359}, {"filename": "/tests/metron/pass/self_reference.h", "start": 276359, "end": 276564}, {"filename": "/tests/metron/pass/slice.h", "start": 276564, "end": 277071}, {"filename": "/tests/metron/pass/structs.h", "start": 277071, "end": 277525}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 277525, "end": 278070}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 278070, "end": 280690}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 280690, "end": 281484}, {"filename": "/tests/metron/pass/tock_task.h", "start": 281484, "end": 281936}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 281936, "end": 282108}, {"filename": "/tests/metron/pass/unions.h", "start": 282108, "end": 282314}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 282314, "end": 282983}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 282983, "end": 283652}], "remote_package_size": 283652});

  })();
