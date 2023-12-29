
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/picorv32/picorv32.h", "start": 71623, "end": 166827}, {"filename": "/examples/pong/README.md", "start": 166827, "end": 166887}, {"filename": "/examples/pong/metron/pong.h", "start": 166887, "end": 170878}, {"filename": "/examples/pong/reference/README.md", "start": 170878, "end": 170938}, {"filename": "/examples/rvsimple/README.md", "start": 170938, "end": 171017}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 171017, "end": 171524}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 171524, "end": 172992}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 172992, "end": 175604}, {"filename": "/examples/rvsimple/metron/config.h", "start": 175604, "end": 176826}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 176826, "end": 182552}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 182552, "end": 183669}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 183669, "end": 185609}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 185609, "end": 186849}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 186849, "end": 188100}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 188100, "end": 188779}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 188779, "end": 189750}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 189750, "end": 191875}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 191875, "end": 192646}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 192646, "end": 193333}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 193333, "end": 194156}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 194156, "end": 195162}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 195162, "end": 196140}, {"filename": "/examples/rvsimple/metron/register.h", "start": 196140, "end": 196836}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 196836, "end": 199886}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 199886, "end": 205467}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 205467, "end": 207967}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 207967, "end": 212705}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 212705, "end": 214700}, {"filename": "/examples/scratch.h", "start": 214700, "end": 215553}, {"filename": "/examples/scratch.sv.d", "start": 215553, "end": 215593}, {"filename": "/examples/tutorial/adder.h", "start": 215593, "end": 215773}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 215773, "end": 216761}, {"filename": "/examples/tutorial/blockram.h", "start": 216761, "end": 217289}, {"filename": "/examples/tutorial/checksum.h", "start": 217289, "end": 218031}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 218031, "end": 218438}, {"filename": "/examples/tutorial/counter.h", "start": 218438, "end": 218589}, {"filename": "/examples/tutorial/declaration_order.h", "start": 218589, "end": 219415}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 219415, "end": 220843}, {"filename": "/examples/tutorial/nonblocking.h", "start": 220843, "end": 220969}, {"filename": "/examples/tutorial/submodules.h", "start": 220969, "end": 222088}, {"filename": "/examples/tutorial/templates.h", "start": 222088, "end": 222571}, {"filename": "/examples/tutorial/tutorial2.h", "start": 222571, "end": 222639}, {"filename": "/examples/tutorial/tutorial3.h", "start": 222639, "end": 222680}, {"filename": "/examples/tutorial/tutorial4.h", "start": 222680, "end": 222721}, {"filename": "/examples/tutorial/tutorial5.h", "start": 222721, "end": 222762}, {"filename": "/examples/tutorial/vga.h", "start": 222762, "end": 223942}, {"filename": "/examples/uart/README.md", "start": 223942, "end": 224186}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 224186, "end": 226824}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 226824, "end": 229515}, {"filename": "/examples/uart/metron/uart_top.h", "start": 229515, "end": 231303}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 231303, "end": 234360}, {"filename": "/tests/metron/fail/README.md", "start": 234360, "end": 234557}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 234557, "end": 234864}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 234864, "end": 235119}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 235119, "end": 235339}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 235339, "end": 235825}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 235825, "end": 236062}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 236062, "end": 236479}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 236479, "end": 236945}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 236945, "end": 237263}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 237263, "end": 237715}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 237715, "end": 237972}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 237972, "end": 238254}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 238254, "end": 238490}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 238490, "end": 238720}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 238720, "end": 239089}, {"filename": "/tests/metron/pass/README.md", "start": 239089, "end": 239170}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 239170, "end": 240671}, {"filename": "/tests/metron/pass/all_func_types.sv.d", "start": 240671, "end": 240743}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 240743, "end": 241160}, {"filename": "/tests/metron/pass/basic_constructor.sv.d", "start": 241160, "end": 241238}, {"filename": "/tests/metron/pass/basic_function.h", "start": 241238, "end": 241490}, {"filename": "/tests/metron/pass/basic_function.sv.d", "start": 241490, "end": 241562}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 241562, "end": 241889}, {"filename": "/tests/metron/pass/basic_increment.sv.d", "start": 241889, "end": 241963}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 241963, "end": 242268}, {"filename": "/tests/metron/pass/basic_inputs.sv.d", "start": 242268, "end": 242336}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 242336, "end": 242955}, {"filename": "/tests/metron/pass/basic_literals.sv.d", "start": 242955, "end": 243027}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 243027, "end": 243283}, {"filename": "/tests/metron/pass/basic_localparam.sv.d", "start": 243283, "end": 243359}, {"filename": "/tests/metron/pass/basic_output.h", "start": 243359, "end": 243631}, {"filename": "/tests/metron/pass/basic_output.sv.d", "start": 243631, "end": 243699}, {"filename": "/tests/metron/pass/basic_param.h", "start": 243699, "end": 243968}, {"filename": "/tests/metron/pass/basic_param.sv.d", "start": 243968, "end": 244034}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 244034, "end": 244235}, {"filename": "/tests/metron/pass/basic_public_reg.sv.d", "start": 244235, "end": 244311}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 244311, "end": 244498}, {"filename": "/tests/metron/pass/basic_public_sig.sv.d", "start": 244498, "end": 244574}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 244574, "end": 244805}, {"filename": "/tests/metron/pass/basic_reg_rww.sv.d", "start": 244805, "end": 244875}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 244875, "end": 245102}, {"filename": "/tests/metron/pass/basic_sig_wwr.sv.d", "start": 245102, "end": 245172}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 245172, "end": 245488}, {"filename": "/tests/metron/pass/basic_submod.sv.d", "start": 245488, "end": 245556}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 245556, "end": 245921}, {"filename": "/tests/metron/pass/basic_submod_param.sv.d", "start": 245921, "end": 246001}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 246001, "end": 246387}, {"filename": "/tests/metron/pass/basic_submod_public_reg.sv.d", "start": 246387, "end": 246477}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 246477, "end": 246966}, {"filename": "/tests/metron/pass/basic_switch.sv.d", "start": 246966, "end": 247034}, {"filename": "/tests/metron/pass/basic_task.h", "start": 247034, "end": 247369}, {"filename": "/tests/metron/pass/basic_task.sv.d", "start": 247369, "end": 247433}, {"filename": "/tests/metron/pass/basic_template.h", "start": 247433, "end": 247935}, {"filename": "/tests/metron/pass/basic_template.sv.d", "start": 247935, "end": 248007}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 248007, "end": 253987}, {"filename": "/tests/metron/pass/bit_casts.sv.d", "start": 253987, "end": 254049}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 254049, "end": 254483}, {"filename": "/tests/metron/pass/bit_concat.sv.d", "start": 254483, "end": 254547}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 254547, "end": 255213}, {"filename": "/tests/metron/pass/bit_dup.sv.d", "start": 255213, "end": 255271}, {"filename": "/tests/metron/pass/bitfields.h", "start": 255271, "end": 256477}, {"filename": "/tests/metron/pass/bitfields.sv.d", "start": 256477, "end": 256539}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 256539, "end": 257356}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.sv.d", "start": 257356, "end": 257476}, {"filename": "/tests/metron/pass/builtins.h", "start": 257476, "end": 257814}, {"filename": "/tests/metron/pass/builtins.sv.d", "start": 257814, "end": 257874}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 257874, "end": 258191}, {"filename": "/tests/metron/pass/call_tick_from_tock.sv.d", "start": 258191, "end": 258273}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 258273, "end": 258858}, {"filename": "/tests/metron/pass/case_with_fallthrough.sv.d", "start": 258858, "end": 258944}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 258944, "end": 259807}, {"filename": "/tests/metron/pass/constructor_arg_passing.sv.d", "start": 259807, "end": 259897}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 259897, "end": 260369}, {"filename": "/tests/metron/pass/constructor_args.sv.d", "start": 260369, "end": 260445}, {"filename": "/tests/metron/pass/counter.h", "start": 260445, "end": 261092}, {"filename": "/tests/metron/pass/counter.sv.d", "start": 261092, "end": 261150}, {"filename": "/tests/metron/pass/defines.h", "start": 261150, "end": 261471}, {"filename": "/tests/metron/pass/defines.sv.d", "start": 261471, "end": 261529}, {"filename": "/tests/metron/pass/dontcare.h", "start": 261529, "end": 261816}, {"filename": "/tests/metron/pass/dontcare.sv.d", "start": 261816, "end": 261876}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 261876, "end": 263271}, {"filename": "/tests/metron/pass/enum_simple.sv.d", "start": 263271, "end": 263337}, {"filename": "/tests/metron/pass/for_loops.h", "start": 263337, "end": 263664}, {"filename": "/tests/metron/pass/for_loops.sv.d", "start": 263664, "end": 263726}, {"filename": "/tests/metron/pass/good_order.h", "start": 263726, "end": 263929}, {"filename": "/tests/metron/pass/good_order.sv.d", "start": 263929, "end": 263993}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 263993, "end": 264414}, {"filename": "/tests/metron/pass/if_with_compound.sv.d", "start": 264414, "end": 264490}, {"filename": "/tests/metron/pass/include_guards.h", "start": 264490, "end": 264687}, {"filename": "/tests/metron/pass/include_guards.sv.d", "start": 264687, "end": 264759}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 264759, "end": 264950}, {"filename": "/tests/metron/pass/include_test_module.sv.d", "start": 264950, "end": 265075}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 265075, "end": 265252}, {"filename": "/tests/metron/pass/include_test_submod.sv.d", "start": 265252, "end": 265334}, {"filename": "/tests/metron/pass/init_chain.h", "start": 265334, "end": 266055}, {"filename": "/tests/metron/pass/init_chain.sv.d", "start": 266055, "end": 266119}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 266119, "end": 266411}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.sv.d", "start": 266411, "end": 266505}, {"filename": "/tests/metron/pass/input_signals.h", "start": 266505, "end": 267301}, {"filename": "/tests/metron/pass/input_signals.sv.d", "start": 267301, "end": 267371}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 267371, "end": 267556}, {"filename": "/tests/metron/pass/local_localparam.sv.d", "start": 267556, "end": 267632}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 267632, "end": 267945}, {"filename": "/tests/metron/pass/magic_comments.sv.d", "start": 267945, "end": 268017}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 268017, "end": 268338}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.sv.d", "start": 268338, "end": 268436}, {"filename": "/tests/metron/pass/minimal.h", "start": 268436, "end": 268616}, {"filename": "/tests/metron/pass/minimal.sv.d", "start": 268616, "end": 268674}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 268674, "end": 269050}, {"filename": "/tests/metron/pass/multi_tick.sv.d", "start": 269050, "end": 269114}, {"filename": "/tests/metron/pass/namespaces.h", "start": 269114, "end": 269495}, {"filename": "/tests/metron/pass/namespaces.sv.d", "start": 269495, "end": 269559}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 269559, "end": 270020}, {"filename": "/tests/metron/pass/nested_structs.sv.d", "start": 270020, "end": 270092}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 270092, "end": 270665}, {"filename": "/tests/metron/pass/nested_submod_calls.sv.d", "start": 270665, "end": 270747}, {"filename": "/tests/metron/pass/noconvert.h", "start": 270747, "end": 271077}, {"filename": "/tests/metron/pass/noconvert.sv.d", "start": 271077, "end": 271139}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 271139, "end": 271492}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.sv.d", "start": 271492, "end": 271604}, {"filename": "/tests/metron/pass/oneliners.h", "start": 271604, "end": 271878}, {"filename": "/tests/metron/pass/oneliners.sv.d", "start": 271878, "end": 271940}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 271940, "end": 272427}, {"filename": "/tests/metron/pass/plus_equals.sv.d", "start": 272427, "end": 272493}, {"filename": "/tests/metron/pass/preproc.h", "start": 272493, "end": 272821}, {"filename": "/tests/metron/pass/private_getter.h", "start": 272821, "end": 273073}, {"filename": "/tests/metron/pass/private_getter.sv.d", "start": 273073, "end": 273145}, {"filename": "/tests/metron/pass/self_reference.h", "start": 273145, "end": 273350}, {"filename": "/tests/metron/pass/self_reference.sv.d", "start": 273350, "end": 273422}, {"filename": "/tests/metron/pass/slice.h", "start": 273422, "end": 273929}, {"filename": "/tests/metron/pass/slice.sv.d", "start": 273929, "end": 273983}, {"filename": "/tests/metron/pass/structs.h", "start": 273983, "end": 274437}, {"filename": "/tests/metron/pass/structs.sv.d", "start": 274437, "end": 274495}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 274495, "end": 275040}, {"filename": "/tests/metron/pass/structs_as_args.sv.d", "start": 275040, "end": 275114}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 275114, "end": 277734}, {"filename": "/tests/metron/pass/structs_as_ports.sv.d", "start": 277734, "end": 277810}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 277810, "end": 278604}, {"filename": "/tests/metron/pass/submod_bindings.sv.d", "start": 278604, "end": 278678}, {"filename": "/tests/metron/pass/tock_task.h", "start": 278678, "end": 279130}, {"filename": "/tests/metron/pass/tock_task.sv.d", "start": 279130, "end": 279192}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 279192, "end": 279364}, {"filename": "/tests/metron/pass/trivial_adder.sv.d", "start": 279364, "end": 279434}, {"filename": "/tests/metron/pass/unions.h", "start": 279434, "end": 279640}, {"filename": "/tests/metron/pass/unions.sv.d", "start": 279640, "end": 279696}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 279696, "end": 280365}, {"filename": "/tests/metron/pass/utf8-mod.bom.sv.d", "start": 280365, "end": 280433}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 280433, "end": 281102}, {"filename": "/tests/metron/pass/utf8-mod.sv.d", "start": 281102, "end": 281162}], "remote_package_size": 281162});

  })();
