
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 361}, {"filename": "/examples/rvtiny/README.md", "start": 361, "end": 525}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 525, "end": 6621}, {"filename": "/examples/rvsimple/README.md", "start": 6621, "end": 6700}, {"filename": "/examples/rvsimple/metron/register.h", "start": 6700, "end": 7385}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7385, "end": 8538}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8538, "end": 9753}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 9753, "end": 10517}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10517, "end": 13010}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 13010, "end": 14858}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 14858, "end": 15534}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15534, "end": 20261}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20261, "end": 21375}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21375, "end": 22346}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22346, "end": 23580}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23580, "end": 25518}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25518, "end": 26018}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 26018, "end": 29061}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 29061, "end": 34655}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 34655, "end": 35665}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 35665, "end": 36492}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36492, "end": 39113}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 39113, "end": 40574}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40574, "end": 46293}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46293, "end": 47166}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 47166, "end": 49284}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49284, "end": 49975}, {"filename": "/examples/pong/README.md", "start": 49975, "end": 50035}, {"filename": "/examples/pong/metron/pong.h", "start": 50035, "end": 53899}, {"filename": "/examples/pong/reference/README.md", "start": 53899, "end": 53959}, {"filename": "/examples/ibex/README.md", "start": 53959, "end": 54010}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 54010, "end": 68420}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68420, "end": 80518}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80518, "end": 82103}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 82103, "end": 98127}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 98127, "end": 100628}, {"filename": "/examples/rvtiny_sync/README.md", "start": 100628, "end": 100680}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 100680, "end": 107635}, {"filename": "/examples/uart/README.md", "start": 107635, "end": 107879}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 107879, "end": 109172}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109172, "end": 110535}, {"filename": "/examples/uart/metron/uart_top.h", "start": 110535, "end": 111595}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 111595, "end": 113441}, {"filename": "/tests/metron_good/include_guards.h", "start": 113441, "end": 113638}, {"filename": "/tests/metron_good/oneliners.h", "start": 113638, "end": 113901}, {"filename": "/tests/metron_good/enum_simple.h", "start": 113901, "end": 115269}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115269, "end": 115434}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115434, "end": 115789}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 115789, "end": 116627}, {"filename": "/tests/metron_good/magic_comments.h", "start": 116627, "end": 116930}, {"filename": "/tests/metron_good/for_loops.h", "start": 116930, "end": 117250}, {"filename": "/tests/metron_good/defines.h", "start": 117250, "end": 117564}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117564, "end": 117991}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 117991, "end": 118171}, {"filename": "/tests/metron_good/force_tick.h", "start": 118171, "end": 118353}, {"filename": "/tests/metron_good/README.md", "start": 118353, "end": 118434}, {"filename": "/tests/metron_good/good_order.h", "start": 118434, "end": 118730}, {"filename": "/tests/metron_good/basic_template.h", "start": 118730, "end": 119161}, {"filename": "/tests/metron_good/bit_casts.h", "start": 119161, "end": 125134}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 125134, "end": 125406}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 125406, "end": 125713}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 125713, "end": 126372}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 126372, "end": 126727}, {"filename": "/tests/metron_good/tock_task.h", "start": 126727, "end": 127083}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 127083, "end": 127303}, {"filename": "/tests/metron_good/all_func_types.h", "start": 127303, "end": 128868}, {"filename": "/tests/metron_good/basic_submod.h", "start": 128868, "end": 129157}, {"filename": "/tests/metron_good/dontcare.h", "start": 129157, "end": 129440}, {"filename": "/tests/metron_good/basic_param.h", "start": 129440, "end": 129699}, {"filename": "/tests/metron_good/basic_output.h", "start": 129699, "end": 129960}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 129960, "end": 130714}, {"filename": "/tests/metron_good/basic_function.h", "start": 130714, "end": 130993}, {"filename": "/tests/metron_good/builtins.h", "start": 130993, "end": 131324}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 131324, "end": 131570}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 131570, "end": 131977}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 131977, "end": 132208}, {"filename": "/tests/metron_good/basic_literals.h", "start": 132208, "end": 132820}, {"filename": "/tests/metron_good/basic_switch.h", "start": 132820, "end": 133297}, {"filename": "/tests/metron_good/bit_dup.h", "start": 133297, "end": 133956}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 133956, "end": 134615}, {"filename": "/tests/metron_good/local_localparam.h", "start": 134615, "end": 134793}, {"filename": "/tests/metron_good/minimal.h", "start": 134793, "end": 134868}, {"filename": "/tests/metron_good/multi_tick.h", "start": 134868, "end": 135234}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 135234, "end": 135497}, {"filename": "/tests/metron_good/namespaces.h", "start": 135497, "end": 135847}, {"filename": "/tests/metron_good/structs.h", "start": 135847, "end": 136066}, {"filename": "/tests/metron_good/basic_task.h", "start": 136066, "end": 136400}, {"filename": "/tests/metron_good/nested_structs.h", "start": 136400, "end": 136572}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 136572, "end": 136886}, {"filename": "/tests/metron_good/private_getter.h", "start": 136886, "end": 137110}, {"filename": "/tests/metron_good/init_chain.h", "start": 137110, "end": 137818}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 137818, "end": 138194}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 138194, "end": 138605}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 138605, "end": 139159}, {"filename": "/tests/metron_good/input_signals.h", "start": 139159, "end": 139821}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 139821, "end": 140366}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 140366, "end": 140525}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 140525, "end": 140820}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 140820, "end": 140960}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 140960, "end": 141371}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 141371, "end": 141911}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141911, "end": 142159}, {"filename": "/tests/metron_bad/README.md", "start": 142159, "end": 142356}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 142356, "end": 142667}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 142667, "end": 143174}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 143174, "end": 143769}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 143769, "end": 144029}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 144029, "end": 144272}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 144272, "end": 144568}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144568, "end": 145014}], "remote_package_size": 145014, "package_uuid": "60f6e559-ec58-4641-b1c7-7820bdf41cd5"});

  })();
