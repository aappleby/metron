
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 287}, {"filename": "/examples/rvtiny/README.md", "start": 287, "end": 451}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 451, "end": 6547}, {"filename": "/examples/rvsimple/README.md", "start": 6547, "end": 6626}, {"filename": "/examples/rvsimple/metron/register.h", "start": 6626, "end": 7311}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7311, "end": 8418}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8418, "end": 9633}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 9633, "end": 10397}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10397, "end": 12890}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 12890, "end": 14738}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 14738, "end": 15414}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15414, "end": 20141}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20141, "end": 21255}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21255, "end": 22226}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22226, "end": 23460}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23460, "end": 25398}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25398, "end": 25898}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 25898, "end": 28941}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 28941, "end": 34535}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 34535, "end": 35545}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 35545, "end": 36372}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36372, "end": 38993}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 38993, "end": 40454}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40454, "end": 46173}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46173, "end": 47046}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 47046, "end": 49164}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49164, "end": 49855}, {"filename": "/examples/pong/README.md", "start": 49855, "end": 49915}, {"filename": "/examples/pong/metron/pong.h", "start": 49915, "end": 53779}, {"filename": "/examples/pong/reference/README.md", "start": 53779, "end": 53839}, {"filename": "/examples/ibex/README.md", "start": 53839, "end": 53890}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 53890, "end": 68300}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68300, "end": 80398}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80398, "end": 81983}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 81983, "end": 98007}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 98007, "end": 100508}, {"filename": "/examples/rvtiny_sync/README.md", "start": 100508, "end": 100560}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 100560, "end": 107515}, {"filename": "/examples/uart/README.md", "start": 107515, "end": 107759}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 107759, "end": 109052}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109052, "end": 110415}, {"filename": "/examples/uart/metron/uart_top.h", "start": 110415, "end": 111475}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 111475, "end": 113321}, {"filename": "/tests/metron_good/include_guards.h", "start": 113321, "end": 113518}, {"filename": "/tests/metron_good/oneliners.h", "start": 113518, "end": 113781}, {"filename": "/tests/metron_good/enum_simple.h", "start": 113781, "end": 115149}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115149, "end": 115314}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115314, "end": 115669}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 115669, "end": 116507}, {"filename": "/tests/metron_good/magic_comments.h", "start": 116507, "end": 116810}, {"filename": "/tests/metron_good/defines.h", "start": 116810, "end": 117124}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117124, "end": 117551}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 117551, "end": 117731}, {"filename": "/tests/metron_good/force_tick.h", "start": 117731, "end": 117913}, {"filename": "/tests/metron_good/README.md", "start": 117913, "end": 117994}, {"filename": "/tests/metron_good/good_order.h", "start": 117994, "end": 118290}, {"filename": "/tests/metron_good/basic_template.h", "start": 118290, "end": 118721}, {"filename": "/tests/metron_good/bit_casts.h", "start": 118721, "end": 124694}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 124694, "end": 124966}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 124966, "end": 125273}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 125273, "end": 125932}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 125932, "end": 126287}, {"filename": "/tests/metron_good/tock_task.h", "start": 126287, "end": 126643}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 126643, "end": 126863}, {"filename": "/tests/metron_good/all_func_types.h", "start": 126863, "end": 128428}, {"filename": "/tests/metron_good/basic_submod.h", "start": 128428, "end": 128717}, {"filename": "/tests/metron_good/dontcare.h", "start": 128717, "end": 129000}, {"filename": "/tests/metron_good/basic_param.h", "start": 129000, "end": 129259}, {"filename": "/tests/metron_good/basic_output.h", "start": 129259, "end": 129520}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 129520, "end": 130274}, {"filename": "/tests/metron_good/basic_function.h", "start": 130274, "end": 130553}, {"filename": "/tests/metron_good/builtins.h", "start": 130553, "end": 130884}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 130884, "end": 131130}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 131130, "end": 131537}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 131537, "end": 131768}, {"filename": "/tests/metron_good/basic_literals.h", "start": 131768, "end": 132380}, {"filename": "/tests/metron_good/basic_switch.h", "start": 132380, "end": 132857}, {"filename": "/tests/metron_good/bit_dup.h", "start": 132857, "end": 133516}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 133516, "end": 134175}, {"filename": "/tests/metron_good/local_localparam.h", "start": 134175, "end": 134353}, {"filename": "/tests/metron_good/minimal.h", "start": 134353, "end": 134428}, {"filename": "/tests/metron_good/multi_tick.h", "start": 134428, "end": 134794}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 134794, "end": 135057}, {"filename": "/tests/metron_good/namespaces.h", "start": 135057, "end": 135407}, {"filename": "/tests/metron_good/structs.h", "start": 135407, "end": 135626}, {"filename": "/tests/metron_good/basic_task.h", "start": 135626, "end": 135960}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 135960, "end": 136274}, {"filename": "/tests/metron_good/private_getter.h", "start": 136274, "end": 136498}, {"filename": "/tests/metron_good/init_chain.h", "start": 136498, "end": 137206}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 137206, "end": 137582}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 137582, "end": 137993}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 137993, "end": 138547}, {"filename": "/tests/metron_good/input_signals.h", "start": 138547, "end": 139209}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 139209, "end": 139754}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 139754, "end": 139913}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 139913, "end": 140208}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 140208, "end": 140348}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 140348, "end": 140759}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 140759, "end": 141299}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141299, "end": 141547}, {"filename": "/tests/metron_bad/README.md", "start": 141547, "end": 141744}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 141744, "end": 142055}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 142055, "end": 142562}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142562, "end": 143157}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 143157, "end": 143417}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 143417, "end": 143660}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 143660, "end": 143956}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 143956, "end": 144402}], "remote_package_size": 144402, "package_uuid": "0e56222c-75e4-4236-8cf1-402760c33459"});

  })();
