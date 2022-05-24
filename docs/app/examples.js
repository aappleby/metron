
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1384}, {"filename": "/examples/rvtiny/README.md", "start": 1384, "end": 1548}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 1548, "end": 7644}, {"filename": "/examples/rvsimple/README.md", "start": 7644, "end": 7723}, {"filename": "/examples/rvsimple/metron/register.h", "start": 7723, "end": 8408}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 8408, "end": 9561}, {"filename": "/examples/rvsimple/metron/config.h", "start": 9561, "end": 10776}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 10776, "end": 11540}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 11540, "end": 14033}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 14033, "end": 15881}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 15881, "end": 16557}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 16557, "end": 21284}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 21284, "end": 22398}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 22398, "end": 23369}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 23369, "end": 24603}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 24603, "end": 26541}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 26541, "end": 27041}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 27041, "end": 30084}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 30084, "end": 35678}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 35678, "end": 36688}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 36688, "end": 37515}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 37515, "end": 40136}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 40136, "end": 41597}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 41597, "end": 47316}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 47316, "end": 48189}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 48189, "end": 50307}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 50307, "end": 50998}, {"filename": "/examples/pong/README.md", "start": 50998, "end": 51058}, {"filename": "/examples/pong/metron/pong.h", "start": 51058, "end": 54922}, {"filename": "/examples/pong/reference/README.md", "start": 54922, "end": 54982}, {"filename": "/examples/ibex/README.md", "start": 54982, "end": 55033}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 55033, "end": 69443}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 69443, "end": 81541}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 81541, "end": 83126}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 83126, "end": 99150}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 99150, "end": 101651}, {"filename": "/examples/rvtiny_sync/README.md", "start": 101651, "end": 101703}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 101703, "end": 108658}, {"filename": "/examples/tutorial/tutorial5.h", "start": 108658, "end": 108699}, {"filename": "/examples/tutorial/vga.h", "start": 108699, "end": 109846}, {"filename": "/examples/tutorial/declaration_order.h", "start": 109846, "end": 110625}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 110625, "end": 111010}, {"filename": "/examples/tutorial/tutorial3.h", "start": 111010, "end": 111051}, {"filename": "/examples/tutorial/submodules.h", "start": 111051, "end": 111958}, {"filename": "/examples/tutorial/adder.h", "start": 111958, "end": 112138}, {"filename": "/examples/tutorial/tutorial4.h", "start": 112138, "end": 112179}, {"filename": "/examples/tutorial/tutorial2.h", "start": 112179, "end": 112247}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 112247, "end": 113228}, {"filename": "/examples/tutorial/counter.h", "start": 113228, "end": 113377}, {"filename": "/examples/tutorial/pure_functions.h", "start": 113377, "end": 113932}, {"filename": "/examples/tutorial/nonblocking.h", "start": 113932, "end": 114164}, {"filename": "/examples/uart/README.md", "start": 114164, "end": 114408}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 114408, "end": 115701}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 115701, "end": 117064}, {"filename": "/examples/uart/metron/uart_top.h", "start": 117064, "end": 118124}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 118124, "end": 119970}, {"filename": "/tests/metron_good/include_guards.h", "start": 119970, "end": 120167}, {"filename": "/tests/metron_good/oneliners.h", "start": 120167, "end": 120430}, {"filename": "/tests/metron_good/enum_simple.h", "start": 120430, "end": 121798}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 121798, "end": 121963}, {"filename": "/tests/metron_good/basic_increment.h", "start": 121963, "end": 122318}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 122318, "end": 123156}, {"filename": "/tests/metron_good/magic_comments.h", "start": 123156, "end": 123459}, {"filename": "/tests/metron_good/for_loops.h", "start": 123459, "end": 123779}, {"filename": "/tests/metron_good/defines.h", "start": 123779, "end": 124093}, {"filename": "/tests/metron_good/bit_concat.h", "start": 124093, "end": 124520}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 124520, "end": 124700}, {"filename": "/tests/metron_good/force_tick.h", "start": 124700, "end": 124882}, {"filename": "/tests/metron_good/README.md", "start": 124882, "end": 124963}, {"filename": "/tests/metron_good/good_order.h", "start": 124963, "end": 125259}, {"filename": "/tests/metron_good/basic_template.h", "start": 125259, "end": 125690}, {"filename": "/tests/metron_good/bit_casts.h", "start": 125690, "end": 131663}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 131663, "end": 131935}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 131935, "end": 132242}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 132242, "end": 132901}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 132901, "end": 133256}, {"filename": "/tests/metron_good/tock_task.h", "start": 133256, "end": 133612}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 133612, "end": 133832}, {"filename": "/tests/metron_good/all_func_types.h", "start": 133832, "end": 135397}, {"filename": "/tests/metron_good/basic_submod.h", "start": 135397, "end": 135686}, {"filename": "/tests/metron_good/dontcare.h", "start": 135686, "end": 135969}, {"filename": "/tests/metron_good/basic_param.h", "start": 135969, "end": 136228}, {"filename": "/tests/metron_good/basic_output.h", "start": 136228, "end": 136489}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 136489, "end": 137243}, {"filename": "/tests/metron_good/basic_function.h", "start": 137243, "end": 137522}, {"filename": "/tests/metron_good/builtins.h", "start": 137522, "end": 137853}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 137853, "end": 138099}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 138099, "end": 138506}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 138506, "end": 138737}, {"filename": "/tests/metron_good/basic_literals.h", "start": 138737, "end": 139349}, {"filename": "/tests/metron_good/basic_switch.h", "start": 139349, "end": 139826}, {"filename": "/tests/metron_good/bit_dup.h", "start": 139826, "end": 140485}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 140485, "end": 141144}, {"filename": "/tests/metron_good/local_localparam.h", "start": 141144, "end": 141322}, {"filename": "/tests/metron_good/minimal.h", "start": 141322, "end": 141397}, {"filename": "/tests/metron_good/multi_tick.h", "start": 141397, "end": 141763}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 141763, "end": 142026}, {"filename": "/tests/metron_good/namespaces.h", "start": 142026, "end": 142376}, {"filename": "/tests/metron_good/structs.h", "start": 142376, "end": 142595}, {"filename": "/tests/metron_good/basic_task.h", "start": 142595, "end": 142929}, {"filename": "/tests/metron_good/nested_structs.h", "start": 142929, "end": 143442}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 143442, "end": 143756}, {"filename": "/tests/metron_good/private_getter.h", "start": 143756, "end": 143980}, {"filename": "/tests/metron_good/init_chain.h", "start": 143980, "end": 144688}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 144688, "end": 145064}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 145064, "end": 145475}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 145475, "end": 146029}, {"filename": "/tests/metron_good/input_signals.h", "start": 146029, "end": 146691}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 146691, "end": 147236}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 147236, "end": 147395}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 147395, "end": 147690}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 147690, "end": 147830}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 147830, "end": 148241}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 148241, "end": 148781}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 148781, "end": 149029}, {"filename": "/tests/metron_bad/README.md", "start": 149029, "end": 149226}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 149226, "end": 149537}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 149537, "end": 150044}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 150044, "end": 150639}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 150639, "end": 150899}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 150899, "end": 151142}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 151142, "end": 151438}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 151438, "end": 151884}], "remote_package_size": 151884, "package_uuid": "43bcc82a-08a8-44d5-8303-7706de0bd0f3"});

  })();
