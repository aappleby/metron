
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/picorv32/picorv32.h", "start": 71623, "end": 166827}, {"filename": "/examples/pong/README.md", "start": 166827, "end": 166887}, {"filename": "/examples/pong/metron/pong.h", "start": 166887, "end": 170878}, {"filename": "/examples/pong/reference/README.md", "start": 170878, "end": 170938}, {"filename": "/examples/rvsimple/README.md", "start": 170938, "end": 171017}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 171017, "end": 171524}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 171524, "end": 172992}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 172992, "end": 175604}, {"filename": "/examples/rvsimple/metron/config.h", "start": 175604, "end": 176826}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 176826, "end": 182552}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 182552, "end": 183669}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 183669, "end": 185609}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 185609, "end": 186849}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 186849, "end": 188100}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 188100, "end": 188779}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 188779, "end": 189750}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 189750, "end": 191875}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 191875, "end": 192646}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 192646, "end": 193333}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 193333, "end": 194156}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 194156, "end": 195162}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 195162, "end": 196140}, {"filename": "/examples/rvsimple/metron/register.h", "start": 196140, "end": 196836}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 196836, "end": 199886}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 199886, "end": 205467}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 205467, "end": 207967}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 207967, "end": 212705}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 212705, "end": 214700}, {"filename": "/examples/scratch.h", "start": 214700, "end": 215068}, {"filename": "/examples/scratch.sv.d", "start": 215068, "end": 215108}, {"filename": "/examples/tutorial/adder.h", "start": 215108, "end": 215288}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 215288, "end": 216276}, {"filename": "/examples/tutorial/blockram.h", "start": 216276, "end": 216804}, {"filename": "/examples/tutorial/checksum.h", "start": 216804, "end": 217546}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 217546, "end": 217953}, {"filename": "/examples/tutorial/counter.h", "start": 217953, "end": 218104}, {"filename": "/examples/tutorial/declaration_order.h", "start": 218104, "end": 218930}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 218930, "end": 220358}, {"filename": "/examples/tutorial/nonblocking.h", "start": 220358, "end": 220484}, {"filename": "/examples/tutorial/submodules.h", "start": 220484, "end": 221603}, {"filename": "/examples/tutorial/templates.h", "start": 221603, "end": 222086}, {"filename": "/examples/tutorial/tutorial2.h", "start": 222086, "end": 222154}, {"filename": "/examples/tutorial/tutorial3.h", "start": 222154, "end": 222195}, {"filename": "/examples/tutorial/tutorial4.h", "start": 222195, "end": 222236}, {"filename": "/examples/tutorial/tutorial5.h", "start": 222236, "end": 222277}, {"filename": "/examples/tutorial/vga.h", "start": 222277, "end": 223457}, {"filename": "/examples/uart/README.md", "start": 223457, "end": 223701}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 223701, "end": 226339}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 226339, "end": 229030}, {"filename": "/examples/uart/metron/uart_top.h", "start": 229030, "end": 230818}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 230818, "end": 233875}, {"filename": "/tests/metron/fail/README.md", "start": 233875, "end": 234072}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 234072, "end": 234379}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 234379, "end": 234634}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 234634, "end": 234854}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 234854, "end": 235340}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 235340, "end": 235577}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 235577, "end": 235994}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 235994, "end": 236460}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 236460, "end": 236778}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 236778, "end": 237230}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 237230, "end": 237487}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 237487, "end": 237769}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 237769, "end": 238005}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 238005, "end": 238235}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 238235, "end": 238604}, {"filename": "/tests/metron/pass/README.md", "start": 238604, "end": 238685}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 238685, "end": 240186}, {"filename": "/tests/metron/pass/all_func_types.sv.d", "start": 240186, "end": 240258}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 240258, "end": 240675}, {"filename": "/tests/metron/pass/basic_constructor.sv.d", "start": 240675, "end": 240753}, {"filename": "/tests/metron/pass/basic_function.h", "start": 240753, "end": 241005}, {"filename": "/tests/metron/pass/basic_function.sv.d", "start": 241005, "end": 241077}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 241077, "end": 241404}, {"filename": "/tests/metron/pass/basic_increment.sv.d", "start": 241404, "end": 241478}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 241478, "end": 241783}, {"filename": "/tests/metron/pass/basic_inputs.sv.d", "start": 241783, "end": 241851}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 241851, "end": 242470}, {"filename": "/tests/metron/pass/basic_literals.sv.d", "start": 242470, "end": 242542}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 242542, "end": 242798}, {"filename": "/tests/metron/pass/basic_localparam.sv.d", "start": 242798, "end": 242874}, {"filename": "/tests/metron/pass/basic_output.h", "start": 242874, "end": 243146}, {"filename": "/tests/metron/pass/basic_output.sv.d", "start": 243146, "end": 243214}, {"filename": "/tests/metron/pass/basic_param.h", "start": 243214, "end": 243483}, {"filename": "/tests/metron/pass/basic_param.sv.d", "start": 243483, "end": 243549}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 243549, "end": 243750}, {"filename": "/tests/metron/pass/basic_public_reg.sv.d", "start": 243750, "end": 243826}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 243826, "end": 244013}, {"filename": "/tests/metron/pass/basic_public_sig.sv.d", "start": 244013, "end": 244089}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 244089, "end": 244320}, {"filename": "/tests/metron/pass/basic_reg_rww.sv.d", "start": 244320, "end": 244390}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 244390, "end": 244617}, {"filename": "/tests/metron/pass/basic_sig_wwr.sv.d", "start": 244617, "end": 244687}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 244687, "end": 245003}, {"filename": "/tests/metron/pass/basic_submod.sv.d", "start": 245003, "end": 245071}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 245071, "end": 245436}, {"filename": "/tests/metron/pass/basic_submod_param.sv.d", "start": 245436, "end": 245516}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 245516, "end": 245902}, {"filename": "/tests/metron/pass/basic_submod_public_reg.sv.d", "start": 245902, "end": 245992}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 245992, "end": 246481}, {"filename": "/tests/metron/pass/basic_switch.sv.d", "start": 246481, "end": 246549}, {"filename": "/tests/metron/pass/basic_task.h", "start": 246549, "end": 246884}, {"filename": "/tests/metron/pass/basic_task.sv.d", "start": 246884, "end": 246948}, {"filename": "/tests/metron/pass/basic_template.h", "start": 246948, "end": 247450}, {"filename": "/tests/metron/pass/basic_template.sv.d", "start": 247450, "end": 247522}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 247522, "end": 253502}, {"filename": "/tests/metron/pass/bit_casts.sv.d", "start": 253502, "end": 253564}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 253564, "end": 253998}, {"filename": "/tests/metron/pass/bit_concat.sv.d", "start": 253998, "end": 254062}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 254062, "end": 254728}, {"filename": "/tests/metron/pass/bit_dup.sv.d", "start": 254728, "end": 254786}, {"filename": "/tests/metron/pass/bitfields.h", "start": 254786, "end": 255992}, {"filename": "/tests/metron/pass/bitfields.sv.d", "start": 255992, "end": 256054}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 256054, "end": 256871}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.sv.d", "start": 256871, "end": 256991}, {"filename": "/tests/metron/pass/builtins.h", "start": 256991, "end": 257329}, {"filename": "/tests/metron/pass/builtins.sv.d", "start": 257329, "end": 257389}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 257389, "end": 257706}, {"filename": "/tests/metron/pass/call_tick_from_tock.sv.d", "start": 257706, "end": 257788}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 257788, "end": 258373}, {"filename": "/tests/metron/pass/case_with_fallthrough.sv.d", "start": 258373, "end": 258459}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 258459, "end": 259322}, {"filename": "/tests/metron/pass/constructor_arg_passing.sv.d", "start": 259322, "end": 259412}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 259412, "end": 259884}, {"filename": "/tests/metron/pass/constructor_args.sv.d", "start": 259884, "end": 259960}, {"filename": "/tests/metron/pass/counter.h", "start": 259960, "end": 260607}, {"filename": "/tests/metron/pass/counter.sv.d", "start": 260607, "end": 260665}, {"filename": "/tests/metron/pass/defines.h", "start": 260665, "end": 260986}, {"filename": "/tests/metron/pass/defines.sv.d", "start": 260986, "end": 261044}, {"filename": "/tests/metron/pass/dontcare.h", "start": 261044, "end": 261331}, {"filename": "/tests/metron/pass/dontcare.sv.d", "start": 261331, "end": 261391}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 261391, "end": 262786}, {"filename": "/tests/metron/pass/enum_simple.sv.d", "start": 262786, "end": 262852}, {"filename": "/tests/metron/pass/for_loops.h", "start": 262852, "end": 263179}, {"filename": "/tests/metron/pass/for_loops.sv.d", "start": 263179, "end": 263241}, {"filename": "/tests/metron/pass/good_order.h", "start": 263241, "end": 263444}, {"filename": "/tests/metron/pass/good_order.sv.d", "start": 263444, "end": 263508}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 263508, "end": 263929}, {"filename": "/tests/metron/pass/if_with_compound.sv.d", "start": 263929, "end": 264005}, {"filename": "/tests/metron/pass/include_guards.h", "start": 264005, "end": 264202}, {"filename": "/tests/metron/pass/include_guards.sv.d", "start": 264202, "end": 264274}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 264274, "end": 264465}, {"filename": "/tests/metron/pass/include_test_module.sv.d", "start": 264465, "end": 264590}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 264590, "end": 264767}, {"filename": "/tests/metron/pass/include_test_submod.sv.d", "start": 264767, "end": 264849}, {"filename": "/tests/metron/pass/init_chain.h", "start": 264849, "end": 265570}, {"filename": "/tests/metron/pass/init_chain.sv.d", "start": 265570, "end": 265634}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 265634, "end": 265926}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.sv.d", "start": 265926, "end": 266020}, {"filename": "/tests/metron/pass/input_signals.h", "start": 266020, "end": 266816}, {"filename": "/tests/metron/pass/input_signals.sv.d", "start": 266816, "end": 266886}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 266886, "end": 267071}, {"filename": "/tests/metron/pass/local_localparam.sv.d", "start": 267071, "end": 267147}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 267147, "end": 267460}, {"filename": "/tests/metron/pass/magic_comments.sv.d", "start": 267460, "end": 267532}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 267532, "end": 267853}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.sv.d", "start": 267853, "end": 267951}, {"filename": "/tests/metron/pass/minimal.h", "start": 267951, "end": 268131}, {"filename": "/tests/metron/pass/minimal.sv.d", "start": 268131, "end": 268189}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 268189, "end": 268565}, {"filename": "/tests/metron/pass/multi_tick.sv.d", "start": 268565, "end": 268629}, {"filename": "/tests/metron/pass/namespaces.h", "start": 268629, "end": 269010}, {"filename": "/tests/metron/pass/namespaces.sv.d", "start": 269010, "end": 269074}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 269074, "end": 269535}, {"filename": "/tests/metron/pass/nested_structs.sv.d", "start": 269535, "end": 269607}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 269607, "end": 270180}, {"filename": "/tests/metron/pass/nested_submod_calls.sv.d", "start": 270180, "end": 270262}, {"filename": "/tests/metron/pass/noconvert.h", "start": 270262, "end": 270592}, {"filename": "/tests/metron/pass/noconvert.sv.d", "start": 270592, "end": 270654}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 270654, "end": 271007}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.sv.d", "start": 271007, "end": 271119}, {"filename": "/tests/metron/pass/oneliners.h", "start": 271119, "end": 271393}, {"filename": "/tests/metron/pass/oneliners.sv.d", "start": 271393, "end": 271455}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 271455, "end": 271942}, {"filename": "/tests/metron/pass/plus_equals.sv.d", "start": 271942, "end": 272008}, {"filename": "/tests/metron/pass/preproc.h", "start": 272008, "end": 272336}, {"filename": "/tests/metron/pass/private_getter.h", "start": 272336, "end": 272588}, {"filename": "/tests/metron/pass/private_getter.sv.d", "start": 272588, "end": 272660}, {"filename": "/tests/metron/pass/self_reference.h", "start": 272660, "end": 272865}, {"filename": "/tests/metron/pass/self_reference.sv.d", "start": 272865, "end": 272937}, {"filename": "/tests/metron/pass/slice.h", "start": 272937, "end": 273444}, {"filename": "/tests/metron/pass/slice.sv.d", "start": 273444, "end": 273498}, {"filename": "/tests/metron/pass/structs.h", "start": 273498, "end": 273952}, {"filename": "/tests/metron/pass/structs.sv.d", "start": 273952, "end": 274010}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 274010, "end": 274555}, {"filename": "/tests/metron/pass/structs_as_args.sv.d", "start": 274555, "end": 274629}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 274629, "end": 277249}, {"filename": "/tests/metron/pass/structs_as_ports.sv.d", "start": 277249, "end": 277325}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 277325, "end": 278119}, {"filename": "/tests/metron/pass/submod_bindings.sv.d", "start": 278119, "end": 278193}, {"filename": "/tests/metron/pass/tock_task.h", "start": 278193, "end": 278645}, {"filename": "/tests/metron/pass/tock_task.sv.d", "start": 278645, "end": 278707}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 278707, "end": 278879}, {"filename": "/tests/metron/pass/trivial_adder.sv.d", "start": 278879, "end": 278949}, {"filename": "/tests/metron/pass/unions.h", "start": 278949, "end": 279155}, {"filename": "/tests/metron/pass/unions.sv.d", "start": 279155, "end": 279211}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 279211, "end": 279880}, {"filename": "/tests/metron/pass/utf8-mod.bom.sv.d", "start": 279880, "end": 279948}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 279948, "end": 280617}, {"filename": "/tests/metron/pass/utf8-mod.sv.d", "start": 280617, "end": 280677}], "remote_package_size": 280677});

  })();
