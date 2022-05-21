
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 213}, {"filename": "/examples/rvtiny/README.md", "start": 213, "end": 377}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 377, "end": 6473}, {"filename": "/examples/rvsimple/README.md", "start": 6473, "end": 6552}, {"filename": "/examples/rvsimple/metron/register.h", "start": 6552, "end": 7237}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7237, "end": 8344}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8344, "end": 9559}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 9559, "end": 10323}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10323, "end": 12816}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 12816, "end": 14664}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 14664, "end": 15340}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15340, "end": 20067}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20067, "end": 21181}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21181, "end": 22152}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22152, "end": 23386}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23386, "end": 25324}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25324, "end": 25824}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 25824, "end": 28867}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 28867, "end": 34461}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 34461, "end": 35471}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 35471, "end": 36298}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36298, "end": 38919}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 38919, "end": 40380}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40380, "end": 46099}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46099, "end": 46972}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 46972, "end": 49090}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49090, "end": 49781}, {"filename": "/examples/pong/README.md", "start": 49781, "end": 49841}, {"filename": "/examples/pong/metron/pong.h", "start": 49841, "end": 53705}, {"filename": "/examples/pong/reference/README.md", "start": 53705, "end": 53765}, {"filename": "/examples/ibex/README.md", "start": 53765, "end": 53816}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 53816, "end": 68226}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68226, "end": 80324}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80324, "end": 81909}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 81909, "end": 97933}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 97933, "end": 100434}, {"filename": "/examples/rvtiny_sync/README.md", "start": 100434, "end": 100486}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 100486, "end": 107441}, {"filename": "/examples/uart/README.md", "start": 107441, "end": 107685}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 107685, "end": 109003}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109003, "end": 110386}, {"filename": "/examples/uart/metron/uart_top.h", "start": 110386, "end": 111412}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 111412, "end": 113331}, {"filename": "/tests/metron_good/include_guards.h", "start": 113331, "end": 113528}, {"filename": "/tests/metron_good/oneliners.h", "start": 113528, "end": 113791}, {"filename": "/tests/metron_good/enum_simple.h", "start": 113791, "end": 115159}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115159, "end": 115324}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115324, "end": 115679}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 115679, "end": 116517}, {"filename": "/tests/metron_good/magic_comments.h", "start": 116517, "end": 116820}, {"filename": "/tests/metron_good/defines.h", "start": 116820, "end": 117134}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117134, "end": 117561}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 117561, "end": 117741}, {"filename": "/tests/metron_good/README.md", "start": 117741, "end": 117822}, {"filename": "/tests/metron_good/good_order.h", "start": 117822, "end": 118118}, {"filename": "/tests/metron_good/basic_template.h", "start": 118118, "end": 118549}, {"filename": "/tests/metron_good/bit_casts.h", "start": 118549, "end": 124522}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 124522, "end": 124829}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 124829, "end": 125488}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 125488, "end": 125843}, {"filename": "/tests/metron_good/tock_task.h", "start": 125843, "end": 126199}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 126199, "end": 126419}, {"filename": "/tests/metron_good/all_func_types.h", "start": 126419, "end": 127984}, {"filename": "/tests/metron_good/basic_submod.h", "start": 127984, "end": 128273}, {"filename": "/tests/metron_good/dontcare.h", "start": 128273, "end": 128556}, {"filename": "/tests/metron_good/basic_param.h", "start": 128556, "end": 128815}, {"filename": "/tests/metron_good/basic_output.h", "start": 128815, "end": 129076}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 129076, "end": 129830}, {"filename": "/tests/metron_good/basic_function.h", "start": 129830, "end": 130109}, {"filename": "/tests/metron_good/builtins.h", "start": 130109, "end": 130440}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 130440, "end": 130686}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 130686, "end": 131093}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 131093, "end": 131324}, {"filename": "/tests/metron_good/basic_literals.h", "start": 131324, "end": 131936}, {"filename": "/tests/metron_good/basic_switch.h", "start": 131936, "end": 132413}, {"filename": "/tests/metron_good/bit_dup.h", "start": 132413, "end": 133072}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 133072, "end": 133731}, {"filename": "/tests/metron_good/local_localparam.h", "start": 133731, "end": 133909}, {"filename": "/tests/metron_good/minimal.h", "start": 133909, "end": 133984}, {"filename": "/tests/metron_good/multi_tick.h", "start": 133984, "end": 134350}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 134350, "end": 134613}, {"filename": "/tests/metron_good/namespaces.h", "start": 134613, "end": 134963}, {"filename": "/tests/metron_good/structs.h", "start": 134963, "end": 135182}, {"filename": "/tests/metron_good/basic_task.h", "start": 135182, "end": 135516}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 135516, "end": 135830}, {"filename": "/tests/metron_good/private_getter.h", "start": 135830, "end": 136054}, {"filename": "/tests/metron_good/init_chain.h", "start": 136054, "end": 136762}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 136762, "end": 137138}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 137138, "end": 137549}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 137549, "end": 138103}, {"filename": "/tests/metron_good/input_signals.h", "start": 138103, "end": 138765}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 138765, "end": 139310}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 139310, "end": 139469}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 139469, "end": 139764}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 139764, "end": 139904}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 139904, "end": 140315}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 140315, "end": 140855}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 140855, "end": 141103}, {"filename": "/tests/metron_bad/README.md", "start": 141103, "end": 141300}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 141300, "end": 141611}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 141611, "end": 142118}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 142118, "end": 142390}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142390, "end": 142985}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 142985, "end": 143245}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 143245, "end": 143488}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 143488, "end": 143784}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 143784, "end": 144230}], "remote_package_size": 144230, "package_uuid": "3e91edfc-423a-40d8-818a-a0de7fec060f"});

  })();
