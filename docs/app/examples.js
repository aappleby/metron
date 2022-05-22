
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1416}, {"filename": "/examples/rvtiny/README.md", "start": 1416, "end": 1580}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 1580, "end": 7676}, {"filename": "/examples/rvsimple/README.md", "start": 7676, "end": 7755}, {"filename": "/examples/rvsimple/metron/register.h", "start": 7755, "end": 8440}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 8440, "end": 9593}, {"filename": "/examples/rvsimple/metron/config.h", "start": 9593, "end": 10808}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 10808, "end": 11572}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 11572, "end": 14065}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 14065, "end": 15913}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 15913, "end": 16589}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 16589, "end": 21316}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 21316, "end": 22430}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 22430, "end": 23401}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 23401, "end": 24635}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 24635, "end": 26573}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 26573, "end": 27073}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 27073, "end": 30116}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 30116, "end": 35710}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 35710, "end": 36720}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 36720, "end": 37547}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 37547, "end": 40168}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 40168, "end": 41629}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 41629, "end": 47348}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 47348, "end": 48221}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 48221, "end": 50339}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 50339, "end": 51030}, {"filename": "/examples/pong/README.md", "start": 51030, "end": 51090}, {"filename": "/examples/pong/metron/pong.h", "start": 51090, "end": 54954}, {"filename": "/examples/pong/reference/README.md", "start": 54954, "end": 55014}, {"filename": "/examples/ibex/README.md", "start": 55014, "end": 55065}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 55065, "end": 69475}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 69475, "end": 81573}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 81573, "end": 83158}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 83158, "end": 99182}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 99182, "end": 101683}, {"filename": "/examples/rvtiny_sync/README.md", "start": 101683, "end": 101735}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 101735, "end": 108690}, {"filename": "/examples/uart/README.md", "start": 108690, "end": 108934}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 108934, "end": 110227}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 110227, "end": 111590}, {"filename": "/examples/uart/metron/uart_top.h", "start": 111590, "end": 112650}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 112650, "end": 114496}, {"filename": "/tests/metron_good/include_guards.h", "start": 114496, "end": 114693}, {"filename": "/tests/metron_good/oneliners.h", "start": 114693, "end": 114956}, {"filename": "/tests/metron_good/enum_simple.h", "start": 114956, "end": 116324}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 116324, "end": 116489}, {"filename": "/tests/metron_good/basic_increment.h", "start": 116489, "end": 116844}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 116844, "end": 117682}, {"filename": "/tests/metron_good/magic_comments.h", "start": 117682, "end": 117985}, {"filename": "/tests/metron_good/defines.h", "start": 117985, "end": 118299}, {"filename": "/tests/metron_good/bit_concat.h", "start": 118299, "end": 118726}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 118726, "end": 118906}, {"filename": "/tests/metron_good/force_tick.h", "start": 118906, "end": 119088}, {"filename": "/tests/metron_good/README.md", "start": 119088, "end": 119169}, {"filename": "/tests/metron_good/good_order.h", "start": 119169, "end": 119465}, {"filename": "/tests/metron_good/basic_template.h", "start": 119465, "end": 119896}, {"filename": "/tests/metron_good/bit_casts.h", "start": 119896, "end": 125869}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 125869, "end": 126141}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 126141, "end": 126448}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 126448, "end": 127107}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 127107, "end": 127462}, {"filename": "/tests/metron_good/tock_task.h", "start": 127462, "end": 127818}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 127818, "end": 128038}, {"filename": "/tests/metron_good/all_func_types.h", "start": 128038, "end": 129603}, {"filename": "/tests/metron_good/basic_submod.h", "start": 129603, "end": 129892}, {"filename": "/tests/metron_good/dontcare.h", "start": 129892, "end": 130175}, {"filename": "/tests/metron_good/basic_param.h", "start": 130175, "end": 130434}, {"filename": "/tests/metron_good/basic_output.h", "start": 130434, "end": 130695}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 130695, "end": 131449}, {"filename": "/tests/metron_good/basic_function.h", "start": 131449, "end": 131728}, {"filename": "/tests/metron_good/builtins.h", "start": 131728, "end": 132059}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 132059, "end": 132305}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 132305, "end": 132712}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 132712, "end": 132943}, {"filename": "/tests/metron_good/basic_literals.h", "start": 132943, "end": 133555}, {"filename": "/tests/metron_good/basic_switch.h", "start": 133555, "end": 134032}, {"filename": "/tests/metron_good/bit_dup.h", "start": 134032, "end": 134691}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 134691, "end": 135350}, {"filename": "/tests/metron_good/local_localparam.h", "start": 135350, "end": 135528}, {"filename": "/tests/metron_good/minimal.h", "start": 135528, "end": 135603}, {"filename": "/tests/metron_good/multi_tick.h", "start": 135603, "end": 135969}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 135969, "end": 136232}, {"filename": "/tests/metron_good/namespaces.h", "start": 136232, "end": 136582}, {"filename": "/tests/metron_good/structs.h", "start": 136582, "end": 136801}, {"filename": "/tests/metron_good/basic_task.h", "start": 136801, "end": 137135}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 137135, "end": 137449}, {"filename": "/tests/metron_good/private_getter.h", "start": 137449, "end": 137673}, {"filename": "/tests/metron_good/init_chain.h", "start": 137673, "end": 138381}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 138381, "end": 138757}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 138757, "end": 139168}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 139168, "end": 139722}, {"filename": "/tests/metron_good/input_signals.h", "start": 139722, "end": 140384}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 140384, "end": 140929}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 140929, "end": 141088}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 141088, "end": 141383}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 141383, "end": 141523}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 141523, "end": 141934}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 141934, "end": 142474}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 142474, "end": 142722}, {"filename": "/tests/metron_bad/README.md", "start": 142722, "end": 142919}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 142919, "end": 143230}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 143230, "end": 143737}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 143737, "end": 144332}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144332, "end": 144592}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 144592, "end": 144835}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 144835, "end": 145131}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 145131, "end": 145577}], "remote_package_size": 145577, "package_uuid": "4afca129-136c-4b05-8f11-971eac31c4e0"});

  })();
