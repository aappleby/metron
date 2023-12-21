
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/picorv32/picorv32.h", "start": 71623, "end": 166827}, {"filename": "/examples/pong/README.md", "start": 166827, "end": 166887}, {"filename": "/examples/pong/metron/pong.h", "start": 166887, "end": 170878}, {"filename": "/examples/pong/reference/README.md", "start": 170878, "end": 170938}, {"filename": "/examples/rvsimple/README.md", "start": 170938, "end": 171017}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 171017, "end": 171524}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 171524, "end": 172992}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 172992, "end": 175604}, {"filename": "/examples/rvsimple/metron/config.h", "start": 175604, "end": 176826}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 176826, "end": 182552}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 182552, "end": 183669}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 183669, "end": 185609}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 185609, "end": 186849}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 186849, "end": 188100}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 188100, "end": 188779}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 188779, "end": 189750}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 189750, "end": 191875}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 191875, "end": 192646}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 192646, "end": 193333}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 193333, "end": 194156}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 194156, "end": 195162}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 195162, "end": 196140}, {"filename": "/examples/rvsimple/metron/register.h", "start": 196140, "end": 196836}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 196836, "end": 199886}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 199886, "end": 205467}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 205467, "end": 207967}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 207967, "end": 212705}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 212705, "end": 214700}, {"filename": "/examples/scratch.h", "start": 214700, "end": 216075}, {"filename": "/examples/scratch.sv.d", "start": 216075, "end": 216115}, {"filename": "/examples/tutorial/adder.h", "start": 216115, "end": 216295}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 216295, "end": 217283}, {"filename": "/examples/tutorial/blockram.h", "start": 217283, "end": 217811}, {"filename": "/examples/tutorial/checksum.h", "start": 217811, "end": 218553}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 218553, "end": 218960}, {"filename": "/examples/tutorial/counter.h", "start": 218960, "end": 219111}, {"filename": "/examples/tutorial/declaration_order.h", "start": 219111, "end": 219937}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 219937, "end": 221365}, {"filename": "/examples/tutorial/nonblocking.h", "start": 221365, "end": 221491}, {"filename": "/examples/tutorial/submodules.h", "start": 221491, "end": 222610}, {"filename": "/examples/tutorial/templates.h", "start": 222610, "end": 223093}, {"filename": "/examples/tutorial/tutorial2.h", "start": 223093, "end": 223161}, {"filename": "/examples/tutorial/tutorial3.h", "start": 223161, "end": 223202}, {"filename": "/examples/tutorial/tutorial4.h", "start": 223202, "end": 223243}, {"filename": "/examples/tutorial/tutorial5.h", "start": 223243, "end": 223284}, {"filename": "/examples/tutorial/vga.h", "start": 223284, "end": 224464}, {"filename": "/examples/uart/README.md", "start": 224464, "end": 224708}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 224708, "end": 227346}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 227346, "end": 230037}, {"filename": "/examples/uart/metron/uart_top.h", "start": 230037, "end": 231825}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 231825, "end": 234882}, {"filename": "/tests/metron/fail/README.md", "start": 234882, "end": 235079}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 235079, "end": 235386}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 235386, "end": 235641}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 235641, "end": 235861}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 235861, "end": 236347}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 236347, "end": 236584}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 236584, "end": 237001}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 237001, "end": 237467}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 237467, "end": 237785}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 237785, "end": 238237}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 238237, "end": 238494}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 238494, "end": 238776}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 238776, "end": 239012}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 239012, "end": 239242}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 239242, "end": 239611}, {"filename": "/tests/metron/pass/README.md", "start": 239611, "end": 239692}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 239692, "end": 241193}, {"filename": "/tests/metron/pass/all_func_types.sv.d", "start": 241193, "end": 241265}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 241265, "end": 241682}, {"filename": "/tests/metron/pass/basic_constructor.sv.d", "start": 241682, "end": 241760}, {"filename": "/tests/metron/pass/basic_function.h", "start": 241760, "end": 242012}, {"filename": "/tests/metron/pass/basic_function.sv.d", "start": 242012, "end": 242084}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 242084, "end": 242411}, {"filename": "/tests/metron/pass/basic_increment.sv.d", "start": 242411, "end": 242485}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 242485, "end": 242790}, {"filename": "/tests/metron/pass/basic_inputs.sv.d", "start": 242790, "end": 242858}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 242858, "end": 243477}, {"filename": "/tests/metron/pass/basic_literals.sv.d", "start": 243477, "end": 243549}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 243549, "end": 243805}, {"filename": "/tests/metron/pass/basic_localparam.sv.d", "start": 243805, "end": 243881}, {"filename": "/tests/metron/pass/basic_output.h", "start": 243881, "end": 244153}, {"filename": "/tests/metron/pass/basic_output.sv.d", "start": 244153, "end": 244221}, {"filename": "/tests/metron/pass/basic_param.h", "start": 244221, "end": 244490}, {"filename": "/tests/metron/pass/basic_param.sv.d", "start": 244490, "end": 244556}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 244556, "end": 244757}, {"filename": "/tests/metron/pass/basic_public_reg.sv.d", "start": 244757, "end": 244833}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 244833, "end": 245020}, {"filename": "/tests/metron/pass/basic_public_sig.sv.d", "start": 245020, "end": 245096}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 245096, "end": 245327}, {"filename": "/tests/metron/pass/basic_reg_rww.sv.d", "start": 245327, "end": 245397}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 245397, "end": 245624}, {"filename": "/tests/metron/pass/basic_sig_wwr.sv.d", "start": 245624, "end": 245694}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 245694, "end": 246010}, {"filename": "/tests/metron/pass/basic_submod.sv.d", "start": 246010, "end": 246078}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 246078, "end": 246443}, {"filename": "/tests/metron/pass/basic_submod_param.sv.d", "start": 246443, "end": 246523}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 246523, "end": 246909}, {"filename": "/tests/metron/pass/basic_submod_public_reg.sv.d", "start": 246909, "end": 246999}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 246999, "end": 247488}, {"filename": "/tests/metron/pass/basic_switch.sv.d", "start": 247488, "end": 247556}, {"filename": "/tests/metron/pass/basic_task.h", "start": 247556, "end": 247891}, {"filename": "/tests/metron/pass/basic_task.sv.d", "start": 247891, "end": 247955}, {"filename": "/tests/metron/pass/basic_template.h", "start": 247955, "end": 248457}, {"filename": "/tests/metron/pass/basic_template.sv.d", "start": 248457, "end": 248529}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 248529, "end": 254509}, {"filename": "/tests/metron/pass/bit_casts.sv.d", "start": 254509, "end": 254571}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 254571, "end": 255005}, {"filename": "/tests/metron/pass/bit_concat.sv.d", "start": 255005, "end": 255069}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 255069, "end": 255735}, {"filename": "/tests/metron/pass/bit_dup.sv.d", "start": 255735, "end": 255793}, {"filename": "/tests/metron/pass/bitfields.h", "start": 255793, "end": 256999}, {"filename": "/tests/metron/pass/bitfields.sv.d", "start": 256999, "end": 257061}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 257061, "end": 257878}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.sv.d", "start": 257878, "end": 257998}, {"filename": "/tests/metron/pass/builtins.h", "start": 257998, "end": 258336}, {"filename": "/tests/metron/pass/builtins.sv.d", "start": 258336, "end": 258396}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 258396, "end": 258713}, {"filename": "/tests/metron/pass/call_tick_from_tock.sv.d", "start": 258713, "end": 258795}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 258795, "end": 259380}, {"filename": "/tests/metron/pass/case_with_fallthrough.sv.d", "start": 259380, "end": 259466}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 259466, "end": 260329}, {"filename": "/tests/metron/pass/constructor_arg_passing.sv.d", "start": 260329, "end": 260419}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 260419, "end": 260891}, {"filename": "/tests/metron/pass/constructor_args.sv.d", "start": 260891, "end": 260967}, {"filename": "/tests/metron/pass/counter.h", "start": 260967, "end": 261614}, {"filename": "/tests/metron/pass/counter.sv.d", "start": 261614, "end": 261672}, {"filename": "/tests/metron/pass/defines.h", "start": 261672, "end": 261993}, {"filename": "/tests/metron/pass/defines.sv.d", "start": 261993, "end": 262051}, {"filename": "/tests/metron/pass/dontcare.h", "start": 262051, "end": 262338}, {"filename": "/tests/metron/pass/dontcare.sv.d", "start": 262338, "end": 262398}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 262398, "end": 263793}, {"filename": "/tests/metron/pass/enum_simple.sv.d", "start": 263793, "end": 263859}, {"filename": "/tests/metron/pass/for_loops.h", "start": 263859, "end": 264186}, {"filename": "/tests/metron/pass/for_loops.sv.d", "start": 264186, "end": 264248}, {"filename": "/tests/metron/pass/good_order.h", "start": 264248, "end": 264451}, {"filename": "/tests/metron/pass/good_order.sv.d", "start": 264451, "end": 264515}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 264515, "end": 264936}, {"filename": "/tests/metron/pass/if_with_compound.sv.d", "start": 264936, "end": 265012}, {"filename": "/tests/metron/pass/include_guards.h", "start": 265012, "end": 265209}, {"filename": "/tests/metron/pass/include_guards.sv.d", "start": 265209, "end": 265281}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 265281, "end": 265472}, {"filename": "/tests/metron/pass/include_test_module.sv.d", "start": 265472, "end": 265597}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 265597, "end": 265774}, {"filename": "/tests/metron/pass/include_test_submod.sv.d", "start": 265774, "end": 265856}, {"filename": "/tests/metron/pass/init_chain.h", "start": 265856, "end": 266577}, {"filename": "/tests/metron/pass/init_chain.sv.d", "start": 266577, "end": 266641}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 266641, "end": 266933}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.sv.d", "start": 266933, "end": 267027}, {"filename": "/tests/metron/pass/input_signals.h", "start": 267027, "end": 267823}, {"filename": "/tests/metron/pass/input_signals.sv.d", "start": 267823, "end": 267893}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 267893, "end": 268078}, {"filename": "/tests/metron/pass/local_localparam.sv.d", "start": 268078, "end": 268154}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 268154, "end": 268467}, {"filename": "/tests/metron/pass/magic_comments.sv.d", "start": 268467, "end": 268539}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 268539, "end": 268860}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.sv.d", "start": 268860, "end": 268958}, {"filename": "/tests/metron/pass/minimal.h", "start": 268958, "end": 269138}, {"filename": "/tests/metron/pass/minimal.sv.d", "start": 269138, "end": 269196}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 269196, "end": 269572}, {"filename": "/tests/metron/pass/multi_tick.sv.d", "start": 269572, "end": 269636}, {"filename": "/tests/metron/pass/namespaces.h", "start": 269636, "end": 270017}, {"filename": "/tests/metron/pass/namespaces.sv.d", "start": 270017, "end": 270081}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 270081, "end": 270542}, {"filename": "/tests/metron/pass/nested_structs.sv.d", "start": 270542, "end": 270614}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 270614, "end": 271187}, {"filename": "/tests/metron/pass/nested_submod_calls.sv.d", "start": 271187, "end": 271269}, {"filename": "/tests/metron/pass/noconvert.h", "start": 271269, "end": 271599}, {"filename": "/tests/metron/pass/noconvert.sv.d", "start": 271599, "end": 271661}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 271661, "end": 272014}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.sv.d", "start": 272014, "end": 272126}, {"filename": "/tests/metron/pass/oneliners.h", "start": 272126, "end": 272400}, {"filename": "/tests/metron/pass/oneliners.sv.d", "start": 272400, "end": 272462}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 272462, "end": 272949}, {"filename": "/tests/metron/pass/plus_equals.sv.d", "start": 272949, "end": 273015}, {"filename": "/tests/metron/pass/private_getter.h", "start": 273015, "end": 273267}, {"filename": "/tests/metron/pass/private_getter.sv.d", "start": 273267, "end": 273339}, {"filename": "/tests/metron/pass/self_reference.h", "start": 273339, "end": 273544}, {"filename": "/tests/metron/pass/self_reference.sv.d", "start": 273544, "end": 273616}, {"filename": "/tests/metron/pass/slice.h", "start": 273616, "end": 274123}, {"filename": "/tests/metron/pass/slice.sv.d", "start": 274123, "end": 274177}, {"filename": "/tests/metron/pass/structs.h", "start": 274177, "end": 274631}, {"filename": "/tests/metron/pass/structs.sv.d", "start": 274631, "end": 274689}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 274689, "end": 275234}, {"filename": "/tests/metron/pass/structs_as_args.sv.d", "start": 275234, "end": 275308}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 275308, "end": 277928}, {"filename": "/tests/metron/pass/structs_as_ports.sv.d", "start": 277928, "end": 278004}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 278004, "end": 278798}, {"filename": "/tests/metron/pass/submod_bindings.sv.d", "start": 278798, "end": 278872}, {"filename": "/tests/metron/pass/tock_task.h", "start": 278872, "end": 279324}, {"filename": "/tests/metron/pass/tock_task.sv.d", "start": 279324, "end": 279386}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 279386, "end": 279558}, {"filename": "/tests/metron/pass/trivial_adder.sv.d", "start": 279558, "end": 279628}, {"filename": "/tests/metron/pass/unions.h", "start": 279628, "end": 279834}, {"filename": "/tests/metron/pass/unions.sv.d", "start": 279834, "end": 279890}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 279890, "end": 280559}, {"filename": "/tests/metron/pass/utf8-mod.bom.sv.d", "start": 280559, "end": 280627}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 280627, "end": 281296}, {"filename": "/tests/metron/pass/utf8-mod.sv.d", "start": 281296, "end": 281356}], "remote_package_size": 281356});

  })();
