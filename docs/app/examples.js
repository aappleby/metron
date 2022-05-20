
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 484}, {"filename": "/examples/rvtiny/README.md", "start": 484, "end": 648}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 648, "end": 6738}, {"filename": "/examples/rvsimple/README.md", "start": 6738, "end": 6817}, {"filename": "/examples/rvsimple/metron/register.h", "start": 6817, "end": 7502}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7502, "end": 8609}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8609, "end": 9824}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 9824, "end": 10588}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10588, "end": 13081}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 13081, "end": 14929}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 14929, "end": 15605}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15605, "end": 20332}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20332, "end": 21446}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21446, "end": 22417}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22417, "end": 23651}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23651, "end": 25589}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25589, "end": 26089}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 26089, "end": 29132}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 29132, "end": 34726}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 34726, "end": 35736}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 35736, "end": 36563}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36563, "end": 39184}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 39184, "end": 40645}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40645, "end": 46364}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46364, "end": 47237}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 47237, "end": 49355}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49355, "end": 50046}, {"filename": "/examples/pong/README.md", "start": 50046, "end": 50106}, {"filename": "/examples/pong/metron/pong.h", "start": 50106, "end": 53970}, {"filename": "/examples/pong/reference/README.md", "start": 53970, "end": 54030}, {"filename": "/examples/ibex/README.md", "start": 54030, "end": 54081}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 54081, "end": 68491}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68491, "end": 80589}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80589, "end": 82174}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 82174, "end": 98198}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 98198, "end": 100699}, {"filename": "/examples/rvtiny_sync/README.md", "start": 100699, "end": 100751}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 100751, "end": 107706}, {"filename": "/examples/uart/README.md", "start": 107706, "end": 107950}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 107950, "end": 109268}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109268, "end": 110651}, {"filename": "/examples/uart/metron/uart_top.h", "start": 110651, "end": 111677}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 111677, "end": 113596}, {"filename": "/tests/metron_good/include_guards.h", "start": 113596, "end": 113793}, {"filename": "/tests/metron_good/oneliners.h", "start": 113793, "end": 114056}, {"filename": "/tests/metron_good/enum_simple.h", "start": 114056, "end": 115424}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115424, "end": 115589}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115589, "end": 115944}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 115944, "end": 116782}, {"filename": "/tests/metron_good/magic_comments.h", "start": 116782, "end": 117085}, {"filename": "/tests/metron_good/defines.h", "start": 117085, "end": 117399}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117399, "end": 117826}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 117826, "end": 118006}, {"filename": "/tests/metron_good/README.md", "start": 118006, "end": 118087}, {"filename": "/tests/metron_good/good_order.h", "start": 118087, "end": 118383}, {"filename": "/tests/metron_good/basic_template.h", "start": 118383, "end": 118814}, {"filename": "/tests/metron_good/bit_casts.h", "start": 118814, "end": 124787}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 124787, "end": 125094}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 125094, "end": 125753}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 125753, "end": 126108}, {"filename": "/tests/metron_good/tock_task.h", "start": 126108, "end": 126464}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 126464, "end": 126684}, {"filename": "/tests/metron_good/all_func_types.h", "start": 126684, "end": 127480}, {"filename": "/tests/metron_good/basic_submod.h", "start": 127480, "end": 127769}, {"filename": "/tests/metron_good/dontcare.h", "start": 127769, "end": 128052}, {"filename": "/tests/metron_good/basic_param.h", "start": 128052, "end": 128311}, {"filename": "/tests/metron_good/basic_output.h", "start": 128311, "end": 128572}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 128572, "end": 129326}, {"filename": "/tests/metron_good/basic_function.h", "start": 129326, "end": 129605}, {"filename": "/tests/metron_good/builtins.h", "start": 129605, "end": 129936}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 129936, "end": 130182}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 130182, "end": 130589}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 130589, "end": 130820}, {"filename": "/tests/metron_good/basic_literals.h", "start": 130820, "end": 131432}, {"filename": "/tests/metron_good/basic_switch.h", "start": 131432, "end": 131909}, {"filename": "/tests/metron_good/bit_dup.h", "start": 131909, "end": 132568}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 132568, "end": 133227}, {"filename": "/tests/metron_good/local_localparam.h", "start": 133227, "end": 133405}, {"filename": "/tests/metron_good/minimal.h", "start": 133405, "end": 133480}, {"filename": "/tests/metron_good/multi_tick.h", "start": 133480, "end": 133846}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 133846, "end": 134109}, {"filename": "/tests/metron_good/namespaces.h", "start": 134109, "end": 134459}, {"filename": "/tests/metron_good/structs.h", "start": 134459, "end": 134678}, {"filename": "/tests/metron_good/basic_task.h", "start": 134678, "end": 135012}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 135012, "end": 135326}, {"filename": "/tests/metron_good/private_getter.h", "start": 135326, "end": 135550}, {"filename": "/tests/metron_good/init_chain.h", "start": 135550, "end": 136258}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 136258, "end": 136634}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 136634, "end": 137045}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 137045, "end": 137599}, {"filename": "/tests/metron_good/input_signals.h", "start": 137599, "end": 138261}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 138261, "end": 138806}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 138806, "end": 138965}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 138965, "end": 139260}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 139260, "end": 139400}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 139400, "end": 139811}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 139811, "end": 140351}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 140351, "end": 140599}, {"filename": "/tests/metron_bad/README.md", "start": 140599, "end": 140796}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 140796, "end": 141107}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 141107, "end": 141614}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 141614, "end": 141886}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 141886, "end": 142481}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 142481, "end": 142741}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 142741, "end": 142984}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 142984, "end": 143280}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 143280, "end": 143726}], "remote_package_size": 143726, "package_uuid": "272c1b2d-4141-4e38-84c1-5a6eb514d38e"});

  })();
