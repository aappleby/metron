
  var Module = typeof Module != 'undefined' ? Module : {};

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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 535}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 535, "end": 20377}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20377, "end": 21007}, {"filename": "/examples/ibex/README.md", "start": 21007, "end": 21058}, {"filename": "/examples/ibex/ibex.hancho", "start": 21058, "end": 21058}, {"filename": "/examples/ibex/ibex_alu.h", "start": 21058, "end": 22697}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22697, "end": 34882}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34882, "end": 49374}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49374, "end": 65451}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65451, "end": 68036}, {"filename": "/examples/j1/dpram.h", "start": 68036, "end": 68497}, {"filename": "/examples/j1/j1.h", "start": 68497, "end": 72788}, {"filename": "/examples/j1/j1.hancho", "start": 72788, "end": 73481}, {"filename": "/examples/picorv32/picorv32.h", "start": 73481, "end": 168685}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168685, "end": 168685}, {"filename": "/examples/pong/README.md", "start": 168685, "end": 168745}, {"filename": "/examples/pong/pong.h", "start": 168745, "end": 172736}, {"filename": "/examples/pong/pong.hancho", "start": 172736, "end": 173232}, {"filename": "/examples/pong/reference/README.md", "start": 173232, "end": 173292}, {"filename": "/examples/rvsimple/README.md", "start": 173292, "end": 173371}, {"filename": "/examples/rvsimple/adder.h", "start": 173371, "end": 173878}, {"filename": "/examples/rvsimple/alu.h", "start": 173878, "end": 175346}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175346, "end": 177958}, {"filename": "/examples/rvsimple/config.h", "start": 177958, "end": 179180}, {"filename": "/examples/rvsimple/constants.h", "start": 179180, "end": 184906}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 184906, "end": 186023}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 186023, "end": 187963}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 187963, "end": 189203}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 189203, "end": 190454}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190454, "end": 191133}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 191133, "end": 192104}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 192104, "end": 194229}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194229, "end": 195000}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 195000, "end": 195687}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 195687, "end": 196510}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 196510, "end": 197516}, {"filename": "/examples/rvsimple/regfile.h", "start": 197516, "end": 198494}, {"filename": "/examples/rvsimple/register.h", "start": 198494, "end": 199190}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 199190, "end": 202240}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 202240, "end": 205277}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 205277, "end": 210858}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 210858, "end": 213358}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 213358, "end": 218096}, {"filename": "/examples/rvsimple/toplevel.h", "start": 218096, "end": 220091}, {"filename": "/examples/scratch.h", "start": 220091, "end": 220459}, {"filename": "/examples/tutorial/adder.h", "start": 220459, "end": 220639}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 220639, "end": 221627}, {"filename": "/examples/tutorial/blockram.h", "start": 221627, "end": 222159}, {"filename": "/examples/tutorial/checksum.h", "start": 222159, "end": 222901}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 222901, "end": 223308}, {"filename": "/examples/tutorial/counter.h", "start": 223308, "end": 223459}, {"filename": "/examples/tutorial/declaration_order.h", "start": 223459, "end": 224285}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 224285, "end": 225244}, {"filename": "/examples/tutorial/nonblocking.h", "start": 225244, "end": 225370}, {"filename": "/examples/tutorial/submodules.h", "start": 225370, "end": 226470}, {"filename": "/examples/tutorial/templates.h", "start": 226470, "end": 226953}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 226953, "end": 226953}, {"filename": "/examples/tutorial/tutorial2.h", "start": 226953, "end": 227021}, {"filename": "/examples/tutorial/tutorial3.h", "start": 227021, "end": 227062}, {"filename": "/examples/tutorial/tutorial4.h", "start": 227062, "end": 227103}, {"filename": "/examples/tutorial/tutorial5.h", "start": 227103, "end": 227144}, {"filename": "/examples/tutorial/vga.h", "start": 227144, "end": 228324}, {"filename": "/examples/uart/README.md", "start": 228324, "end": 228568}, {"filename": "/examples/uart/uart.hancho", "start": 228568, "end": 231488}, {"filename": "/examples/uart/uart_hello.h", "start": 231488, "end": 234156}, {"filename": "/examples/uart/uart_rx.h", "start": 234156, "end": 236847}, {"filename": "/examples/uart/uart_top.h", "start": 236847, "end": 238723}, {"filename": "/examples/uart/uart_tx.h", "start": 238723, "end": 241780}, {"filename": "/tests/metron/fail/README.md", "start": 241780, "end": 241977}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 241977, "end": 242284}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 242284, "end": 242539}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 242539, "end": 242759}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 242759, "end": 243245}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 243245, "end": 243482}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 243482, "end": 243899}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 243899, "end": 244365}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 244365, "end": 244683}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 244683, "end": 245135}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 245135, "end": 245392}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 245392, "end": 245674}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 245674, "end": 245910}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 245910, "end": 246140}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 246140, "end": 246509}, {"filename": "/tests/metron/pass/README.md", "start": 246509, "end": 246590}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 246590, "end": 248091}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 248091, "end": 248508}, {"filename": "/tests/metron/pass/basic_function.h", "start": 248508, "end": 248760}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 248760, "end": 249087}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 249087, "end": 249392}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 249392, "end": 250011}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 250011, "end": 250267}, {"filename": "/tests/metron/pass/basic_output.h", "start": 250267, "end": 250539}, {"filename": "/tests/metron/pass/basic_param.h", "start": 250539, "end": 250808}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 250808, "end": 251009}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 251009, "end": 251196}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 251196, "end": 251427}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 251427, "end": 251654}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 251654, "end": 251970}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 251970, "end": 252335}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 252335, "end": 252721}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 252721, "end": 253210}, {"filename": "/tests/metron/pass/basic_task.h", "start": 253210, "end": 253545}, {"filename": "/tests/metron/pass/basic_template.h", "start": 253545, "end": 254047}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 254047, "end": 260027}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 260027, "end": 260461}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 260461, "end": 261127}, {"filename": "/tests/metron/pass/bitfields.h", "start": 261127, "end": 262333}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 262333, "end": 263150}, {"filename": "/tests/metron/pass/builtins.h", "start": 263150, "end": 263488}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 263488, "end": 263805}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 263805, "end": 264390}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 264390, "end": 265245}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 265245, "end": 265723}, {"filename": "/tests/metron/pass/counter.h", "start": 265723, "end": 266370}, {"filename": "/tests/metron/pass/defines.h", "start": 266370, "end": 266691}, {"filename": "/tests/metron/pass/dontcare.h", "start": 266691, "end": 266978}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 266978, "end": 268373}, {"filename": "/tests/metron/pass/for_loops.h", "start": 268373, "end": 268700}, {"filename": "/tests/metron/pass/good_order.h", "start": 268700, "end": 268903}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 268903, "end": 269324}, {"filename": "/tests/metron/pass/include_guards.h", "start": 269324, "end": 269521}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 269521, "end": 269712}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 269712, "end": 269889}, {"filename": "/tests/metron/pass/init_chain.h", "start": 269889, "end": 270610}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 270610, "end": 270902}, {"filename": "/tests/metron/pass/input_signals.h", "start": 270902, "end": 271575}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 271575, "end": 271760}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 271760, "end": 272073}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 272073, "end": 272394}, {"filename": "/tests/metron/pass/minimal.h", "start": 272394, "end": 272574}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 272574, "end": 272950}, {"filename": "/tests/metron/pass/namespaces.h", "start": 272950, "end": 273331}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 273331, "end": 273792}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 273792, "end": 274365}, {"filename": "/tests/metron/pass/noconvert.h", "start": 274365, "end": 274695}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 274695, "end": 275048}, {"filename": "/tests/metron/pass/oneliners.h", "start": 275048, "end": 275322}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 275322, "end": 275809}, {"filename": "/tests/metron/pass/preproc.h", "start": 275809, "end": 276137}, {"filename": "/tests/metron/pass/private_getter.h", "start": 276137, "end": 276389}, {"filename": "/tests/metron/pass/self_reference.h", "start": 276389, "end": 276594}, {"filename": "/tests/metron/pass/slice.h", "start": 276594, "end": 277101}, {"filename": "/tests/metron/pass/structs.h", "start": 277101, "end": 277555}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 277555, "end": 278100}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 278100, "end": 280720}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 280720, "end": 281514}, {"filename": "/tests/metron/pass/tock_task.h", "start": 281514, "end": 281966}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 281966, "end": 282138}, {"filename": "/tests/metron/pass/unions.h", "start": 282138, "end": 282344}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 282344, "end": 283013}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 283013, "end": 283682}], "remote_package_size": 283682});

  })();
