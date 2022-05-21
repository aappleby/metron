
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1439}, {"filename": "/examples/rvtiny/README.md", "start": 1439, "end": 1603}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 1603, "end": 7693}, {"filename": "/examples/rvsimple/README.md", "start": 7693, "end": 7772}, {"filename": "/examples/rvsimple/metron/register.h", "start": 7772, "end": 8457}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 8457, "end": 9564}, {"filename": "/examples/rvsimple/metron/config.h", "start": 9564, "end": 10779}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 10779, "end": 11543}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 11543, "end": 14036}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 14036, "end": 15884}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 15884, "end": 16560}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 16560, "end": 21287}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 21287, "end": 22401}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 22401, "end": 23372}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 23372, "end": 24606}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 24606, "end": 26544}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 26544, "end": 27044}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 27044, "end": 30087}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 30087, "end": 35681}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 35681, "end": 36691}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 36691, "end": 37518}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 37518, "end": 40139}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 40139, "end": 41600}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 41600, "end": 47319}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 47319, "end": 48192}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 48192, "end": 50310}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 50310, "end": 51001}, {"filename": "/examples/pong/README.md", "start": 51001, "end": 51061}, {"filename": "/examples/pong/metron/pong.h", "start": 51061, "end": 54925}, {"filename": "/examples/pong/reference/README.md", "start": 54925, "end": 54985}, {"filename": "/examples/ibex/README.md", "start": 54985, "end": 55036}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 55036, "end": 69446}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 69446, "end": 81544}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 81544, "end": 83129}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 83129, "end": 99153}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 99153, "end": 101654}, {"filename": "/examples/rvtiny_sync/README.md", "start": 101654, "end": 101706}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 101706, "end": 108661}, {"filename": "/examples/uart/README.md", "start": 108661, "end": 108905}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 108905, "end": 110223}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 110223, "end": 111606}, {"filename": "/examples/uart/metron/uart_top.h", "start": 111606, "end": 112632}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 112632, "end": 114551}, {"filename": "/tests/metron_good/include_guards.h", "start": 114551, "end": 114748}, {"filename": "/tests/metron_good/oneliners.h", "start": 114748, "end": 115011}, {"filename": "/tests/metron_good/enum_simple.h", "start": 115011, "end": 116379}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 116379, "end": 116544}, {"filename": "/tests/metron_good/basic_increment.h", "start": 116544, "end": 116899}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 116899, "end": 117737}, {"filename": "/tests/metron_good/magic_comments.h", "start": 117737, "end": 118040}, {"filename": "/tests/metron_good/defines.h", "start": 118040, "end": 118354}, {"filename": "/tests/metron_good/bit_concat.h", "start": 118354, "end": 118781}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 118781, "end": 118961}, {"filename": "/tests/metron_good/README.md", "start": 118961, "end": 119042}, {"filename": "/tests/metron_good/good_order.h", "start": 119042, "end": 119338}, {"filename": "/tests/metron_good/basic_template.h", "start": 119338, "end": 119769}, {"filename": "/tests/metron_good/bit_casts.h", "start": 119769, "end": 125742}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 125742, "end": 126049}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 126049, "end": 126708}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 126708, "end": 127063}, {"filename": "/tests/metron_good/tock_task.h", "start": 127063, "end": 127419}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 127419, "end": 127639}, {"filename": "/tests/metron_good/all_func_types.h", "start": 127639, "end": 128435}, {"filename": "/tests/metron_good/basic_submod.h", "start": 128435, "end": 128724}, {"filename": "/tests/metron_good/dontcare.h", "start": 128724, "end": 129007}, {"filename": "/tests/metron_good/basic_param.h", "start": 129007, "end": 129266}, {"filename": "/tests/metron_good/basic_output.h", "start": 129266, "end": 129527}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 129527, "end": 130281}, {"filename": "/tests/metron_good/basic_function.h", "start": 130281, "end": 130560}, {"filename": "/tests/metron_good/builtins.h", "start": 130560, "end": 130891}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 130891, "end": 131137}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 131137, "end": 131544}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 131544, "end": 131775}, {"filename": "/tests/metron_good/basic_literals.h", "start": 131775, "end": 132387}, {"filename": "/tests/metron_good/basic_switch.h", "start": 132387, "end": 132864}, {"filename": "/tests/metron_good/bit_dup.h", "start": 132864, "end": 133523}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 133523, "end": 134182}, {"filename": "/tests/metron_good/local_localparam.h", "start": 134182, "end": 134360}, {"filename": "/tests/metron_good/minimal.h", "start": 134360, "end": 134435}, {"filename": "/tests/metron_good/multi_tick.h", "start": 134435, "end": 134801}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 134801, "end": 135064}, {"filename": "/tests/metron_good/namespaces.h", "start": 135064, "end": 135414}, {"filename": "/tests/metron_good/structs.h", "start": 135414, "end": 135633}, {"filename": "/tests/metron_good/basic_task.h", "start": 135633, "end": 135967}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 135967, "end": 136281}, {"filename": "/tests/metron_good/private_getter.h", "start": 136281, "end": 136505}, {"filename": "/tests/metron_good/init_chain.h", "start": 136505, "end": 137213}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 137213, "end": 137589}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 137589, "end": 138000}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 138000, "end": 138554}, {"filename": "/tests/metron_good/input_signals.h", "start": 138554, "end": 139216}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 139216, "end": 139761}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 139761, "end": 139920}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 139920, "end": 140215}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 140215, "end": 140355}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 140355, "end": 140766}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 140766, "end": 141306}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141306, "end": 141554}, {"filename": "/tests/metron_bad/README.md", "start": 141554, "end": 141751}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 141751, "end": 142062}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 142062, "end": 142569}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 142569, "end": 142841}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142841, "end": 143436}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 143436, "end": 143696}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 143696, "end": 143939}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 143939, "end": 144235}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144235, "end": 144681}], "remote_package_size": 144681, "package_uuid": "03c64ba9-55ff-4c11-8a8a-c329f8989e5b"});

  })();
