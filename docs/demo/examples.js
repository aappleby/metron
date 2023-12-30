
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/picorv32/picorv32.h", "start": 71623, "end": 166827}, {"filename": "/examples/pong/README.md", "start": 166827, "end": 166887}, {"filename": "/examples/pong/metron/pong.h", "start": 166887, "end": 170878}, {"filename": "/examples/pong/reference/README.md", "start": 170878, "end": 170938}, {"filename": "/examples/rvsimple/README.md", "start": 170938, "end": 171017}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 171017, "end": 171524}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 171524, "end": 172992}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 172992, "end": 175604}, {"filename": "/examples/rvsimple/metron/config.h", "start": 175604, "end": 176826}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 176826, "end": 182552}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 182552, "end": 183669}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 183669, "end": 185609}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 185609, "end": 186849}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 186849, "end": 188100}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 188100, "end": 188779}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 188779, "end": 189750}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 189750, "end": 191875}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 191875, "end": 192646}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 192646, "end": 193333}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 193333, "end": 194156}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 194156, "end": 195162}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 195162, "end": 196140}, {"filename": "/examples/rvsimple/metron/register.h", "start": 196140, "end": 196836}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 196836, "end": 199886}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 199886, "end": 205467}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 205467, "end": 207967}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 207967, "end": 212705}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 212705, "end": 214700}, {"filename": "/examples/scratch.h", "start": 214700, "end": 215607}, {"filename": "/examples/scratch.sv.d", "start": 215607, "end": 215647}, {"filename": "/examples/tutorial/adder.h", "start": 215647, "end": 215827}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 215827, "end": 216815}, {"filename": "/examples/tutorial/blockram.h", "start": 216815, "end": 217343}, {"filename": "/examples/tutorial/checksum.h", "start": 217343, "end": 218085}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 218085, "end": 218492}, {"filename": "/examples/tutorial/counter.h", "start": 218492, "end": 218643}, {"filename": "/examples/tutorial/declaration_order.h", "start": 218643, "end": 219469}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 219469, "end": 220897}, {"filename": "/examples/tutorial/nonblocking.h", "start": 220897, "end": 221023}, {"filename": "/examples/tutorial/submodules.h", "start": 221023, "end": 222142}, {"filename": "/examples/tutorial/templates.h", "start": 222142, "end": 222625}, {"filename": "/examples/tutorial/tutorial2.h", "start": 222625, "end": 222693}, {"filename": "/examples/tutorial/tutorial3.h", "start": 222693, "end": 222734}, {"filename": "/examples/tutorial/tutorial4.h", "start": 222734, "end": 222775}, {"filename": "/examples/tutorial/tutorial5.h", "start": 222775, "end": 222816}, {"filename": "/examples/tutorial/vga.h", "start": 222816, "end": 223996}, {"filename": "/examples/uart/README.md", "start": 223996, "end": 224240}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 224240, "end": 226878}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 226878, "end": 229569}, {"filename": "/examples/uart/metron/uart_top.h", "start": 229569, "end": 231357}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 231357, "end": 234414}, {"filename": "/tests/metron/fail/README.md", "start": 234414, "end": 234611}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 234611, "end": 234918}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 234918, "end": 235173}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 235173, "end": 235393}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 235393, "end": 235879}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 235879, "end": 236116}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 236116, "end": 236533}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 236533, "end": 236999}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 236999, "end": 237317}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 237317, "end": 237769}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 237769, "end": 238026}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 238026, "end": 238308}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 238308, "end": 238544}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 238544, "end": 238774}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 238774, "end": 239143}, {"filename": "/tests/metron/pass/README.md", "start": 239143, "end": 239224}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 239224, "end": 240725}, {"filename": "/tests/metron/pass/all_func_types.sv.d", "start": 240725, "end": 240797}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 240797, "end": 241214}, {"filename": "/tests/metron/pass/basic_constructor.sv.d", "start": 241214, "end": 241292}, {"filename": "/tests/metron/pass/basic_function.h", "start": 241292, "end": 241544}, {"filename": "/tests/metron/pass/basic_function.sv.d", "start": 241544, "end": 241616}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 241616, "end": 241943}, {"filename": "/tests/metron/pass/basic_increment.sv.d", "start": 241943, "end": 242017}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 242017, "end": 242322}, {"filename": "/tests/metron/pass/basic_inputs.sv.d", "start": 242322, "end": 242390}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 242390, "end": 243009}, {"filename": "/tests/metron/pass/basic_literals.sv.d", "start": 243009, "end": 243081}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 243081, "end": 243337}, {"filename": "/tests/metron/pass/basic_localparam.sv.d", "start": 243337, "end": 243413}, {"filename": "/tests/metron/pass/basic_output.h", "start": 243413, "end": 243685}, {"filename": "/tests/metron/pass/basic_output.sv.d", "start": 243685, "end": 243753}, {"filename": "/tests/metron/pass/basic_param.h", "start": 243753, "end": 244022}, {"filename": "/tests/metron/pass/basic_param.sv.d", "start": 244022, "end": 244088}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 244088, "end": 244289}, {"filename": "/tests/metron/pass/basic_public_reg.sv.d", "start": 244289, "end": 244365}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 244365, "end": 244552}, {"filename": "/tests/metron/pass/basic_public_sig.sv.d", "start": 244552, "end": 244628}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 244628, "end": 244859}, {"filename": "/tests/metron/pass/basic_reg_rww.sv.d", "start": 244859, "end": 244929}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 244929, "end": 245156}, {"filename": "/tests/metron/pass/basic_sig_wwr.sv.d", "start": 245156, "end": 245226}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 245226, "end": 245542}, {"filename": "/tests/metron/pass/basic_submod.sv.d", "start": 245542, "end": 245610}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 245610, "end": 245975}, {"filename": "/tests/metron/pass/basic_submod_param.sv.d", "start": 245975, "end": 246055}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 246055, "end": 246441}, {"filename": "/tests/metron/pass/basic_submod_public_reg.sv.d", "start": 246441, "end": 246531}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 246531, "end": 247020}, {"filename": "/tests/metron/pass/basic_switch.sv.d", "start": 247020, "end": 247088}, {"filename": "/tests/metron/pass/basic_task.h", "start": 247088, "end": 247423}, {"filename": "/tests/metron/pass/basic_task.sv.d", "start": 247423, "end": 247487}, {"filename": "/tests/metron/pass/basic_template.h", "start": 247487, "end": 247989}, {"filename": "/tests/metron/pass/basic_template.sv.d", "start": 247989, "end": 248061}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 248061, "end": 254041}, {"filename": "/tests/metron/pass/bit_casts.sv.d", "start": 254041, "end": 254103}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 254103, "end": 254537}, {"filename": "/tests/metron/pass/bit_concat.sv.d", "start": 254537, "end": 254601}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 254601, "end": 255267}, {"filename": "/tests/metron/pass/bit_dup.sv.d", "start": 255267, "end": 255325}, {"filename": "/tests/metron/pass/bitfields.h", "start": 255325, "end": 256531}, {"filename": "/tests/metron/pass/bitfields.sv.d", "start": 256531, "end": 256593}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 256593, "end": 257410}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.sv.d", "start": 257410, "end": 257530}, {"filename": "/tests/metron/pass/builtins.h", "start": 257530, "end": 257868}, {"filename": "/tests/metron/pass/builtins.sv.d", "start": 257868, "end": 257928}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 257928, "end": 258245}, {"filename": "/tests/metron/pass/call_tick_from_tock.sv.d", "start": 258245, "end": 258327}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 258327, "end": 258912}, {"filename": "/tests/metron/pass/case_with_fallthrough.sv.d", "start": 258912, "end": 258998}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 258998, "end": 259861}, {"filename": "/tests/metron/pass/constructor_arg_passing.sv.d", "start": 259861, "end": 259951}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 259951, "end": 260423}, {"filename": "/tests/metron/pass/constructor_args.sv.d", "start": 260423, "end": 260499}, {"filename": "/tests/metron/pass/counter.h", "start": 260499, "end": 261146}, {"filename": "/tests/metron/pass/counter.sv.d", "start": 261146, "end": 261204}, {"filename": "/tests/metron/pass/defines.h", "start": 261204, "end": 261525}, {"filename": "/tests/metron/pass/defines.sv.d", "start": 261525, "end": 261583}, {"filename": "/tests/metron/pass/dontcare.h", "start": 261583, "end": 261870}, {"filename": "/tests/metron/pass/dontcare.sv.d", "start": 261870, "end": 261930}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 261930, "end": 263325}, {"filename": "/tests/metron/pass/enum_simple.sv.d", "start": 263325, "end": 263391}, {"filename": "/tests/metron/pass/for_loops.h", "start": 263391, "end": 263718}, {"filename": "/tests/metron/pass/for_loops.sv.d", "start": 263718, "end": 263780}, {"filename": "/tests/metron/pass/good_order.h", "start": 263780, "end": 263983}, {"filename": "/tests/metron/pass/good_order.sv.d", "start": 263983, "end": 264047}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 264047, "end": 264468}, {"filename": "/tests/metron/pass/if_with_compound.sv.d", "start": 264468, "end": 264544}, {"filename": "/tests/metron/pass/include_guards.h", "start": 264544, "end": 264741}, {"filename": "/tests/metron/pass/include_guards.sv.d", "start": 264741, "end": 264813}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 264813, "end": 265004}, {"filename": "/tests/metron/pass/include_test_module.sv.d", "start": 265004, "end": 265129}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 265129, "end": 265306}, {"filename": "/tests/metron/pass/include_test_submod.sv.d", "start": 265306, "end": 265388}, {"filename": "/tests/metron/pass/init_chain.h", "start": 265388, "end": 266109}, {"filename": "/tests/metron/pass/init_chain.sv.d", "start": 266109, "end": 266173}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 266173, "end": 266465}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.sv.d", "start": 266465, "end": 266559}, {"filename": "/tests/metron/pass/input_signals.h", "start": 266559, "end": 267355}, {"filename": "/tests/metron/pass/input_signals.sv.d", "start": 267355, "end": 267425}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 267425, "end": 267610}, {"filename": "/tests/metron/pass/local_localparam.sv.d", "start": 267610, "end": 267686}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 267686, "end": 267999}, {"filename": "/tests/metron/pass/magic_comments.sv.d", "start": 267999, "end": 268071}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 268071, "end": 268392}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.sv.d", "start": 268392, "end": 268490}, {"filename": "/tests/metron/pass/minimal.h", "start": 268490, "end": 268670}, {"filename": "/tests/metron/pass/minimal.sv.d", "start": 268670, "end": 268728}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 268728, "end": 269104}, {"filename": "/tests/metron/pass/multi_tick.sv.d", "start": 269104, "end": 269168}, {"filename": "/tests/metron/pass/namespaces.h", "start": 269168, "end": 269549}, {"filename": "/tests/metron/pass/namespaces.sv.d", "start": 269549, "end": 269613}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 269613, "end": 270074}, {"filename": "/tests/metron/pass/nested_structs.sv.d", "start": 270074, "end": 270146}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 270146, "end": 270719}, {"filename": "/tests/metron/pass/nested_submod_calls.sv.d", "start": 270719, "end": 270801}, {"filename": "/tests/metron/pass/noconvert.h", "start": 270801, "end": 271131}, {"filename": "/tests/metron/pass/noconvert.sv.d", "start": 271131, "end": 271193}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 271193, "end": 271546}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.sv.d", "start": 271546, "end": 271658}, {"filename": "/tests/metron/pass/oneliners.h", "start": 271658, "end": 271932}, {"filename": "/tests/metron/pass/oneliners.sv.d", "start": 271932, "end": 271994}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 271994, "end": 272481}, {"filename": "/tests/metron/pass/plus_equals.sv.d", "start": 272481, "end": 272547}, {"filename": "/tests/metron/pass/preproc.h", "start": 272547, "end": 272875}, {"filename": "/tests/metron/pass/private_getter.h", "start": 272875, "end": 273127}, {"filename": "/tests/metron/pass/private_getter.sv.d", "start": 273127, "end": 273199}, {"filename": "/tests/metron/pass/self_reference.h", "start": 273199, "end": 273404}, {"filename": "/tests/metron/pass/self_reference.sv.d", "start": 273404, "end": 273476}, {"filename": "/tests/metron/pass/slice.h", "start": 273476, "end": 273983}, {"filename": "/tests/metron/pass/slice.sv.d", "start": 273983, "end": 274037}, {"filename": "/tests/metron/pass/structs.h", "start": 274037, "end": 274491}, {"filename": "/tests/metron/pass/structs.sv.d", "start": 274491, "end": 274549}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 274549, "end": 275094}, {"filename": "/tests/metron/pass/structs_as_args.sv.d", "start": 275094, "end": 275168}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 275168, "end": 277788}, {"filename": "/tests/metron/pass/structs_as_ports.sv.d", "start": 277788, "end": 277864}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 277864, "end": 278658}, {"filename": "/tests/metron/pass/submod_bindings.sv.d", "start": 278658, "end": 278732}, {"filename": "/tests/metron/pass/tock_task.h", "start": 278732, "end": 279184}, {"filename": "/tests/metron/pass/tock_task.sv.d", "start": 279184, "end": 279246}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 279246, "end": 279418}, {"filename": "/tests/metron/pass/trivial_adder.sv.d", "start": 279418, "end": 279488}, {"filename": "/tests/metron/pass/unions.h", "start": 279488, "end": 279694}, {"filename": "/tests/metron/pass/unions.sv.d", "start": 279694, "end": 279750}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 279750, "end": 280419}, {"filename": "/tests/metron/pass/utf8-mod.bom.sv.d", "start": 280419, "end": 280487}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 280487, "end": 281156}, {"filename": "/tests/metron/pass/utf8-mod.sv.d", "start": 281156, "end": 281216}], "remote_package_size": 281216});

  })();
