
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1384}, {"filename": "/examples/rvtiny/README.md", "start": 1384, "end": 1548}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 1548, "end": 7644}, {"filename": "/examples/rvsimple/README.md", "start": 7644, "end": 7723}, {"filename": "/examples/rvsimple/metron/register.h", "start": 7723, "end": 8408}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 8408, "end": 9561}, {"filename": "/examples/rvsimple/metron/config.h", "start": 9561, "end": 10776}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 10776, "end": 11540}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 11540, "end": 14033}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 14033, "end": 15881}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 15881, "end": 16557}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 16557, "end": 21284}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 21284, "end": 22398}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 22398, "end": 23369}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 23369, "end": 24603}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 24603, "end": 26541}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 26541, "end": 27041}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 27041, "end": 30084}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 30084, "end": 35678}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 35678, "end": 36688}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 36688, "end": 37515}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 37515, "end": 40136}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 40136, "end": 41597}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 41597, "end": 47316}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 47316, "end": 48189}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 48189, "end": 50307}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 50307, "end": 50998}, {"filename": "/examples/pong/README.md", "start": 50998, "end": 51058}, {"filename": "/examples/pong/metron/pong.h", "start": 51058, "end": 54922}, {"filename": "/examples/pong/reference/README.md", "start": 54922, "end": 54982}, {"filename": "/examples/ibex/README.md", "start": 54982, "end": 55033}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 55033, "end": 69443}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 69443, "end": 81541}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 81541, "end": 83126}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 83126, "end": 99150}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 99150, "end": 101651}, {"filename": "/examples/rvtiny_sync/README.md", "start": 101651, "end": 101703}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 101703, "end": 108658}, {"filename": "/examples/tutorial/tutorial5.h", "start": 108658, "end": 108699}, {"filename": "/examples/tutorial/vga.h", "start": 108699, "end": 109846}, {"filename": "/examples/tutorial/declaration_order.h", "start": 109846, "end": 110625}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 110625, "end": 111010}, {"filename": "/examples/tutorial/tutorial3.h", "start": 111010, "end": 111051}, {"filename": "/examples/tutorial/submodules.h", "start": 111051, "end": 111958}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 111958, "end": 113376}, {"filename": "/examples/tutorial/adder.h", "start": 113376, "end": 113556}, {"filename": "/examples/tutorial/tutorial4.h", "start": 113556, "end": 113597}, {"filename": "/examples/tutorial/tutorial2.h", "start": 113597, "end": 113665}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 113665, "end": 114646}, {"filename": "/examples/tutorial/counter.h", "start": 114646, "end": 114795}, {"filename": "/examples/tutorial/nonblocking.h", "start": 114795, "end": 115027}, {"filename": "/examples/uart/README.md", "start": 115027, "end": 115271}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 115271, "end": 116564}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 116564, "end": 117927}, {"filename": "/examples/uart/metron/uart_top.h", "start": 117927, "end": 118987}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 118987, "end": 120833}, {"filename": "/tests/metron_good/include_guards.h", "start": 120833, "end": 121030}, {"filename": "/tests/metron_good/oneliners.h", "start": 121030, "end": 121293}, {"filename": "/tests/metron_good/enum_simple.h", "start": 121293, "end": 122661}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 122661, "end": 122826}, {"filename": "/tests/metron_good/basic_increment.h", "start": 122826, "end": 123181}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 123181, "end": 124019}, {"filename": "/tests/metron_good/magic_comments.h", "start": 124019, "end": 124322}, {"filename": "/tests/metron_good/for_loops.h", "start": 124322, "end": 124642}, {"filename": "/tests/metron_good/defines.h", "start": 124642, "end": 124956}, {"filename": "/tests/metron_good/bit_concat.h", "start": 124956, "end": 125383}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 125383, "end": 125563}, {"filename": "/tests/metron_good/force_tick.h", "start": 125563, "end": 125745}, {"filename": "/tests/metron_good/README.md", "start": 125745, "end": 125826}, {"filename": "/tests/metron_good/good_order.h", "start": 125826, "end": 126122}, {"filename": "/tests/metron_good/basic_template.h", "start": 126122, "end": 126553}, {"filename": "/tests/metron_good/bit_casts.h", "start": 126553, "end": 132526}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 132526, "end": 132798}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 132798, "end": 133105}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 133105, "end": 133764}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 133764, "end": 134119}, {"filename": "/tests/metron_good/tock_task.h", "start": 134119, "end": 134475}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 134475, "end": 134695}, {"filename": "/tests/metron_good/all_func_types.h", "start": 134695, "end": 136260}, {"filename": "/tests/metron_good/basic_submod.h", "start": 136260, "end": 136549}, {"filename": "/tests/metron_good/dontcare.h", "start": 136549, "end": 136832}, {"filename": "/tests/metron_good/basic_param.h", "start": 136832, "end": 137091}, {"filename": "/tests/metron_good/basic_output.h", "start": 137091, "end": 137352}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 137352, "end": 138106}, {"filename": "/tests/metron_good/basic_function.h", "start": 138106, "end": 138385}, {"filename": "/tests/metron_good/builtins.h", "start": 138385, "end": 138716}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 138716, "end": 138962}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 138962, "end": 139369}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 139369, "end": 139600}, {"filename": "/tests/metron_good/basic_literals.h", "start": 139600, "end": 140212}, {"filename": "/tests/metron_good/basic_switch.h", "start": 140212, "end": 140689}, {"filename": "/tests/metron_good/bit_dup.h", "start": 140689, "end": 141348}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 141348, "end": 142007}, {"filename": "/tests/metron_good/local_localparam.h", "start": 142007, "end": 142185}, {"filename": "/tests/metron_good/minimal.h", "start": 142185, "end": 142260}, {"filename": "/tests/metron_good/multi_tick.h", "start": 142260, "end": 142626}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 142626, "end": 142889}, {"filename": "/tests/metron_good/namespaces.h", "start": 142889, "end": 143239}, {"filename": "/tests/metron_good/structs.h", "start": 143239, "end": 143458}, {"filename": "/tests/metron_good/basic_task.h", "start": 143458, "end": 143792}, {"filename": "/tests/metron_good/nested_structs.h", "start": 143792, "end": 144305}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 144305, "end": 144619}, {"filename": "/tests/metron_good/private_getter.h", "start": 144619, "end": 144843}, {"filename": "/tests/metron_good/init_chain.h", "start": 144843, "end": 145551}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 145551, "end": 145927}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 145927, "end": 146338}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 146338, "end": 146892}, {"filename": "/tests/metron_good/input_signals.h", "start": 146892, "end": 147554}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 147554, "end": 148099}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 148099, "end": 148258}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 148258, "end": 148553}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 148553, "end": 148693}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 148693, "end": 149104}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 149104, "end": 149644}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 149644, "end": 149892}, {"filename": "/tests/metron_bad/README.md", "start": 149892, "end": 150089}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 150089, "end": 150400}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 150400, "end": 150907}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 150907, "end": 151502}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 151502, "end": 151762}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 151762, "end": 152005}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 152005, "end": 152301}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 152301, "end": 152747}], "remote_package_size": 152747, "package_uuid": "ea8071f9-75d5-43b5-9fc4-fdcd4ab8963b"});

  })();
