
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22810}, {"filename": "/examples/ibex/README.md", "start": 22810, "end": 22861}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22861, "end": 24446}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24446, "end": 36544}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36544, "end": 50954}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50954, "end": 66978}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 66978, "end": 69479}, {"filename": "/examples/j1/metron/dpram.h", "start": 69479, "end": 69917}, {"filename": "/examples/j1/metron/j1.h", "start": 69917, "end": 73937}, {"filename": "/examples/pong/README.md", "start": 73937, "end": 73997}, {"filename": "/examples/pong/metron/pong.h", "start": 73997, "end": 77861}, {"filename": "/examples/pong/reference/README.md", "start": 77861, "end": 77921}, {"filename": "/examples/rvsimple/README.md", "start": 77921, "end": 78000}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78000, "end": 78500}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78500, "end": 79961}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 79961, "end": 82566}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82566, "end": 83781}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83781, "end": 89500}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89500, "end": 90610}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90610, "end": 92543}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92543, "end": 93770}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93770, "end": 95014}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95014, "end": 95683}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95683, "end": 96647}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96647, "end": 98765}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 98765, "end": 99529}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99529, "end": 100209}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100209, "end": 101025}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101025, "end": 102024}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102024, "end": 102995}, {"filename": "/examples/rvsimple/metron/register.h", "start": 102995, "end": 103680}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 103680, "end": 106723}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 106723, "end": 112297}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112297, "end": 114790}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 114790, "end": 119517}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119517, "end": 121505}, {"filename": "/examples/scratch.h", "start": 121505, "end": 122047}, {"filename": "/examples/tutorial/adder.h", "start": 122047, "end": 122227}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122227, "end": 123208}, {"filename": "/examples/tutorial/blockram.h", "start": 123208, "end": 123725}, {"filename": "/examples/tutorial/checksum.h", "start": 123725, "end": 124448}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124448, "end": 124984}, {"filename": "/examples/tutorial/counter.h", "start": 124984, "end": 125133}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125133, "end": 125912}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 125912, "end": 127330}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127330, "end": 127452}, {"filename": "/examples/tutorial/submodules.h", "start": 127452, "end": 128569}, {"filename": "/examples/tutorial/templates.h", "start": 128569, "end": 129040}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129040, "end": 129108}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129108, "end": 129149}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129149, "end": 129190}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129190, "end": 129231}, {"filename": "/examples/tutorial/vga.h", "start": 129231, "end": 130378}, {"filename": "/examples/uart/README.md", "start": 130378, "end": 130622}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 130622, "end": 133202}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 133202, "end": 135763}, {"filename": "/examples/uart/metron/uart_top.h", "start": 135763, "end": 137527}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 137527, "end": 140546}, {"filename": "/tests/metron_bad/README.md", "start": 140546, "end": 140743}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 140743, "end": 141039}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141039, "end": 141287}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141287, "end": 141530}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 141530, "end": 142125}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 142125, "end": 142535}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 142535, "end": 143075}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 143075, "end": 143386}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 143386, "end": 143832}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 143832, "end": 144092}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 144092, "end": 144364}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 144364, "end": 144871}, {"filename": "/tests/metron_good/README.md", "start": 144871, "end": 144952}, {"filename": "/tests/metron_good/all_func_types.h", "start": 144952, "end": 146517}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 146517, "end": 146924}, {"filename": "/tests/metron_good/basic_function.h", "start": 146924, "end": 147203}, {"filename": "/tests/metron_good/basic_increment.h", "start": 147203, "end": 147558}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 147558, "end": 147853}, {"filename": "/tests/metron_good/basic_literals.h", "start": 147853, "end": 148465}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 148465, "end": 148711}, {"filename": "/tests/metron_good/basic_output.h", "start": 148711, "end": 148972}, {"filename": "/tests/metron_good/basic_param.h", "start": 148972, "end": 149231}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 149231, "end": 149462}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 149462, "end": 149642}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 149642, "end": 149905}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 149905, "end": 150125}, {"filename": "/tests/metron_good/basic_submod.h", "start": 150125, "end": 150414}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 150414, "end": 150769}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 150769, "end": 151145}, {"filename": "/tests/metron_good/basic_switch.h", "start": 151145, "end": 151622}, {"filename": "/tests/metron_good/basic_task.h", "start": 151622, "end": 151956}, {"filename": "/tests/metron_good/basic_template.h", "start": 151956, "end": 152442}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 152442, "end": 152601}, {"filename": "/tests/metron_good/bit_casts.h", "start": 152601, "end": 158574}, {"filename": "/tests/metron_good/bit_concat.h", "start": 158574, "end": 159001}, {"filename": "/tests/metron_good/bit_dup.h", "start": 159001, "end": 159660}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 159660, "end": 160498}, {"filename": "/tests/metron_good/builtins.h", "start": 160498, "end": 160829}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 160829, "end": 161136}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 161136, "end": 161690}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 161690, "end": 162583}, {"filename": "/tests/metron_good/constructor_args.h", "start": 162583, "end": 163091}, {"filename": "/tests/metron_good/defines.h", "start": 163091, "end": 163405}, {"filename": "/tests/metron_good/dontcare.h", "start": 163405, "end": 163685}, {"filename": "/tests/metron_good/enum_simple.h", "start": 163685, "end": 165053}, {"filename": "/tests/metron_good/for_loops.h", "start": 165053, "end": 165373}, {"filename": "/tests/metron_good/good_order.h", "start": 165373, "end": 165669}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 165669, "end": 166080}, {"filename": "/tests/metron_good/include_guards.h", "start": 166080, "end": 166277}, {"filename": "/tests/metron_good/init_chain.h", "start": 166277, "end": 166985}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 166985, "end": 167272}, {"filename": "/tests/metron_good/input_signals.h", "start": 167272, "end": 167934}, {"filename": "/tests/metron_good/local_localparam.h", "start": 167934, "end": 168112}, {"filename": "/tests/metron_good/magic_comments.h", "start": 168112, "end": 168415}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 168415, "end": 168729}, {"filename": "/tests/metron_good/minimal.h", "start": 168729, "end": 168804}, {"filename": "/tests/metron_good/multi_tick.h", "start": 168804, "end": 169170}, {"filename": "/tests/metron_good/namespaces.h", "start": 169170, "end": 169520}, {"filename": "/tests/metron_good/nested_structs.h", "start": 169520, "end": 170033}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 170033, "end": 170578}, {"filename": "/tests/metron_good/oneliners.h", "start": 170578, "end": 170841}, {"filename": "/tests/metron_good/plus_equals.h", "start": 170841, "end": 171265}, {"filename": "/tests/metron_good/private_getter.h", "start": 171265, "end": 171489}, {"filename": "/tests/metron_good/structs.h", "start": 171489, "end": 171708}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 171708, "end": 172244}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 172244, "end": 174595}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 174595, "end": 175349}, {"filename": "/tests/metron_good/tock_task.h", "start": 175349, "end": 175705}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 175705, "end": 175870}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 175870, "end": 176529}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 176529, "end": 177188}], "remote_package_size": 177188});

  })();
