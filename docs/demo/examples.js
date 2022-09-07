
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
Module['FS_createPath']("/examples", "gb_spu", true, true);
Module['FS_createPath']("/examples/gb_spu", "metron", true, true);
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples/j1", "metron", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
Module['FS_createPath']("/examples", "rvtiny", true, true);
Module['FS_createPath']("/examples/rvtiny", "metron", true, true);
Module['FS_createPath']("/examples", "rvtiny_sync", true, true);
Module['FS_createPath']("/examples/rvtiny_sync", "metron", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/examples/uart", "metron", true, true);
Module['FS_createPath']("/", "tests", true, true);
Module['FS_createPath']("/tests", "metron_bad", true, true);
Module['FS_createPath']("/tests", "metron_good", true, true);

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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22810}, {"filename": "/examples/ibex/README.md", "start": 22810, "end": 22861}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22861, "end": 24446}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24446, "end": 36544}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36544, "end": 50954}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50954, "end": 66978}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 66978, "end": 69479}, {"filename": "/examples/j1/metron/dpram.h", "start": 69479, "end": 69917}, {"filename": "/examples/j1/metron/j1.h", "start": 69917, "end": 73942}, {"filename": "/examples/pong/README.md", "start": 73942, "end": 74002}, {"filename": "/examples/pong/metron/pong.h", "start": 74002, "end": 77866}, {"filename": "/examples/pong/reference/README.md", "start": 77866, "end": 77926}, {"filename": "/examples/rvsimple/README.md", "start": 77926, "end": 78005}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78005, "end": 78505}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78505, "end": 79966}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 79966, "end": 82587}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82587, "end": 83802}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83802, "end": 89521}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89521, "end": 90635}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90635, "end": 92573}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92573, "end": 93807}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93807, "end": 94960}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 94960, "end": 95636}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95636, "end": 96509}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96509, "end": 98627}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 98627, "end": 99391}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99391, "end": 100082}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100082, "end": 100909}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 100909, "end": 101919}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 101919, "end": 102890}, {"filename": "/examples/rvsimple/metron/register.h", "start": 102890, "end": 103575}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 103575, "end": 106618}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 106618, "end": 112212}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112212, "end": 114705}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 114705, "end": 119432}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119432, "end": 121280}, {"filename": "/examples/rvtiny/README.md", "start": 121280, "end": 121444}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 121444, "end": 127540}, {"filename": "/examples/rvtiny_sync/README.md", "start": 127540, "end": 127592}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 127592, "end": 134547}, {"filename": "/examples/scratch.h", "start": 134547, "end": 134754}, {"filename": "/examples/tutorial/adder.h", "start": 134754, "end": 134934}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 134934, "end": 135915}, {"filename": "/examples/tutorial/blockram.h", "start": 135915, "end": 136432}, {"filename": "/examples/tutorial/checksum.h", "start": 136432, "end": 137155}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 137155, "end": 137691}, {"filename": "/examples/tutorial/counter.h", "start": 137691, "end": 137840}, {"filename": "/examples/tutorial/declaration_order.h", "start": 137840, "end": 138619}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 138619, "end": 140037}, {"filename": "/examples/tutorial/nonblocking.h", "start": 140037, "end": 140159}, {"filename": "/examples/tutorial/submodules.h", "start": 140159, "end": 141276}, {"filename": "/examples/tutorial/templates.h", "start": 141276, "end": 141747}, {"filename": "/examples/tutorial/tutorial2.h", "start": 141747, "end": 141815}, {"filename": "/examples/tutorial/tutorial3.h", "start": 141815, "end": 141856}, {"filename": "/examples/tutorial/tutorial4.h", "start": 141856, "end": 141897}, {"filename": "/examples/tutorial/tutorial5.h", "start": 141897, "end": 141938}, {"filename": "/examples/tutorial/vga.h", "start": 141938, "end": 143085}, {"filename": "/examples/uart/README.md", "start": 143085, "end": 143329}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 143329, "end": 145909}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 145909, "end": 148470}, {"filename": "/examples/uart/metron/uart_top.h", "start": 148470, "end": 150234}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 150234, "end": 153253}, {"filename": "/tests/metron_bad/README.md", "start": 153253, "end": 153450}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 153450, "end": 153746}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 153746, "end": 153994}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 153994, "end": 154237}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 154237, "end": 154832}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 154832, "end": 154972}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 154972, "end": 155382}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 155382, "end": 155922}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 155922, "end": 156233}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 156233, "end": 156679}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 156679, "end": 156939}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 156939, "end": 157211}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 157211, "end": 157718}, {"filename": "/tests/metron_good/README.md", "start": 157718, "end": 157799}, {"filename": "/tests/metron_good/all_func_types.h", "start": 157799, "end": 159364}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 159364, "end": 159771}, {"filename": "/tests/metron_good/basic_function.h", "start": 159771, "end": 160050}, {"filename": "/tests/metron_good/basic_increment.h", "start": 160050, "end": 160405}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 160405, "end": 160700}, {"filename": "/tests/metron_good/basic_literals.h", "start": 160700, "end": 161312}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 161312, "end": 161558}, {"filename": "/tests/metron_good/basic_output.h", "start": 161558, "end": 161819}, {"filename": "/tests/metron_good/basic_param.h", "start": 161819, "end": 162078}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 162078, "end": 162309}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 162309, "end": 162489}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 162489, "end": 162752}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 162752, "end": 162972}, {"filename": "/tests/metron_good/basic_submod.h", "start": 162972, "end": 163261}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 163261, "end": 163616}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 163616, "end": 163992}, {"filename": "/tests/metron_good/basic_switch.h", "start": 163992, "end": 164469}, {"filename": "/tests/metron_good/basic_task.h", "start": 164469, "end": 164803}, {"filename": "/tests/metron_good/basic_template.h", "start": 164803, "end": 165234}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 165234, "end": 165393}, {"filename": "/tests/metron_good/bit_casts.h", "start": 165393, "end": 171366}, {"filename": "/tests/metron_good/bit_concat.h", "start": 171366, "end": 171793}, {"filename": "/tests/metron_good/bit_dup.h", "start": 171793, "end": 172452}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 172452, "end": 173290}, {"filename": "/tests/metron_good/builtins.h", "start": 173290, "end": 173621}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 173621, "end": 173928}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 173928, "end": 174482}, {"filename": "/tests/metron_good/defines.h", "start": 174482, "end": 174796}, {"filename": "/tests/metron_good/dontcare.h", "start": 174796, "end": 175079}, {"filename": "/tests/metron_good/enum_simple.h", "start": 175079, "end": 176447}, {"filename": "/tests/metron_good/for_loops.h", "start": 176447, "end": 176767}, {"filename": "/tests/metron_good/good_order.h", "start": 176767, "end": 177063}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 177063, "end": 177474}, {"filename": "/tests/metron_good/include_guards.h", "start": 177474, "end": 177671}, {"filename": "/tests/metron_good/init_chain.h", "start": 177671, "end": 178379}, {"filename": "/tests/metron_good/input_signals.h", "start": 178379, "end": 179041}, {"filename": "/tests/metron_good/local_localparam.h", "start": 179041, "end": 179219}, {"filename": "/tests/metron_good/magic_comments.h", "start": 179219, "end": 179522}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 179522, "end": 179836}, {"filename": "/tests/metron_good/minimal.h", "start": 179836, "end": 179911}, {"filename": "/tests/metron_good/multi_tick.h", "start": 179911, "end": 180277}, {"filename": "/tests/metron_good/namespaces.h", "start": 180277, "end": 180627}, {"filename": "/tests/metron_good/nested_structs.h", "start": 180627, "end": 181140}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 181140, "end": 181685}, {"filename": "/tests/metron_good/oneliners.h", "start": 181685, "end": 181948}, {"filename": "/tests/metron_good/plus_equals.h", "start": 181948, "end": 182298}, {"filename": "/tests/metron_good/private_getter.h", "start": 182298, "end": 182522}, {"filename": "/tests/metron_good/structs.h", "start": 182522, "end": 182741}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 182741, "end": 183495}, {"filename": "/tests/metron_good/tock_task.h", "start": 183495, "end": 183851}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 183851, "end": 184016}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 184016, "end": 184675}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 184675, "end": 185334}], "remote_package_size": 185334});

  })();
