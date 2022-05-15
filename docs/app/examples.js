
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 353}, {"filename": "/examples/rvtiny/README.md", "start": 353, "end": 517}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 517, "end": 6607}, {"filename": "/examples/rvsimple/README.md", "start": 6607, "end": 6686}, {"filename": "/examples/rvsimple/metron/register.h", "start": 6686, "end": 7371}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7371, "end": 8478}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8478, "end": 9693}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 9693, "end": 10457}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10457, "end": 12950}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 12950, "end": 14798}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 14798, "end": 15474}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15474, "end": 20201}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20201, "end": 21315}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21315, "end": 22286}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22286, "end": 23520}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23520, "end": 25458}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25458, "end": 25958}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 25958, "end": 29001}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 29001, "end": 34595}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 34595, "end": 35605}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 35605, "end": 36432}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36432, "end": 39053}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 39053, "end": 40514}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40514, "end": 46233}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46233, "end": 47106}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 47106, "end": 49224}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49224, "end": 49915}, {"filename": "/examples/pong/README.md", "start": 49915, "end": 49975}, {"filename": "/examples/pong/metron/pong.h", "start": 49975, "end": 53839}, {"filename": "/examples/pong/reference/README.md", "start": 53839, "end": 53899}, {"filename": "/examples/ibex/README.md", "start": 53899, "end": 53950}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 53950, "end": 68360}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68360, "end": 80458}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80458, "end": 82043}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 82043, "end": 98067}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 98067, "end": 100568}, {"filename": "/examples/rvtiny_sync/README.md", "start": 100568, "end": 100620}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 100620, "end": 107575}, {"filename": "/examples/uart/README.md", "start": 107575, "end": 107819}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 107819, "end": 109137}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109137, "end": 110520}, {"filename": "/examples/uart/metron/uart_top.h", "start": 110520, "end": 111546}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 111546, "end": 113465}, {"filename": "/tests/metron_good/include_guards.h", "start": 113465, "end": 113662}, {"filename": "/tests/metron_good/oneliners.h", "start": 113662, "end": 113925}, {"filename": "/tests/metron_good/enum_simple.h", "start": 113925, "end": 115293}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115293, "end": 115458}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115458, "end": 115813}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 115813, "end": 116482}, {"filename": "/tests/metron_good/magic_comments.h", "start": 116482, "end": 116783}, {"filename": "/tests/metron_good/defines.h", "start": 116783, "end": 117097}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117097, "end": 117501}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 117501, "end": 117681}, {"filename": "/tests/metron_good/README.md", "start": 117681, "end": 117762}, {"filename": "/tests/metron_good/good_order.h", "start": 117762, "end": 118058}, {"filename": "/tests/metron_good/basic_template.h", "start": 118058, "end": 118493}, {"filename": "/tests/metron_good/bit_casts.h", "start": 118493, "end": 124399}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 124399, "end": 124706}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 124706, "end": 125362}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 125362, "end": 125717}, {"filename": "/tests/metron_good/tock_task.h", "start": 125717, "end": 126045}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 126045, "end": 126265}, {"filename": "/tests/metron_good/basic_submod.h", "start": 126265, "end": 126554}, {"filename": "/tests/metron_good/dontcare.h", "start": 126554, "end": 126802}, {"filename": "/tests/metron_good/basic_param.h", "start": 126802, "end": 127061}, {"filename": "/tests/metron_good/basic_output.h", "start": 127061, "end": 127327}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 127327, "end": 128106}, {"filename": "/tests/metron_good/basic_function.h", "start": 128106, "end": 128385}, {"filename": "/tests/metron_good/builtins.h", "start": 128385, "end": 128691}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 128691, "end": 128937}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 128937, "end": 129341}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 129341, "end": 129572}, {"filename": "/tests/metron_good/basic_literals.h", "start": 129572, "end": 130158}, {"filename": "/tests/metron_good/basic_switch.h", "start": 130158, "end": 130635}, {"filename": "/tests/metron_good/bit_dup.h", "start": 130635, "end": 131268}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 131268, "end": 131924}, {"filename": "/tests/metron_good/local_localparam.h", "start": 131924, "end": 132102}, {"filename": "/tests/metron_good/minimal.h", "start": 132102, "end": 132177}, {"filename": "/tests/metron_good/multi_tick.h", "start": 132177, "end": 132543}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 132543, "end": 132806}, {"filename": "/tests/metron_good/namespaces.h", "start": 132806, "end": 133074}, {"filename": "/tests/metron_good/structs.h", "start": 133074, "end": 133544}, {"filename": "/tests/metron_good/basic_task.h", "start": 133544, "end": 133908}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 133908, "end": 134228}, {"filename": "/tests/metron_good/private_getter.h", "start": 134228, "end": 134426}, {"filename": "/tests/metron_good/init_chain.h", "start": 134426, "end": 135120}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 135120, "end": 135501}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 135501, "end": 135912}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 135912, "end": 136466}, {"filename": "/tests/metron_good/input_signals.h", "start": 136466, "end": 137128}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 137128, "end": 137693}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 137693, "end": 137823}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 137823, "end": 138118}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 138118, "end": 138258}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 138258, "end": 138592}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 138592, "end": 139132}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 139132, "end": 139380}, {"filename": "/tests/metron_bad/README.md", "start": 139380, "end": 139577}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 139577, "end": 139888}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 139888, "end": 140395}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 140395, "end": 140667}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 140667, "end": 141262}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 141262, "end": 141522}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141522, "end": 141765}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141765, "end": 142061}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 142061, "end": 142507}], "remote_package_size": 142507, "package_uuid": "af1b990f-c5e4-4adc-8bbf-b3f042f6e34e"});

  })();
