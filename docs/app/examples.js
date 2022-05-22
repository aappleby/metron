
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 452}, {"filename": "/examples/rvtiny/README.md", "start": 452, "end": 616}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 616, "end": 6712}, {"filename": "/examples/rvsimple/README.md", "start": 6712, "end": 6791}, {"filename": "/examples/rvsimple/metron/register.h", "start": 6791, "end": 7476}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7476, "end": 8629}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8629, "end": 9844}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 9844, "end": 10608}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10608, "end": 13101}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 13101, "end": 14949}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 14949, "end": 15625}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15625, "end": 20352}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20352, "end": 21466}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21466, "end": 22437}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22437, "end": 23671}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23671, "end": 25609}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25609, "end": 26109}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 26109, "end": 29152}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 29152, "end": 34746}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 34746, "end": 35756}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 35756, "end": 36583}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36583, "end": 39204}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 39204, "end": 40665}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40665, "end": 46384}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46384, "end": 47257}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 47257, "end": 49375}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49375, "end": 50066}, {"filename": "/examples/pong/README.md", "start": 50066, "end": 50126}, {"filename": "/examples/pong/metron/pong.h", "start": 50126, "end": 53990}, {"filename": "/examples/pong/reference/README.md", "start": 53990, "end": 54050}, {"filename": "/examples/ibex/README.md", "start": 54050, "end": 54101}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 54101, "end": 68511}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68511, "end": 80609}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80609, "end": 82194}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 82194, "end": 98218}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 98218, "end": 100719}, {"filename": "/examples/rvtiny_sync/README.md", "start": 100719, "end": 100771}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 100771, "end": 107726}, {"filename": "/examples/uart/README.md", "start": 107726, "end": 107970}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 107970, "end": 109263}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109263, "end": 110626}, {"filename": "/examples/uart/metron/uart_top.h", "start": 110626, "end": 111686}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 111686, "end": 113532}, {"filename": "/tests/metron_good/include_guards.h", "start": 113532, "end": 113729}, {"filename": "/tests/metron_good/oneliners.h", "start": 113729, "end": 113992}, {"filename": "/tests/metron_good/enum_simple.h", "start": 113992, "end": 115360}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115360, "end": 115525}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115525, "end": 115880}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 115880, "end": 116718}, {"filename": "/tests/metron_good/magic_comments.h", "start": 116718, "end": 117021}, {"filename": "/tests/metron_good/for_loops.h", "start": 117021, "end": 117341}, {"filename": "/tests/metron_good/defines.h", "start": 117341, "end": 117655}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117655, "end": 118082}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 118082, "end": 118262}, {"filename": "/tests/metron_good/force_tick.h", "start": 118262, "end": 118444}, {"filename": "/tests/metron_good/README.md", "start": 118444, "end": 118525}, {"filename": "/tests/metron_good/good_order.h", "start": 118525, "end": 118821}, {"filename": "/tests/metron_good/basic_template.h", "start": 118821, "end": 119252}, {"filename": "/tests/metron_good/bit_casts.h", "start": 119252, "end": 125225}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 125225, "end": 125497}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 125497, "end": 125804}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 125804, "end": 126463}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 126463, "end": 126818}, {"filename": "/tests/metron_good/tock_task.h", "start": 126818, "end": 127174}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 127174, "end": 127394}, {"filename": "/tests/metron_good/all_func_types.h", "start": 127394, "end": 128959}, {"filename": "/tests/metron_good/basic_submod.h", "start": 128959, "end": 129248}, {"filename": "/tests/metron_good/dontcare.h", "start": 129248, "end": 129531}, {"filename": "/tests/metron_good/basic_param.h", "start": 129531, "end": 129790}, {"filename": "/tests/metron_good/basic_output.h", "start": 129790, "end": 130051}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 130051, "end": 130805}, {"filename": "/tests/metron_good/basic_function.h", "start": 130805, "end": 131084}, {"filename": "/tests/metron_good/builtins.h", "start": 131084, "end": 131415}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 131415, "end": 131661}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 131661, "end": 132068}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 132068, "end": 132299}, {"filename": "/tests/metron_good/basic_literals.h", "start": 132299, "end": 132911}, {"filename": "/tests/metron_good/basic_switch.h", "start": 132911, "end": 133388}, {"filename": "/tests/metron_good/bit_dup.h", "start": 133388, "end": 134047}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 134047, "end": 134706}, {"filename": "/tests/metron_good/local_localparam.h", "start": 134706, "end": 134884}, {"filename": "/tests/metron_good/minimal.h", "start": 134884, "end": 134959}, {"filename": "/tests/metron_good/multi_tick.h", "start": 134959, "end": 135325}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 135325, "end": 135588}, {"filename": "/tests/metron_good/namespaces.h", "start": 135588, "end": 135938}, {"filename": "/tests/metron_good/structs.h", "start": 135938, "end": 136157}, {"filename": "/tests/metron_good/basic_task.h", "start": 136157, "end": 136491}, {"filename": "/tests/metron_good/nested_structs.h", "start": 136491, "end": 137004}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 137004, "end": 137318}, {"filename": "/tests/metron_good/private_getter.h", "start": 137318, "end": 137542}, {"filename": "/tests/metron_good/init_chain.h", "start": 137542, "end": 138250}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 138250, "end": 138626}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 138626, "end": 139037}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 139037, "end": 139591}, {"filename": "/tests/metron_good/input_signals.h", "start": 139591, "end": 140253}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 140253, "end": 140798}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 140798, "end": 140957}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 140957, "end": 141252}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 141252, "end": 141392}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 141392, "end": 141803}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 141803, "end": 142343}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 142343, "end": 142591}, {"filename": "/tests/metron_bad/README.md", "start": 142591, "end": 142788}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 142788, "end": 143099}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 143099, "end": 143606}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 143606, "end": 144201}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144201, "end": 144461}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 144461, "end": 144704}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 144704, "end": 145000}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 145000, "end": 145446}], "remote_package_size": 145446, "package_uuid": "18852b15-1f97-4074-8870-c2e99010bd7a"});

  })();
