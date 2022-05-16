
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 694}, {"filename": "/examples/rvtiny/README.md", "start": 694, "end": 858}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 858, "end": 6948}, {"filename": "/examples/rvsimple/README.md", "start": 6948, "end": 7027}, {"filename": "/examples/rvsimple/metron/register.h", "start": 7027, "end": 7712}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7712, "end": 8819}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8819, "end": 10034}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 10034, "end": 10798}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10798, "end": 13291}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 13291, "end": 15139}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 15139, "end": 15815}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15815, "end": 20542}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20542, "end": 21656}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21656, "end": 22627}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22627, "end": 23861}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23861, "end": 25799}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25799, "end": 26299}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 26299, "end": 29342}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 29342, "end": 34936}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 34936, "end": 35946}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 35946, "end": 36773}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36773, "end": 39394}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 39394, "end": 40855}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40855, "end": 46574}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46574, "end": 47447}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 47447, "end": 49565}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49565, "end": 50256}, {"filename": "/examples/pong/README.md", "start": 50256, "end": 50316}, {"filename": "/examples/pong/metron/pong.h", "start": 50316, "end": 54180}, {"filename": "/examples/pong/reference/README.md", "start": 54180, "end": 54240}, {"filename": "/examples/ibex/README.md", "start": 54240, "end": 54291}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 54291, "end": 68701}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68701, "end": 80799}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80799, "end": 82384}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 82384, "end": 98408}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 98408, "end": 100909}, {"filename": "/examples/rvtiny_sync/README.md", "start": 100909, "end": 100961}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 100961, "end": 107916}, {"filename": "/examples/uart/README.md", "start": 107916, "end": 108160}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 108160, "end": 109478}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109478, "end": 110861}, {"filename": "/examples/uart/metron/uart_top.h", "start": 110861, "end": 111887}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 111887, "end": 113806}, {"filename": "/tests/metron_good/include_guards.h", "start": 113806, "end": 114003}, {"filename": "/tests/metron_good/oneliners.h", "start": 114003, "end": 114266}, {"filename": "/tests/metron_good/enum_simple.h", "start": 114266, "end": 115634}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115634, "end": 115799}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115799, "end": 116154}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 116154, "end": 116823}, {"filename": "/tests/metron_good/magic_comments.h", "start": 116823, "end": 117124}, {"filename": "/tests/metron_good/defines.h", "start": 117124, "end": 117438}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117438, "end": 117842}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 117842, "end": 118022}, {"filename": "/tests/metron_good/README.md", "start": 118022, "end": 118103}, {"filename": "/tests/metron_good/good_order.h", "start": 118103, "end": 118399}, {"filename": "/tests/metron_good/basic_template.h", "start": 118399, "end": 118834}, {"filename": "/tests/metron_good/bit_casts.h", "start": 118834, "end": 124740}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 124740, "end": 125047}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 125047, "end": 125703}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 125703, "end": 126058}, {"filename": "/tests/metron_good/tock_task.h", "start": 126058, "end": 126386}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 126386, "end": 126606}, {"filename": "/tests/metron_good/basic_submod.h", "start": 126606, "end": 126895}, {"filename": "/tests/metron_good/dontcare.h", "start": 126895, "end": 127143}, {"filename": "/tests/metron_good/basic_param.h", "start": 127143, "end": 127402}, {"filename": "/tests/metron_good/basic_output.h", "start": 127402, "end": 127668}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 127668, "end": 128447}, {"filename": "/tests/metron_good/basic_function.h", "start": 128447, "end": 128726}, {"filename": "/tests/metron_good/builtins.h", "start": 128726, "end": 129032}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 129032, "end": 129278}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 129278, "end": 129682}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 129682, "end": 129913}, {"filename": "/tests/metron_good/basic_literals.h", "start": 129913, "end": 130499}, {"filename": "/tests/metron_good/basic_switch.h", "start": 130499, "end": 130976}, {"filename": "/tests/metron_good/bit_dup.h", "start": 130976, "end": 131609}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 131609, "end": 132265}, {"filename": "/tests/metron_good/local_localparam.h", "start": 132265, "end": 132443}, {"filename": "/tests/metron_good/minimal.h", "start": 132443, "end": 132518}, {"filename": "/tests/metron_good/multi_tick.h", "start": 132518, "end": 132884}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 132884, "end": 133147}, {"filename": "/tests/metron_good/namespaces.h", "start": 133147, "end": 133415}, {"filename": "/tests/metron_good/structs.h", "start": 133415, "end": 133885}, {"filename": "/tests/metron_good/basic_task.h", "start": 133885, "end": 134249}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 134249, "end": 134569}, {"filename": "/tests/metron_good/private_getter.h", "start": 134569, "end": 134767}, {"filename": "/tests/metron_good/init_chain.h", "start": 134767, "end": 135461}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 135461, "end": 135842}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 135842, "end": 136253}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 136253, "end": 136807}, {"filename": "/tests/metron_good/input_signals.h", "start": 136807, "end": 137469}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 137469, "end": 138034}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 138034, "end": 138164}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 138164, "end": 138459}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 138459, "end": 138599}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 138599, "end": 138933}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 138933, "end": 139473}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 139473, "end": 139721}, {"filename": "/tests/metron_bad/README.md", "start": 139721, "end": 139918}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 139918, "end": 140229}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 140229, "end": 140736}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 140736, "end": 141008}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 141008, "end": 141603}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 141603, "end": 141863}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141863, "end": 142106}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 142106, "end": 142402}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 142402, "end": 142848}], "remote_package_size": 142848, "package_uuid": "da53552b-2af9-40a0-bc73-a49a04a00498"});

  })();
