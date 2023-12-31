
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/picorv32/picorv32.h", "start": 71623, "end": 166827}, {"filename": "/examples/pong/README.md", "start": 166827, "end": 166887}, {"filename": "/examples/pong/metron/pong.h", "start": 166887, "end": 170878}, {"filename": "/examples/pong/reference/README.md", "start": 170878, "end": 170938}, {"filename": "/examples/rvsimple/README.md", "start": 170938, "end": 171017}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 171017, "end": 171524}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 171524, "end": 172992}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 172992, "end": 175604}, {"filename": "/examples/rvsimple/metron/config.h", "start": 175604, "end": 176826}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 176826, "end": 182552}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 182552, "end": 183669}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 183669, "end": 185609}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 185609, "end": 186849}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 186849, "end": 188100}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 188100, "end": 188779}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 188779, "end": 189750}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 189750, "end": 191875}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 191875, "end": 192646}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 192646, "end": 193333}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 193333, "end": 194156}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 194156, "end": 195162}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 195162, "end": 196140}, {"filename": "/examples/rvsimple/metron/register.h", "start": 196140, "end": 196836}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 196836, "end": 199886}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 199886, "end": 205467}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 205467, "end": 207967}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 207967, "end": 212705}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 212705, "end": 214700}, {"filename": "/examples/scratch.h", "start": 214700, "end": 215250}, {"filename": "/examples/scratch.sv.d", "start": 215250, "end": 215290}, {"filename": "/examples/tutorial/adder.h", "start": 215290, "end": 215470}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 215470, "end": 216458}, {"filename": "/examples/tutorial/blockram.h", "start": 216458, "end": 216986}, {"filename": "/examples/tutorial/checksum.h", "start": 216986, "end": 217728}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 217728, "end": 218135}, {"filename": "/examples/tutorial/counter.h", "start": 218135, "end": 218286}, {"filename": "/examples/tutorial/declaration_order.h", "start": 218286, "end": 219112}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 219112, "end": 220540}, {"filename": "/examples/tutorial/nonblocking.h", "start": 220540, "end": 220666}, {"filename": "/examples/tutorial/submodules.h", "start": 220666, "end": 221785}, {"filename": "/examples/tutorial/templates.h", "start": 221785, "end": 222268}, {"filename": "/examples/tutorial/tutorial2.h", "start": 222268, "end": 222336}, {"filename": "/examples/tutorial/tutorial3.h", "start": 222336, "end": 222377}, {"filename": "/examples/tutorial/tutorial4.h", "start": 222377, "end": 222418}, {"filename": "/examples/tutorial/tutorial5.h", "start": 222418, "end": 222459}, {"filename": "/examples/tutorial/vga.h", "start": 222459, "end": 223639}, {"filename": "/examples/uart/README.md", "start": 223639, "end": 223883}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 223883, "end": 226521}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 226521, "end": 229212}, {"filename": "/examples/uart/metron/uart_top.h", "start": 229212, "end": 231000}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 231000, "end": 234057}, {"filename": "/tests/metron/fail/README.md", "start": 234057, "end": 234254}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 234254, "end": 234561}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 234561, "end": 234816}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 234816, "end": 235036}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 235036, "end": 235522}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 235522, "end": 235759}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 235759, "end": 236176}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 236176, "end": 236642}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 236642, "end": 236960}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 236960, "end": 237412}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 237412, "end": 237669}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 237669, "end": 237951}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 237951, "end": 238187}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 238187, "end": 238417}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 238417, "end": 238786}, {"filename": "/tests/metron/pass/README.md", "start": 238786, "end": 238867}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 238867, "end": 240368}, {"filename": "/tests/metron/pass/all_func_types.sv.d", "start": 240368, "end": 240440}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 240440, "end": 240857}, {"filename": "/tests/metron/pass/basic_constructor.sv.d", "start": 240857, "end": 240935}, {"filename": "/tests/metron/pass/basic_function.h", "start": 240935, "end": 241187}, {"filename": "/tests/metron/pass/basic_function.sv.d", "start": 241187, "end": 241259}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 241259, "end": 241586}, {"filename": "/tests/metron/pass/basic_increment.sv.d", "start": 241586, "end": 241660}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 241660, "end": 241965}, {"filename": "/tests/metron/pass/basic_inputs.sv.d", "start": 241965, "end": 242033}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 242033, "end": 242652}, {"filename": "/tests/metron/pass/basic_literals.sv.d", "start": 242652, "end": 242724}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 242724, "end": 242980}, {"filename": "/tests/metron/pass/basic_localparam.sv.d", "start": 242980, "end": 243056}, {"filename": "/tests/metron/pass/basic_output.h", "start": 243056, "end": 243328}, {"filename": "/tests/metron/pass/basic_output.sv.d", "start": 243328, "end": 243396}, {"filename": "/tests/metron/pass/basic_param.h", "start": 243396, "end": 243665}, {"filename": "/tests/metron/pass/basic_param.sv.d", "start": 243665, "end": 243731}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 243731, "end": 243932}, {"filename": "/tests/metron/pass/basic_public_reg.sv.d", "start": 243932, "end": 244008}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 244008, "end": 244195}, {"filename": "/tests/metron/pass/basic_public_sig.sv.d", "start": 244195, "end": 244271}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 244271, "end": 244502}, {"filename": "/tests/metron/pass/basic_reg_rww.sv.d", "start": 244502, "end": 244572}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 244572, "end": 244799}, {"filename": "/tests/metron/pass/basic_sig_wwr.sv.d", "start": 244799, "end": 244869}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 244869, "end": 245185}, {"filename": "/tests/metron/pass/basic_submod.sv.d", "start": 245185, "end": 245253}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 245253, "end": 245618}, {"filename": "/tests/metron/pass/basic_submod_param.sv.d", "start": 245618, "end": 245698}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 245698, "end": 246084}, {"filename": "/tests/metron/pass/basic_submod_public_reg.sv.d", "start": 246084, "end": 246174}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 246174, "end": 246663}, {"filename": "/tests/metron/pass/basic_switch.sv.d", "start": 246663, "end": 246731}, {"filename": "/tests/metron/pass/basic_task.h", "start": 246731, "end": 247066}, {"filename": "/tests/metron/pass/basic_task.sv.d", "start": 247066, "end": 247130}, {"filename": "/tests/metron/pass/basic_template.h", "start": 247130, "end": 247632}, {"filename": "/tests/metron/pass/basic_template.sv.d", "start": 247632, "end": 247704}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 247704, "end": 253684}, {"filename": "/tests/metron/pass/bit_casts.sv.d", "start": 253684, "end": 253746}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 253746, "end": 254180}, {"filename": "/tests/metron/pass/bit_concat.sv.d", "start": 254180, "end": 254244}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 254244, "end": 254910}, {"filename": "/tests/metron/pass/bit_dup.sv.d", "start": 254910, "end": 254968}, {"filename": "/tests/metron/pass/bitfields.h", "start": 254968, "end": 256174}, {"filename": "/tests/metron/pass/bitfields.sv.d", "start": 256174, "end": 256236}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 256236, "end": 257053}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.sv.d", "start": 257053, "end": 257173}, {"filename": "/tests/metron/pass/builtins.h", "start": 257173, "end": 257511}, {"filename": "/tests/metron/pass/builtins.sv.d", "start": 257511, "end": 257571}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 257571, "end": 257888}, {"filename": "/tests/metron/pass/call_tick_from_tock.sv.d", "start": 257888, "end": 257970}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 257970, "end": 258555}, {"filename": "/tests/metron/pass/case_with_fallthrough.sv.d", "start": 258555, "end": 258641}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 258641, "end": 259504}, {"filename": "/tests/metron/pass/constructor_arg_passing.sv.d", "start": 259504, "end": 259594}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 259594, "end": 260066}, {"filename": "/tests/metron/pass/constructor_args.sv.d", "start": 260066, "end": 260142}, {"filename": "/tests/metron/pass/counter.h", "start": 260142, "end": 260789}, {"filename": "/tests/metron/pass/counter.sv.d", "start": 260789, "end": 260847}, {"filename": "/tests/metron/pass/defines.h", "start": 260847, "end": 261168}, {"filename": "/tests/metron/pass/defines.sv.d", "start": 261168, "end": 261226}, {"filename": "/tests/metron/pass/dontcare.h", "start": 261226, "end": 261513}, {"filename": "/tests/metron/pass/dontcare.sv.d", "start": 261513, "end": 261573}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 261573, "end": 262968}, {"filename": "/tests/metron/pass/enum_simple.sv.d", "start": 262968, "end": 263034}, {"filename": "/tests/metron/pass/for_loops.h", "start": 263034, "end": 263361}, {"filename": "/tests/metron/pass/for_loops.sv.d", "start": 263361, "end": 263423}, {"filename": "/tests/metron/pass/good_order.h", "start": 263423, "end": 263626}, {"filename": "/tests/metron/pass/good_order.sv.d", "start": 263626, "end": 263690}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 263690, "end": 264111}, {"filename": "/tests/metron/pass/if_with_compound.sv.d", "start": 264111, "end": 264187}, {"filename": "/tests/metron/pass/include_guards.h", "start": 264187, "end": 264384}, {"filename": "/tests/metron/pass/include_guards.sv.d", "start": 264384, "end": 264456}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 264456, "end": 264647}, {"filename": "/tests/metron/pass/include_test_module.sv.d", "start": 264647, "end": 264772}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 264772, "end": 264949}, {"filename": "/tests/metron/pass/include_test_submod.sv.d", "start": 264949, "end": 265031}, {"filename": "/tests/metron/pass/init_chain.h", "start": 265031, "end": 265752}, {"filename": "/tests/metron/pass/init_chain.sv.d", "start": 265752, "end": 265816}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 265816, "end": 266108}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.sv.d", "start": 266108, "end": 266202}, {"filename": "/tests/metron/pass/input_signals.h", "start": 266202, "end": 266998}, {"filename": "/tests/metron/pass/input_signals.sv.d", "start": 266998, "end": 267068}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 267068, "end": 267253}, {"filename": "/tests/metron/pass/local_localparam.sv.d", "start": 267253, "end": 267329}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 267329, "end": 267642}, {"filename": "/tests/metron/pass/magic_comments.sv.d", "start": 267642, "end": 267714}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 267714, "end": 268035}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.sv.d", "start": 268035, "end": 268133}, {"filename": "/tests/metron/pass/minimal.h", "start": 268133, "end": 268313}, {"filename": "/tests/metron/pass/minimal.sv.d", "start": 268313, "end": 268371}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 268371, "end": 268747}, {"filename": "/tests/metron/pass/multi_tick.sv.d", "start": 268747, "end": 268811}, {"filename": "/tests/metron/pass/namespaces.h", "start": 268811, "end": 269192}, {"filename": "/tests/metron/pass/namespaces.sv.d", "start": 269192, "end": 269256}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 269256, "end": 269717}, {"filename": "/tests/metron/pass/nested_structs.sv.d", "start": 269717, "end": 269789}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 269789, "end": 270362}, {"filename": "/tests/metron/pass/nested_submod_calls.sv.d", "start": 270362, "end": 270444}, {"filename": "/tests/metron/pass/noconvert.h", "start": 270444, "end": 270774}, {"filename": "/tests/metron/pass/noconvert.sv.d", "start": 270774, "end": 270836}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 270836, "end": 271189}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.sv.d", "start": 271189, "end": 271301}, {"filename": "/tests/metron/pass/oneliners.h", "start": 271301, "end": 271575}, {"filename": "/tests/metron/pass/oneliners.sv.d", "start": 271575, "end": 271637}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 271637, "end": 272124}, {"filename": "/tests/metron/pass/plus_equals.sv.d", "start": 272124, "end": 272190}, {"filename": "/tests/metron/pass/preproc.h", "start": 272190, "end": 272518}, {"filename": "/tests/metron/pass/private_getter.h", "start": 272518, "end": 272770}, {"filename": "/tests/metron/pass/private_getter.sv.d", "start": 272770, "end": 272842}, {"filename": "/tests/metron/pass/self_reference.h", "start": 272842, "end": 273047}, {"filename": "/tests/metron/pass/self_reference.sv.d", "start": 273047, "end": 273119}, {"filename": "/tests/metron/pass/slice.h", "start": 273119, "end": 273626}, {"filename": "/tests/metron/pass/slice.sv.d", "start": 273626, "end": 273680}, {"filename": "/tests/metron/pass/structs.h", "start": 273680, "end": 274134}, {"filename": "/tests/metron/pass/structs.sv.d", "start": 274134, "end": 274192}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 274192, "end": 274737}, {"filename": "/tests/metron/pass/structs_as_args.sv.d", "start": 274737, "end": 274811}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 274811, "end": 277431}, {"filename": "/tests/metron/pass/structs_as_ports.sv.d", "start": 277431, "end": 277507}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 277507, "end": 278301}, {"filename": "/tests/metron/pass/submod_bindings.sv.d", "start": 278301, "end": 278375}, {"filename": "/tests/metron/pass/tock_task.h", "start": 278375, "end": 278827}, {"filename": "/tests/metron/pass/tock_task.sv.d", "start": 278827, "end": 278889}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 278889, "end": 279061}, {"filename": "/tests/metron/pass/trivial_adder.sv.d", "start": 279061, "end": 279131}, {"filename": "/tests/metron/pass/unions.h", "start": 279131, "end": 279337}, {"filename": "/tests/metron/pass/unions.sv.d", "start": 279337, "end": 279393}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 279393, "end": 280062}, {"filename": "/tests/metron/pass/utf8-mod.bom.sv.d", "start": 280062, "end": 280130}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 280130, "end": 280799}, {"filename": "/tests/metron/pass/utf8-mod.sv.d", "start": 280799, "end": 280859}], "remote_package_size": 280859});

  })();
