
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/picorv32/picorv32.h", "start": 71623, "end": 166827}, {"filename": "/examples/pong/README.md", "start": 166827, "end": 166887}, {"filename": "/examples/pong/metron/pong.h", "start": 166887, "end": 170878}, {"filename": "/examples/pong/reference/README.md", "start": 170878, "end": 170938}, {"filename": "/examples/rvsimple/README.md", "start": 170938, "end": 171017}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 171017, "end": 171524}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 171524, "end": 172992}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 172992, "end": 175604}, {"filename": "/examples/rvsimple/metron/config.h", "start": 175604, "end": 176826}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 176826, "end": 182552}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 182552, "end": 183669}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 183669, "end": 185609}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 185609, "end": 186849}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 186849, "end": 188100}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 188100, "end": 188779}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 188779, "end": 189750}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 189750, "end": 191875}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 191875, "end": 192646}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 192646, "end": 193333}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 193333, "end": 194156}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 194156, "end": 195162}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 195162, "end": 196140}, {"filename": "/examples/rvsimple/metron/register.h", "start": 196140, "end": 196836}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 196836, "end": 199886}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 199886, "end": 205467}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 205467, "end": 207967}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 207967, "end": 212705}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 212705, "end": 214700}, {"filename": "/examples/scratch.h", "start": 214700, "end": 215052}, {"filename": "/examples/scratch.sv.d", "start": 215052, "end": 215092}, {"filename": "/examples/tutorial/adder.h", "start": 215092, "end": 215272}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 215272, "end": 216260}, {"filename": "/examples/tutorial/blockram.h", "start": 216260, "end": 216788}, {"filename": "/examples/tutorial/checksum.h", "start": 216788, "end": 217530}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 217530, "end": 217937}, {"filename": "/examples/tutorial/counter.h", "start": 217937, "end": 218088}, {"filename": "/examples/tutorial/declaration_order.h", "start": 218088, "end": 218914}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 218914, "end": 220342}, {"filename": "/examples/tutorial/nonblocking.h", "start": 220342, "end": 220468}, {"filename": "/examples/tutorial/submodules.h", "start": 220468, "end": 221587}, {"filename": "/examples/tutorial/templates.h", "start": 221587, "end": 222070}, {"filename": "/examples/tutorial/tutorial2.h", "start": 222070, "end": 222138}, {"filename": "/examples/tutorial/tutorial3.h", "start": 222138, "end": 222179}, {"filename": "/examples/tutorial/tutorial4.h", "start": 222179, "end": 222220}, {"filename": "/examples/tutorial/tutorial5.h", "start": 222220, "end": 222261}, {"filename": "/examples/tutorial/vga.h", "start": 222261, "end": 223441}, {"filename": "/examples/uart/README.md", "start": 223441, "end": 223685}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 223685, "end": 226323}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 226323, "end": 229014}, {"filename": "/examples/uart/metron/uart_top.h", "start": 229014, "end": 230802}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 230802, "end": 233859}, {"filename": "/tests/metron/fail/README.md", "start": 233859, "end": 234056}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 234056, "end": 234363}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 234363, "end": 234618}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 234618, "end": 234838}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 234838, "end": 235324}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 235324, "end": 235561}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 235561, "end": 235978}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 235978, "end": 236444}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 236444, "end": 236762}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 236762, "end": 237214}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 237214, "end": 237471}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 237471, "end": 237753}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 237753, "end": 237989}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 237989, "end": 238219}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 238219, "end": 238588}, {"filename": "/tests/metron/pass/README.md", "start": 238588, "end": 238669}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 238669, "end": 240170}, {"filename": "/tests/metron/pass/all_func_types.sv.d", "start": 240170, "end": 240242}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 240242, "end": 240659}, {"filename": "/tests/metron/pass/basic_constructor.sv.d", "start": 240659, "end": 240737}, {"filename": "/tests/metron/pass/basic_function.h", "start": 240737, "end": 240989}, {"filename": "/tests/metron/pass/basic_function.sv.d", "start": 240989, "end": 241061}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 241061, "end": 241388}, {"filename": "/tests/metron/pass/basic_increment.sv.d", "start": 241388, "end": 241462}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 241462, "end": 241767}, {"filename": "/tests/metron/pass/basic_inputs.sv.d", "start": 241767, "end": 241835}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 241835, "end": 242454}, {"filename": "/tests/metron/pass/basic_literals.sv.d", "start": 242454, "end": 242526}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 242526, "end": 242782}, {"filename": "/tests/metron/pass/basic_localparam.sv.d", "start": 242782, "end": 242858}, {"filename": "/tests/metron/pass/basic_output.h", "start": 242858, "end": 243130}, {"filename": "/tests/metron/pass/basic_output.sv.d", "start": 243130, "end": 243198}, {"filename": "/tests/metron/pass/basic_param.h", "start": 243198, "end": 243467}, {"filename": "/tests/metron/pass/basic_param.sv.d", "start": 243467, "end": 243533}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 243533, "end": 243734}, {"filename": "/tests/metron/pass/basic_public_reg.sv.d", "start": 243734, "end": 243810}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 243810, "end": 243997}, {"filename": "/tests/metron/pass/basic_public_sig.sv.d", "start": 243997, "end": 244073}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 244073, "end": 244304}, {"filename": "/tests/metron/pass/basic_reg_rww.sv.d", "start": 244304, "end": 244374}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 244374, "end": 244601}, {"filename": "/tests/metron/pass/basic_sig_wwr.sv.d", "start": 244601, "end": 244671}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 244671, "end": 244987}, {"filename": "/tests/metron/pass/basic_submod.sv.d", "start": 244987, "end": 245055}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 245055, "end": 245420}, {"filename": "/tests/metron/pass/basic_submod_param.sv.d", "start": 245420, "end": 245500}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 245500, "end": 245886}, {"filename": "/tests/metron/pass/basic_submod_public_reg.sv.d", "start": 245886, "end": 245976}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 245976, "end": 246465}, {"filename": "/tests/metron/pass/basic_switch.sv.d", "start": 246465, "end": 246533}, {"filename": "/tests/metron/pass/basic_task.h", "start": 246533, "end": 246868}, {"filename": "/tests/metron/pass/basic_task.sv.d", "start": 246868, "end": 246932}, {"filename": "/tests/metron/pass/basic_template.h", "start": 246932, "end": 247434}, {"filename": "/tests/metron/pass/basic_template.sv.d", "start": 247434, "end": 247506}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 247506, "end": 253486}, {"filename": "/tests/metron/pass/bit_casts.sv.d", "start": 253486, "end": 253548}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 253548, "end": 253982}, {"filename": "/tests/metron/pass/bit_concat.sv.d", "start": 253982, "end": 254046}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 254046, "end": 254712}, {"filename": "/tests/metron/pass/bit_dup.sv.d", "start": 254712, "end": 254770}, {"filename": "/tests/metron/pass/bitfields.h", "start": 254770, "end": 255976}, {"filename": "/tests/metron/pass/bitfields.sv.d", "start": 255976, "end": 256038}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 256038, "end": 256855}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.sv.d", "start": 256855, "end": 256975}, {"filename": "/tests/metron/pass/builtins.h", "start": 256975, "end": 257313}, {"filename": "/tests/metron/pass/builtins.sv.d", "start": 257313, "end": 257373}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 257373, "end": 257690}, {"filename": "/tests/metron/pass/call_tick_from_tock.sv.d", "start": 257690, "end": 257772}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 257772, "end": 258357}, {"filename": "/tests/metron/pass/case_with_fallthrough.sv.d", "start": 258357, "end": 258443}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 258443, "end": 259306}, {"filename": "/tests/metron/pass/constructor_arg_passing.sv.d", "start": 259306, "end": 259396}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 259396, "end": 259868}, {"filename": "/tests/metron/pass/constructor_args.sv.d", "start": 259868, "end": 259944}, {"filename": "/tests/metron/pass/counter.h", "start": 259944, "end": 260591}, {"filename": "/tests/metron/pass/counter.sv.d", "start": 260591, "end": 260649}, {"filename": "/tests/metron/pass/defines.h", "start": 260649, "end": 260970}, {"filename": "/tests/metron/pass/defines.sv.d", "start": 260970, "end": 261028}, {"filename": "/tests/metron/pass/dontcare.h", "start": 261028, "end": 261315}, {"filename": "/tests/metron/pass/dontcare.sv.d", "start": 261315, "end": 261375}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 261375, "end": 262770}, {"filename": "/tests/metron/pass/enum_simple.sv.d", "start": 262770, "end": 262836}, {"filename": "/tests/metron/pass/for_loops.h", "start": 262836, "end": 263163}, {"filename": "/tests/metron/pass/for_loops.sv.d", "start": 263163, "end": 263225}, {"filename": "/tests/metron/pass/good_order.h", "start": 263225, "end": 263428}, {"filename": "/tests/metron/pass/good_order.sv.d", "start": 263428, "end": 263492}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 263492, "end": 263913}, {"filename": "/tests/metron/pass/if_with_compound.sv.d", "start": 263913, "end": 263989}, {"filename": "/tests/metron/pass/include_guards.h", "start": 263989, "end": 264186}, {"filename": "/tests/metron/pass/include_guards.sv.d", "start": 264186, "end": 264258}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 264258, "end": 264449}, {"filename": "/tests/metron/pass/include_test_module.sv.d", "start": 264449, "end": 264574}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 264574, "end": 264751}, {"filename": "/tests/metron/pass/include_test_submod.sv.d", "start": 264751, "end": 264833}, {"filename": "/tests/metron/pass/init_chain.h", "start": 264833, "end": 265554}, {"filename": "/tests/metron/pass/init_chain.sv.d", "start": 265554, "end": 265618}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 265618, "end": 265910}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.sv.d", "start": 265910, "end": 266004}, {"filename": "/tests/metron/pass/input_signals.h", "start": 266004, "end": 266800}, {"filename": "/tests/metron/pass/input_signals.sv.d", "start": 266800, "end": 266870}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 266870, "end": 267055}, {"filename": "/tests/metron/pass/local_localparam.sv.d", "start": 267055, "end": 267131}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 267131, "end": 267444}, {"filename": "/tests/metron/pass/magic_comments.sv.d", "start": 267444, "end": 267516}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 267516, "end": 267837}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.sv.d", "start": 267837, "end": 267935}, {"filename": "/tests/metron/pass/minimal.h", "start": 267935, "end": 268115}, {"filename": "/tests/metron/pass/minimal.sv.d", "start": 268115, "end": 268173}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 268173, "end": 268549}, {"filename": "/tests/metron/pass/multi_tick.sv.d", "start": 268549, "end": 268613}, {"filename": "/tests/metron/pass/namespaces.h", "start": 268613, "end": 268994}, {"filename": "/tests/metron/pass/namespaces.sv.d", "start": 268994, "end": 269058}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 269058, "end": 269519}, {"filename": "/tests/metron/pass/nested_structs.sv.d", "start": 269519, "end": 269591}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 269591, "end": 270164}, {"filename": "/tests/metron/pass/nested_submod_calls.sv.d", "start": 270164, "end": 270246}, {"filename": "/tests/metron/pass/noconvert.h", "start": 270246, "end": 270576}, {"filename": "/tests/metron/pass/noconvert.sv.d", "start": 270576, "end": 270638}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 270638, "end": 270991}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.sv.d", "start": 270991, "end": 271103}, {"filename": "/tests/metron/pass/oneliners.h", "start": 271103, "end": 271377}, {"filename": "/tests/metron/pass/oneliners.sv.d", "start": 271377, "end": 271439}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 271439, "end": 271926}, {"filename": "/tests/metron/pass/plus_equals.sv.d", "start": 271926, "end": 271992}, {"filename": "/tests/metron/pass/private_getter.h", "start": 271992, "end": 272244}, {"filename": "/tests/metron/pass/private_getter.sv.d", "start": 272244, "end": 272316}, {"filename": "/tests/metron/pass/self_reference.h", "start": 272316, "end": 272521}, {"filename": "/tests/metron/pass/self_reference.sv.d", "start": 272521, "end": 272593}, {"filename": "/tests/metron/pass/slice.h", "start": 272593, "end": 273100}, {"filename": "/tests/metron/pass/slice.sv.d", "start": 273100, "end": 273154}, {"filename": "/tests/metron/pass/structs.h", "start": 273154, "end": 273608}, {"filename": "/tests/metron/pass/structs.sv.d", "start": 273608, "end": 273666}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 273666, "end": 274211}, {"filename": "/tests/metron/pass/structs_as_args.sv.d", "start": 274211, "end": 274285}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 274285, "end": 276905}, {"filename": "/tests/metron/pass/structs_as_ports.sv.d", "start": 276905, "end": 276981}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 276981, "end": 277775}, {"filename": "/tests/metron/pass/submod_bindings.sv.d", "start": 277775, "end": 277849}, {"filename": "/tests/metron/pass/tock_task.h", "start": 277849, "end": 278301}, {"filename": "/tests/metron/pass/tock_task.sv.d", "start": 278301, "end": 278363}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 278363, "end": 278535}, {"filename": "/tests/metron/pass/trivial_adder.sv.d", "start": 278535, "end": 278605}, {"filename": "/tests/metron/pass/unions.h", "start": 278605, "end": 278811}, {"filename": "/tests/metron/pass/unions.sv.d", "start": 278811, "end": 278867}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 278867, "end": 279536}, {"filename": "/tests/metron/pass/utf8-mod.bom.sv.d", "start": 279536, "end": 279604}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 279604, "end": 280273}, {"filename": "/tests/metron/pass/utf8-mod.sv.d", "start": 280273, "end": 280333}], "remote_package_size": 280333});

  })();
