
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
      var PACKAGE_NAME = '/home/aappleby/repos/metron/docs/demo/examples.data';
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
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples", "picorv32", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/", "tests", true, true);
Module['FS_createPath']("/tests", "metron", true, true);
Module['FS_createPath']("/tests/metron", "fail", true, true);
Module['FS_createPath']("/tests/metron", "pass", true, true);

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
          }          Module['removeRunDependency']('datafile_/home/aappleby/repos/metron/docs/demo/examples.data');

      };
      Module['addRunDependency']('datafile_/home/aappleby/repos/metron/docs/demo/examples.data');

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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 535}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 535, "end": 20377}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20377, "end": 21145}, {"filename": "/examples/ibex/README.md", "start": 21145, "end": 21196}, {"filename": "/examples/ibex/ibex.hancho", "start": 21196, "end": 21196}, {"filename": "/examples/ibex/ibex_alu.h", "start": 21196, "end": 22835}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22835, "end": 35020}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 35020, "end": 49512}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49512, "end": 65589}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65589, "end": 68174}, {"filename": "/examples/j1/dpram.h", "start": 68174, "end": 68635}, {"filename": "/examples/j1/j1.h", "start": 68635, "end": 72926}, {"filename": "/examples/j1/j1.hancho", "start": 72926, "end": 73619}, {"filename": "/examples/picorv32/picorv32.h", "start": 73619, "end": 168823}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168823, "end": 168823}, {"filename": "/examples/pong/README.md", "start": 168823, "end": 168883}, {"filename": "/examples/pong/pong.h", "start": 168883, "end": 172874}, {"filename": "/examples/pong/pong.hancho", "start": 172874, "end": 173370}, {"filename": "/examples/pong/reference/README.md", "start": 173370, "end": 173430}, {"filename": "/examples/rvsimple/README.md", "start": 173430, "end": 173509}, {"filename": "/examples/rvsimple/adder.h", "start": 173509, "end": 174016}, {"filename": "/examples/rvsimple/alu.h", "start": 174016, "end": 175484}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175484, "end": 178096}, {"filename": "/examples/rvsimple/config.h", "start": 178096, "end": 179318}, {"filename": "/examples/rvsimple/constants.h", "start": 179318, "end": 185044}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 185044, "end": 186161}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 186161, "end": 188101}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 188101, "end": 189341}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 189341, "end": 190592}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190592, "end": 191271}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 191271, "end": 192242}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 192242, "end": 194367}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194367, "end": 195138}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 195138, "end": 195825}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 195825, "end": 196648}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 196648, "end": 197654}, {"filename": "/examples/rvsimple/regfile.h", "start": 197654, "end": 198632}, {"filename": "/examples/rvsimple/register.h", "start": 198632, "end": 199328}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 199328, "end": 202378}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 202378, "end": 205632}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 205632, "end": 211213}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 211213, "end": 213713}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 213713, "end": 218451}, {"filename": "/examples/rvsimple/toplevel.h", "start": 218451, "end": 220446}, {"filename": "/examples/scratch.h", "start": 220446, "end": 220814}, {"filename": "/examples/tutorial/adder.h", "start": 220814, "end": 220994}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 220994, "end": 221982}, {"filename": "/examples/tutorial/blockram.h", "start": 221982, "end": 222514}, {"filename": "/examples/tutorial/checksum.h", "start": 222514, "end": 223256}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 223256, "end": 223663}, {"filename": "/examples/tutorial/counter.h", "start": 223663, "end": 223814}, {"filename": "/examples/tutorial/declaration_order.h", "start": 223814, "end": 224640}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 224640, "end": 225599}, {"filename": "/examples/tutorial/nonblocking.h", "start": 225599, "end": 225725}, {"filename": "/examples/tutorial/submodules.h", "start": 225725, "end": 226825}, {"filename": "/examples/tutorial/templates.h", "start": 226825, "end": 227308}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 227308, "end": 227308}, {"filename": "/examples/tutorial/tutorial2.h", "start": 227308, "end": 227376}, {"filename": "/examples/tutorial/tutorial3.h", "start": 227376, "end": 227417}, {"filename": "/examples/tutorial/tutorial4.h", "start": 227417, "end": 227458}, {"filename": "/examples/tutorial/tutorial5.h", "start": 227458, "end": 227499}, {"filename": "/examples/tutorial/vga.h", "start": 227499, "end": 228679}, {"filename": "/examples/uart/README.md", "start": 228679, "end": 228923}, {"filename": "/examples/uart/uart.hancho", "start": 228923, "end": 231508}, {"filename": "/examples/uart/uart_hello.h", "start": 231508, "end": 234176}, {"filename": "/examples/uart/uart_rx.h", "start": 234176, "end": 236867}, {"filename": "/examples/uart/uart_top.h", "start": 236867, "end": 238743}, {"filename": "/examples/uart/uart_tx.h", "start": 238743, "end": 241800}, {"filename": "/tests/metron/fail/README.md", "start": 241800, "end": 241997}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 241997, "end": 242304}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 242304, "end": 242559}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 242559, "end": 242779}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 242779, "end": 243265}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 243265, "end": 243502}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 243502, "end": 243919}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 243919, "end": 244385}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 244385, "end": 244703}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 244703, "end": 245155}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 245155, "end": 245412}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 245412, "end": 245694}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 245694, "end": 245930}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 245930, "end": 246160}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 246160, "end": 246529}, {"filename": "/tests/metron/pass/README.md", "start": 246529, "end": 246610}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 246610, "end": 248111}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 248111, "end": 248528}, {"filename": "/tests/metron/pass/basic_function.h", "start": 248528, "end": 248780}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 248780, "end": 249107}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 249107, "end": 249412}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 249412, "end": 250031}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 250031, "end": 250287}, {"filename": "/tests/metron/pass/basic_output.h", "start": 250287, "end": 250559}, {"filename": "/tests/metron/pass/basic_param.h", "start": 250559, "end": 250828}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 250828, "end": 251029}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 251029, "end": 251216}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 251216, "end": 251447}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 251447, "end": 251674}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 251674, "end": 251990}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 251990, "end": 252355}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 252355, "end": 252741}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 252741, "end": 253230}, {"filename": "/tests/metron/pass/basic_task.h", "start": 253230, "end": 253565}, {"filename": "/tests/metron/pass/basic_template.h", "start": 253565, "end": 254067}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 254067, "end": 260047}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 260047, "end": 260481}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 260481, "end": 261147}, {"filename": "/tests/metron/pass/bitfields.h", "start": 261147, "end": 262353}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 262353, "end": 263170}, {"filename": "/tests/metron/pass/builtins.h", "start": 263170, "end": 263508}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 263508, "end": 263825}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 263825, "end": 264410}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 264410, "end": 265265}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 265265, "end": 265743}, {"filename": "/tests/metron/pass/counter.h", "start": 265743, "end": 266390}, {"filename": "/tests/metron/pass/defines.h", "start": 266390, "end": 266711}, {"filename": "/tests/metron/pass/dontcare.h", "start": 266711, "end": 266998}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 266998, "end": 268393}, {"filename": "/tests/metron/pass/for_loops.h", "start": 268393, "end": 268720}, {"filename": "/tests/metron/pass/good_order.h", "start": 268720, "end": 268923}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 268923, "end": 269344}, {"filename": "/tests/metron/pass/include_guards.h", "start": 269344, "end": 269541}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 269541, "end": 269732}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 269732, "end": 269909}, {"filename": "/tests/metron/pass/init_chain.h", "start": 269909, "end": 270630}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 270630, "end": 270922}, {"filename": "/tests/metron/pass/input_signals.h", "start": 270922, "end": 271595}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 271595, "end": 271780}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 271780, "end": 272093}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 272093, "end": 272414}, {"filename": "/tests/metron/pass/minimal.h", "start": 272414, "end": 272594}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 272594, "end": 272970}, {"filename": "/tests/metron/pass/namespaces.h", "start": 272970, "end": 273351}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 273351, "end": 273812}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 273812, "end": 274385}, {"filename": "/tests/metron/pass/noconvert.h", "start": 274385, "end": 274715}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 274715, "end": 275068}, {"filename": "/tests/metron/pass/oneliners.h", "start": 275068, "end": 275342}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 275342, "end": 275829}, {"filename": "/tests/metron/pass/preproc.h", "start": 275829, "end": 276157}, {"filename": "/tests/metron/pass/private_getter.h", "start": 276157, "end": 276409}, {"filename": "/tests/metron/pass/self_reference.h", "start": 276409, "end": 276614}, {"filename": "/tests/metron/pass/slice.h", "start": 276614, "end": 277121}, {"filename": "/tests/metron/pass/structs.h", "start": 277121, "end": 277575}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 277575, "end": 278120}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 278120, "end": 280740}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 280740, "end": 281534}, {"filename": "/tests/metron/pass/tock_task.h", "start": 281534, "end": 281986}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 281986, "end": 282158}, {"filename": "/tests/metron/pass/unions.h", "start": 282158, "end": 282364}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 282364, "end": 283033}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 283033, "end": 283702}], "remote_package_size": 283702});

  })();
