
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 353}, {"filename": "/examples/rvtiny/README.md", "start": 353, "end": 517}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 517, "end": 6607}, {"filename": "/examples/rvsimple/README.md", "start": 6607, "end": 6686}, {"filename": "/examples/rvsimple/metron/register.h", "start": 6686, "end": 7371}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7371, "end": 8478}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8478, "end": 9693}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 9693, "end": 10457}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10457, "end": 12950}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 12950, "end": 14798}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 14798, "end": 15474}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15474, "end": 20201}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20201, "end": 21315}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21315, "end": 22286}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22286, "end": 23520}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23520, "end": 25458}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25458, "end": 25958}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 25958, "end": 29001}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 29001, "end": 34595}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 34595, "end": 35605}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 35605, "end": 36432}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36432, "end": 39053}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 39053, "end": 40514}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40514, "end": 46233}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46233, "end": 47106}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 47106, "end": 49224}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49224, "end": 49915}, {"filename": "/examples/pong/README.md", "start": 49915, "end": 49975}, {"filename": "/examples/pong/metron/pong.h", "start": 49975, "end": 53839}, {"filename": "/examples/pong/reference/README.md", "start": 53839, "end": 53899}, {"filename": "/examples/ibex/README.md", "start": 53899, "end": 53950}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 53950, "end": 68360}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68360, "end": 80458}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80458, "end": 82043}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 82043, "end": 98067}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 98067, "end": 100568}, {"filename": "/examples/rvtiny_sync/README.md", "start": 100568, "end": 100620}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 100620, "end": 107575}, {"filename": "/examples/uart/README.md", "start": 107575, "end": 107819}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 107819, "end": 109137}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109137, "end": 110520}, {"filename": "/examples/uart/metron/uart_top.h", "start": 110520, "end": 111546}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 111546, "end": 113465}, {"filename": "/tests/metron_good/include_guards.h", "start": 113465, "end": 113662}, {"filename": "/tests/metron_good/oneliners.h", "start": 113662, "end": 113925}, {"filename": "/tests/metron_good/enum_simple.h", "start": 113925, "end": 115293}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115293, "end": 115458}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115458, "end": 115826}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 115826, "end": 116495}, {"filename": "/tests/metron_good/magic_comments.h", "start": 116495, "end": 116796}, {"filename": "/tests/metron_good/defines.h", "start": 116796, "end": 117110}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117110, "end": 117514}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 117514, "end": 117694}, {"filename": "/tests/metron_good/README.md", "start": 117694, "end": 117775}, {"filename": "/tests/metron_good/good_order.h", "start": 117775, "end": 118071}, {"filename": "/tests/metron_good/basic_template.h", "start": 118071, "end": 118506}, {"filename": "/tests/metron_good/bit_casts.h", "start": 118506, "end": 124412}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 124412, "end": 124719}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 124719, "end": 125375}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 125375, "end": 125730}, {"filename": "/tests/metron_good/tock_task.h", "start": 125730, "end": 126058}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 126058, "end": 126278}, {"filename": "/tests/metron_good/basic_submod.h", "start": 126278, "end": 126567}, {"filename": "/tests/metron_good/dontcare.h", "start": 126567, "end": 126815}, {"filename": "/tests/metron_good/basic_param.h", "start": 126815, "end": 127074}, {"filename": "/tests/metron_good/basic_output.h", "start": 127074, "end": 127340}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 127340, "end": 128119}, {"filename": "/tests/metron_good/basic_function.h", "start": 128119, "end": 128398}, {"filename": "/tests/metron_good/builtins.h", "start": 128398, "end": 128704}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 128704, "end": 128950}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 128950, "end": 129354}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 129354, "end": 129585}, {"filename": "/tests/metron_good/basic_literals.h", "start": 129585, "end": 130171}, {"filename": "/tests/metron_good/basic_switch.h", "start": 130171, "end": 130648}, {"filename": "/tests/metron_good/bit_dup.h", "start": 130648, "end": 131281}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 131281, "end": 131937}, {"filename": "/tests/metron_good/local_localparam.h", "start": 131937, "end": 132115}, {"filename": "/tests/metron_good/minimal.h", "start": 132115, "end": 132190}, {"filename": "/tests/metron_good/multi_tick.h", "start": 132190, "end": 132556}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 132556, "end": 132819}, {"filename": "/tests/metron_good/namespaces.h", "start": 132819, "end": 133087}, {"filename": "/tests/metron_good/structs.h", "start": 133087, "end": 133557}, {"filename": "/tests/metron_good/basic_task.h", "start": 133557, "end": 133921}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 133921, "end": 134241}, {"filename": "/tests/metron_good/private_getter.h", "start": 134241, "end": 134439}, {"filename": "/tests/metron_good/init_chain.h", "start": 134439, "end": 135133}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 135133, "end": 135514}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 135514, "end": 135925}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 135925, "end": 136479}, {"filename": "/tests/metron_good/input_signals.h", "start": 136479, "end": 137141}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 137141, "end": 137706}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 137706, "end": 137836}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 137836, "end": 138131}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 138131, "end": 138271}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 138271, "end": 138605}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 138605, "end": 139145}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 139145, "end": 139393}, {"filename": "/tests/metron_bad/README.md", "start": 139393, "end": 139590}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 139590, "end": 139901}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 139901, "end": 140408}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 140408, "end": 140680}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 140680, "end": 141275}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 141275, "end": 141535}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141535, "end": 141778}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141778, "end": 142074}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 142074, "end": 142520}], "remote_package_size": 142520, "package_uuid": "3d53132d-dba6-409e-b518-151e7a2c2c7b"});

  })();
