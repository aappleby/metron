
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 838}, {"filename": "/examples/rvtiny/README.md", "start": 838, "end": 1002}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 1002, "end": 7092}, {"filename": "/examples/rvsimple/README.md", "start": 7092, "end": 7171}, {"filename": "/examples/rvsimple/metron/register.h", "start": 7171, "end": 7856}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7856, "end": 8963}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8963, "end": 10178}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 10178, "end": 10942}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10942, "end": 13435}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 13435, "end": 15283}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 15283, "end": 15959}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15959, "end": 20686}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20686, "end": 21800}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21800, "end": 22771}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22771, "end": 24005}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 24005, "end": 25943}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25943, "end": 26443}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 26443, "end": 29486}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 29486, "end": 35080}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 35080, "end": 36090}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 36090, "end": 36917}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36917, "end": 39538}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 39538, "end": 40999}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40999, "end": 46718}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46718, "end": 47591}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 47591, "end": 49709}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49709, "end": 50400}, {"filename": "/examples/pong/README.md", "start": 50400, "end": 50460}, {"filename": "/examples/pong/metron/pong.h", "start": 50460, "end": 54324}, {"filename": "/examples/pong/reference/README.md", "start": 54324, "end": 54384}, {"filename": "/examples/ibex/README.md", "start": 54384, "end": 54435}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 54435, "end": 68845}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68845, "end": 80943}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80943, "end": 82528}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 82528, "end": 98552}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 98552, "end": 101053}, {"filename": "/examples/rvtiny_sync/README.md", "start": 101053, "end": 101105}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 101105, "end": 108060}, {"filename": "/examples/uart/README.md", "start": 108060, "end": 108304}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 108304, "end": 109622}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109622, "end": 111005}, {"filename": "/examples/uart/metron/uart_top.h", "start": 111005, "end": 112031}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 112031, "end": 113950}, {"filename": "/tests/metron_good/include_guards.h", "start": 113950, "end": 114147}, {"filename": "/tests/metron_good/oneliners.h", "start": 114147, "end": 114410}, {"filename": "/tests/metron_good/enum_simple.h", "start": 114410, "end": 115778}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115778, "end": 115943}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115943, "end": 116298}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 116298, "end": 117136}, {"filename": "/tests/metron_good/magic_comments.h", "start": 117136, "end": 117439}, {"filename": "/tests/metron_good/defines.h", "start": 117439, "end": 117753}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117753, "end": 118180}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 118180, "end": 118360}, {"filename": "/tests/metron_good/README.md", "start": 118360, "end": 118441}, {"filename": "/tests/metron_good/good_order.h", "start": 118441, "end": 118737}, {"filename": "/tests/metron_good/basic_template.h", "start": 118737, "end": 119168}, {"filename": "/tests/metron_good/bit_casts.h", "start": 119168, "end": 125141}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 125141, "end": 125448}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 125448, "end": 126107}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 126107, "end": 126462}, {"filename": "/tests/metron_good/tock_task.h", "start": 126462, "end": 126818}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 126818, "end": 127038}, {"filename": "/tests/metron_good/all_func_types.h", "start": 127038, "end": 127834}, {"filename": "/tests/metron_good/basic_submod.h", "start": 127834, "end": 128123}, {"filename": "/tests/metron_good/dontcare.h", "start": 128123, "end": 128406}, {"filename": "/tests/metron_good/basic_param.h", "start": 128406, "end": 128665}, {"filename": "/tests/metron_good/basic_output.h", "start": 128665, "end": 128926}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 128926, "end": 129680}, {"filename": "/tests/metron_good/basic_function.h", "start": 129680, "end": 129959}, {"filename": "/tests/metron_good/builtins.h", "start": 129959, "end": 130290}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 130290, "end": 130536}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 130536, "end": 130943}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 130943, "end": 131174}, {"filename": "/tests/metron_good/basic_literals.h", "start": 131174, "end": 131786}, {"filename": "/tests/metron_good/basic_switch.h", "start": 131786, "end": 132263}, {"filename": "/tests/metron_good/bit_dup.h", "start": 132263, "end": 132922}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 132922, "end": 133581}, {"filename": "/tests/metron_good/local_localparam.h", "start": 133581, "end": 133759}, {"filename": "/tests/metron_good/minimal.h", "start": 133759, "end": 133834}, {"filename": "/tests/metron_good/multi_tick.h", "start": 133834, "end": 134200}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 134200, "end": 134463}, {"filename": "/tests/metron_good/namespaces.h", "start": 134463, "end": 134813}, {"filename": "/tests/metron_good/structs.h", "start": 134813, "end": 135032}, {"filename": "/tests/metron_good/basic_task.h", "start": 135032, "end": 135366}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 135366, "end": 135680}, {"filename": "/tests/metron_good/private_getter.h", "start": 135680, "end": 135904}, {"filename": "/tests/metron_good/init_chain.h", "start": 135904, "end": 136612}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 136612, "end": 136988}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 136988, "end": 137399}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 137399, "end": 137953}, {"filename": "/tests/metron_good/input_signals.h", "start": 137953, "end": 138615}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 138615, "end": 139160}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 139160, "end": 139319}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 139319, "end": 139614}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 139614, "end": 139754}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 139754, "end": 140165}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 140165, "end": 140705}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 140705, "end": 140953}, {"filename": "/tests/metron_bad/README.md", "start": 140953, "end": 141150}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 141150, "end": 141461}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 141461, "end": 141968}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 141968, "end": 142240}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142240, "end": 142835}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 142835, "end": 143095}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 143095, "end": 143338}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 143338, "end": 143634}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 143634, "end": 144080}], "remote_package_size": 144080, "package_uuid": "c034a097-0314-42f8-9c33-09f3763ac37c"});

  })();
