
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 344}, {"filename": "/examples/rvtiny/README.md", "start": 344, "end": 508}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 508, "end": 6604}, {"filename": "/examples/rvsimple/README.md", "start": 6604, "end": 6683}, {"filename": "/examples/rvsimple/metron/register.h", "start": 6683, "end": 7368}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7368, "end": 8521}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8521, "end": 9736}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 9736, "end": 10500}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10500, "end": 12993}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 12993, "end": 14841}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 14841, "end": 15517}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15517, "end": 20244}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20244, "end": 21358}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21358, "end": 22329}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22329, "end": 23563}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23563, "end": 25501}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25501, "end": 26001}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 26001, "end": 29044}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 29044, "end": 34638}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 34638, "end": 35648}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 35648, "end": 36475}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36475, "end": 39096}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 39096, "end": 40557}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40557, "end": 46276}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46276, "end": 47149}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 47149, "end": 49267}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49267, "end": 49958}, {"filename": "/examples/pong/README.md", "start": 49958, "end": 50018}, {"filename": "/examples/pong/metron/pong.h", "start": 50018, "end": 53882}, {"filename": "/examples/pong/reference/README.md", "start": 53882, "end": 53942}, {"filename": "/examples/ibex/README.md", "start": 53942, "end": 53993}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 53993, "end": 68403}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68403, "end": 80501}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80501, "end": 82086}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 82086, "end": 98110}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 98110, "end": 100611}, {"filename": "/examples/rvtiny_sync/README.md", "start": 100611, "end": 100663}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 100663, "end": 107618}, {"filename": "/examples/uart/README.md", "start": 107618, "end": 107862}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 107862, "end": 109155}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109155, "end": 110518}, {"filename": "/examples/uart/metron/uart_top.h", "start": 110518, "end": 111578}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 111578, "end": 113424}, {"filename": "/tests/metron_good/include_guards.h", "start": 113424, "end": 113621}, {"filename": "/tests/metron_good/oneliners.h", "start": 113621, "end": 113884}, {"filename": "/tests/metron_good/enum_simple.h", "start": 113884, "end": 115252}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115252, "end": 115417}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115417, "end": 115772}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 115772, "end": 116610}, {"filename": "/tests/metron_good/magic_comments.h", "start": 116610, "end": 116913}, {"filename": "/tests/metron_good/for_loops.h", "start": 116913, "end": 117233}, {"filename": "/tests/metron_good/defines.h", "start": 117233, "end": 117547}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117547, "end": 117974}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 117974, "end": 118154}, {"filename": "/tests/metron_good/force_tick.h", "start": 118154, "end": 118336}, {"filename": "/tests/metron_good/README.md", "start": 118336, "end": 118417}, {"filename": "/tests/metron_good/good_order.h", "start": 118417, "end": 118713}, {"filename": "/tests/metron_good/basic_template.h", "start": 118713, "end": 119144}, {"filename": "/tests/metron_good/bit_casts.h", "start": 119144, "end": 125117}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 125117, "end": 125389}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 125389, "end": 125696}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 125696, "end": 126355}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 126355, "end": 126710}, {"filename": "/tests/metron_good/tock_task.h", "start": 126710, "end": 127066}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 127066, "end": 127286}, {"filename": "/tests/metron_good/all_func_types.h", "start": 127286, "end": 128851}, {"filename": "/tests/metron_good/basic_submod.h", "start": 128851, "end": 129140}, {"filename": "/tests/metron_good/dontcare.h", "start": 129140, "end": 129423}, {"filename": "/tests/metron_good/basic_param.h", "start": 129423, "end": 129682}, {"filename": "/tests/metron_good/basic_output.h", "start": 129682, "end": 129943}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 129943, "end": 130697}, {"filename": "/tests/metron_good/basic_function.h", "start": 130697, "end": 130976}, {"filename": "/tests/metron_good/builtins.h", "start": 130976, "end": 131307}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 131307, "end": 131553}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 131553, "end": 131960}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 131960, "end": 132191}, {"filename": "/tests/metron_good/basic_literals.h", "start": 132191, "end": 132803}, {"filename": "/tests/metron_good/basic_switch.h", "start": 132803, "end": 133280}, {"filename": "/tests/metron_good/bit_dup.h", "start": 133280, "end": 133939}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 133939, "end": 134598}, {"filename": "/tests/metron_good/local_localparam.h", "start": 134598, "end": 134776}, {"filename": "/tests/metron_good/minimal.h", "start": 134776, "end": 134851}, {"filename": "/tests/metron_good/multi_tick.h", "start": 134851, "end": 135217}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 135217, "end": 135480}, {"filename": "/tests/metron_good/namespaces.h", "start": 135480, "end": 135830}, {"filename": "/tests/metron_good/structs.h", "start": 135830, "end": 136049}, {"filename": "/tests/metron_good/basic_task.h", "start": 136049, "end": 136383}, {"filename": "/tests/metron_good/nested_structs.h", "start": 136383, "end": 136413}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 136413, "end": 136727}, {"filename": "/tests/metron_good/private_getter.h", "start": 136727, "end": 136951}, {"filename": "/tests/metron_good/init_chain.h", "start": 136951, "end": 137659}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 137659, "end": 138035}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 138035, "end": 138446}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 138446, "end": 139000}, {"filename": "/tests/metron_good/input_signals.h", "start": 139000, "end": 139662}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 139662, "end": 140207}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 140207, "end": 140366}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 140366, "end": 140661}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 140661, "end": 140801}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 140801, "end": 141212}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 141212, "end": 141752}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141752, "end": 142000}, {"filename": "/tests/metron_bad/README.md", "start": 142000, "end": 142197}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 142197, "end": 142508}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 142508, "end": 143015}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 143015, "end": 143610}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 143610, "end": 143870}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 143870, "end": 144113}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 144113, "end": 144409}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144409, "end": 144855}], "remote_package_size": 144855, "package_uuid": "e085eca6-7539-4baa-b959-f1153e747011"});

  })();
