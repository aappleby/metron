
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1416}, {"filename": "/examples/rvtiny/README.md", "start": 1416, "end": 1580}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 1580, "end": 7676}, {"filename": "/examples/rvsimple/README.md", "start": 7676, "end": 7755}, {"filename": "/examples/rvsimple/metron/register.h", "start": 7755, "end": 8440}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 8440, "end": 9547}, {"filename": "/examples/rvsimple/metron/config.h", "start": 9547, "end": 10762}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 10762, "end": 11526}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 11526, "end": 14019}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 14019, "end": 15867}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 15867, "end": 16543}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 16543, "end": 21270}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 21270, "end": 22384}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 22384, "end": 23355}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 23355, "end": 24589}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 24589, "end": 26527}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 26527, "end": 27027}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 27027, "end": 30070}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 30070, "end": 35664}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 35664, "end": 36674}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 36674, "end": 37501}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 37501, "end": 40122}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 40122, "end": 41583}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 41583, "end": 47302}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 47302, "end": 48175}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 48175, "end": 50293}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 50293, "end": 50984}, {"filename": "/examples/pong/README.md", "start": 50984, "end": 51044}, {"filename": "/examples/pong/metron/pong.h", "start": 51044, "end": 54908}, {"filename": "/examples/pong/reference/README.md", "start": 54908, "end": 54968}, {"filename": "/examples/ibex/README.md", "start": 54968, "end": 55019}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 55019, "end": 69429}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 69429, "end": 81527}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 81527, "end": 83112}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 83112, "end": 99136}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 99136, "end": 101637}, {"filename": "/examples/rvtiny_sync/README.md", "start": 101637, "end": 101689}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 101689, "end": 108644}, {"filename": "/examples/uart/README.md", "start": 108644, "end": 108888}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 108888, "end": 110181}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 110181, "end": 111544}, {"filename": "/examples/uart/metron/uart_top.h", "start": 111544, "end": 112604}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 112604, "end": 114450}, {"filename": "/tests/metron_good/include_guards.h", "start": 114450, "end": 114647}, {"filename": "/tests/metron_good/oneliners.h", "start": 114647, "end": 114910}, {"filename": "/tests/metron_good/enum_simple.h", "start": 114910, "end": 116278}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 116278, "end": 116443}, {"filename": "/tests/metron_good/basic_increment.h", "start": 116443, "end": 116798}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 116798, "end": 117636}, {"filename": "/tests/metron_good/magic_comments.h", "start": 117636, "end": 117939}, {"filename": "/tests/metron_good/defines.h", "start": 117939, "end": 118253}, {"filename": "/tests/metron_good/bit_concat.h", "start": 118253, "end": 118680}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 118680, "end": 118860}, {"filename": "/tests/metron_good/force_tick.h", "start": 118860, "end": 119042}, {"filename": "/tests/metron_good/README.md", "start": 119042, "end": 119123}, {"filename": "/tests/metron_good/good_order.h", "start": 119123, "end": 119419}, {"filename": "/tests/metron_good/basic_template.h", "start": 119419, "end": 119850}, {"filename": "/tests/metron_good/bit_casts.h", "start": 119850, "end": 125823}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 125823, "end": 126095}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 126095, "end": 126402}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 126402, "end": 127061}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 127061, "end": 127416}, {"filename": "/tests/metron_good/tock_task.h", "start": 127416, "end": 127772}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 127772, "end": 127992}, {"filename": "/tests/metron_good/all_func_types.h", "start": 127992, "end": 129557}, {"filename": "/tests/metron_good/basic_submod.h", "start": 129557, "end": 129846}, {"filename": "/tests/metron_good/dontcare.h", "start": 129846, "end": 130129}, {"filename": "/tests/metron_good/basic_param.h", "start": 130129, "end": 130388}, {"filename": "/tests/metron_good/basic_output.h", "start": 130388, "end": 130649}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 130649, "end": 131403}, {"filename": "/tests/metron_good/basic_function.h", "start": 131403, "end": 131682}, {"filename": "/tests/metron_good/builtins.h", "start": 131682, "end": 132013}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 132013, "end": 132259}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 132259, "end": 132666}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 132666, "end": 132897}, {"filename": "/tests/metron_good/basic_literals.h", "start": 132897, "end": 133509}, {"filename": "/tests/metron_good/basic_switch.h", "start": 133509, "end": 133986}, {"filename": "/tests/metron_good/bit_dup.h", "start": 133986, "end": 134645}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 134645, "end": 135304}, {"filename": "/tests/metron_good/local_localparam.h", "start": 135304, "end": 135482}, {"filename": "/tests/metron_good/minimal.h", "start": 135482, "end": 135557}, {"filename": "/tests/metron_good/multi_tick.h", "start": 135557, "end": 135923}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 135923, "end": 136186}, {"filename": "/tests/metron_good/namespaces.h", "start": 136186, "end": 136536}, {"filename": "/tests/metron_good/structs.h", "start": 136536, "end": 136755}, {"filename": "/tests/metron_good/basic_task.h", "start": 136755, "end": 137089}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 137089, "end": 137403}, {"filename": "/tests/metron_good/private_getter.h", "start": 137403, "end": 137627}, {"filename": "/tests/metron_good/init_chain.h", "start": 137627, "end": 138335}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 138335, "end": 138711}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 138711, "end": 139122}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 139122, "end": 139676}, {"filename": "/tests/metron_good/input_signals.h", "start": 139676, "end": 140338}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 140338, "end": 140883}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 140883, "end": 141042}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 141042, "end": 141337}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 141337, "end": 141477}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 141477, "end": 141888}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 141888, "end": 142428}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 142428, "end": 142676}, {"filename": "/tests/metron_bad/README.md", "start": 142676, "end": 142873}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 142873, "end": 143184}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 143184, "end": 143691}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 143691, "end": 144286}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144286, "end": 144546}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 144546, "end": 144789}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 144789, "end": 145085}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 145085, "end": 145531}], "remote_package_size": 145531, "package_uuid": "880cb9fb-46f3-4d14-a6b4-e45007e1e945"});

  })();
