
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 577}, {"filename": "/examples/rvtiny/README.md", "start": 577, "end": 741}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 741, "end": 6837}, {"filename": "/examples/rvsimple/README.md", "start": 6837, "end": 6916}, {"filename": "/examples/rvsimple/metron/register.h", "start": 6916, "end": 7601}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7601, "end": 8754}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8754, "end": 9969}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 9969, "end": 10733}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10733, "end": 13226}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 13226, "end": 15074}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 15074, "end": 15750}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15750, "end": 20477}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20477, "end": 21591}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21591, "end": 22562}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22562, "end": 23796}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23796, "end": 25734}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25734, "end": 26234}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 26234, "end": 29277}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 29277, "end": 34871}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 34871, "end": 35881}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 35881, "end": 36708}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36708, "end": 39329}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 39329, "end": 40790}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40790, "end": 46509}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46509, "end": 47382}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 47382, "end": 49500}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49500, "end": 50191}, {"filename": "/examples/pong/README.md", "start": 50191, "end": 50251}, {"filename": "/examples/pong/metron/pong.h", "start": 50251, "end": 54115}, {"filename": "/examples/pong/reference/README.md", "start": 54115, "end": 54175}, {"filename": "/examples/ibex/README.md", "start": 54175, "end": 54226}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 54226, "end": 68636}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68636, "end": 80734}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80734, "end": 82319}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 82319, "end": 98343}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 98343, "end": 100844}, {"filename": "/examples/rvtiny_sync/README.md", "start": 100844, "end": 100896}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 100896, "end": 107851}, {"filename": "/examples/uart/README.md", "start": 107851, "end": 108095}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 108095, "end": 109388}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109388, "end": 110751}, {"filename": "/examples/uart/metron/uart_top.h", "start": 110751, "end": 111811}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 111811, "end": 113657}, {"filename": "/tests/metron_good/include_guards.h", "start": 113657, "end": 113854}, {"filename": "/tests/metron_good/oneliners.h", "start": 113854, "end": 114117}, {"filename": "/tests/metron_good/enum_simple.h", "start": 114117, "end": 115485}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115485, "end": 115650}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115650, "end": 116005}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 116005, "end": 116843}, {"filename": "/tests/metron_good/magic_comments.h", "start": 116843, "end": 117146}, {"filename": "/tests/metron_good/for_loops.h", "start": 117146, "end": 117466}, {"filename": "/tests/metron_good/defines.h", "start": 117466, "end": 117780}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117780, "end": 118207}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 118207, "end": 118387}, {"filename": "/tests/metron_good/force_tick.h", "start": 118387, "end": 118569}, {"filename": "/tests/metron_good/README.md", "start": 118569, "end": 118650}, {"filename": "/tests/metron_good/good_order.h", "start": 118650, "end": 118946}, {"filename": "/tests/metron_good/basic_template.h", "start": 118946, "end": 119377}, {"filename": "/tests/metron_good/bit_casts.h", "start": 119377, "end": 125350}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 125350, "end": 125622}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 125622, "end": 125929}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 125929, "end": 126588}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 126588, "end": 126943}, {"filename": "/tests/metron_good/tock_task.h", "start": 126943, "end": 127299}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 127299, "end": 127519}, {"filename": "/tests/metron_good/all_func_types.h", "start": 127519, "end": 129084}, {"filename": "/tests/metron_good/basic_submod.h", "start": 129084, "end": 129373}, {"filename": "/tests/metron_good/dontcare.h", "start": 129373, "end": 129656}, {"filename": "/tests/metron_good/basic_param.h", "start": 129656, "end": 129915}, {"filename": "/tests/metron_good/basic_output.h", "start": 129915, "end": 130176}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 130176, "end": 130930}, {"filename": "/tests/metron_good/basic_function.h", "start": 130930, "end": 131209}, {"filename": "/tests/metron_good/builtins.h", "start": 131209, "end": 131540}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 131540, "end": 131786}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 131786, "end": 132193}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 132193, "end": 132424}, {"filename": "/tests/metron_good/basic_literals.h", "start": 132424, "end": 133036}, {"filename": "/tests/metron_good/basic_switch.h", "start": 133036, "end": 133513}, {"filename": "/tests/metron_good/bit_dup.h", "start": 133513, "end": 134172}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 134172, "end": 134831}, {"filename": "/tests/metron_good/local_localparam.h", "start": 134831, "end": 135009}, {"filename": "/tests/metron_good/minimal.h", "start": 135009, "end": 135084}, {"filename": "/tests/metron_good/multi_tick.h", "start": 135084, "end": 135450}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 135450, "end": 135713}, {"filename": "/tests/metron_good/namespaces.h", "start": 135713, "end": 136063}, {"filename": "/tests/metron_good/structs.h", "start": 136063, "end": 136282}, {"filename": "/tests/metron_good/basic_task.h", "start": 136282, "end": 136616}, {"filename": "/tests/metron_good/nested_structs.h", "start": 136616, "end": 137031}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 137031, "end": 137345}, {"filename": "/tests/metron_good/private_getter.h", "start": 137345, "end": 137569}, {"filename": "/tests/metron_good/init_chain.h", "start": 137569, "end": 138277}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 138277, "end": 138653}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 138653, "end": 139064}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 139064, "end": 139618}, {"filename": "/tests/metron_good/input_signals.h", "start": 139618, "end": 140280}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 140280, "end": 140825}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 140825, "end": 140984}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 140984, "end": 141279}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 141279, "end": 141419}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 141419, "end": 141830}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 141830, "end": 142370}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 142370, "end": 142618}, {"filename": "/tests/metron_bad/README.md", "start": 142618, "end": 142815}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 142815, "end": 143126}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 143126, "end": 143633}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 143633, "end": 144228}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144228, "end": 144488}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 144488, "end": 144731}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 144731, "end": 145027}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 145027, "end": 145473}], "remote_package_size": 145473, "package_uuid": "669dd1d7-e39c-4ba6-8f9c-f25df350a2c4"});

  })();
