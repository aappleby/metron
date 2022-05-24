
  var Module = typeof Module !== 'undefined' ? Module : {};

  if (!Module.expectedDataFileDownloads) {
    Module.expectedDataFileDownloads = 0;
  }

  Module.expectedDataFileDownloads++;
  (function() {
    // When running as a pthread, FS operations are proxied to the main thread, so we don't need to
    // fetch the .data bundle on the worker
    if (Module['ENVIRONMENT_IS_PTHREAD']) return;
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
      var PACKAGE_UUID = metadata['package_uuid'];

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
            if (Module['setStatus']) Module['setStatus']('Downloading data... (' + loaded + '/' + total + ')');
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
Module['FS_createPath']("/examples", "rvtiny", true, true);
Module['FS_createPath']("/examples/rvtiny", "metron", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples", "rvtiny_sync", true, true);
Module['FS_createPath']("/examples/rvtiny_sync", "metron", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/examples/uart", "metron", true, true);
Module['FS_createPath']("/", "tests", true, true);
Module['FS_createPath']("/tests", "metron_good", true, true);
Module['FS_createPath']("/tests", "metron_bad", true, true);

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
          Module['addRunDependency']('fp ' + this.name);
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
          Module['removeRunDependency']('fp ' + that.name);
          this.requests[this.name] = null;
        }
      };

      var files = metadata['files'];
      for (var i = 0; i < files.length; ++i) {
        new DataRequest(files[i]['start'], files[i]['end'], files[i]['audio'] || 0).open('GET', files[i]['filename']);
      }

      function processPackageData(arrayBuffer) {
        assert(arrayBuffer, 'Loading data file failed.');
        assert(arrayBuffer instanceof ArrayBuffer, 'bad input to processPackageData');
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1384}, {"filename": "/examples/rvtiny/README.md", "start": 1384, "end": 1548}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 1548, "end": 7644}, {"filename": "/examples/rvsimple/README.md", "start": 7644, "end": 7723}, {"filename": "/examples/rvsimple/metron/register.h", "start": 7723, "end": 8408}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 8408, "end": 9561}, {"filename": "/examples/rvsimple/metron/config.h", "start": 9561, "end": 10776}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 10776, "end": 11540}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 11540, "end": 14033}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 14033, "end": 15881}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 15881, "end": 16557}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 16557, "end": 21284}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 21284, "end": 22398}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 22398, "end": 23369}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 23369, "end": 24603}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 24603, "end": 26541}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 26541, "end": 27041}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 27041, "end": 30084}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 30084, "end": 35678}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 35678, "end": 36688}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 36688, "end": 37515}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 37515, "end": 40136}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 40136, "end": 41597}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 41597, "end": 47316}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 47316, "end": 48189}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 48189, "end": 50307}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 50307, "end": 50998}, {"filename": "/examples/pong/README.md", "start": 50998, "end": 51058}, {"filename": "/examples/pong/metron/pong.h", "start": 51058, "end": 54922}, {"filename": "/examples/pong/reference/README.md", "start": 54922, "end": 54982}, {"filename": "/examples/ibex/README.md", "start": 54982, "end": 55033}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 55033, "end": 69443}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 69443, "end": 81541}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 81541, "end": 83126}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 83126, "end": 99150}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 99150, "end": 101651}, {"filename": "/examples/rvtiny_sync/README.md", "start": 101651, "end": 101703}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 101703, "end": 108658}, {"filename": "/examples/tutorial/tutorial5.h", "start": 108658, "end": 108699}, {"filename": "/examples/tutorial/vga.h", "start": 108699, "end": 109846}, {"filename": "/examples/tutorial/declaration_order.h", "start": 109846, "end": 110625}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 110625, "end": 111010}, {"filename": "/examples/tutorial/tutorial3.h", "start": 111010, "end": 111051}, {"filename": "/examples/tutorial/submodules.h", "start": 111051, "end": 112238}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 112238, "end": 113656}, {"filename": "/examples/tutorial/adder.h", "start": 113656, "end": 113836}, {"filename": "/examples/tutorial/tutorial4.h", "start": 113836, "end": 113877}, {"filename": "/examples/tutorial/tutorial2.h", "start": 113877, "end": 113945}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 113945, "end": 114926}, {"filename": "/examples/tutorial/counter.h", "start": 114926, "end": 115075}, {"filename": "/examples/tutorial/nonblocking.h", "start": 115075, "end": 115307}, {"filename": "/examples/uart/README.md", "start": 115307, "end": 115551}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 115551, "end": 116844}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 116844, "end": 118207}, {"filename": "/examples/uart/metron/uart_top.h", "start": 118207, "end": 119267}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 119267, "end": 121113}, {"filename": "/tests/metron_good/include_guards.h", "start": 121113, "end": 121310}, {"filename": "/tests/metron_good/oneliners.h", "start": 121310, "end": 121573}, {"filename": "/tests/metron_good/enum_simple.h", "start": 121573, "end": 122941}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 122941, "end": 123106}, {"filename": "/tests/metron_good/basic_increment.h", "start": 123106, "end": 123461}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 123461, "end": 124299}, {"filename": "/tests/metron_good/magic_comments.h", "start": 124299, "end": 124602}, {"filename": "/tests/metron_good/for_loops.h", "start": 124602, "end": 124922}, {"filename": "/tests/metron_good/defines.h", "start": 124922, "end": 125236}, {"filename": "/tests/metron_good/bit_concat.h", "start": 125236, "end": 125663}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 125663, "end": 125843}, {"filename": "/tests/metron_good/force_tick.h", "start": 125843, "end": 126025}, {"filename": "/tests/metron_good/README.md", "start": 126025, "end": 126106}, {"filename": "/tests/metron_good/good_order.h", "start": 126106, "end": 126402}, {"filename": "/tests/metron_good/basic_template.h", "start": 126402, "end": 126833}, {"filename": "/tests/metron_good/bit_casts.h", "start": 126833, "end": 132806}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 132806, "end": 133078}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 133078, "end": 133385}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 133385, "end": 134044}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 134044, "end": 134399}, {"filename": "/tests/metron_good/tock_task.h", "start": 134399, "end": 134755}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 134755, "end": 134975}, {"filename": "/tests/metron_good/all_func_types.h", "start": 134975, "end": 136540}, {"filename": "/tests/metron_good/basic_submod.h", "start": 136540, "end": 136829}, {"filename": "/tests/metron_good/dontcare.h", "start": 136829, "end": 137112}, {"filename": "/tests/metron_good/basic_param.h", "start": 137112, "end": 137371}, {"filename": "/tests/metron_good/basic_output.h", "start": 137371, "end": 137632}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 137632, "end": 138386}, {"filename": "/tests/metron_good/basic_function.h", "start": 138386, "end": 138665}, {"filename": "/tests/metron_good/builtins.h", "start": 138665, "end": 138996}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 138996, "end": 139242}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 139242, "end": 139649}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 139649, "end": 139880}, {"filename": "/tests/metron_good/basic_literals.h", "start": 139880, "end": 140492}, {"filename": "/tests/metron_good/basic_switch.h", "start": 140492, "end": 140969}, {"filename": "/tests/metron_good/bit_dup.h", "start": 140969, "end": 141628}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 141628, "end": 142287}, {"filename": "/tests/metron_good/local_localparam.h", "start": 142287, "end": 142465}, {"filename": "/tests/metron_good/minimal.h", "start": 142465, "end": 142540}, {"filename": "/tests/metron_good/multi_tick.h", "start": 142540, "end": 142906}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 142906, "end": 143169}, {"filename": "/tests/metron_good/namespaces.h", "start": 143169, "end": 143519}, {"filename": "/tests/metron_good/structs.h", "start": 143519, "end": 143738}, {"filename": "/tests/metron_good/basic_task.h", "start": 143738, "end": 144072}, {"filename": "/tests/metron_good/nested_structs.h", "start": 144072, "end": 144585}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 144585, "end": 144899}, {"filename": "/tests/metron_good/private_getter.h", "start": 144899, "end": 145123}, {"filename": "/tests/metron_good/init_chain.h", "start": 145123, "end": 145831}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 145831, "end": 146207}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 146207, "end": 146618}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 146618, "end": 147172}, {"filename": "/tests/metron_good/input_signals.h", "start": 147172, "end": 147834}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 147834, "end": 148379}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 148379, "end": 148538}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 148538, "end": 148833}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 148833, "end": 148973}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 148973, "end": 149384}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 149384, "end": 149924}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 149924, "end": 150172}, {"filename": "/tests/metron_bad/README.md", "start": 150172, "end": 150369}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 150369, "end": 150680}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 150680, "end": 151187}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 151187, "end": 151782}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 151782, "end": 152042}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 152042, "end": 152285}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 152285, "end": 152581}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 152581, "end": 153027}], "remote_package_size": 153027, "package_uuid": "fd4c1a2e-17a4-4c3c-a2c8-79c4033d022c"});

  })();
