
  var Module = typeof Module != 'undefined' ? Module : {};

  if (!Module.expectedDataFileDownloads) {
    Module.expectedDataFileDownloads = 0;
  }

  Module.expectedDataFileDownloads++;
  (() => {
    // Do not attempt to redownload the virtual filesystem data when in a pthread or a Wasm Worker context.
    var isPthread = typeof ENVIRONMENT_IS_PTHREAD != 'undefined' && ENVIRONMENT_IS_PTHREAD;
    var isWasmWorker = typeof ENVIRONMENT_IS_WASM_WORKER != 'undefined' && ENVIRONMENT_IS_WASM_WORKER;
    if (isPthread || isWasmWorker) return;
    function loadPackage(metadata) {

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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 368}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 368, "end": 20210}, {"filename": "/examples/gb_spu/MetroBoySPU2.sv.d", "start": 20210, "end": 20242}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20242, "end": 20803}, {"filename": "/examples/ibex/README.md", "start": 20803, "end": 20854}, {"filename": "/examples/ibex/ibex.hancho", "start": 20854, "end": 20854}, {"filename": "/examples/ibex/ibex_alu.h", "start": 20854, "end": 22493}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22493, "end": 34678}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34678, "end": 49170}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49170, "end": 65247}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65247, "end": 67832}, {"filename": "/examples/j1/dpram.h", "start": 67832, "end": 68293}, {"filename": "/examples/j1/dpram.sv.d", "start": 68293, "end": 68311}, {"filename": "/examples/j1/j1.h", "start": 68311, "end": 72602}, {"filename": "/examples/j1/j1.hancho", "start": 72602, "end": 73222}, {"filename": "/examples/j1/j1.sv.d", "start": 73222, "end": 73245}, {"filename": "/examples/j1/main.d", "start": 73245, "end": 73328}, {"filename": "/examples/picorv32/picorv32.h", "start": 73328, "end": 168532}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168532, "end": 168532}, {"filename": "/examples/pong/README.md", "start": 168532, "end": 168592}, {"filename": "/examples/pong/main.d", "start": 168592, "end": 168814}, {"filename": "/examples/pong/pong.h", "start": 168814, "end": 172805}, {"filename": "/examples/pong/pong.hancho", "start": 172805, "end": 173328}, {"filename": "/examples/pong/pong.sv.d", "start": 173328, "end": 173344}, {"filename": "/examples/pong/reference/README.md", "start": 173344, "end": 173404}, {"filename": "/examples/rvsimple/README.md", "start": 173404, "end": 173483}, {"filename": "/examples/rvsimple/adder.h", "start": 173483, "end": 173990}, {"filename": "/examples/rvsimple/alu.h", "start": 173990, "end": 175458}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175458, "end": 178070}, {"filename": "/examples/rvsimple/config.h", "start": 178070, "end": 179292}, {"filename": "/examples/rvsimple/constants.h", "start": 179292, "end": 185018}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 185018, "end": 186135}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 186135, "end": 188075}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 188075, "end": 189315}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 189315, "end": 190566}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190566, "end": 191245}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 191245, "end": 192216}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 192216, "end": 194341}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194341, "end": 195112}, {"filename": "/examples/rvsimple/main.d", "start": 195112, "end": 197660}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 197660, "end": 198347}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 198347, "end": 199170}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 199170, "end": 200176}, {"filename": "/examples/rvsimple/regfile.h", "start": 200176, "end": 201154}, {"filename": "/examples/rvsimple/register.h", "start": 201154, "end": 201850}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 201850, "end": 204900}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 204900, "end": 208084}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 208084, "end": 213665}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 213665, "end": 216165}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 216165, "end": 220903}, {"filename": "/examples/rvsimple/toplevel.h", "start": 220903, "end": 222898}, {"filename": "/examples/scratch.h", "start": 222898, "end": 223266}, {"filename": "/examples/tutorial/adder.h", "start": 223266, "end": 223446}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 223446, "end": 224434}, {"filename": "/examples/tutorial/blockram.h", "start": 224434, "end": 224966}, {"filename": "/examples/tutorial/checksum.h", "start": 224966, "end": 225708}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 225708, "end": 226115}, {"filename": "/examples/tutorial/counter.h", "start": 226115, "end": 226266}, {"filename": "/examples/tutorial/declaration_order.h", "start": 226266, "end": 227092}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 227092, "end": 228051}, {"filename": "/examples/tutorial/nonblocking.h", "start": 228051, "end": 228177}, {"filename": "/examples/tutorial/submodules.h", "start": 228177, "end": 229277}, {"filename": "/examples/tutorial/templates.h", "start": 229277, "end": 229760}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 229760, "end": 229760}, {"filename": "/examples/tutorial/tutorial2.h", "start": 229760, "end": 229828}, {"filename": "/examples/tutorial/tutorial3.h", "start": 229828, "end": 229869}, {"filename": "/examples/tutorial/tutorial4.h", "start": 229869, "end": 229910}, {"filename": "/examples/tutorial/tutorial5.h", "start": 229910, "end": 229951}, {"filename": "/examples/tutorial/vga.h", "start": 229951, "end": 231131}, {"filename": "/examples/uart/README.md", "start": 231131, "end": 231375}, {"filename": "/examples/uart/main.d", "start": 231375, "end": 231852}, {"filename": "/examples/uart/uart.hancho", "start": 231852, "end": 234839}, {"filename": "/examples/uart/uart_hello.h", "start": 234839, "end": 237507}, {"filename": "/examples/uart/uart_rx.h", "start": 237507, "end": 240198}, {"filename": "/examples/uart/uart_top.h", "start": 240198, "end": 242050}, {"filename": "/examples/uart/uart_tx.h", "start": 242050, "end": 245099}, {"filename": "/tests/metron/fail/README.md", "start": 245099, "end": 245296}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 245296, "end": 245603}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 245603, "end": 245858}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 245858, "end": 246078}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 246078, "end": 246564}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 246564, "end": 246801}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 246801, "end": 247218}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 247218, "end": 247684}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 247684, "end": 248002}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 248002, "end": 248454}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 248454, "end": 248711}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 248711, "end": 248993}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 248993, "end": 249229}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 249229, "end": 249459}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 249459, "end": 249828}, {"filename": "/tests/metron/pass/README.md", "start": 249828, "end": 249909}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 249909, "end": 251410}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 251410, "end": 251827}, {"filename": "/tests/metron/pass/basic_function.h", "start": 251827, "end": 252079}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 252079, "end": 252406}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 252406, "end": 252711}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 252711, "end": 253330}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 253330, "end": 253586}, {"filename": "/tests/metron/pass/basic_output.h", "start": 253586, "end": 253858}, {"filename": "/tests/metron/pass/basic_param.h", "start": 253858, "end": 254127}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 254127, "end": 254328}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 254328, "end": 254515}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 254515, "end": 254746}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 254746, "end": 254973}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 254973, "end": 255289}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 255289, "end": 255654}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 255654, "end": 256040}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 256040, "end": 256529}, {"filename": "/tests/metron/pass/basic_task.h", "start": 256529, "end": 256864}, {"filename": "/tests/metron/pass/basic_template.h", "start": 256864, "end": 257366}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 257366, "end": 263346}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 263346, "end": 263780}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 263780, "end": 264446}, {"filename": "/tests/metron/pass/bitfields.h", "start": 264446, "end": 265652}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 265652, "end": 266469}, {"filename": "/tests/metron/pass/builtins.h", "start": 266469, "end": 266807}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 266807, "end": 267124}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 267124, "end": 267709}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 267709, "end": 268564}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 268564, "end": 269042}, {"filename": "/tests/metron/pass/counter.h", "start": 269042, "end": 269689}, {"filename": "/tests/metron/pass/defines.h", "start": 269689, "end": 270010}, {"filename": "/tests/metron/pass/dontcare.h", "start": 270010, "end": 270297}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 270297, "end": 271692}, {"filename": "/tests/metron/pass/for_loops.h", "start": 271692, "end": 272019}, {"filename": "/tests/metron/pass/good_order.h", "start": 272019, "end": 272222}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 272222, "end": 272643}, {"filename": "/tests/metron/pass/include_guards.h", "start": 272643, "end": 272840}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 272840, "end": 273031}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 273031, "end": 273208}, {"filename": "/tests/metron/pass/init_chain.h", "start": 273208, "end": 273929}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 273929, "end": 274221}, {"filename": "/tests/metron/pass/input_signals.h", "start": 274221, "end": 274894}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 274894, "end": 275079}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 275079, "end": 275392}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 275392, "end": 275713}, {"filename": "/tests/metron/pass/minimal.h", "start": 275713, "end": 275893}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 275893, "end": 276269}, {"filename": "/tests/metron/pass/namespaces.h", "start": 276269, "end": 276650}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 276650, "end": 277111}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 277111, "end": 277684}, {"filename": "/tests/metron/pass/noconvert.h", "start": 277684, "end": 278014}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 278014, "end": 278367}, {"filename": "/tests/metron/pass/oneliners.h", "start": 278367, "end": 278641}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 278641, "end": 279128}, {"filename": "/tests/metron/pass/preproc.h", "start": 279128, "end": 279456}, {"filename": "/tests/metron/pass/private_getter.h", "start": 279456, "end": 279708}, {"filename": "/tests/metron/pass/self_reference.h", "start": 279708, "end": 279913}, {"filename": "/tests/metron/pass/slice.h", "start": 279913, "end": 280420}, {"filename": "/tests/metron/pass/structs.h", "start": 280420, "end": 280874}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 280874, "end": 281419}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 281419, "end": 284039}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 284039, "end": 284833}, {"filename": "/tests/metron/pass/tock_task.h", "start": 284833, "end": 285285}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 285285, "end": 285457}, {"filename": "/tests/metron/pass/unions.h", "start": 285457, "end": 285663}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 285663, "end": 286332}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 286332, "end": 287001}], "remote_package_size": 287001});

  })();
