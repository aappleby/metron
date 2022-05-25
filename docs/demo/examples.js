
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
    loadPackage({"files": [{"filename": "/examples/lfsr", "start": 0, "end": 16760}, {"filename": "/examples/scratch.h", "start": 16760, "end": 17162}, {"filename": "/examples/rvtiny/README.md", "start": 17162, "end": 17326}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 17326, "end": 23422}, {"filename": "/examples/rvsimple/README.md", "start": 23422, "end": 23501}, {"filename": "/examples/rvsimple/metron/register.h", "start": 23501, "end": 24186}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 24186, "end": 25339}, {"filename": "/examples/rvsimple/metron/config.h", "start": 25339, "end": 26554}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 26554, "end": 27318}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 27318, "end": 29811}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 29811, "end": 31659}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 31659, "end": 32335}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 32335, "end": 37062}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 37062, "end": 38176}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 38176, "end": 39147}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 39147, "end": 40381}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 40381, "end": 42319}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 42319, "end": 42819}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 42819, "end": 45862}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 45862, "end": 51456}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 51456, "end": 52466}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 52466, "end": 53293}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 53293, "end": 55914}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 55914, "end": 57375}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 57375, "end": 63094}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 63094, "end": 63967}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 63967, "end": 66085}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 66085, "end": 66776}, {"filename": "/examples/pong/README.md", "start": 66776, "end": 66836}, {"filename": "/examples/pong/metron/pong.h", "start": 66836, "end": 70700}, {"filename": "/examples/pong/reference/README.md", "start": 70700, "end": 70760}, {"filename": "/examples/ibex/README.md", "start": 70760, "end": 70811}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 70811, "end": 85221}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 85221, "end": 97319}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 97319, "end": 98904}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 98904, "end": 114928}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 114928, "end": 117429}, {"filename": "/examples/rvtiny_sync/README.md", "start": 117429, "end": 117481}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 117481, "end": 124436}, {"filename": "/examples/tutorial/tutorial5.h", "start": 124436, "end": 124477}, {"filename": "/examples/tutorial/vga.h", "start": 124477, "end": 125624}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125624, "end": 126403}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 126403, "end": 126788}, {"filename": "/examples/tutorial/tutorial3.h", "start": 126788, "end": 126829}, {"filename": "/examples/tutorial/submodules.h", "start": 126829, "end": 128016}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 128016, "end": 129434}, {"filename": "/examples/tutorial/adder.h", "start": 129434, "end": 129614}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129614, "end": 129655}, {"filename": "/examples/tutorial/blockram.h", "start": 129655, "end": 130034}, {"filename": "/examples/tutorial/tutorial2.h", "start": 130034, "end": 130102}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 130102, "end": 131083}, {"filename": "/examples/tutorial/counter.h", "start": 131083, "end": 131232}, {"filename": "/examples/tutorial/nonblocking.h", "start": 131232, "end": 131464}, {"filename": "/examples/uart/README.md", "start": 131464, "end": 131708}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 131708, "end": 133001}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 133001, "end": 134364}, {"filename": "/examples/uart/metron/uart_top.h", "start": 134364, "end": 135424}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 135424, "end": 137270}, {"filename": "/tests/metron_good/include_guards.h", "start": 137270, "end": 137467}, {"filename": "/tests/metron_good/oneliners.h", "start": 137467, "end": 137730}, {"filename": "/tests/metron_good/enum_simple.h", "start": 137730, "end": 139098}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 139098, "end": 139263}, {"filename": "/tests/metron_good/basic_increment.h", "start": 139263, "end": 139618}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 139618, "end": 140456}, {"filename": "/tests/metron_good/magic_comments.h", "start": 140456, "end": 140759}, {"filename": "/tests/metron_good/for_loops.h", "start": 140759, "end": 141079}, {"filename": "/tests/metron_good/defines.h", "start": 141079, "end": 141393}, {"filename": "/tests/metron_good/bit_concat.h", "start": 141393, "end": 141820}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 141820, "end": 142000}, {"filename": "/tests/metron_good/force_tick.h", "start": 142000, "end": 142182}, {"filename": "/tests/metron_good/README.md", "start": 142182, "end": 142263}, {"filename": "/tests/metron_good/good_order.h", "start": 142263, "end": 142559}, {"filename": "/tests/metron_good/basic_template.h", "start": 142559, "end": 142990}, {"filename": "/tests/metron_good/bit_casts.h", "start": 142990, "end": 148963}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 148963, "end": 149235}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 149235, "end": 149542}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 149542, "end": 150201}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 150201, "end": 150556}, {"filename": "/tests/metron_good/tock_task.h", "start": 150556, "end": 150912}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 150912, "end": 151132}, {"filename": "/tests/metron_good/all_func_types.h", "start": 151132, "end": 152697}, {"filename": "/tests/metron_good/basic_submod.h", "start": 152697, "end": 152986}, {"filename": "/tests/metron_good/dontcare.h", "start": 152986, "end": 153269}, {"filename": "/tests/metron_good/basic_param.h", "start": 153269, "end": 153528}, {"filename": "/tests/metron_good/basic_output.h", "start": 153528, "end": 153789}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 153789, "end": 154543}, {"filename": "/tests/metron_good/basic_function.h", "start": 154543, "end": 154822}, {"filename": "/tests/metron_good/builtins.h", "start": 154822, "end": 155153}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 155153, "end": 155399}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 155399, "end": 155806}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 155806, "end": 156037}, {"filename": "/tests/metron_good/basic_literals.h", "start": 156037, "end": 156649}, {"filename": "/tests/metron_good/basic_switch.h", "start": 156649, "end": 157126}, {"filename": "/tests/metron_good/bit_dup.h", "start": 157126, "end": 157785}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 157785, "end": 158444}, {"filename": "/tests/metron_good/local_localparam.h", "start": 158444, "end": 158622}, {"filename": "/tests/metron_good/minimal.h", "start": 158622, "end": 158697}, {"filename": "/tests/metron_good/multi_tick.h", "start": 158697, "end": 159063}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 159063, "end": 159326}, {"filename": "/tests/metron_good/namespaces.h", "start": 159326, "end": 159676}, {"filename": "/tests/metron_good/structs.h", "start": 159676, "end": 159895}, {"filename": "/tests/metron_good/basic_task.h", "start": 159895, "end": 160229}, {"filename": "/tests/metron_good/nested_structs.h", "start": 160229, "end": 160742}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 160742, "end": 161056}, {"filename": "/tests/metron_good/private_getter.h", "start": 161056, "end": 161280}, {"filename": "/tests/metron_good/init_chain.h", "start": 161280, "end": 161988}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 161988, "end": 162364}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 162364, "end": 162775}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 162775, "end": 163329}, {"filename": "/tests/metron_good/input_signals.h", "start": 163329, "end": 163991}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 163991, "end": 164536}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 164536, "end": 164695}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 164695, "end": 164990}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 164990, "end": 165130}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 165130, "end": 165540}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 165540, "end": 166080}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 166080, "end": 166328}, {"filename": "/tests/metron_bad/README.md", "start": 166328, "end": 166525}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 166525, "end": 166836}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 166836, "end": 167343}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 167343, "end": 167938}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 167938, "end": 168198}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 168198, "end": 168441}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 168441, "end": 168737}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 168737, "end": 169183}], "remote_package_size": 169183, "package_uuid": "d5db58de-f95f-4544-a429-ce91fdf90cb0"});

  })();
