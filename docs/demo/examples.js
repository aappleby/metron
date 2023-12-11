
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/pong/README.md", "start": 71623, "end": 71683}, {"filename": "/examples/pong/metron/pong.h", "start": 71683, "end": 75674}, {"filename": "/examples/pong/reference/README.md", "start": 75674, "end": 75734}, {"filename": "/examples/rvsimple/README.md", "start": 75734, "end": 75813}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 75813, "end": 76320}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 76320, "end": 77788}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 77788, "end": 80400}, {"filename": "/examples/rvsimple/metron/config.h", "start": 80400, "end": 81622}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 81622, "end": 87348}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 87348, "end": 88465}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 88465, "end": 90405}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 90405, "end": 91645}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 91645, "end": 92896}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 92896, "end": 93575}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 93575, "end": 94546}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 94546, "end": 96671}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 96671, "end": 97442}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 97442, "end": 98129}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 98129, "end": 98952}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 98952, "end": 99958}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 99958, "end": 100936}, {"filename": "/examples/rvsimple/metron/register.h", "start": 100936, "end": 101632}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 101632, "end": 104682}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 104682, "end": 110263}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 110263, "end": 112763}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 112763, "end": 117501}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 117501, "end": 119496}, {"filename": "/examples/scratch.h", "start": 119496, "end": 119811}, {"filename": "/examples/tutorial/adder.h", "start": 119811, "end": 119993}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 119993, "end": 120981}, {"filename": "/examples/tutorial/blockram.h", "start": 120981, "end": 121509}, {"filename": "/examples/tutorial/checksum.h", "start": 121509, "end": 122251}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 122251, "end": 122827}, {"filename": "/examples/tutorial/counter.h", "start": 122827, "end": 122978}, {"filename": "/examples/tutorial/declaration_order.h", "start": 122978, "end": 123804}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 123804, "end": 125232}, {"filename": "/examples/tutorial/nonblocking.h", "start": 125232, "end": 125358}, {"filename": "/examples/tutorial/submodules.h", "start": 125358, "end": 126477}, {"filename": "/examples/tutorial/templates.h", "start": 126477, "end": 126960}, {"filename": "/examples/tutorial/tutorial2.h", "start": 126960, "end": 127028}, {"filename": "/examples/tutorial/tutorial3.h", "start": 127028, "end": 127069}, {"filename": "/examples/tutorial/tutorial4.h", "start": 127069, "end": 127110}, {"filename": "/examples/tutorial/tutorial5.h", "start": 127110, "end": 127151}, {"filename": "/examples/tutorial/vga.h", "start": 127151, "end": 128331}, {"filename": "/examples/uart/README.md", "start": 128331, "end": 128575}, {"filename": "/examples/uart/message.txt", "start": 128575, "end": 129087}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 129087, "end": 131796}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 131796, "end": 134487}, {"filename": "/examples/uart/metron/uart_top.h", "start": 134487, "end": 136275}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 136275, "end": 139332}, {"filename": "/tests/metron_bad/README.md", "start": 139332, "end": 139529}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 139529, "end": 139836}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 139836, "end": 140091}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 140091, "end": 140341}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 140341, "end": 140947}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 140947, "end": 141195}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 141195, "end": 141612}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 141612, "end": 142163}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 142163, "end": 142481}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 142481, "end": 142934}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 142934, "end": 143191}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 143191, "end": 143473}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 143473, "end": 143991}, {"filename": "/tests/metron_bad/unorderable_ticks.h", "start": 143991, "end": 144227}, {"filename": "/tests/metron_bad/unorderable_tocks.h", "start": 144227, "end": 144457}, {"filename": "/tests/metron_bad/wrong_submod_call_order.h", "start": 144457, "end": 144829}, {"filename": "/tests/metron_good/README.md", "start": 144829, "end": 144910}, {"filename": "/tests/metron_good/all_func_types.h", "start": 144910, "end": 146594}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 146594, "end": 147011}, {"filename": "/tests/metron_good/basic_function.h", "start": 147011, "end": 147300}, {"filename": "/tests/metron_good/basic_increment.h", "start": 147300, "end": 147670}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 147670, "end": 147975}, {"filename": "/tests/metron_good/basic_literals.h", "start": 147975, "end": 148594}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 148594, "end": 148850}, {"filename": "/tests/metron_good/basic_output.h", "start": 148850, "end": 149122}, {"filename": "/tests/metron_good/basic_param.h", "start": 149122, "end": 149391}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 149391, "end": 149632}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 149632, "end": 149819}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 149819, "end": 150093}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 150093, "end": 150320}, {"filename": "/tests/metron_good/basic_submod.h", "start": 150320, "end": 150636}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 150636, "end": 151001}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 151001, "end": 151387}, {"filename": "/tests/metron_good/basic_switch.h", "start": 151387, "end": 151876}, {"filename": "/tests/metron_good/basic_task.h", "start": 151876, "end": 152224}, {"filename": "/tests/metron_good/basic_template.h", "start": 152224, "end": 152705}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 152705, "end": 152871}, {"filename": "/tests/metron_good/bit_casts.h", "start": 152871, "end": 158851}, {"filename": "/tests/metron_good/bit_concat.h", "start": 158851, "end": 159285}, {"filename": "/tests/metron_good/bit_dup.h", "start": 159285, "end": 159951}, {"filename": "/tests/metron_good/bitfields.h", "start": 159951, "end": 161157}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 161157, "end": 162067}, {"filename": "/tests/metron_good/builtins.h", "start": 162067, "end": 162405}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 162405, "end": 162722}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 162722, "end": 163287}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 163287, "end": 164150}, {"filename": "/tests/metron_good/constructor_args.h", "start": 164150, "end": 164622}, {"filename": "/tests/metron_good/counter.h", "start": 164622, "end": 165269}, {"filename": "/tests/metron_good/defines.h", "start": 165269, "end": 165590}, {"filename": "/tests/metron_good/dontcare.h", "start": 165590, "end": 165877}, {"filename": "/tests/metron_good/enum_simple.h", "start": 165877, "end": 167272}, {"filename": "/tests/metron_good/for_loops.h", "start": 167272, "end": 167599}, {"filename": "/tests/metron_good/good_order.h", "start": 167599, "end": 167802}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 167802, "end": 168223}, {"filename": "/tests/metron_good/include_guards.h", "start": 168223, "end": 168420}, {"filename": "/tests/metron_good/init_chain.h", "start": 168420, "end": 169155}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 169155, "end": 169447}, {"filename": "/tests/metron_good/input_signals.h", "start": 169447, "end": 170245}, {"filename": "/tests/metron_good/local_localparam.h", "start": 170245, "end": 170430}, {"filename": "/tests/metron_good/magic_comments.h", "start": 170430, "end": 170743}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 170743, "end": 171064}, {"filename": "/tests/metron_good/minimal.h", "start": 171064, "end": 171244}, {"filename": "/tests/metron_good/multi_tick.h", "start": 171244, "end": 171623}, {"filename": "/tests/metron_good/namespaces.h", "start": 171623, "end": 171983}, {"filename": "/tests/metron_good/nested_structs.h", "start": 171983, "end": 172444}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 172444, "end": 172996}, {"filename": "/tests/metron_good/noconvert.h", "start": 172996, "end": 173326}, {"filename": "/tests/metron_good/oneliners.h", "start": 173326, "end": 173600}, {"filename": "/tests/metron_good/plus_equals.h", "start": 173600, "end": 174087}, {"filename": "/tests/metron_good/private_getter.h", "start": 174087, "end": 174318}, {"filename": "/tests/metron_good/self_reference.h", "start": 174318, "end": 174523}, {"filename": "/tests/metron_good/slice.h", "start": 174523, "end": 175030}, {"filename": "/tests/metron_good/structs.h", "start": 175030, "end": 175495}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 175495, "end": 176040}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 176040, "end": 178660}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 178660, "end": 179421}, {"filename": "/tests/metron_good/tock_task.h", "start": 179421, "end": 179796}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 179796, "end": 179968}, {"filename": "/tests/metron_good/unions.h", "start": 179968, "end": 180174}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 180174, "end": 180843}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 180843, "end": 181512}], "remote_package_size": 181512});

  })();
