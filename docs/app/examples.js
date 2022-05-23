
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1384}, {"filename": "/examples/rvtiny/README.md", "start": 1384, "end": 1548}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 1548, "end": 7644}, {"filename": "/examples/rvsimple/README.md", "start": 7644, "end": 7723}, {"filename": "/examples/rvsimple/metron/register.h", "start": 7723, "end": 8408}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 8408, "end": 9561}, {"filename": "/examples/rvsimple/metron/config.h", "start": 9561, "end": 10776}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 10776, "end": 11540}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 11540, "end": 14033}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 14033, "end": 15881}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 15881, "end": 16557}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 16557, "end": 21284}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 21284, "end": 22398}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 22398, "end": 23369}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 23369, "end": 24603}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 24603, "end": 26541}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 26541, "end": 27041}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 27041, "end": 30084}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 30084, "end": 35678}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 35678, "end": 36688}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 36688, "end": 37515}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 37515, "end": 40136}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 40136, "end": 41597}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 41597, "end": 47316}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 47316, "end": 48189}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 48189, "end": 50307}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 50307, "end": 50998}, {"filename": "/examples/pong/README.md", "start": 50998, "end": 51058}, {"filename": "/examples/pong/metron/pong.h", "start": 51058, "end": 54922}, {"filename": "/examples/pong/reference/README.md", "start": 54922, "end": 54982}, {"filename": "/examples/ibex/README.md", "start": 54982, "end": 55033}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 55033, "end": 69443}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 69443, "end": 81541}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 81541, "end": 83126}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 83126, "end": 99150}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 99150, "end": 101651}, {"filename": "/examples/rvtiny_sync/README.md", "start": 101651, "end": 101703}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 101703, "end": 108658}, {"filename": "/examples/tutorial/pong.h", "start": 108658, "end": 109969}, {"filename": "/examples/tutorial/tutorial5.h", "start": 109969, "end": 110010}, {"filename": "/examples/tutorial/tutorial1.h", "start": 110010, "end": 110086}, {"filename": "/examples/tutorial/tutorial3.h", "start": 110086, "end": 110127}, {"filename": "/examples/tutorial/tutorial4.h", "start": 110127, "end": 110168}, {"filename": "/examples/tutorial/tutorial2.h", "start": 110168, "end": 110236}, {"filename": "/examples/uart/README.md", "start": 110236, "end": 110480}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 110480, "end": 111773}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 111773, "end": 113136}, {"filename": "/examples/uart/metron/uart_top.h", "start": 113136, "end": 114196}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 114196, "end": 116042}, {"filename": "/tests/metron_good/include_guards.h", "start": 116042, "end": 116239}, {"filename": "/tests/metron_good/oneliners.h", "start": 116239, "end": 116502}, {"filename": "/tests/metron_good/enum_simple.h", "start": 116502, "end": 117870}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 117870, "end": 118035}, {"filename": "/tests/metron_good/basic_increment.h", "start": 118035, "end": 118390}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 118390, "end": 119228}, {"filename": "/tests/metron_good/magic_comments.h", "start": 119228, "end": 119531}, {"filename": "/tests/metron_good/for_loops.h", "start": 119531, "end": 119851}, {"filename": "/tests/metron_good/defines.h", "start": 119851, "end": 120165}, {"filename": "/tests/metron_good/bit_concat.h", "start": 120165, "end": 120592}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 120592, "end": 120772}, {"filename": "/tests/metron_good/force_tick.h", "start": 120772, "end": 120954}, {"filename": "/tests/metron_good/README.md", "start": 120954, "end": 121035}, {"filename": "/tests/metron_good/good_order.h", "start": 121035, "end": 121331}, {"filename": "/tests/metron_good/basic_template.h", "start": 121331, "end": 121762}, {"filename": "/tests/metron_good/bit_casts.h", "start": 121762, "end": 127735}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 127735, "end": 128007}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 128007, "end": 128314}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 128314, "end": 128973}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 128973, "end": 129328}, {"filename": "/tests/metron_good/tock_task.h", "start": 129328, "end": 129684}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 129684, "end": 129904}, {"filename": "/tests/metron_good/all_func_types.h", "start": 129904, "end": 131469}, {"filename": "/tests/metron_good/basic_submod.h", "start": 131469, "end": 131758}, {"filename": "/tests/metron_good/dontcare.h", "start": 131758, "end": 132041}, {"filename": "/tests/metron_good/basic_param.h", "start": 132041, "end": 132300}, {"filename": "/tests/metron_good/basic_output.h", "start": 132300, "end": 132561}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 132561, "end": 133315}, {"filename": "/tests/metron_good/basic_function.h", "start": 133315, "end": 133594}, {"filename": "/tests/metron_good/builtins.h", "start": 133594, "end": 133925}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 133925, "end": 134171}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 134171, "end": 134578}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 134578, "end": 134809}, {"filename": "/tests/metron_good/basic_literals.h", "start": 134809, "end": 135421}, {"filename": "/tests/metron_good/basic_switch.h", "start": 135421, "end": 135898}, {"filename": "/tests/metron_good/bit_dup.h", "start": 135898, "end": 136557}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 136557, "end": 137216}, {"filename": "/tests/metron_good/local_localparam.h", "start": 137216, "end": 137394}, {"filename": "/tests/metron_good/minimal.h", "start": 137394, "end": 137469}, {"filename": "/tests/metron_good/multi_tick.h", "start": 137469, "end": 137835}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 137835, "end": 138098}, {"filename": "/tests/metron_good/namespaces.h", "start": 138098, "end": 138448}, {"filename": "/tests/metron_good/structs.h", "start": 138448, "end": 138667}, {"filename": "/tests/metron_good/basic_task.h", "start": 138667, "end": 139001}, {"filename": "/tests/metron_good/nested_structs.h", "start": 139001, "end": 139514}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 139514, "end": 139828}, {"filename": "/tests/metron_good/private_getter.h", "start": 139828, "end": 140052}, {"filename": "/tests/metron_good/init_chain.h", "start": 140052, "end": 140760}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 140760, "end": 141136}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 141136, "end": 141547}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 141547, "end": 142101}, {"filename": "/tests/metron_good/input_signals.h", "start": 142101, "end": 142763}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 142763, "end": 143308}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 143308, "end": 143467}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 143467, "end": 143762}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 143762, "end": 143902}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 143902, "end": 144313}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 144313, "end": 144853}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 144853, "end": 145101}, {"filename": "/tests/metron_bad/README.md", "start": 145101, "end": 145298}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 145298, "end": 145609}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 145609, "end": 146116}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 146116, "end": 146711}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 146711, "end": 146971}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 146971, "end": 147214}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 147214, "end": 147510}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 147510, "end": 147956}], "remote_package_size": 147956, "package_uuid": "151e7923-a9dd-4d7e-bbdb-ea55701d869a"});

  })();
