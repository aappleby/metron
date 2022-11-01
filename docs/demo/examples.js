
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
Module['FS_createPath']("/examples", "pinwheel", true, true);
Module['FS_createPath']("/examples/pinwheel", "metron", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22810}, {"filename": "/examples/ibex/README.md", "start": 22810, "end": 22861}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22861, "end": 24446}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24446, "end": 36544}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36544, "end": 50954}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50954, "end": 66978}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 66978, "end": 69479}, {"filename": "/examples/j1/metron/dpram.h", "start": 69479, "end": 69917}, {"filename": "/examples/j1/metron/j1.h", "start": 69917, "end": 73942}, {"filename": "/examples/pinwheel/README.md", "start": 73942, "end": 74106}, {"filename": "/examples/pinwheel/metron/constants.h", "start": 74106, "end": 76664}, {"filename": "/examples/pinwheel/metron/pinwheel.h", "start": 76664, "end": 83284}, {"filename": "/examples/pong/README.md", "start": 83284, "end": 83344}, {"filename": "/examples/pong/metron/pong.h", "start": 83344, "end": 87208}, {"filename": "/examples/pong/reference/README.md", "start": 87208, "end": 87268}, {"filename": "/examples/rvsimple/README.md", "start": 87268, "end": 87347}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 87347, "end": 87847}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 87847, "end": 89308}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 89308, "end": 91929}, {"filename": "/examples/rvsimple/metron/config.h", "start": 91929, "end": 93144}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 93144, "end": 98863}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 98863, "end": 99977}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 99977, "end": 101915}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 101915, "end": 103149}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 103149, "end": 104302}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 104302, "end": 104978}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 104978, "end": 105851}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 105851, "end": 107969}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 107969, "end": 108733}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 108733, "end": 109424}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 109424, "end": 110251}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 110251, "end": 111261}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 111261, "end": 112232}, {"filename": "/examples/rvsimple/metron/register.h", "start": 112232, "end": 112917}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 112917, "end": 115960}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 115960, "end": 121554}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 121554, "end": 124047}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 124047, "end": 128774}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 128774, "end": 130622}, {"filename": "/examples/scratch.h", "start": 130622, "end": 130829}, {"filename": "/examples/tutorial/adder.h", "start": 130829, "end": 131009}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 131009, "end": 131990}, {"filename": "/examples/tutorial/blockram.h", "start": 131990, "end": 132507}, {"filename": "/examples/tutorial/checksum.h", "start": 132507, "end": 133230}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 133230, "end": 133766}, {"filename": "/examples/tutorial/counter.h", "start": 133766, "end": 133915}, {"filename": "/examples/tutorial/declaration_order.h", "start": 133915, "end": 134694}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 134694, "end": 136112}, {"filename": "/examples/tutorial/nonblocking.h", "start": 136112, "end": 136234}, {"filename": "/examples/tutorial/submodules.h", "start": 136234, "end": 137351}, {"filename": "/examples/tutorial/templates.h", "start": 137351, "end": 137822}, {"filename": "/examples/tutorial/tutorial2.h", "start": 137822, "end": 137890}, {"filename": "/examples/tutorial/tutorial3.h", "start": 137890, "end": 137931}, {"filename": "/examples/tutorial/tutorial4.h", "start": 137931, "end": 137972}, {"filename": "/examples/tutorial/tutorial5.h", "start": 137972, "end": 138013}, {"filename": "/examples/tutorial/vga.h", "start": 138013, "end": 139160}, {"filename": "/examples/uart/README.md", "start": 139160, "end": 139404}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 139404, "end": 141984}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 141984, "end": 144545}, {"filename": "/examples/uart/metron/uart_top.h", "start": 144545, "end": 146309}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 146309, "end": 149328}, {"filename": "/tests/metron_bad/README.md", "start": 149328, "end": 149525}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 149525, "end": 149821}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 149821, "end": 150069}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 150069, "end": 150312}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 150312, "end": 150907}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 150907, "end": 151047}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 151047, "end": 151457}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 151457, "end": 151997}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 151997, "end": 152308}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 152308, "end": 152754}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 152754, "end": 153014}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 153014, "end": 153286}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 153286, "end": 153793}, {"filename": "/tests/metron_good/README.md", "start": 153793, "end": 153874}, {"filename": "/tests/metron_good/all_func_types.h", "start": 153874, "end": 155439}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 155439, "end": 155846}, {"filename": "/tests/metron_good/basic_function.h", "start": 155846, "end": 156125}, {"filename": "/tests/metron_good/basic_increment.h", "start": 156125, "end": 156480}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 156480, "end": 156775}, {"filename": "/tests/metron_good/basic_literals.h", "start": 156775, "end": 157387}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 157387, "end": 157633}, {"filename": "/tests/metron_good/basic_output.h", "start": 157633, "end": 157894}, {"filename": "/tests/metron_good/basic_param.h", "start": 157894, "end": 158153}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 158153, "end": 158384}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 158384, "end": 158564}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 158564, "end": 158827}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 158827, "end": 159047}, {"filename": "/tests/metron_good/basic_submod.h", "start": 159047, "end": 159336}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 159336, "end": 159691}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 159691, "end": 160067}, {"filename": "/tests/metron_good/basic_switch.h", "start": 160067, "end": 160544}, {"filename": "/tests/metron_good/basic_task.h", "start": 160544, "end": 160878}, {"filename": "/tests/metron_good/basic_template.h", "start": 160878, "end": 161309}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 161309, "end": 161468}, {"filename": "/tests/metron_good/bit_casts.h", "start": 161468, "end": 167441}, {"filename": "/tests/metron_good/bit_concat.h", "start": 167441, "end": 167868}, {"filename": "/tests/metron_good/bit_dup.h", "start": 167868, "end": 168527}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 168527, "end": 169365}, {"filename": "/tests/metron_good/builtins.h", "start": 169365, "end": 169696}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 169696, "end": 170003}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 170003, "end": 170557}, {"filename": "/tests/metron_good/defines.h", "start": 170557, "end": 170871}, {"filename": "/tests/metron_good/dontcare.h", "start": 170871, "end": 171154}, {"filename": "/tests/metron_good/enum_simple.h", "start": 171154, "end": 172522}, {"filename": "/tests/metron_good/for_loops.h", "start": 172522, "end": 172842}, {"filename": "/tests/metron_good/good_order.h", "start": 172842, "end": 173138}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 173138, "end": 173549}, {"filename": "/tests/metron_good/include_guards.h", "start": 173549, "end": 173746}, {"filename": "/tests/metron_good/init_chain.h", "start": 173746, "end": 174454}, {"filename": "/tests/metron_good/input_signals.h", "start": 174454, "end": 175116}, {"filename": "/tests/metron_good/local_localparam.h", "start": 175116, "end": 175294}, {"filename": "/tests/metron_good/magic_comments.h", "start": 175294, "end": 175597}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 175597, "end": 175911}, {"filename": "/tests/metron_good/minimal.h", "start": 175911, "end": 175986}, {"filename": "/tests/metron_good/multi_tick.h", "start": 175986, "end": 176352}, {"filename": "/tests/metron_good/namespaces.h", "start": 176352, "end": 176702}, {"filename": "/tests/metron_good/nested_structs.h", "start": 176702, "end": 177215}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 177215, "end": 177760}, {"filename": "/tests/metron_good/oneliners.h", "start": 177760, "end": 178023}, {"filename": "/tests/metron_good/plus_equals.h", "start": 178023, "end": 178373}, {"filename": "/tests/metron_good/private_getter.h", "start": 178373, "end": 178597}, {"filename": "/tests/metron_good/structs.h", "start": 178597, "end": 178816}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 178816, "end": 179570}, {"filename": "/tests/metron_good/tock_task.h", "start": 179570, "end": 179926}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 179926, "end": 180091}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 180091, "end": 180750}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 180750, "end": 181409}], "remote_package_size": 181409});

  })();
