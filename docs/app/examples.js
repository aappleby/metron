
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 298}, {"filename": "/examples/rvtiny/README.md", "start": 298, "end": 462}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 462, "end": 6558}, {"filename": "/examples/rvsimple/README.md", "start": 6558, "end": 6637}, {"filename": "/examples/rvsimple/metron/register.h", "start": 6637, "end": 7322}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7322, "end": 8429}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8429, "end": 9644}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 9644, "end": 10408}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10408, "end": 12901}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 12901, "end": 14749}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 14749, "end": 15425}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15425, "end": 20152}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20152, "end": 21266}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21266, "end": 22237}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22237, "end": 23471}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23471, "end": 25409}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25409, "end": 25909}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 25909, "end": 28952}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 28952, "end": 34546}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 34546, "end": 35556}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 35556, "end": 36383}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36383, "end": 39004}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 39004, "end": 40465}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40465, "end": 46184}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46184, "end": 47057}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 47057, "end": 49175}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49175, "end": 49866}, {"filename": "/examples/pong/README.md", "start": 49866, "end": 49926}, {"filename": "/examples/pong/metron/pong.h", "start": 49926, "end": 53790}, {"filename": "/examples/pong/reference/README.md", "start": 53790, "end": 53850}, {"filename": "/examples/ibex/README.md", "start": 53850, "end": 53901}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 53901, "end": 68311}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68311, "end": 80409}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80409, "end": 81994}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 81994, "end": 98018}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 98018, "end": 100519}, {"filename": "/examples/rvtiny_sync/README.md", "start": 100519, "end": 100571}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 100571, "end": 107526}, {"filename": "/examples/uart/README.md", "start": 107526, "end": 107770}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 107770, "end": 109063}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109063, "end": 110426}, {"filename": "/examples/uart/metron/uart_top.h", "start": 110426, "end": 111486}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 111486, "end": 113332}, {"filename": "/tests/metron_good/include_guards.h", "start": 113332, "end": 113529}, {"filename": "/tests/metron_good/oneliners.h", "start": 113529, "end": 113792}, {"filename": "/tests/metron_good/enum_simple.h", "start": 113792, "end": 115160}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115160, "end": 115325}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115325, "end": 115680}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 115680, "end": 116518}, {"filename": "/tests/metron_good/magic_comments.h", "start": 116518, "end": 116821}, {"filename": "/tests/metron_good/defines.h", "start": 116821, "end": 117135}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117135, "end": 117562}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 117562, "end": 117742}, {"filename": "/tests/metron_good/README.md", "start": 117742, "end": 117823}, {"filename": "/tests/metron_good/good_order.h", "start": 117823, "end": 118119}, {"filename": "/tests/metron_good/basic_template.h", "start": 118119, "end": 118550}, {"filename": "/tests/metron_good/bit_casts.h", "start": 118550, "end": 124523}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 124523, "end": 124830}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 124830, "end": 125489}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 125489, "end": 125844}, {"filename": "/tests/metron_good/tock_task.h", "start": 125844, "end": 126200}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 126200, "end": 126420}, {"filename": "/tests/metron_good/all_func_types.h", "start": 126420, "end": 127985}, {"filename": "/tests/metron_good/basic_submod.h", "start": 127985, "end": 128274}, {"filename": "/tests/metron_good/dontcare.h", "start": 128274, "end": 128557}, {"filename": "/tests/metron_good/basic_param.h", "start": 128557, "end": 128816}, {"filename": "/tests/metron_good/basic_output.h", "start": 128816, "end": 129077}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 129077, "end": 129831}, {"filename": "/tests/metron_good/basic_function.h", "start": 129831, "end": 130110}, {"filename": "/tests/metron_good/builtins.h", "start": 130110, "end": 130441}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 130441, "end": 130687}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 130687, "end": 131094}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 131094, "end": 131325}, {"filename": "/tests/metron_good/basic_literals.h", "start": 131325, "end": 131937}, {"filename": "/tests/metron_good/basic_switch.h", "start": 131937, "end": 132414}, {"filename": "/tests/metron_good/bit_dup.h", "start": 132414, "end": 133073}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 133073, "end": 133732}, {"filename": "/tests/metron_good/local_localparam.h", "start": 133732, "end": 133910}, {"filename": "/tests/metron_good/minimal.h", "start": 133910, "end": 133985}, {"filename": "/tests/metron_good/multi_tick.h", "start": 133985, "end": 134351}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 134351, "end": 134614}, {"filename": "/tests/metron_good/namespaces.h", "start": 134614, "end": 134964}, {"filename": "/tests/metron_good/structs.h", "start": 134964, "end": 135183}, {"filename": "/tests/metron_good/basic_task.h", "start": 135183, "end": 135517}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 135517, "end": 135831}, {"filename": "/tests/metron_good/private_getter.h", "start": 135831, "end": 136055}, {"filename": "/tests/metron_good/init_chain.h", "start": 136055, "end": 136763}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 136763, "end": 137139}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 137139, "end": 137550}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 137550, "end": 138104}, {"filename": "/tests/metron_good/input_signals.h", "start": 138104, "end": 138766}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 138766, "end": 139311}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 139311, "end": 139470}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 139470, "end": 139765}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 139765, "end": 139905}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 139905, "end": 140316}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 140316, "end": 140856}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 140856, "end": 141104}, {"filename": "/tests/metron_bad/README.md", "start": 141104, "end": 141301}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 141301, "end": 141612}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 141612, "end": 142119}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 142119, "end": 142391}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142391, "end": 142986}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 142986, "end": 143246}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 143246, "end": 143489}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 143489, "end": 143785}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 143785, "end": 144231}], "remote_package_size": 144231, "package_uuid": "1dee5113-b1be-451d-8bd0-32ae455a3abf"});

  })();
