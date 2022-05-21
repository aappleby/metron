
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 192}, {"filename": "/examples/rvtiny/README.md", "start": 192, "end": 356}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 356, "end": 6452}, {"filename": "/examples/rvsimple/README.md", "start": 6452, "end": 6531}, {"filename": "/examples/rvsimple/metron/register.h", "start": 6531, "end": 7216}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7216, "end": 8323}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8323, "end": 9538}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 9538, "end": 10302}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10302, "end": 12795}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 12795, "end": 14643}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 14643, "end": 15319}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15319, "end": 20046}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20046, "end": 21160}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21160, "end": 22131}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22131, "end": 23365}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23365, "end": 25303}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25303, "end": 25803}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 25803, "end": 28846}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 28846, "end": 34440}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 34440, "end": 35450}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 35450, "end": 36277}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36277, "end": 38898}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 38898, "end": 40359}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40359, "end": 46078}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46078, "end": 46951}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 46951, "end": 49069}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49069, "end": 49760}, {"filename": "/examples/pong/README.md", "start": 49760, "end": 49820}, {"filename": "/examples/pong/metron/pong.h", "start": 49820, "end": 53684}, {"filename": "/examples/pong/reference/README.md", "start": 53684, "end": 53744}, {"filename": "/examples/ibex/README.md", "start": 53744, "end": 53795}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 53795, "end": 68205}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68205, "end": 80303}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80303, "end": 81888}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 81888, "end": 97912}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 97912, "end": 100413}, {"filename": "/examples/rvtiny_sync/README.md", "start": 100413, "end": 100465}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 100465, "end": 107420}, {"filename": "/examples/uart/README.md", "start": 107420, "end": 107664}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 107664, "end": 108957}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 108957, "end": 110320}, {"filename": "/examples/uart/metron/uart_top.h", "start": 110320, "end": 111380}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 111380, "end": 113226}, {"filename": "/tests/metron_good/include_guards.h", "start": 113226, "end": 113423}, {"filename": "/tests/metron_good/oneliners.h", "start": 113423, "end": 113686}, {"filename": "/tests/metron_good/enum_simple.h", "start": 113686, "end": 115054}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115054, "end": 115219}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115219, "end": 115574}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 115574, "end": 116412}, {"filename": "/tests/metron_good/magic_comments.h", "start": 116412, "end": 116715}, {"filename": "/tests/metron_good/defines.h", "start": 116715, "end": 117029}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117029, "end": 117456}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 117456, "end": 117636}, {"filename": "/tests/metron_good/force_tick.h", "start": 117636, "end": 117818}, {"filename": "/tests/metron_good/README.md", "start": 117818, "end": 117899}, {"filename": "/tests/metron_good/good_order.h", "start": 117899, "end": 118195}, {"filename": "/tests/metron_good/basic_template.h", "start": 118195, "end": 118626}, {"filename": "/tests/metron_good/bit_casts.h", "start": 118626, "end": 124599}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 124599, "end": 124906}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 124906, "end": 125565}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 125565, "end": 125920}, {"filename": "/tests/metron_good/tock_task.h", "start": 125920, "end": 126276}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 126276, "end": 126496}, {"filename": "/tests/metron_good/all_func_types.h", "start": 126496, "end": 128061}, {"filename": "/tests/metron_good/basic_submod.h", "start": 128061, "end": 128350}, {"filename": "/tests/metron_good/dontcare.h", "start": 128350, "end": 128633}, {"filename": "/tests/metron_good/basic_param.h", "start": 128633, "end": 128892}, {"filename": "/tests/metron_good/basic_output.h", "start": 128892, "end": 129153}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 129153, "end": 129907}, {"filename": "/tests/metron_good/basic_function.h", "start": 129907, "end": 130186}, {"filename": "/tests/metron_good/builtins.h", "start": 130186, "end": 130517}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 130517, "end": 130763}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 130763, "end": 131170}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 131170, "end": 131401}, {"filename": "/tests/metron_good/basic_literals.h", "start": 131401, "end": 132013}, {"filename": "/tests/metron_good/basic_switch.h", "start": 132013, "end": 132490}, {"filename": "/tests/metron_good/bit_dup.h", "start": 132490, "end": 133149}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 133149, "end": 133808}, {"filename": "/tests/metron_good/local_localparam.h", "start": 133808, "end": 133986}, {"filename": "/tests/metron_good/minimal.h", "start": 133986, "end": 134061}, {"filename": "/tests/metron_good/multi_tick.h", "start": 134061, "end": 134427}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 134427, "end": 134690}, {"filename": "/tests/metron_good/namespaces.h", "start": 134690, "end": 135040}, {"filename": "/tests/metron_good/structs.h", "start": 135040, "end": 135259}, {"filename": "/tests/metron_good/basic_task.h", "start": 135259, "end": 135593}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 135593, "end": 135907}, {"filename": "/tests/metron_good/private_getter.h", "start": 135907, "end": 136131}, {"filename": "/tests/metron_good/init_chain.h", "start": 136131, "end": 136839}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 136839, "end": 137215}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 137215, "end": 137626}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 137626, "end": 138180}, {"filename": "/tests/metron_good/input_signals.h", "start": 138180, "end": 138842}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 138842, "end": 139387}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 139387, "end": 139546}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 139546, "end": 139841}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 139841, "end": 139981}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 139981, "end": 140392}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 140392, "end": 140932}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 140932, "end": 141180}, {"filename": "/tests/metron_bad/README.md", "start": 141180, "end": 141377}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 141377, "end": 141688}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 141688, "end": 142195}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 142195, "end": 142467}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142467, "end": 143062}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 143062, "end": 143322}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 143322, "end": 143565}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 143565, "end": 143861}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 143861, "end": 144307}], "remote_package_size": 144307, "package_uuid": "774bcbb8-1d5a-44e1-98ab-91a613c9a078"});

  })();
