
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 434}, {"filename": "/examples/rvtiny/README.md", "start": 434, "end": 598}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 598, "end": 6694}, {"filename": "/examples/rvsimple/README.md", "start": 6694, "end": 6773}, {"filename": "/examples/rvsimple/metron/register.h", "start": 6773, "end": 7458}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7458, "end": 8611}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8611, "end": 9826}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 9826, "end": 10590}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10590, "end": 13083}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 13083, "end": 14931}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 14931, "end": 15607}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15607, "end": 20334}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20334, "end": 21448}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21448, "end": 22419}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22419, "end": 23653}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23653, "end": 25591}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25591, "end": 26091}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 26091, "end": 29134}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 29134, "end": 34728}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 34728, "end": 35738}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 35738, "end": 36565}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36565, "end": 39186}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 39186, "end": 40647}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40647, "end": 46366}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46366, "end": 47239}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 47239, "end": 49357}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49357, "end": 50048}, {"filename": "/examples/pong/README.md", "start": 50048, "end": 50108}, {"filename": "/examples/pong/metron/pong.h", "start": 50108, "end": 53972}, {"filename": "/examples/pong/reference/README.md", "start": 53972, "end": 54032}, {"filename": "/examples/ibex/README.md", "start": 54032, "end": 54083}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 54083, "end": 68493}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68493, "end": 80591}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80591, "end": 82176}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 82176, "end": 98200}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 98200, "end": 100701}, {"filename": "/examples/rvtiny_sync/README.md", "start": 100701, "end": 100753}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 100753, "end": 107708}, {"filename": "/examples/uart/README.md", "start": 107708, "end": 107952}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 107952, "end": 109245}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109245, "end": 110608}, {"filename": "/examples/uart/metron/uart_top.h", "start": 110608, "end": 111668}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 111668, "end": 113514}, {"filename": "/tests/metron_good/include_guards.h", "start": 113514, "end": 113711}, {"filename": "/tests/metron_good/oneliners.h", "start": 113711, "end": 113974}, {"filename": "/tests/metron_good/enum_simple.h", "start": 113974, "end": 115342}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115342, "end": 115507}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115507, "end": 115862}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 115862, "end": 116700}, {"filename": "/tests/metron_good/magic_comments.h", "start": 116700, "end": 117003}, {"filename": "/tests/metron_good/for_loops.h", "start": 117003, "end": 117323}, {"filename": "/tests/metron_good/defines.h", "start": 117323, "end": 117637}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117637, "end": 118064}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 118064, "end": 118244}, {"filename": "/tests/metron_good/force_tick.h", "start": 118244, "end": 118426}, {"filename": "/tests/metron_good/README.md", "start": 118426, "end": 118507}, {"filename": "/tests/metron_good/good_order.h", "start": 118507, "end": 118803}, {"filename": "/tests/metron_good/basic_template.h", "start": 118803, "end": 119234}, {"filename": "/tests/metron_good/bit_casts.h", "start": 119234, "end": 125207}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 125207, "end": 125479}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 125479, "end": 125786}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 125786, "end": 126445}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 126445, "end": 126800}, {"filename": "/tests/metron_good/tock_task.h", "start": 126800, "end": 127156}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 127156, "end": 127376}, {"filename": "/tests/metron_good/all_func_types.h", "start": 127376, "end": 128941}, {"filename": "/tests/metron_good/basic_submod.h", "start": 128941, "end": 129230}, {"filename": "/tests/metron_good/dontcare.h", "start": 129230, "end": 129513}, {"filename": "/tests/metron_good/basic_param.h", "start": 129513, "end": 129772}, {"filename": "/tests/metron_good/basic_output.h", "start": 129772, "end": 130033}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 130033, "end": 130787}, {"filename": "/tests/metron_good/basic_function.h", "start": 130787, "end": 131066}, {"filename": "/tests/metron_good/builtins.h", "start": 131066, "end": 131397}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 131397, "end": 131643}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 131643, "end": 132050}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 132050, "end": 132281}, {"filename": "/tests/metron_good/basic_literals.h", "start": 132281, "end": 132893}, {"filename": "/tests/metron_good/basic_switch.h", "start": 132893, "end": 133370}, {"filename": "/tests/metron_good/bit_dup.h", "start": 133370, "end": 134029}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 134029, "end": 134688}, {"filename": "/tests/metron_good/local_localparam.h", "start": 134688, "end": 134866}, {"filename": "/tests/metron_good/minimal.h", "start": 134866, "end": 134941}, {"filename": "/tests/metron_good/multi_tick.h", "start": 134941, "end": 135307}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 135307, "end": 135570}, {"filename": "/tests/metron_good/namespaces.h", "start": 135570, "end": 135920}, {"filename": "/tests/metron_good/structs.h", "start": 135920, "end": 136139}, {"filename": "/tests/metron_good/basic_task.h", "start": 136139, "end": 136473}, {"filename": "/tests/metron_good/nested_structs.h", "start": 136473, "end": 136499}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 136499, "end": 136813}, {"filename": "/tests/metron_good/private_getter.h", "start": 136813, "end": 137037}, {"filename": "/tests/metron_good/init_chain.h", "start": 137037, "end": 137745}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 137745, "end": 138121}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 138121, "end": 138532}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 138532, "end": 139086}, {"filename": "/tests/metron_good/input_signals.h", "start": 139086, "end": 139748}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 139748, "end": 140293}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 140293, "end": 140452}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 140452, "end": 140747}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 140747, "end": 140887}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 140887, "end": 141298}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 141298, "end": 141838}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141838, "end": 142086}, {"filename": "/tests/metron_bad/README.md", "start": 142086, "end": 142283}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 142283, "end": 142594}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 142594, "end": 143101}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 143101, "end": 143696}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 143696, "end": 143956}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 143956, "end": 144199}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 144199, "end": 144495}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144495, "end": 144941}], "remote_package_size": 144941, "package_uuid": "2f1c192b-521a-418c-b21c-1dab560116f9"});

  })();
