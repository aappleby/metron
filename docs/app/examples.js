
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 4047}, {"filename": "/examples/rvtiny/README.md", "start": 4047, "end": 4211}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 4211, "end": 10301}, {"filename": "/examples/rvsimple/README.md", "start": 10301, "end": 10380}, {"filename": "/examples/rvsimple/metron/register.h", "start": 10380, "end": 11065}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 11065, "end": 12172}, {"filename": "/examples/rvsimple/metron/config.h", "start": 12172, "end": 13387}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 13387, "end": 14151}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 14151, "end": 16644}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 16644, "end": 18492}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 18492, "end": 19168}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 19168, "end": 23895}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 23895, "end": 25009}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 25009, "end": 25980}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 25980, "end": 27214}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 27214, "end": 29152}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 29152, "end": 29652}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 29652, "end": 32695}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 32695, "end": 38289}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 38289, "end": 39299}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 39299, "end": 40126}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 40126, "end": 42747}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 42747, "end": 44208}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 44208, "end": 49927}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 49927, "end": 50800}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 50800, "end": 52918}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 52918, "end": 53609}, {"filename": "/examples/pong/README.md", "start": 53609, "end": 53669}, {"filename": "/examples/pong/metron/pong.h", "start": 53669, "end": 57533}, {"filename": "/examples/pong/reference/README.md", "start": 57533, "end": 57593}, {"filename": "/examples/ibex/README.md", "start": 57593, "end": 57644}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 57644, "end": 72054}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 72054, "end": 84152}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 84152, "end": 85737}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 85737, "end": 101761}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 101761, "end": 104262}, {"filename": "/examples/rvtiny_sync/README.md", "start": 104262, "end": 104314}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 104314, "end": 111269}, {"filename": "/examples/uart/README.md", "start": 111269, "end": 111513}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 111513, "end": 112831}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 112831, "end": 114214}, {"filename": "/examples/uart/metron/uart_top.h", "start": 114214, "end": 115240}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 115240, "end": 117159}, {"filename": "/tests/metron_good/include_guards.h", "start": 117159, "end": 117356}, {"filename": "/tests/metron_good/oneliners.h", "start": 117356, "end": 117619}, {"filename": "/tests/metron_good/enum_simple.h", "start": 117619, "end": 118987}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 118987, "end": 119152}, {"filename": "/tests/metron_good/basic_increment.h", "start": 119152, "end": 119507}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 119507, "end": 120176}, {"filename": "/tests/metron_good/magic_comments.h", "start": 120176, "end": 120479}, {"filename": "/tests/metron_good/defines.h", "start": 120479, "end": 120793}, {"filename": "/tests/metron_good/bit_concat.h", "start": 120793, "end": 121220}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 121220, "end": 121400}, {"filename": "/tests/metron_good/README.md", "start": 121400, "end": 121481}, {"filename": "/tests/metron_good/good_order.h", "start": 121481, "end": 121777}, {"filename": "/tests/metron_good/basic_template.h", "start": 121777, "end": 122208}, {"filename": "/tests/metron_good/bit_casts.h", "start": 122208, "end": 128181}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 128181, "end": 128488}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 128488, "end": 129144}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 129144, "end": 129499}, {"filename": "/tests/metron_good/tock_task.h", "start": 129499, "end": 129855}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 129855, "end": 130075}, {"filename": "/tests/metron_good/all_func_types.h", "start": 130075, "end": 130871}, {"filename": "/tests/metron_good/basic_submod.h", "start": 130871, "end": 131160}, {"filename": "/tests/metron_good/dontcare.h", "start": 131160, "end": 131408}, {"filename": "/tests/metron_good/basic_param.h", "start": 131408, "end": 131667}, {"filename": "/tests/metron_good/basic_output.h", "start": 131667, "end": 131933}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 131933, "end": 132712}, {"filename": "/tests/metron_good/basic_function.h", "start": 132712, "end": 132991}, {"filename": "/tests/metron_good/builtins.h", "start": 132991, "end": 133322}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 133322, "end": 133568}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 133568, "end": 133972}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 133972, "end": 134203}, {"filename": "/tests/metron_good/basic_literals.h", "start": 134203, "end": 134815}, {"filename": "/tests/metron_good/basic_switch.h", "start": 134815, "end": 135292}, {"filename": "/tests/metron_good/bit_dup.h", "start": 135292, "end": 135951}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 135951, "end": 136607}, {"filename": "/tests/metron_good/local_localparam.h", "start": 136607, "end": 136785}, {"filename": "/tests/metron_good/minimal.h", "start": 136785, "end": 136860}, {"filename": "/tests/metron_good/multi_tick.h", "start": 136860, "end": 137226}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 137226, "end": 137489}, {"filename": "/tests/metron_good/namespaces.h", "start": 137489, "end": 137757}, {"filename": "/tests/metron_good/structs.h", "start": 137757, "end": 137976}, {"filename": "/tests/metron_good/basic_task.h", "start": 137976, "end": 138310}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 138310, "end": 138630}, {"filename": "/tests/metron_good/private_getter.h", "start": 138630, "end": 138828}, {"filename": "/tests/metron_good/init_chain.h", "start": 138828, "end": 139536}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 139536, "end": 139917}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 139917, "end": 140328}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 140328, "end": 140882}, {"filename": "/tests/metron_good/input_signals.h", "start": 140882, "end": 141544}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 141544, "end": 142109}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 142109, "end": 142239}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 142239, "end": 142534}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 142534, "end": 142674}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 142674, "end": 143008}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 143008, "end": 143548}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 143548, "end": 143796}, {"filename": "/tests/metron_bad/README.md", "start": 143796, "end": 143993}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 143993, "end": 144304}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 144304, "end": 144811}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 144811, "end": 145083}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 145083, "end": 145678}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 145678, "end": 145938}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 145938, "end": 146181}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 146181, "end": 146477}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 146477, "end": 146923}], "remote_package_size": 146923, "package_uuid": "af7ce5d2-fa1f-41ad-8d49-71c0d811e18f"});

  })();
