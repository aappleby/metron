
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
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "rvtiny", true, true);
Module['FS_createPath']("/examples/rvtiny", "metron", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/examples/uart", "metron", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "rvtiny_sync", true, true);
Module['FS_createPath']("/examples/rvtiny_sync", "metron", true, true);
Module['FS_createPath']("/examples", "gb_spu", true, true);
Module['FS_createPath']("/examples/gb_spu", "metron", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples/j1", "metron", true, true);
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1263}, {"filename": "/examples/ibex/README.md", "start": 1263, "end": 1314}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 1314, "end": 17338}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 17338, "end": 18923}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 18923, "end": 31021}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 31021, "end": 33522}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33522, "end": 47932}, {"filename": "/examples/rvsimple/README.md", "start": 47932, "end": 48011}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 48011, "end": 49125}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 49125, "end": 49889}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 49889, "end": 52382}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 52382, "end": 53392}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 53392, "end": 54219}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 54219, "end": 54910}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 54910, "end": 55586}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 55586, "end": 57704}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 57704, "end": 60325}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 60325, "end": 66044}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 66044, "end": 67505}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 67505, "end": 68739}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 68739, "end": 70677}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 70677, "end": 72525}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 72525, "end": 73678}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 73678, "end": 79272}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 79272, "end": 83999}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 83999, "end": 84499}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 84499, "end": 85372}, {"filename": "/examples/rvsimple/metron/register.h", "start": 85372, "end": 86057}, {"filename": "/examples/rvsimple/metron/config.h", "start": 86057, "end": 87272}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 87272, "end": 88243}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 88243, "end": 91286}, {"filename": "/examples/pong/README.md", "start": 91286, "end": 91346}, {"filename": "/examples/pong/metron/pong.h", "start": 91346, "end": 95210}, {"filename": "/examples/pong/reference/README.md", "start": 95210, "end": 95270}, {"filename": "/examples/rvtiny/README.md", "start": 95270, "end": 95434}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 95434, "end": 101530}, {"filename": "/examples/uart/README.md", "start": 101530, "end": 101774}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 101774, "end": 104793}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 104793, "end": 107354}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 107354, "end": 109934}, {"filename": "/examples/uart/metron/uart_top.h", "start": 109934, "end": 111698}, {"filename": "/examples/tutorial/nonblocking.h", "start": 111698, "end": 111820}, {"filename": "/examples/tutorial/checksum.h", "start": 111820, "end": 112543}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 112543, "end": 113524}, {"filename": "/examples/tutorial/tutorial5.h", "start": 113524, "end": 113565}, {"filename": "/examples/tutorial/tutorial2.h", "start": 113565, "end": 113633}, {"filename": "/examples/tutorial/vga.h", "start": 113633, "end": 114780}, {"filename": "/examples/tutorial/templates.h", "start": 114780, "end": 115251}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 115251, "end": 116669}, {"filename": "/examples/tutorial/tutorial4.h", "start": 116669, "end": 116710}, {"filename": "/examples/tutorial/submodules.h", "start": 116710, "end": 117827}, {"filename": "/examples/tutorial/blockram.h", "start": 117827, "end": 118344}, {"filename": "/examples/tutorial/tutorial3.h", "start": 118344, "end": 118385}, {"filename": "/examples/tutorial/declaration_order.h", "start": 118385, "end": 119164}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 119164, "end": 119700}, {"filename": "/examples/tutorial/adder.h", "start": 119700, "end": 119880}, {"filename": "/examples/tutorial/counter.h", "start": 119880, "end": 120029}, {"filename": "/examples/rvtiny_sync/README.md", "start": 120029, "end": 120081}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 120081, "end": 127036}, {"filename": "/examples/gb_spu/regs.h", "start": 127036, "end": 146967}, {"filename": "/examples/gb_spu/Sch_Channel4.h", "start": 146967, "end": 152323}, {"filename": "/examples/gb_spu/Sch_Channel3.h", "start": 152323, "end": 152570}, {"filename": "/examples/gb_spu/Sch_Channel1.h", "start": 152570, "end": 157909}, {"filename": "/examples/gb_spu/gates.h", "start": 157909, "end": 161309}, {"filename": "/examples/gb_spu/Sch_Channel2.h", "start": 161309, "end": 161651}, {"filename": "/examples/gb_spu/Sch_ApuControl.h", "start": 161651, "end": 162118}, {"filename": "/examples/gb_spu/metron/gb_spu.h", "start": 162118, "end": 182909}, {"filename": "/examples/j1/metron/j1.h", "start": 182909, "end": 186934}, {"filename": "/examples/j1/metron/dpram.h", "start": 186934, "end": 187372}, {"filename": "/tests/metron_good/all_func_types.h", "start": 187372, "end": 188937}, {"filename": "/tests/metron_good/tock_task.h", "start": 188937, "end": 189293}, {"filename": "/tests/metron_good/namespaces.h", "start": 189293, "end": 189643}, {"filename": "/tests/metron_good/private_getter.h", "start": 189643, "end": 189867}, {"filename": "/tests/metron_good/defines.h", "start": 189867, "end": 190181}, {"filename": "/tests/metron_good/minimal.h", "start": 190181, "end": 190256}, {"filename": "/tests/metron_good/basic_increment.h", "start": 190256, "end": 190611}, {"filename": "/tests/metron_good/basic_submod.h", "start": 190611, "end": 190900}, {"filename": "/tests/metron_good/bit_dup.h", "start": 190900, "end": 191559}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 191559, "end": 191935}, {"filename": "/tests/metron_good/magic_comments.h", "start": 191935, "end": 192238}, {"filename": "/tests/metron_good/local_localparam.h", "start": 192238, "end": 192416}, {"filename": "/tests/metron_good/include_guards.h", "start": 192416, "end": 192613}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 192613, "end": 193272}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 193272, "end": 193437}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 193437, "end": 193848}, {"filename": "/tests/metron_good/README.md", "start": 193848, "end": 193929}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 193929, "end": 194474}, {"filename": "/tests/metron_good/basic_function.h", "start": 194474, "end": 194753}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 194753, "end": 195060}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 195060, "end": 195355}, {"filename": "/tests/metron_good/input_signals.h", "start": 195355, "end": 196017}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 196017, "end": 196372}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 196372, "end": 196635}, {"filename": "/tests/metron_good/multi_tick.h", "start": 196635, "end": 197001}, {"filename": "/tests/metron_good/for_loops.h", "start": 197001, "end": 197321}, {"filename": "/tests/metron_good/bit_casts.h", "start": 197321, "end": 203294}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 203294, "end": 203848}, {"filename": "/tests/metron_good/init_chain.h", "start": 203848, "end": 204556}, {"filename": "/tests/metron_good/oneliners.h", "start": 204556, "end": 204819}, {"filename": "/tests/metron_good/plus_equals.h", "start": 204819, "end": 205169}, {"filename": "/tests/metron_good/basic_task.h", "start": 205169, "end": 205503}, {"filename": "/tests/metron_good/basic_output.h", "start": 205503, "end": 205764}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 205764, "end": 206518}, {"filename": "/tests/metron_good/basic_param.h", "start": 206518, "end": 206777}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 206777, "end": 206936}, {"filename": "/tests/metron_good/basic_switch.h", "start": 206936, "end": 207413}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 207413, "end": 207593}, {"filename": "/tests/metron_good/good_order.h", "start": 207593, "end": 207889}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 207889, "end": 208548}, {"filename": "/tests/metron_good/enum_simple.h", "start": 208548, "end": 209916}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 209916, "end": 210162}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 210162, "end": 211000}, {"filename": "/tests/metron_good/builtins.h", "start": 211000, "end": 211331}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 211331, "end": 211551}, {"filename": "/tests/metron_good/bit_concat.h", "start": 211551, "end": 211978}, {"filename": "/tests/metron_good/basic_literals.h", "start": 211978, "end": 212590}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 212590, "end": 212997}, {"filename": "/tests/metron_good/dontcare.h", "start": 212997, "end": 213280}, {"filename": "/tests/metron_good/basic_template.h", "start": 213280, "end": 213711}, {"filename": "/tests/metron_good/structs.h", "start": 213711, "end": 213930}, {"filename": "/tests/metron_good/nested_structs.h", "start": 213930, "end": 214443}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 214443, "end": 214757}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 214757, "end": 214988}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 214988, "end": 215231}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 215231, "end": 215826}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 215826, "end": 216098}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 216098, "end": 216346}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 216346, "end": 216756}, {"filename": "/tests/metron_bad/README.md", "start": 216756, "end": 216953}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 216953, "end": 217213}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 217213, "end": 217353}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 217353, "end": 217664}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 217664, "end": 218204}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 218204, "end": 218650}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 218650, "end": 218946}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 218946, "end": 219453}], "remote_package_size": 219453, "package_uuid": "8ab9e5df-5bd3-4781-87b4-969e4010b7f2"});

  })();
