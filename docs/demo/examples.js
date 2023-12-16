
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
Module['FS_createPath']("/examples", "picorv32", true, true);
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/picorv32/picorv32.h", "start": 71623, "end": 166827}, {"filename": "/examples/pong/README.md", "start": 166827, "end": 166887}, {"filename": "/examples/pong/metron/pong.h", "start": 166887, "end": 170878}, {"filename": "/examples/pong/reference/README.md", "start": 170878, "end": 170938}, {"filename": "/examples/rvsimple/README.md", "start": 170938, "end": 171017}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 171017, "end": 171524}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 171524, "end": 172992}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 172992, "end": 175604}, {"filename": "/examples/rvsimple/metron/config.h", "start": 175604, "end": 176826}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 176826, "end": 182552}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 182552, "end": 183669}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 183669, "end": 185609}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 185609, "end": 186849}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 186849, "end": 188100}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 188100, "end": 188779}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 188779, "end": 189750}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 189750, "end": 191875}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 191875, "end": 192646}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 192646, "end": 193333}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 193333, "end": 194156}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 194156, "end": 195162}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 195162, "end": 196140}, {"filename": "/examples/rvsimple/metron/register.h", "start": 196140, "end": 196836}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 196836, "end": 199886}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 199886, "end": 205467}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 205467, "end": 207967}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 207967, "end": 212705}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 212705, "end": 214700}, {"filename": "/examples/scratch.h", "start": 214700, "end": 215126}, {"filename": "/examples/tutorial/adder.h", "start": 215126, "end": 215306}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 215306, "end": 216294}, {"filename": "/examples/tutorial/blockram.h", "start": 216294, "end": 216822}, {"filename": "/examples/tutorial/checksum.h", "start": 216822, "end": 217564}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 217564, "end": 217971}, {"filename": "/examples/tutorial/counter.h", "start": 217971, "end": 218122}, {"filename": "/examples/tutorial/declaration_order.h", "start": 218122, "end": 218948}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 218948, "end": 220376}, {"filename": "/examples/tutorial/nonblocking.h", "start": 220376, "end": 220502}, {"filename": "/examples/tutorial/submodules.h", "start": 220502, "end": 221621}, {"filename": "/examples/tutorial/templates.h", "start": 221621, "end": 222104}, {"filename": "/examples/tutorial/tutorial2.h", "start": 222104, "end": 222172}, {"filename": "/examples/tutorial/tutorial3.h", "start": 222172, "end": 222213}, {"filename": "/examples/tutorial/tutorial4.h", "start": 222213, "end": 222254}, {"filename": "/examples/tutorial/tutorial5.h", "start": 222254, "end": 222295}, {"filename": "/examples/tutorial/vga.h", "start": 222295, "end": 223475}, {"filename": "/examples/uart/README.md", "start": 223475, "end": 223719}, {"filename": "/examples/uart/message.txt", "start": 223719, "end": 224231}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 224231, "end": 226869}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 226869, "end": 229560}, {"filename": "/examples/uart/metron/uart_top.h", "start": 229560, "end": 231348}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 231348, "end": 234405}, {"filename": "/tests/metron_bad/README.md", "start": 234405, "end": 234602}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 234602, "end": 234909}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 234909, "end": 235164}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 235164, "end": 235384}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 235384, "end": 235870}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 235870, "end": 236107}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 236107, "end": 236524}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 236524, "end": 236990}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 236990, "end": 237308}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 237308, "end": 237760}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 237760, "end": 238017}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 238017, "end": 238299}, {"filename": "/tests/metron_bad/unorderable_ticks.h", "start": 238299, "end": 238535}, {"filename": "/tests/metron_bad/unorderable_tocks.h", "start": 238535, "end": 238765}, {"filename": "/tests/metron_bad/wrong_submod_call_order.h", "start": 238765, "end": 239134}, {"filename": "/tests/metron_good/README.md", "start": 239134, "end": 239215}, {"filename": "/tests/metron_good/all_func_types.h", "start": 239215, "end": 240716}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 240716, "end": 241133}, {"filename": "/tests/metron_good/basic_function.h", "start": 241133, "end": 241385}, {"filename": "/tests/metron_good/basic_increment.h", "start": 241385, "end": 241712}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 241712, "end": 242017}, {"filename": "/tests/metron_good/basic_literals.h", "start": 242017, "end": 242636}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 242636, "end": 242892}, {"filename": "/tests/metron_good/basic_output.h", "start": 242892, "end": 243164}, {"filename": "/tests/metron_good/basic_param.h", "start": 243164, "end": 243433}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 243433, "end": 243634}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 243634, "end": 243821}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 243821, "end": 244052}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 244052, "end": 244279}, {"filename": "/tests/metron_good/basic_submod.h", "start": 244279, "end": 244595}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 244595, "end": 244960}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 244960, "end": 245346}, {"filename": "/tests/metron_good/basic_switch.h", "start": 245346, "end": 245835}, {"filename": "/tests/metron_good/basic_task.h", "start": 245835, "end": 246170}, {"filename": "/tests/metron_good/basic_template.h", "start": 246170, "end": 246672}, {"filename": "/tests/metron_good/bit_casts.h", "start": 246672, "end": 252652}, {"filename": "/tests/metron_good/bit_concat.h", "start": 252652, "end": 253086}, {"filename": "/tests/metron_good/bit_dup.h", "start": 253086, "end": 253752}, {"filename": "/tests/metron_good/bitfields.h", "start": 253752, "end": 254958}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 254958, "end": 255775}, {"filename": "/tests/metron_good/builtins.h", "start": 255775, "end": 256113}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 256113, "end": 256430}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 256430, "end": 257015}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 257015, "end": 257878}, {"filename": "/tests/metron_good/constructor_args.h", "start": 257878, "end": 258350}, {"filename": "/tests/metron_good/counter.h", "start": 258350, "end": 258997}, {"filename": "/tests/metron_good/defines.h", "start": 258997, "end": 259318}, {"filename": "/tests/metron_good/dontcare.h", "start": 259318, "end": 259605}, {"filename": "/tests/metron_good/enum_simple.h", "start": 259605, "end": 261000}, {"filename": "/tests/metron_good/for_loops.h", "start": 261000, "end": 261327}, {"filename": "/tests/metron_good/good_order.h", "start": 261327, "end": 261530}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 261530, "end": 261951}, {"filename": "/tests/metron_good/include_guards.h", "start": 261951, "end": 262148}, {"filename": "/tests/metron_good/include_test_module.h", "start": 262148, "end": 262339}, {"filename": "/tests/metron_good/include_test_submod.h", "start": 262339, "end": 262516}, {"filename": "/tests/metron_good/init_chain.h", "start": 262516, "end": 263237}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 263237, "end": 263529}, {"filename": "/tests/metron_good/input_signals.h", "start": 263529, "end": 264325}, {"filename": "/tests/metron_good/local_localparam.h", "start": 264325, "end": 264510}, {"filename": "/tests/metron_good/magic_comments.h", "start": 264510, "end": 264823}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 264823, "end": 265144}, {"filename": "/tests/metron_good/minimal.h", "start": 265144, "end": 265324}, {"filename": "/tests/metron_good/multi_tick.h", "start": 265324, "end": 265700}, {"filename": "/tests/metron_good/namespaces.h", "start": 265700, "end": 266081}, {"filename": "/tests/metron_good/nested_structs.h", "start": 266081, "end": 266542}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 266542, "end": 267115}, {"filename": "/tests/metron_good/noconvert.h", "start": 267115, "end": 267445}, {"filename": "/tests/metron_good/oneliners.h", "start": 267445, "end": 267719}, {"filename": "/tests/metron_good/plus_equals.h", "start": 267719, "end": 268206}, {"filename": "/tests/metron_good/private_getter.h", "start": 268206, "end": 268458}, {"filename": "/tests/metron_good/self_reference.h", "start": 268458, "end": 268663}, {"filename": "/tests/metron_good/slice.h", "start": 268663, "end": 269170}, {"filename": "/tests/metron_good/structs.h", "start": 269170, "end": 269624}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 269624, "end": 270169}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 270169, "end": 272789}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 272789, "end": 273583}, {"filename": "/tests/metron_good/tock_task.h", "start": 273583, "end": 274035}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 274035, "end": 274207}, {"filename": "/tests/metron_good/unions.h", "start": 274207, "end": 274413}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 274413, "end": 275082}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 275082, "end": 275751}], "remote_package_size": 275751});

  })();
