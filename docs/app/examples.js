
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
      var PACKAGE_NAME = 'docs/app/examples.data';
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
          }          Module['removeRunDependency']('datafile_docs/app/examples.data');

      };
      Module['addRunDependency']('datafile_docs/app/examples.data');

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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 452}, {"filename": "/examples/rvtiny/README.md", "start": 452, "end": 616}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 616, "end": 6712}, {"filename": "/examples/rvsimple/README.md", "start": 6712, "end": 6791}, {"filename": "/examples/rvsimple/metron/register.h", "start": 6791, "end": 7476}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7476, "end": 8629}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8629, "end": 9844}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 9844, "end": 10608}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10608, "end": 13101}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 13101, "end": 14949}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 14949, "end": 15625}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15625, "end": 20352}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20352, "end": 21466}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21466, "end": 22437}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22437, "end": 23671}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23671, "end": 25609}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25609, "end": 26109}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 26109, "end": 29152}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 29152, "end": 34746}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 34746, "end": 35756}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 35756, "end": 36583}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36583, "end": 39204}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 39204, "end": 40665}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40665, "end": 46384}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46384, "end": 47257}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 47257, "end": 49375}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49375, "end": 50066}, {"filename": "/examples/pong/README.md", "start": 50066, "end": 50126}, {"filename": "/examples/pong/metron/pong.h", "start": 50126, "end": 53990}, {"filename": "/examples/pong/reference/README.md", "start": 53990, "end": 54050}, {"filename": "/examples/ibex/README.md", "start": 54050, "end": 54101}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 54101, "end": 68511}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68511, "end": 80609}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80609, "end": 82194}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 82194, "end": 98218}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 98218, "end": 100719}, {"filename": "/examples/rvtiny_sync/README.md", "start": 100719, "end": 100771}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 100771, "end": 107726}, {"filename": "/examples/tutorial/tutorial1.h", "start": 107726, "end": 107794}, {"filename": "/examples/tutorial/tutorial2.h", "start": 107794, "end": 107862}, {"filename": "/examples/uart/README.md", "start": 107862, "end": 108106}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 108106, "end": 109399}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109399, "end": 110762}, {"filename": "/examples/uart/metron/uart_top.h", "start": 110762, "end": 111822}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 111822, "end": 113668}, {"filename": "/tests/metron_good/include_guards.h", "start": 113668, "end": 113865}, {"filename": "/tests/metron_good/oneliners.h", "start": 113865, "end": 114128}, {"filename": "/tests/metron_good/enum_simple.h", "start": 114128, "end": 115496}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115496, "end": 115661}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115661, "end": 116016}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 116016, "end": 116854}, {"filename": "/tests/metron_good/magic_comments.h", "start": 116854, "end": 117157}, {"filename": "/tests/metron_good/for_loops.h", "start": 117157, "end": 117477}, {"filename": "/tests/metron_good/defines.h", "start": 117477, "end": 117791}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117791, "end": 118218}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 118218, "end": 118398}, {"filename": "/tests/metron_good/force_tick.h", "start": 118398, "end": 118580}, {"filename": "/tests/metron_good/README.md", "start": 118580, "end": 118661}, {"filename": "/tests/metron_good/good_order.h", "start": 118661, "end": 118957}, {"filename": "/tests/metron_good/basic_template.h", "start": 118957, "end": 119388}, {"filename": "/tests/metron_good/bit_casts.h", "start": 119388, "end": 125361}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 125361, "end": 125633}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 125633, "end": 125940}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 125940, "end": 126599}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 126599, "end": 126954}, {"filename": "/tests/metron_good/tock_task.h", "start": 126954, "end": 127310}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 127310, "end": 127530}, {"filename": "/tests/metron_good/all_func_types.h", "start": 127530, "end": 129095}, {"filename": "/tests/metron_good/basic_submod.h", "start": 129095, "end": 129384}, {"filename": "/tests/metron_good/dontcare.h", "start": 129384, "end": 129667}, {"filename": "/tests/metron_good/basic_param.h", "start": 129667, "end": 129926}, {"filename": "/tests/metron_good/basic_output.h", "start": 129926, "end": 130187}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 130187, "end": 130941}, {"filename": "/tests/metron_good/basic_function.h", "start": 130941, "end": 131220}, {"filename": "/tests/metron_good/builtins.h", "start": 131220, "end": 131551}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 131551, "end": 131797}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 131797, "end": 132204}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 132204, "end": 132435}, {"filename": "/tests/metron_good/basic_literals.h", "start": 132435, "end": 133047}, {"filename": "/tests/metron_good/basic_switch.h", "start": 133047, "end": 133524}, {"filename": "/tests/metron_good/bit_dup.h", "start": 133524, "end": 134183}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 134183, "end": 134842}, {"filename": "/tests/metron_good/local_localparam.h", "start": 134842, "end": 135020}, {"filename": "/tests/metron_good/minimal.h", "start": 135020, "end": 135095}, {"filename": "/tests/metron_good/multi_tick.h", "start": 135095, "end": 135461}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 135461, "end": 135724}, {"filename": "/tests/metron_good/namespaces.h", "start": 135724, "end": 136074}, {"filename": "/tests/metron_good/structs.h", "start": 136074, "end": 136293}, {"filename": "/tests/metron_good/basic_task.h", "start": 136293, "end": 136627}, {"filename": "/tests/metron_good/nested_structs.h", "start": 136627, "end": 137140}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 137140, "end": 137454}, {"filename": "/tests/metron_good/private_getter.h", "start": 137454, "end": 137678}, {"filename": "/tests/metron_good/init_chain.h", "start": 137678, "end": 138386}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 138386, "end": 138762}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 138762, "end": 139173}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 139173, "end": 139727}, {"filename": "/tests/metron_good/input_signals.h", "start": 139727, "end": 140389}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 140389, "end": 140934}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 140934, "end": 141093}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 141093, "end": 141388}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 141388, "end": 141528}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 141528, "end": 141939}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 141939, "end": 142479}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 142479, "end": 142727}, {"filename": "/tests/metron_bad/README.md", "start": 142727, "end": 142924}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 142924, "end": 143235}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 143235, "end": 143742}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 143742, "end": 144337}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144337, "end": 144597}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 144597, "end": 144840}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 144840, "end": 145136}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 145136, "end": 145582}], "remote_package_size": 145582, "package_uuid": "5da7f23a-9ece-4f44-b262-eba74c064d51"});

  })();
