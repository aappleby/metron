
  var Module = typeof Module !== 'undefined' ? Module : {};

  if (!Module.expectedDataFileDownloads) {
    Module.expectedDataFileDownloads = 0;
  }

  Module.expectedDataFileDownloads++;
  (function() {
    // Do not attempt to redownload the virtual filesystem data when in a pthread or a Wasm Worker context.
    if (Module['ENVIRONMENT_IS_PTHREAD'] || Module['$ww']) return;
    var loadPackage = function(metadata) {

      var PACKAGE_PATH = '';
      if (typeof window === 'object') {
        PACKAGE_PATH = window['encodeURIComponent'](window.location.pathname.toString().substring(0, window.location.pathname.toString().lastIndexOf('/')) + '/');
      } else if (typeof process === 'undefined' && typeof location !== 'undefined') {
        // web worker
        PACKAGE_PATH = encodeURIComponent(location.pathname.toString().substring(0, location.pathname.toString().lastIndexOf('/')) + '/');
      }
      var PACKAGE_NAME = 'docs/demo/examples.data';
      var REMOTE_PACKAGE_BASE = 'examples.data';
      if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
        Module['locateFile'] = Module['locateFilePackage'];
        err('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
      }
      var REMOTE_PACKAGE_NAME = Module['locateFile'] ? Module['locateFile'](REMOTE_PACKAGE_BASE, '') : REMOTE_PACKAGE_BASE;
var REMOTE_PACKAGE_SIZE = metadata['remote_package_size'];

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
            if (Module['setStatus']) Module['setStatus'](`Downloading data... (${loaded}/${total})`);
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
Module['FS_createPath']("/examples", "gb_spu", true, true);
Module['FS_createPath']("/examples/gb_spu", "metron", true, true);
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples/j1", "metron", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/examples/uart", "metron", true, true);
Module['FS_createPath']("/", "tests", true, true);
Module['FS_createPath']("/tests", "metron_bad", true, true);
Module['FS_createPath']("/tests", "metron_good", true, true);

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
          Module['addRunDependency'](`fp ${this.name}`);
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
          Module['removeRunDependency'](`fp ${that.name}`);
          this.requests[this.name] = null;
        }
      };

      var files = metadata['files'];
      for (var i = 0; i < files.length; ++i) {
        new DataRequest(files[i]['start'], files[i]['end'], files[i]['audio'] || 0).open('GET', files[i]['filename']);
      }

      function processPackageData(arrayBuffer) {
        assert(arrayBuffer, 'Loading data file failed.');
        assert(arrayBuffer.constructor.name === ArrayBuffer.name, 'bad input to processPackageData');
        var byteArray = new Uint8Array(arrayBuffer);
        var curr;
        // Reuse the bytearray from the XHR as the source for file reads.
          DataRequest.prototype.byteArray = byteArray;
          var files = metadata['files'];
          for (var i = 0; i < files.length; ++i) {
            DataRequest.prototype.requests[files[i].filename].onload();
          }          Module['removeRunDependency']('datafile_docs/demo/examples.data');

      };
      Module['addRunDependency']('datafile_docs/demo/examples.data');

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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22817}, {"filename": "/examples/ibex/README.md", "start": 22817, "end": 22868}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22868, "end": 24466}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24466, "end": 36565}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36565, "end": 50989}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50989, "end": 67025}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67025, "end": 69539}, {"filename": "/examples/j1/metron/dpram.h", "start": 69539, "end": 69990}, {"filename": "/examples/j1/metron/j1.h", "start": 69990, "end": 74023}, {"filename": "/examples/pong/README.md", "start": 74023, "end": 74083}, {"filename": "/examples/pong/metron/pong.h", "start": 74083, "end": 77960}, {"filename": "/examples/pong/reference/README.md", "start": 77960, "end": 78020}, {"filename": "/examples/rvsimple/README.md", "start": 78020, "end": 78099}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78099, "end": 78612}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78612, "end": 80086}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80086, "end": 82704}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82704, "end": 83932}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83932, "end": 89664}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89664, "end": 90787}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90787, "end": 92733}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92733, "end": 93973}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93973, "end": 95230}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95230, "end": 95912}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95912, "end": 96889}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96889, "end": 99020}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99020, "end": 99797}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99797, "end": 100490}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100490, "end": 101319}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101319, "end": 102331}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102331, "end": 103315}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103315, "end": 104013}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104013, "end": 107069}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107069, "end": 112656}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112656, "end": 115162}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115162, "end": 119902}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119902, "end": 121903}, {"filename": "/examples/scratch.h", "start": 121903, "end": 122593}, {"filename": "/examples/tutorial/adder.h", "start": 122593, "end": 122773}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122773, "end": 123767}, {"filename": "/examples/tutorial/blockram.h", "start": 123767, "end": 124297}, {"filename": "/examples/tutorial/checksum.h", "start": 124297, "end": 125033}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 125033, "end": 125569}, {"filename": "/examples/tutorial/counter.h", "start": 125569, "end": 125718}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125718, "end": 126497}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126497, "end": 127915}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127915, "end": 128037}, {"filename": "/examples/tutorial/submodules.h", "start": 128037, "end": 129154}, {"filename": "/examples/tutorial/templates.h", "start": 129154, "end": 129643}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129643, "end": 129711}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129711, "end": 129752}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129752, "end": 129793}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129793, "end": 129834}, {"filename": "/examples/tutorial/vga.h", "start": 129834, "end": 130994}, {"filename": "/examples/uart/README.md", "start": 130994, "end": 131238}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 131238, "end": 133831}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 133831, "end": 136405}, {"filename": "/examples/uart/metron/uart_top.h", "start": 136405, "end": 138182}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 138182, "end": 141214}, {"filename": "/tests/metron_bad/README.md", "start": 141214, "end": 141411}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141411, "end": 141720}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141720, "end": 141981}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141981, "end": 142237}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142237, "end": 142845}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 142845, "end": 143085}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 143085, "end": 143508}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 143508, "end": 144061}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 144061, "end": 144385}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144385, "end": 144844}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144844, "end": 145117}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 145117, "end": 145402}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 145402, "end": 145922}, {"filename": "/tests/metron_good/README.md", "start": 145922, "end": 146003}, {"filename": "/tests/metron_good/all_func_types.h", "start": 146003, "end": 147675}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 147675, "end": 148095}, {"filename": "/tests/metron_good/basic_function.h", "start": 148095, "end": 148387}, {"filename": "/tests/metron_good/basic_increment.h", "start": 148387, "end": 148755}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 148755, "end": 149063}, {"filename": "/tests/metron_good/basic_literals.h", "start": 149063, "end": 149688}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 149688, "end": 149947}, {"filename": "/tests/metron_good/basic_output.h", "start": 149947, "end": 150221}, {"filename": "/tests/metron_good/basic_param.h", "start": 150221, "end": 150493}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 150493, "end": 150737}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 150737, "end": 150930}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 150930, "end": 151206}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 151206, "end": 151439}, {"filename": "/tests/metron_good/basic_submod.h", "start": 151439, "end": 151741}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 151741, "end": 152109}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 152109, "end": 152498}, {"filename": "/tests/metron_good/basic_switch.h", "start": 152498, "end": 152988}, {"filename": "/tests/metron_good/basic_task.h", "start": 152988, "end": 153335}, {"filename": "/tests/metron_good/basic_template.h", "start": 153335, "end": 153834}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 153834, "end": 154006}, {"filename": "/tests/metron_good/bit_casts.h", "start": 154006, "end": 159992}, {"filename": "/tests/metron_good/bit_concat.h", "start": 159992, "end": 160432}, {"filename": "/tests/metron_good/bit_dup.h", "start": 160432, "end": 161104}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 161104, "end": 161955}, {"filename": "/tests/metron_good/builtins.h", "start": 161955, "end": 162299}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 162299, "end": 162619}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 162619, "end": 163186}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 163186, "end": 164026}, {"filename": "/tests/metron_good/constructor_args.h", "start": 164026, "end": 164498}, {"filename": "/tests/metron_good/defines.h", "start": 164498, "end": 164825}, {"filename": "/tests/metron_good/dontcare.h", "start": 164825, "end": 165118}, {"filename": "/tests/metron_good/enum_simple.h", "start": 165118, "end": 166560}, {"filename": "/tests/metron_good/for_loops.h", "start": 166560, "end": 166893}, {"filename": "/tests/metron_good/good_order.h", "start": 166893, "end": 167202}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 167202, "end": 167626}, {"filename": "/tests/metron_good/include_guards.h", "start": 167626, "end": 167823}, {"filename": "/tests/metron_good/init_chain.h", "start": 167823, "end": 168544}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 168544, "end": 168844}, {"filename": "/tests/metron_good/input_signals.h", "start": 168844, "end": 169519}, {"filename": "/tests/metron_good/local_localparam.h", "start": 169519, "end": 169710}, {"filename": "/tests/metron_good/magic_comments.h", "start": 169710, "end": 170026}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 170026, "end": 170353}, {"filename": "/tests/metron_good/minimal.h", "start": 170353, "end": 170441}, {"filename": "/tests/metron_good/multi_tick.h", "start": 170441, "end": 170820}, {"filename": "/tests/metron_good/namespaces.h", "start": 170820, "end": 171183}, {"filename": "/tests/metron_good/nested_structs.h", "start": 171183, "end": 171611}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171611, "end": 172169}, {"filename": "/tests/metron_good/oneliners.h", "start": 172169, "end": 172445}, {"filename": "/tests/metron_good/plus_equals.h", "start": 172445, "end": 172869}, {"filename": "/tests/metron_good/private_getter.h", "start": 172869, "end": 173106}, {"filename": "/tests/metron_good/structs.h", "start": 173106, "end": 173338}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 173338, "end": 173887}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 173887, "end": 176444}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 176444, "end": 177211}, {"filename": "/tests/metron_good/tock_task.h", "start": 177211, "end": 177580}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 177580, "end": 177758}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 177758, "end": 178430}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 178430, "end": 179102}], "remote_package_size": 179102});

  })();
