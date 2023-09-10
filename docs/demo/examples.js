
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22817}, {"filename": "/examples/ibex/README.md", "start": 22817, "end": 22868}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22868, "end": 24466}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24466, "end": 36565}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36565, "end": 50989}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50989, "end": 67025}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67025, "end": 69539}, {"filename": "/examples/j1/metron/dpram.h", "start": 69539, "end": 69990}, {"filename": "/examples/j1/metron/j1.h", "start": 69990, "end": 74023}, {"filename": "/examples/pong/README.md", "start": 74023, "end": 74083}, {"filename": "/examples/pong/metron/pong.h", "start": 74083, "end": 77960}, {"filename": "/examples/pong/reference/README.md", "start": 77960, "end": 78020}, {"filename": "/examples/rvsimple/README.md", "start": 78020, "end": 78099}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78099, "end": 78612}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78612, "end": 80086}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80086, "end": 82704}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82704, "end": 83932}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83932, "end": 89664}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89664, "end": 90787}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90787, "end": 92733}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92733, "end": 93973}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93973, "end": 95230}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95230, "end": 95912}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95912, "end": 96889}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96889, "end": 99020}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99020, "end": 99797}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99797, "end": 100490}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100490, "end": 101319}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101319, "end": 102331}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102331, "end": 103315}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103315, "end": 104013}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104013, "end": 107069}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107069, "end": 112656}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112656, "end": 115162}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115162, "end": 119902}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119902, "end": 121903}, {"filename": "/examples/scratch.h", "start": 121903, "end": 122276}, {"filename": "/examples/tutorial/adder.h", "start": 122276, "end": 122456}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122456, "end": 123450}, {"filename": "/examples/tutorial/blockram.h", "start": 123450, "end": 123980}, {"filename": "/examples/tutorial/checksum.h", "start": 123980, "end": 124716}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124716, "end": 125252}, {"filename": "/examples/tutorial/counter.h", "start": 125252, "end": 125401}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125401, "end": 126180}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126180, "end": 127598}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127598, "end": 127720}, {"filename": "/examples/tutorial/submodules.h", "start": 127720, "end": 128837}, {"filename": "/examples/tutorial/templates.h", "start": 128837, "end": 129326}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129326, "end": 129394}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129394, "end": 129435}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129435, "end": 129476}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129476, "end": 129517}, {"filename": "/examples/tutorial/vga.h", "start": 129517, "end": 130677}, {"filename": "/examples/uart/README.md", "start": 130677, "end": 130921}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 130921, "end": 133514}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 133514, "end": 136088}, {"filename": "/examples/uart/metron/uart_top.h", "start": 136088, "end": 137865}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 137865, "end": 140897}, {"filename": "/tests/metron_bad/README.md", "start": 140897, "end": 141094}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141094, "end": 141403}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141403, "end": 141664}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141664, "end": 141920}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 141920, "end": 142528}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 142528, "end": 142768}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 142768, "end": 143191}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 143191, "end": 143744}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 143744, "end": 144068}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144068, "end": 144527}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144527, "end": 144800}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 144800, "end": 145085}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 145085, "end": 145605}, {"filename": "/tests/metron_good/README.md", "start": 145605, "end": 145686}, {"filename": "/tests/metron_good/all_func_types.h", "start": 145686, "end": 147358}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 147358, "end": 147778}, {"filename": "/tests/metron_good/basic_function.h", "start": 147778, "end": 148070}, {"filename": "/tests/metron_good/basic_increment.h", "start": 148070, "end": 148438}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 148438, "end": 148746}, {"filename": "/tests/metron_good/basic_literals.h", "start": 148746, "end": 149371}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 149371, "end": 149630}, {"filename": "/tests/metron_good/basic_output.h", "start": 149630, "end": 149904}, {"filename": "/tests/metron_good/basic_param.h", "start": 149904, "end": 150176}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 150176, "end": 150420}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 150420, "end": 150613}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 150613, "end": 150889}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 150889, "end": 151122}, {"filename": "/tests/metron_good/basic_submod.h", "start": 151122, "end": 151424}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 151424, "end": 151792}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 151792, "end": 152181}, {"filename": "/tests/metron_good/basic_switch.h", "start": 152181, "end": 152671}, {"filename": "/tests/metron_good/basic_task.h", "start": 152671, "end": 153018}, {"filename": "/tests/metron_good/basic_template.h", "start": 153018, "end": 153517}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 153517, "end": 153689}, {"filename": "/tests/metron_good/bit_casts.h", "start": 153689, "end": 159675}, {"filename": "/tests/metron_good/bit_concat.h", "start": 159675, "end": 160115}, {"filename": "/tests/metron_good/bit_dup.h", "start": 160115, "end": 160787}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 160787, "end": 161638}, {"filename": "/tests/metron_good/builtins.h", "start": 161638, "end": 161982}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 161982, "end": 162302}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 162302, "end": 162869}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 162869, "end": 163723}, {"filename": "/tests/metron_good/constructor_args.h", "start": 163723, "end": 164195}, {"filename": "/tests/metron_good/defines.h", "start": 164195, "end": 164522}, {"filename": "/tests/metron_good/dontcare.h", "start": 164522, "end": 164815}, {"filename": "/tests/metron_good/enum_simple.h", "start": 164815, "end": 166257}, {"filename": "/tests/metron_good/for_loops.h", "start": 166257, "end": 166590}, {"filename": "/tests/metron_good/good_order.h", "start": 166590, "end": 166899}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 166899, "end": 167323}, {"filename": "/tests/metron_good/include_guards.h", "start": 167323, "end": 167520}, {"filename": "/tests/metron_good/init_chain.h", "start": 167520, "end": 168241}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 168241, "end": 168539}, {"filename": "/tests/metron_good/input_signals.h", "start": 168539, "end": 169214}, {"filename": "/tests/metron_good/local_localparam.h", "start": 169214, "end": 169405}, {"filename": "/tests/metron_good/magic_comments.h", "start": 169405, "end": 169721}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 169721, "end": 170048}, {"filename": "/tests/metron_good/minimal.h", "start": 170048, "end": 170136}, {"filename": "/tests/metron_good/multi_tick.h", "start": 170136, "end": 170515}, {"filename": "/tests/metron_good/namespaces.h", "start": 170515, "end": 170878}, {"filename": "/tests/metron_good/nested_structs.h", "start": 170878, "end": 171306}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171306, "end": 171864}, {"filename": "/tests/metron_good/oneliners.h", "start": 171864, "end": 172140}, {"filename": "/tests/metron_good/plus_equals.h", "start": 172140, "end": 172564}, {"filename": "/tests/metron_good/private_getter.h", "start": 172564, "end": 172801}, {"filename": "/tests/metron_good/structs.h", "start": 172801, "end": 173033}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 173033, "end": 173582}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 173582, "end": 176139}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 176139, "end": 176906}, {"filename": "/tests/metron_good/tock_task.h", "start": 176906, "end": 177275}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 177275, "end": 177453}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 177453, "end": 178125}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 178125, "end": 178797}], "remote_package_size": 178797});

  })();
