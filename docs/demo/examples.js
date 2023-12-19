
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/picorv32/picorv32.h", "start": 71623, "end": 166827}, {"filename": "/examples/pong/README.md", "start": 166827, "end": 166887}, {"filename": "/examples/pong/metron/pong.h", "start": 166887, "end": 170878}, {"filename": "/examples/pong/reference/README.md", "start": 170878, "end": 170938}, {"filename": "/examples/rvsimple/README.md", "start": 170938, "end": 171017}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 171017, "end": 171524}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 171524, "end": 172992}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 172992, "end": 175604}, {"filename": "/examples/rvsimple/metron/config.h", "start": 175604, "end": 176826}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 176826, "end": 182552}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 182552, "end": 183669}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 183669, "end": 185609}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 185609, "end": 186849}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 186849, "end": 188100}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 188100, "end": 188779}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 188779, "end": 189750}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 189750, "end": 191875}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 191875, "end": 192646}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 192646, "end": 193333}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 193333, "end": 194156}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 194156, "end": 195162}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 195162, "end": 196140}, {"filename": "/examples/rvsimple/metron/register.h", "start": 196140, "end": 196836}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 196836, "end": 199886}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 199886, "end": 205467}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 205467, "end": 207967}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 207967, "end": 212705}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 212705, "end": 214700}, {"filename": "/examples/scratch.h", "start": 214700, "end": 215052}, {"filename": "/examples/scratch.sv.d", "start": 215052, "end": 215092}, {"filename": "/examples/tutorial/adder.h", "start": 215092, "end": 215272}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 215272, "end": 216260}, {"filename": "/examples/tutorial/blockram.h", "start": 216260, "end": 216788}, {"filename": "/examples/tutorial/checksum.h", "start": 216788, "end": 217530}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 217530, "end": 217937}, {"filename": "/examples/tutorial/counter.h", "start": 217937, "end": 218088}, {"filename": "/examples/tutorial/declaration_order.h", "start": 218088, "end": 218914}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 218914, "end": 220342}, {"filename": "/examples/tutorial/nonblocking.h", "start": 220342, "end": 220468}, {"filename": "/examples/tutorial/submodules.h", "start": 220468, "end": 221587}, {"filename": "/examples/tutorial/templates.h", "start": 221587, "end": 222070}, {"filename": "/examples/tutorial/tutorial2.h", "start": 222070, "end": 222138}, {"filename": "/examples/tutorial/tutorial3.h", "start": 222138, "end": 222179}, {"filename": "/examples/tutorial/tutorial4.h", "start": 222179, "end": 222220}, {"filename": "/examples/tutorial/tutorial5.h", "start": 222220, "end": 222261}, {"filename": "/examples/tutorial/vga.h", "start": 222261, "end": 223441}, {"filename": "/examples/uart/README.md", "start": 223441, "end": 223685}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 223685, "end": 226323}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 226323, "end": 229014}, {"filename": "/examples/uart/metron/uart_top.h", "start": 229014, "end": 230802}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 230802, "end": 233859}, {"filename": "/tests/metron/fail/README.md", "start": 233859, "end": 234056}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 234056, "end": 234363}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 234363, "end": 234618}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 234618, "end": 234838}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 234838, "end": 235324}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 235324, "end": 235561}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 235561, "end": 235978}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 235978, "end": 236444}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 236444, "end": 236762}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 236762, "end": 237214}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 237214, "end": 237471}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 237471, "end": 237753}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 237753, "end": 237989}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 237989, "end": 238219}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 238219, "end": 238588}, {"filename": "/tests/metron/pass/README.md", "start": 238588, "end": 238669}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 238669, "end": 240170}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 240170, "end": 240587}, {"filename": "/tests/metron/pass/basic_function.h", "start": 240587, "end": 240839}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 240839, "end": 241166}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 241166, "end": 241471}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 241471, "end": 242090}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 242090, "end": 242346}, {"filename": "/tests/metron/pass/basic_output.h", "start": 242346, "end": 242618}, {"filename": "/tests/metron/pass/basic_param.h", "start": 242618, "end": 242887}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 242887, "end": 243088}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 243088, "end": 243275}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 243275, "end": 243506}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 243506, "end": 243733}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 243733, "end": 244049}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 244049, "end": 244414}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 244414, "end": 244800}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 244800, "end": 245289}, {"filename": "/tests/metron/pass/basic_task.h", "start": 245289, "end": 245624}, {"filename": "/tests/metron/pass/basic_template.h", "start": 245624, "end": 246126}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 246126, "end": 252106}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 252106, "end": 252540}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 252540, "end": 253206}, {"filename": "/tests/metron/pass/bitfields.h", "start": 253206, "end": 254412}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 254412, "end": 255229}, {"filename": "/tests/metron/pass/builtins.h", "start": 255229, "end": 255567}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 255567, "end": 255884}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 255884, "end": 256469}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 256469, "end": 257332}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 257332, "end": 257804}, {"filename": "/tests/metron/pass/counter.h", "start": 257804, "end": 258451}, {"filename": "/tests/metron/pass/defines.h", "start": 258451, "end": 258772}, {"filename": "/tests/metron/pass/dontcare.h", "start": 258772, "end": 259059}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 259059, "end": 260454}, {"filename": "/tests/metron/pass/for_loops.h", "start": 260454, "end": 260781}, {"filename": "/tests/metron/pass/good_order.h", "start": 260781, "end": 260984}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 260984, "end": 261405}, {"filename": "/tests/metron/pass/include_guards.h", "start": 261405, "end": 261602}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 261602, "end": 261793}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 261793, "end": 261970}, {"filename": "/tests/metron/pass/init_chain.h", "start": 261970, "end": 262691}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 262691, "end": 262983}, {"filename": "/tests/metron/pass/input_signals.h", "start": 262983, "end": 263779}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 263779, "end": 263964}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 263964, "end": 264277}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 264277, "end": 264598}, {"filename": "/tests/metron/pass/minimal.h", "start": 264598, "end": 264778}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 264778, "end": 265154}, {"filename": "/tests/metron/pass/namespaces.h", "start": 265154, "end": 265535}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 265535, "end": 265996}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 265996, "end": 266569}, {"filename": "/tests/metron/pass/noconvert.h", "start": 266569, "end": 266899}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 266899, "end": 267252}, {"filename": "/tests/metron/pass/oneliners.h", "start": 267252, "end": 267526}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 267526, "end": 268013}, {"filename": "/tests/metron/pass/private_getter.h", "start": 268013, "end": 268265}, {"filename": "/tests/metron/pass/self_reference.h", "start": 268265, "end": 268470}, {"filename": "/tests/metron/pass/slice.h", "start": 268470, "end": 268977}, {"filename": "/tests/metron/pass/structs.h", "start": 268977, "end": 269431}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 269431, "end": 269976}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 269976, "end": 272596}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 272596, "end": 273390}, {"filename": "/tests/metron/pass/tock_task.h", "start": 273390, "end": 273842}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 273842, "end": 274014}, {"filename": "/tests/metron/pass/unions.h", "start": 274014, "end": 274220}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 274220, "end": 274889}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 274889, "end": 275558}], "remote_package_size": 275558});

  })();
