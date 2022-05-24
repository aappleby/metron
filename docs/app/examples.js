
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1384}, {"filename": "/examples/rvtiny/README.md", "start": 1384, "end": 1548}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 1548, "end": 7644}, {"filename": "/examples/rvsimple/README.md", "start": 7644, "end": 7723}, {"filename": "/examples/rvsimple/metron/register.h", "start": 7723, "end": 8408}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 8408, "end": 9561}, {"filename": "/examples/rvsimple/metron/config.h", "start": 9561, "end": 10776}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 10776, "end": 11540}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 11540, "end": 14033}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 14033, "end": 15881}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 15881, "end": 16557}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 16557, "end": 21284}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 21284, "end": 22398}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 22398, "end": 23369}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 23369, "end": 24603}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 24603, "end": 26541}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 26541, "end": 27041}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 27041, "end": 30084}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 30084, "end": 35678}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 35678, "end": 36688}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 36688, "end": 37515}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 37515, "end": 40136}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 40136, "end": 41597}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 41597, "end": 47316}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 47316, "end": 48189}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 48189, "end": 50307}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 50307, "end": 50998}, {"filename": "/examples/pong/README.md", "start": 50998, "end": 51058}, {"filename": "/examples/pong/metron/pong.h", "start": 51058, "end": 54922}, {"filename": "/examples/pong/reference/README.md", "start": 54922, "end": 54982}, {"filename": "/examples/ibex/README.md", "start": 54982, "end": 55033}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 55033, "end": 69443}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 69443, "end": 81541}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 81541, "end": 83126}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 83126, "end": 99150}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 99150, "end": 101651}, {"filename": "/examples/rvtiny_sync/README.md", "start": 101651, "end": 101703}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 101703, "end": 108658}, {"filename": "/examples/tutorial/tutorial5.h", "start": 108658, "end": 108699}, {"filename": "/examples/tutorial/vga.h", "start": 108699, "end": 109846}, {"filename": "/examples/tutorial/tutorial3.h", "start": 109846, "end": 109887}, {"filename": "/examples/tutorial/submodules.h", "start": 109887, "end": 110582}, {"filename": "/examples/tutorial/adder.h", "start": 110582, "end": 110762}, {"filename": "/examples/tutorial/tutorial4.h", "start": 110762, "end": 110803}, {"filename": "/examples/tutorial/tutorial2.h", "start": 110803, "end": 110871}, {"filename": "/examples/tutorial/counter.h", "start": 110871, "end": 111020}, {"filename": "/examples/tutorial/nonblocking.h", "start": 111020, "end": 111252}, {"filename": "/examples/uart/README.md", "start": 111252, "end": 111496}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 111496, "end": 112789}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 112789, "end": 114152}, {"filename": "/examples/uart/metron/uart_top.h", "start": 114152, "end": 115212}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 115212, "end": 117058}, {"filename": "/tests/metron_good/include_guards.h", "start": 117058, "end": 117255}, {"filename": "/tests/metron_good/oneliners.h", "start": 117255, "end": 117518}, {"filename": "/tests/metron_good/enum_simple.h", "start": 117518, "end": 118886}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 118886, "end": 119051}, {"filename": "/tests/metron_good/basic_increment.h", "start": 119051, "end": 119406}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 119406, "end": 120244}, {"filename": "/tests/metron_good/magic_comments.h", "start": 120244, "end": 120547}, {"filename": "/tests/metron_good/for_loops.h", "start": 120547, "end": 120867}, {"filename": "/tests/metron_good/defines.h", "start": 120867, "end": 121181}, {"filename": "/tests/metron_good/bit_concat.h", "start": 121181, "end": 121608}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 121608, "end": 121788}, {"filename": "/tests/metron_good/force_tick.h", "start": 121788, "end": 121970}, {"filename": "/tests/metron_good/README.md", "start": 121970, "end": 122051}, {"filename": "/tests/metron_good/good_order.h", "start": 122051, "end": 122347}, {"filename": "/tests/metron_good/basic_template.h", "start": 122347, "end": 122778}, {"filename": "/tests/metron_good/bit_casts.h", "start": 122778, "end": 128751}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 128751, "end": 129023}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 129023, "end": 129330}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 129330, "end": 129989}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 129989, "end": 130344}, {"filename": "/tests/metron_good/tock_task.h", "start": 130344, "end": 130700}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 130700, "end": 130920}, {"filename": "/tests/metron_good/all_func_types.h", "start": 130920, "end": 132485}, {"filename": "/tests/metron_good/basic_submod.h", "start": 132485, "end": 132774}, {"filename": "/tests/metron_good/dontcare.h", "start": 132774, "end": 133057}, {"filename": "/tests/metron_good/basic_param.h", "start": 133057, "end": 133316}, {"filename": "/tests/metron_good/basic_output.h", "start": 133316, "end": 133577}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 133577, "end": 134331}, {"filename": "/tests/metron_good/basic_function.h", "start": 134331, "end": 134610}, {"filename": "/tests/metron_good/builtins.h", "start": 134610, "end": 134941}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 134941, "end": 135187}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 135187, "end": 135594}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 135594, "end": 135825}, {"filename": "/tests/metron_good/basic_literals.h", "start": 135825, "end": 136437}, {"filename": "/tests/metron_good/basic_switch.h", "start": 136437, "end": 136914}, {"filename": "/tests/metron_good/bit_dup.h", "start": 136914, "end": 137573}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 137573, "end": 138232}, {"filename": "/tests/metron_good/local_localparam.h", "start": 138232, "end": 138410}, {"filename": "/tests/metron_good/minimal.h", "start": 138410, "end": 138485}, {"filename": "/tests/metron_good/multi_tick.h", "start": 138485, "end": 138851}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 138851, "end": 139114}, {"filename": "/tests/metron_good/namespaces.h", "start": 139114, "end": 139464}, {"filename": "/tests/metron_good/structs.h", "start": 139464, "end": 139683}, {"filename": "/tests/metron_good/basic_task.h", "start": 139683, "end": 140017}, {"filename": "/tests/metron_good/nested_structs.h", "start": 140017, "end": 140530}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 140530, "end": 140844}, {"filename": "/tests/metron_good/private_getter.h", "start": 140844, "end": 141068}, {"filename": "/tests/metron_good/init_chain.h", "start": 141068, "end": 141776}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 141776, "end": 142152}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 142152, "end": 142563}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 142563, "end": 143117}, {"filename": "/tests/metron_good/input_signals.h", "start": 143117, "end": 143779}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 143779, "end": 144324}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 144324, "end": 144483}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 144483, "end": 144778}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 144778, "end": 144918}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 144918, "end": 145329}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 145329, "end": 145869}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 145869, "end": 146117}, {"filename": "/tests/metron_bad/README.md", "start": 146117, "end": 146314}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 146314, "end": 146625}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 146625, "end": 147132}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 147132, "end": 147727}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 147727, "end": 147987}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 147987, "end": 148230}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 148230, "end": 148526}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 148526, "end": 148972}], "remote_package_size": 148972, "package_uuid": "62a1a8b4-36f7-4b3f-8b3f-4c54f35f5a94"});

  })();
