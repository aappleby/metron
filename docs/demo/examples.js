
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1263}, {"filename": "/examples/rvtiny/README.md", "start": 1263, "end": 1427}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 1427, "end": 7523}, {"filename": "/examples/rvsimple/README.md", "start": 7523, "end": 7602}, {"filename": "/examples/rvsimple/metron/register.h", "start": 7602, "end": 8287}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 8287, "end": 9440}, {"filename": "/examples/rvsimple/metron/config.h", "start": 9440, "end": 10655}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 10655, "end": 11419}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 11419, "end": 13912}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 13912, "end": 15760}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 15760, "end": 16436}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 16436, "end": 21163}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 21163, "end": 22277}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 22277, "end": 23248}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 23248, "end": 24482}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 24482, "end": 26420}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 26420, "end": 26920}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 26920, "end": 29963}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 29963, "end": 35557}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 35557, "end": 36567}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 36567, "end": 37394}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 37394, "end": 40015}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 40015, "end": 41476}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 41476, "end": 47195}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 47195, "end": 48068}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 48068, "end": 50186}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 50186, "end": 50877}, {"filename": "/examples/pong/README.md", "start": 50877, "end": 50937}, {"filename": "/examples/pong/metron/pong.h", "start": 50937, "end": 54801}, {"filename": "/examples/pong/reference/README.md", "start": 54801, "end": 54861}, {"filename": "/examples/ibex/README.md", "start": 54861, "end": 54912}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 54912, "end": 69322}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 69322, "end": 81420}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 81420, "end": 83005}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 83005, "end": 99029}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 99029, "end": 101530}, {"filename": "/examples/rvtiny_sync/README.md", "start": 101530, "end": 101582}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 101582, "end": 108537}, {"filename": "/examples/tutorial/tutorial5.h", "start": 108537, "end": 108578}, {"filename": "/examples/tutorial/vga.h", "start": 108578, "end": 109725}, {"filename": "/examples/tutorial/declaration_order.h", "start": 109725, "end": 110504}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 110504, "end": 110889}, {"filename": "/examples/tutorial/tutorial3.h", "start": 110889, "end": 110930}, {"filename": "/examples/tutorial/checksum.h", "start": 110930, "end": 111268}, {"filename": "/examples/tutorial/submodules.h", "start": 111268, "end": 112455}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 112455, "end": 113873}, {"filename": "/examples/tutorial/adder.h", "start": 113873, "end": 114053}, {"filename": "/examples/tutorial/tutorial4.h", "start": 114053, "end": 114094}, {"filename": "/examples/tutorial/blockram.h", "start": 114094, "end": 114473}, {"filename": "/examples/tutorial/tutorial2.h", "start": 114473, "end": 114541}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 114541, "end": 115522}, {"filename": "/examples/tutorial/counter.h", "start": 115522, "end": 115671}, {"filename": "/examples/tutorial/nonblocking.h", "start": 115671, "end": 115903}, {"filename": "/examples/uart/README.md", "start": 115903, "end": 116147}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 116147, "end": 117440}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 117440, "end": 118803}, {"filename": "/examples/uart/metron/uart_top.h", "start": 118803, "end": 119863}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 119863, "end": 121709}, {"filename": "/tests/metron_good/include_guards.h", "start": 121709, "end": 121906}, {"filename": "/tests/metron_good/oneliners.h", "start": 121906, "end": 122169}, {"filename": "/tests/metron_good/enum_simple.h", "start": 122169, "end": 123537}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 123537, "end": 123702}, {"filename": "/tests/metron_good/basic_increment.h", "start": 123702, "end": 124057}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 124057, "end": 124895}, {"filename": "/tests/metron_good/magic_comments.h", "start": 124895, "end": 125198}, {"filename": "/tests/metron_good/for_loops.h", "start": 125198, "end": 125518}, {"filename": "/tests/metron_good/defines.h", "start": 125518, "end": 125832}, {"filename": "/tests/metron_good/bit_concat.h", "start": 125832, "end": 126259}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 126259, "end": 126439}, {"filename": "/tests/metron_good/force_tick.h", "start": 126439, "end": 126621}, {"filename": "/tests/metron_good/README.md", "start": 126621, "end": 126702}, {"filename": "/tests/metron_good/good_order.h", "start": 126702, "end": 126998}, {"filename": "/tests/metron_good/basic_template.h", "start": 126998, "end": 127429}, {"filename": "/tests/metron_good/bit_casts.h", "start": 127429, "end": 133402}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 133402, "end": 133674}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 133674, "end": 133981}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 133981, "end": 134640}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 134640, "end": 134995}, {"filename": "/tests/metron_good/tock_task.h", "start": 134995, "end": 135351}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 135351, "end": 135571}, {"filename": "/tests/metron_good/all_func_types.h", "start": 135571, "end": 137136}, {"filename": "/tests/metron_good/basic_submod.h", "start": 137136, "end": 137425}, {"filename": "/tests/metron_good/dontcare.h", "start": 137425, "end": 137708}, {"filename": "/tests/metron_good/basic_param.h", "start": 137708, "end": 137967}, {"filename": "/tests/metron_good/basic_output.h", "start": 137967, "end": 138228}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 138228, "end": 138982}, {"filename": "/tests/metron_good/basic_function.h", "start": 138982, "end": 139261}, {"filename": "/tests/metron_good/builtins.h", "start": 139261, "end": 139592}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 139592, "end": 139838}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 139838, "end": 140245}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 140245, "end": 140476}, {"filename": "/tests/metron_good/basic_literals.h", "start": 140476, "end": 141088}, {"filename": "/tests/metron_good/basic_switch.h", "start": 141088, "end": 141565}, {"filename": "/tests/metron_good/bit_dup.h", "start": 141565, "end": 142224}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 142224, "end": 142883}, {"filename": "/tests/metron_good/local_localparam.h", "start": 142883, "end": 143061}, {"filename": "/tests/metron_good/minimal.h", "start": 143061, "end": 143136}, {"filename": "/tests/metron_good/multi_tick.h", "start": 143136, "end": 143502}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 143502, "end": 143765}, {"filename": "/tests/metron_good/namespaces.h", "start": 143765, "end": 144115}, {"filename": "/tests/metron_good/structs.h", "start": 144115, "end": 144334}, {"filename": "/tests/metron_good/basic_task.h", "start": 144334, "end": 144668}, {"filename": "/tests/metron_good/nested_structs.h", "start": 144668, "end": 145181}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 145181, "end": 145495}, {"filename": "/tests/metron_good/private_getter.h", "start": 145495, "end": 145719}, {"filename": "/tests/metron_good/init_chain.h", "start": 145719, "end": 146427}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 146427, "end": 146803}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 146803, "end": 147214}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 147214, "end": 147768}, {"filename": "/tests/metron_good/input_signals.h", "start": 147768, "end": 148430}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 148430, "end": 148975}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 148975, "end": 149134}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 149134, "end": 149429}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 149429, "end": 149569}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 149569, "end": 149979}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 149979, "end": 150519}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 150519, "end": 150767}, {"filename": "/tests/metron_bad/README.md", "start": 150767, "end": 150964}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 150964, "end": 151275}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 151275, "end": 151782}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 151782, "end": 152377}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 152377, "end": 152637}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 152637, "end": 152880}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 152880, "end": 153176}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 153176, "end": 153622}], "remote_package_size": 153622, "package_uuid": "74427139-0b72-42d8-8450-84517a5d290e"});

  })();
