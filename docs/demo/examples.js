
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/picorv32/picorv32.h", "start": 71623, "end": 166827}, {"filename": "/examples/pong/README.md", "start": 166827, "end": 166887}, {"filename": "/examples/pong/metron/pong.h", "start": 166887, "end": 170878}, {"filename": "/examples/pong/reference/README.md", "start": 170878, "end": 170938}, {"filename": "/examples/rvsimple/README.md", "start": 170938, "end": 171017}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 171017, "end": 171524}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 171524, "end": 172992}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 172992, "end": 175604}, {"filename": "/examples/rvsimple/metron/config.h", "start": 175604, "end": 176826}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 176826, "end": 182552}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 182552, "end": 183669}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 183669, "end": 185609}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 185609, "end": 186849}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 186849, "end": 188100}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 188100, "end": 188779}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 188779, "end": 189750}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 189750, "end": 191875}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 191875, "end": 192646}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 192646, "end": 193333}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 193333, "end": 194156}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 194156, "end": 195162}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 195162, "end": 196140}, {"filename": "/examples/rvsimple/metron/register.h", "start": 196140, "end": 196836}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 196836, "end": 199886}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 199886, "end": 205467}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 205467, "end": 207967}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 207967, "end": 212705}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 212705, "end": 214700}, {"filename": "/examples/scratch.h", "start": 214700, "end": 215052}, {"filename": "/examples/scratch.sv.d", "start": 215052, "end": 215092}, {"filename": "/examples/tutorial/adder.h", "start": 215092, "end": 215272}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 215272, "end": 216260}, {"filename": "/examples/tutorial/blockram.h", "start": 216260, "end": 216788}, {"filename": "/examples/tutorial/checksum.h", "start": 216788, "end": 217530}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 217530, "end": 217937}, {"filename": "/examples/tutorial/counter.h", "start": 217937, "end": 218088}, {"filename": "/examples/tutorial/declaration_order.h", "start": 218088, "end": 218914}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 218914, "end": 220342}, {"filename": "/examples/tutorial/nonblocking.h", "start": 220342, "end": 220468}, {"filename": "/examples/tutorial/submodules.h", "start": 220468, "end": 221587}, {"filename": "/examples/tutorial/templates.h", "start": 221587, "end": 222070}, {"filename": "/examples/tutorial/tutorial2.h", "start": 222070, "end": 222138}, {"filename": "/examples/tutorial/tutorial3.h", "start": 222138, "end": 222179}, {"filename": "/examples/tutorial/tutorial4.h", "start": 222179, "end": 222220}, {"filename": "/examples/tutorial/tutorial5.h", "start": 222220, "end": 222261}, {"filename": "/examples/tutorial/vga.h", "start": 222261, "end": 223441}, {"filename": "/examples/uart/README.md", "start": 223441, "end": 223685}, {"filename": "/examples/uart/message.txt", "start": 223685, "end": 224197}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 224197, "end": 226835}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 226835, "end": 229526}, {"filename": "/examples/uart/metron/uart_top.h", "start": 229526, "end": 231314}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 231314, "end": 234371}, {"filename": "/tests/metron_bad/README.md", "start": 234371, "end": 234568}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 234568, "end": 234875}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 234875, "end": 235130}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 235130, "end": 235350}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 235350, "end": 235836}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 235836, "end": 236073}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 236073, "end": 236490}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 236490, "end": 236956}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 236956, "end": 237274}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 237274, "end": 237726}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 237726, "end": 237983}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 237983, "end": 238265}, {"filename": "/tests/metron_bad/unorderable_ticks.h", "start": 238265, "end": 238501}, {"filename": "/tests/metron_bad/unorderable_tocks.h", "start": 238501, "end": 238731}, {"filename": "/tests/metron_bad/wrong_submod_call_order.h", "start": 238731, "end": 239100}, {"filename": "/tests/metron_good/README.md", "start": 239100, "end": 239181}, {"filename": "/tests/metron_good/all_func_types.h", "start": 239181, "end": 240682}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 240682, "end": 241099}, {"filename": "/tests/metron_good/basic_function.h", "start": 241099, "end": 241351}, {"filename": "/tests/metron_good/basic_increment.h", "start": 241351, "end": 241678}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 241678, "end": 241983}, {"filename": "/tests/metron_good/basic_literals.h", "start": 241983, "end": 242602}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 242602, "end": 242858}, {"filename": "/tests/metron_good/basic_output.h", "start": 242858, "end": 243130}, {"filename": "/tests/metron_good/basic_param.h", "start": 243130, "end": 243399}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 243399, "end": 243600}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 243600, "end": 243787}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 243787, "end": 244018}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 244018, "end": 244245}, {"filename": "/tests/metron_good/basic_submod.h", "start": 244245, "end": 244561}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 244561, "end": 244926}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 244926, "end": 245312}, {"filename": "/tests/metron_good/basic_switch.h", "start": 245312, "end": 245801}, {"filename": "/tests/metron_good/basic_task.h", "start": 245801, "end": 246136}, {"filename": "/tests/metron_good/basic_template.h", "start": 246136, "end": 246638}, {"filename": "/tests/metron_good/bit_casts.h", "start": 246638, "end": 252618}, {"filename": "/tests/metron_good/bit_concat.h", "start": 252618, "end": 253052}, {"filename": "/tests/metron_good/bit_dup.h", "start": 253052, "end": 253718}, {"filename": "/tests/metron_good/bitfields.h", "start": 253718, "end": 254924}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 254924, "end": 255741}, {"filename": "/tests/metron_good/builtins.h", "start": 255741, "end": 256079}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 256079, "end": 256396}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 256396, "end": 256981}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 256981, "end": 257844}, {"filename": "/tests/metron_good/constructor_args.h", "start": 257844, "end": 258316}, {"filename": "/tests/metron_good/counter.h", "start": 258316, "end": 258963}, {"filename": "/tests/metron_good/defines.h", "start": 258963, "end": 259284}, {"filename": "/tests/metron_good/dontcare.h", "start": 259284, "end": 259571}, {"filename": "/tests/metron_good/enum_simple.h", "start": 259571, "end": 260966}, {"filename": "/tests/metron_good/for_loops.h", "start": 260966, "end": 261293}, {"filename": "/tests/metron_good/good_order.h", "start": 261293, "end": 261496}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 261496, "end": 261917}, {"filename": "/tests/metron_good/include_guards.h", "start": 261917, "end": 262114}, {"filename": "/tests/metron_good/include_test_module.h", "start": 262114, "end": 262305}, {"filename": "/tests/metron_good/include_test_submod.h", "start": 262305, "end": 262482}, {"filename": "/tests/metron_good/init_chain.h", "start": 262482, "end": 263203}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 263203, "end": 263495}, {"filename": "/tests/metron_good/input_signals.h", "start": 263495, "end": 264291}, {"filename": "/tests/metron_good/local_localparam.h", "start": 264291, "end": 264476}, {"filename": "/tests/metron_good/magic_comments.h", "start": 264476, "end": 264789}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 264789, "end": 265110}, {"filename": "/tests/metron_good/minimal.h", "start": 265110, "end": 265290}, {"filename": "/tests/metron_good/multi_tick.h", "start": 265290, "end": 265666}, {"filename": "/tests/metron_good/namespaces.h", "start": 265666, "end": 266047}, {"filename": "/tests/metron_good/nested_structs.h", "start": 266047, "end": 266508}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 266508, "end": 267081}, {"filename": "/tests/metron_good/noconvert.h", "start": 267081, "end": 267411}, {"filename": "/tests/metron_good/nonblocking_assign_to_struct_union.h", "start": 267411, "end": 267764}, {"filename": "/tests/metron_good/oneliners.h", "start": 267764, "end": 268038}, {"filename": "/tests/metron_good/plus_equals.h", "start": 268038, "end": 268525}, {"filename": "/tests/metron_good/private_getter.h", "start": 268525, "end": 268777}, {"filename": "/tests/metron_good/self_reference.h", "start": 268777, "end": 268982}, {"filename": "/tests/metron_good/slice.h", "start": 268982, "end": 269489}, {"filename": "/tests/metron_good/structs.h", "start": 269489, "end": 269943}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 269943, "end": 270488}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 270488, "end": 273108}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 273108, "end": 273902}, {"filename": "/tests/metron_good/tock_task.h", "start": 273902, "end": 274354}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 274354, "end": 274526}, {"filename": "/tests/metron_good/unions.h", "start": 274526, "end": 274732}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 274732, "end": 275401}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 275401, "end": 276070}], "remote_package_size": 276070});

  })();
