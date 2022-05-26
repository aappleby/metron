
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1263}, {"filename": "/examples/rvtiny/README.md", "start": 1263, "end": 1427}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 1427, "end": 7523}, {"filename": "/examples/rvsimple/README.md", "start": 7523, "end": 7602}, {"filename": "/examples/rvsimple/metron/register.h", "start": 7602, "end": 8287}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 8287, "end": 9440}, {"filename": "/examples/rvsimple/metron/config.h", "start": 9440, "end": 10655}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 10655, "end": 11419}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 11419, "end": 13912}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 13912, "end": 15760}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 15760, "end": 16436}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 16436, "end": 21163}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 21163, "end": 22277}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 22277, "end": 23248}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 23248, "end": 24482}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 24482, "end": 26420}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 26420, "end": 26920}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 26920, "end": 29963}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 29963, "end": 35557}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 35557, "end": 36567}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 36567, "end": 37394}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 37394, "end": 40015}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 40015, "end": 41476}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 41476, "end": 47195}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 47195, "end": 48068}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 48068, "end": 50186}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 50186, "end": 50877}, {"filename": "/examples/pong/README.md", "start": 50877, "end": 50937}, {"filename": "/examples/pong/metron/pong.h", "start": 50937, "end": 54801}, {"filename": "/examples/pong/reference/README.md", "start": 54801, "end": 54861}, {"filename": "/examples/ibex/README.md", "start": 54861, "end": 54912}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 54912, "end": 69322}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 69322, "end": 81420}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 81420, "end": 83005}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 83005, "end": 99029}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 99029, "end": 101530}, {"filename": "/examples/rvtiny_sync/README.md", "start": 101530, "end": 101582}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 101582, "end": 108537}, {"filename": "/examples/tutorial/tutorial5.h", "start": 108537, "end": 108578}, {"filename": "/examples/tutorial/vga.h", "start": 108578, "end": 109725}, {"filename": "/examples/tutorial/declaration_order.h", "start": 109725, "end": 110504}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 110504, "end": 111040}, {"filename": "/examples/tutorial/tutorial3.h", "start": 111040, "end": 111081}, {"filename": "/examples/tutorial/checksum.h", "start": 111081, "end": 112274}, {"filename": "/examples/tutorial/submodules.h", "start": 112274, "end": 113391}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 113391, "end": 114809}, {"filename": "/examples/tutorial/adder.h", "start": 114809, "end": 114989}, {"filename": "/examples/tutorial/tutorial4.h", "start": 114989, "end": 115030}, {"filename": "/examples/tutorial/tutorial2.h", "start": 115030, "end": 115098}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 115098, "end": 116079}, {"filename": "/examples/tutorial/counter.h", "start": 116079, "end": 116228}, {"filename": "/examples/tutorial/nonblocking.h", "start": 116228, "end": 116350}, {"filename": "/examples/tutorial/templates.h", "start": 116350, "end": 116821}, {"filename": "/examples/uart/README.md", "start": 116821, "end": 117065}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 117065, "end": 118358}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 118358, "end": 119721}, {"filename": "/examples/uart/metron/uart_top.h", "start": 119721, "end": 120781}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 120781, "end": 122627}, {"filename": "/tests/metron_good/include_guards.h", "start": 122627, "end": 122824}, {"filename": "/tests/metron_good/oneliners.h", "start": 122824, "end": 123087}, {"filename": "/tests/metron_good/enum_simple.h", "start": 123087, "end": 124455}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 124455, "end": 124620}, {"filename": "/tests/metron_good/basic_increment.h", "start": 124620, "end": 124975}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 124975, "end": 125813}, {"filename": "/tests/metron_good/magic_comments.h", "start": 125813, "end": 126116}, {"filename": "/tests/metron_good/for_loops.h", "start": 126116, "end": 126436}, {"filename": "/tests/metron_good/defines.h", "start": 126436, "end": 126750}, {"filename": "/tests/metron_good/bit_concat.h", "start": 126750, "end": 127177}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 127177, "end": 127357}, {"filename": "/tests/metron_good/README.md", "start": 127357, "end": 127438}, {"filename": "/tests/metron_good/good_order.h", "start": 127438, "end": 127734}, {"filename": "/tests/metron_good/basic_template.h", "start": 127734, "end": 128165}, {"filename": "/tests/metron_good/bit_casts.h", "start": 128165, "end": 134138}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 134138, "end": 134445}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 134445, "end": 135104}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 135104, "end": 135459}, {"filename": "/tests/metron_good/tock_task.h", "start": 135459, "end": 135815}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 135815, "end": 136035}, {"filename": "/tests/metron_good/all_func_types.h", "start": 136035, "end": 137600}, {"filename": "/tests/metron_good/basic_submod.h", "start": 137600, "end": 137889}, {"filename": "/tests/metron_good/dontcare.h", "start": 137889, "end": 138172}, {"filename": "/tests/metron_good/basic_param.h", "start": 138172, "end": 138431}, {"filename": "/tests/metron_good/basic_output.h", "start": 138431, "end": 138692}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 138692, "end": 139446}, {"filename": "/tests/metron_good/basic_function.h", "start": 139446, "end": 139725}, {"filename": "/tests/metron_good/builtins.h", "start": 139725, "end": 140056}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 140056, "end": 140302}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 140302, "end": 140709}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 140709, "end": 140940}, {"filename": "/tests/metron_good/basic_literals.h", "start": 140940, "end": 141552}, {"filename": "/tests/metron_good/basic_switch.h", "start": 141552, "end": 142029}, {"filename": "/tests/metron_good/bit_dup.h", "start": 142029, "end": 142688}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 142688, "end": 143347}, {"filename": "/tests/metron_good/local_localparam.h", "start": 143347, "end": 143525}, {"filename": "/tests/metron_good/minimal.h", "start": 143525, "end": 143600}, {"filename": "/tests/metron_good/multi_tick.h", "start": 143600, "end": 143966}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 143966, "end": 144229}, {"filename": "/tests/metron_good/namespaces.h", "start": 144229, "end": 144579}, {"filename": "/tests/metron_good/structs.h", "start": 144579, "end": 144798}, {"filename": "/tests/metron_good/basic_task.h", "start": 144798, "end": 145132}, {"filename": "/tests/metron_good/nested_structs.h", "start": 145132, "end": 145645}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 145645, "end": 145959}, {"filename": "/tests/metron_good/private_getter.h", "start": 145959, "end": 146183}, {"filename": "/tests/metron_good/init_chain.h", "start": 146183, "end": 146891}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 146891, "end": 147267}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 147267, "end": 147678}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 147678, "end": 148232}, {"filename": "/tests/metron_good/input_signals.h", "start": 148232, "end": 148894}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 148894, "end": 149439}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 149439, "end": 149598}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 149598, "end": 149893}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 149893, "end": 150033}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 150033, "end": 150443}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 150443, "end": 150983}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 150983, "end": 151231}, {"filename": "/tests/metron_bad/README.md", "start": 151231, "end": 151428}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 151428, "end": 151739}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 151739, "end": 152246}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 152246, "end": 152518}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 152518, "end": 153113}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 153113, "end": 153373}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 153373, "end": 153616}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 153616, "end": 153912}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 153912, "end": 154358}], "remote_package_size": 154358, "package_uuid": "515ba7cd-312d-4391-837c-9cc6e5df011d"});

  })();
