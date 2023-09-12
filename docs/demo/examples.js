
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22817}, {"filename": "/examples/ibex/README.md", "start": 22817, "end": 22868}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22868, "end": 24466}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24466, "end": 36565}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36565, "end": 50989}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50989, "end": 67025}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67025, "end": 69539}, {"filename": "/examples/j1/metron/dpram.h", "start": 69539, "end": 69990}, {"filename": "/examples/j1/metron/j1.h", "start": 69990, "end": 74023}, {"filename": "/examples/pong/README.md", "start": 74023, "end": 74083}, {"filename": "/examples/pong/metron/pong.h", "start": 74083, "end": 77960}, {"filename": "/examples/pong/reference/README.md", "start": 77960, "end": 78020}, {"filename": "/examples/rvsimple/README.md", "start": 78020, "end": 78099}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78099, "end": 78612}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78612, "end": 80086}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80086, "end": 82704}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82704, "end": 83932}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83932, "end": 89664}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89664, "end": 90787}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90787, "end": 92733}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92733, "end": 93973}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93973, "end": 95230}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95230, "end": 95912}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95912, "end": 96889}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96889, "end": 99020}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99020, "end": 99797}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99797, "end": 100490}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100490, "end": 101319}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101319, "end": 102331}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102331, "end": 103315}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103315, "end": 104013}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104013, "end": 107069}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107069, "end": 112656}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112656, "end": 115162}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115162, "end": 119902}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119902, "end": 121903}, {"filename": "/examples/scratch.h", "start": 121903, "end": 124593}, {"filename": "/examples/tutorial/adder.h", "start": 124593, "end": 124773}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 124773, "end": 125767}, {"filename": "/examples/tutorial/blockram.h", "start": 125767, "end": 126297}, {"filename": "/examples/tutorial/checksum.h", "start": 126297, "end": 127033}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 127033, "end": 127569}, {"filename": "/examples/tutorial/counter.h", "start": 127569, "end": 127718}, {"filename": "/examples/tutorial/declaration_order.h", "start": 127718, "end": 128497}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 128497, "end": 129915}, {"filename": "/examples/tutorial/nonblocking.h", "start": 129915, "end": 130037}, {"filename": "/examples/tutorial/submodules.h", "start": 130037, "end": 131154}, {"filename": "/examples/tutorial/templates.h", "start": 131154, "end": 131643}, {"filename": "/examples/tutorial/tutorial2.h", "start": 131643, "end": 131711}, {"filename": "/examples/tutorial/tutorial3.h", "start": 131711, "end": 131752}, {"filename": "/examples/tutorial/tutorial4.h", "start": 131752, "end": 131793}, {"filename": "/examples/tutorial/tutorial5.h", "start": 131793, "end": 131834}, {"filename": "/examples/tutorial/vga.h", "start": 131834, "end": 132994}, {"filename": "/examples/uart/README.md", "start": 132994, "end": 133238}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 133238, "end": 135831}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 135831, "end": 138405}, {"filename": "/examples/uart/metron/uart_top.h", "start": 138405, "end": 140182}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 140182, "end": 143214}, {"filename": "/tests/metron_bad/README.md", "start": 143214, "end": 143411}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 143411, "end": 143720}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 143720, "end": 143981}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 143981, "end": 144237}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 144237, "end": 144845}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 144845, "end": 145085}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 145085, "end": 145508}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 145508, "end": 146061}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 146061, "end": 146385}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 146385, "end": 146844}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 146844, "end": 147117}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 147117, "end": 147402}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 147402, "end": 147922}, {"filename": "/tests/metron_good/README.md", "start": 147922, "end": 148003}, {"filename": "/tests/metron_good/all_func_types.h", "start": 148003, "end": 149675}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 149675, "end": 150095}, {"filename": "/tests/metron_good/basic_function.h", "start": 150095, "end": 150387}, {"filename": "/tests/metron_good/basic_increment.h", "start": 150387, "end": 150755}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 150755, "end": 151063}, {"filename": "/tests/metron_good/basic_literals.h", "start": 151063, "end": 151688}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 151688, "end": 151947}, {"filename": "/tests/metron_good/basic_output.h", "start": 151947, "end": 152221}, {"filename": "/tests/metron_good/basic_param.h", "start": 152221, "end": 152493}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 152493, "end": 152737}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 152737, "end": 152930}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 152930, "end": 153206}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 153206, "end": 153439}, {"filename": "/tests/metron_good/basic_submod.h", "start": 153439, "end": 153758}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 153758, "end": 154126}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 154126, "end": 154515}, {"filename": "/tests/metron_good/basic_switch.h", "start": 154515, "end": 155005}, {"filename": "/tests/metron_good/basic_task.h", "start": 155005, "end": 155352}, {"filename": "/tests/metron_good/basic_template.h", "start": 155352, "end": 155851}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 155851, "end": 156023}, {"filename": "/tests/metron_good/bit_casts.h", "start": 156023, "end": 162009}, {"filename": "/tests/metron_good/bit_concat.h", "start": 162009, "end": 162449}, {"filename": "/tests/metron_good/bit_dup.h", "start": 162449, "end": 163121}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 163121, "end": 163972}, {"filename": "/tests/metron_good/builtins.h", "start": 163972, "end": 164316}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 164316, "end": 164636}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 164636, "end": 165203}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 165203, "end": 166057}, {"filename": "/tests/metron_good/constructor_args.h", "start": 166057, "end": 166529}, {"filename": "/tests/metron_good/defines.h", "start": 166529, "end": 166856}, {"filename": "/tests/metron_good/dontcare.h", "start": 166856, "end": 167149}, {"filename": "/tests/metron_good/enum_simple.h", "start": 167149, "end": 168591}, {"filename": "/tests/metron_good/for_loops.h", "start": 168591, "end": 168924}, {"filename": "/tests/metron_good/good_order.h", "start": 168924, "end": 169233}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 169233, "end": 169657}, {"filename": "/tests/metron_good/include_guards.h", "start": 169657, "end": 169854}, {"filename": "/tests/metron_good/init_chain.h", "start": 169854, "end": 170575}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 170575, "end": 170873}, {"filename": "/tests/metron_good/input_signals.h", "start": 170873, "end": 171548}, {"filename": "/tests/metron_good/local_localparam.h", "start": 171548, "end": 171739}, {"filename": "/tests/metron_good/magic_comments.h", "start": 171739, "end": 172055}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 172055, "end": 172382}, {"filename": "/tests/metron_good/minimal.h", "start": 172382, "end": 172470}, {"filename": "/tests/metron_good/multi_tick.h", "start": 172470, "end": 172849}, {"filename": "/tests/metron_good/namespaces.h", "start": 172849, "end": 173212}, {"filename": "/tests/metron_good/nested_structs.h", "start": 173212, "end": 173640}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 173640, "end": 174198}, {"filename": "/tests/metron_good/oneliners.h", "start": 174198, "end": 174474}, {"filename": "/tests/metron_good/plus_equals.h", "start": 174474, "end": 174898}, {"filename": "/tests/metron_good/private_getter.h", "start": 174898, "end": 175135}, {"filename": "/tests/metron_good/structs.h", "start": 175135, "end": 175367}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 175367, "end": 175916}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 175916, "end": 178473}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 178473, "end": 179240}, {"filename": "/tests/metron_good/tock_task.h", "start": 179240, "end": 179609}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 179609, "end": 179787}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 179787, "end": 180459}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 180459, "end": 181131}], "remote_package_size": 181131});

  })();
