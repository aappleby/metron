
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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 348}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 348, "end": 20190}, {"filename": "/examples/gb_spu/MetroBoySPU2.sv.d", "start": 20190, "end": 20222}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20222, "end": 20793}, {"filename": "/examples/ibex/README.md", "start": 20793, "end": 20844}, {"filename": "/examples/ibex/ibex.hancho", "start": 20844, "end": 20844}, {"filename": "/examples/ibex/ibex_alu.h", "start": 20844, "end": 22483}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22483, "end": 34668}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34668, "end": 49160}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49160, "end": 65237}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65237, "end": 67822}, {"filename": "/examples/j1/dpram.h", "start": 67822, "end": 68283}, {"filename": "/examples/j1/dpram.sv.d", "start": 68283, "end": 68301}, {"filename": "/examples/j1/j1.h", "start": 68301, "end": 72592}, {"filename": "/examples/j1/j1.hancho", "start": 72592, "end": 73205}, {"filename": "/examples/j1/j1.sv.d", "start": 73205, "end": 73228}, {"filename": "/examples/j1/main.d", "start": 73228, "end": 73311}, {"filename": "/examples/picorv32/picorv32.h", "start": 73311, "end": 168515}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168515, "end": 168515}, {"filename": "/examples/pong/README.md", "start": 168515, "end": 168575}, {"filename": "/examples/pong/main.d", "start": 168575, "end": 168797}, {"filename": "/examples/pong/pong.h", "start": 168797, "end": 172788}, {"filename": "/examples/pong/pong.hancho", "start": 172788, "end": 173261}, {"filename": "/examples/pong/pong.sv.d", "start": 173261, "end": 173277}, {"filename": "/examples/pong/reference/README.md", "start": 173277, "end": 173337}, {"filename": "/examples/rvsimple/README.md", "start": 173337, "end": 173416}, {"filename": "/examples/rvsimple/adder.h", "start": 173416, "end": 173923}, {"filename": "/examples/rvsimple/alu.h", "start": 173923, "end": 175391}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175391, "end": 178003}, {"filename": "/examples/rvsimple/config.h", "start": 178003, "end": 179225}, {"filename": "/examples/rvsimple/constants.h", "start": 179225, "end": 184951}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 184951, "end": 186068}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 186068, "end": 188008}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 188008, "end": 189248}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 189248, "end": 190499}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190499, "end": 191178}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 191178, "end": 192149}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 192149, "end": 194274}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194274, "end": 195045}, {"filename": "/examples/rvsimple/main.d", "start": 195045, "end": 197593}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 197593, "end": 198280}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 198280, "end": 199103}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 199103, "end": 200109}, {"filename": "/examples/rvsimple/regfile.h", "start": 200109, "end": 201087}, {"filename": "/examples/rvsimple/register.h", "start": 201087, "end": 201783}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 201783, "end": 204833}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 204833, "end": 207905}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 207905, "end": 213486}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 213486, "end": 215986}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 215986, "end": 220724}, {"filename": "/examples/rvsimple/toplevel.h", "start": 220724, "end": 222719}, {"filename": "/examples/scratch.h", "start": 222719, "end": 223087}, {"filename": "/examples/tutorial/adder.h", "start": 223087, "end": 223267}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 223267, "end": 224255}, {"filename": "/examples/tutorial/blockram.h", "start": 224255, "end": 224787}, {"filename": "/examples/tutorial/checksum.h", "start": 224787, "end": 225529}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 225529, "end": 225936}, {"filename": "/examples/tutorial/counter.h", "start": 225936, "end": 226087}, {"filename": "/examples/tutorial/declaration_order.h", "start": 226087, "end": 226913}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 226913, "end": 227872}, {"filename": "/examples/tutorial/nonblocking.h", "start": 227872, "end": 227998}, {"filename": "/examples/tutorial/submodules.h", "start": 227998, "end": 229098}, {"filename": "/examples/tutorial/templates.h", "start": 229098, "end": 229581}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 229581, "end": 229581}, {"filename": "/examples/tutorial/tutorial2.h", "start": 229581, "end": 229649}, {"filename": "/examples/tutorial/tutorial3.h", "start": 229649, "end": 229690}, {"filename": "/examples/tutorial/tutorial4.h", "start": 229690, "end": 229731}, {"filename": "/examples/tutorial/tutorial5.h", "start": 229731, "end": 229772}, {"filename": "/examples/tutorial/vga.h", "start": 229772, "end": 230952}, {"filename": "/examples/uart/README.md", "start": 230952, "end": 231196}, {"filename": "/examples/uart/main.d", "start": 231196, "end": 231673}, {"filename": "/examples/uart/uart.hancho", "start": 231673, "end": 234483}, {"filename": "/examples/uart/uart_hello.h", "start": 234483, "end": 237151}, {"filename": "/examples/uart/uart_rx.h", "start": 237151, "end": 239842}, {"filename": "/examples/uart/uart_top.h", "start": 239842, "end": 241694}, {"filename": "/examples/uart/uart_tx.h", "start": 241694, "end": 244743}, {"filename": "/tests/metron/fail/README.md", "start": 244743, "end": 244940}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 244940, "end": 245247}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 245247, "end": 245502}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 245502, "end": 245722}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 245722, "end": 246208}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 246208, "end": 246445}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 246445, "end": 246862}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 246862, "end": 247328}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 247328, "end": 247646}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 247646, "end": 248098}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 248098, "end": 248355}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 248355, "end": 248637}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 248637, "end": 248873}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 248873, "end": 249103}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 249103, "end": 249472}, {"filename": "/tests/metron/pass/README.md", "start": 249472, "end": 249553}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 249553, "end": 251054}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 251054, "end": 251471}, {"filename": "/tests/metron/pass/basic_function.h", "start": 251471, "end": 251723}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 251723, "end": 252050}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 252050, "end": 252355}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 252355, "end": 252974}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 252974, "end": 253230}, {"filename": "/tests/metron/pass/basic_output.h", "start": 253230, "end": 253502}, {"filename": "/tests/metron/pass/basic_param.h", "start": 253502, "end": 253771}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 253771, "end": 253972}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 253972, "end": 254159}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 254159, "end": 254390}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 254390, "end": 254617}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 254617, "end": 254933}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 254933, "end": 255298}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 255298, "end": 255684}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 255684, "end": 256173}, {"filename": "/tests/metron/pass/basic_task.h", "start": 256173, "end": 256508}, {"filename": "/tests/metron/pass/basic_template.h", "start": 256508, "end": 257010}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 257010, "end": 262990}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 262990, "end": 263424}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 263424, "end": 264090}, {"filename": "/tests/metron/pass/bitfields.h", "start": 264090, "end": 265296}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 265296, "end": 266113}, {"filename": "/tests/metron/pass/builtins.h", "start": 266113, "end": 266451}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 266451, "end": 266768}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 266768, "end": 267353}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 267353, "end": 268208}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 268208, "end": 268686}, {"filename": "/tests/metron/pass/counter.h", "start": 268686, "end": 269333}, {"filename": "/tests/metron/pass/defines.h", "start": 269333, "end": 269654}, {"filename": "/tests/metron/pass/dontcare.h", "start": 269654, "end": 269941}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 269941, "end": 271336}, {"filename": "/tests/metron/pass/for_loops.h", "start": 271336, "end": 271663}, {"filename": "/tests/metron/pass/good_order.h", "start": 271663, "end": 271866}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 271866, "end": 272287}, {"filename": "/tests/metron/pass/include_guards.h", "start": 272287, "end": 272484}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 272484, "end": 272675}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 272675, "end": 272852}, {"filename": "/tests/metron/pass/init_chain.h", "start": 272852, "end": 273573}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 273573, "end": 273865}, {"filename": "/tests/metron/pass/input_signals.h", "start": 273865, "end": 274538}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 274538, "end": 274723}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 274723, "end": 275036}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 275036, "end": 275357}, {"filename": "/tests/metron/pass/minimal.h", "start": 275357, "end": 275537}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 275537, "end": 275913}, {"filename": "/tests/metron/pass/namespaces.h", "start": 275913, "end": 276294}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 276294, "end": 276755}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 276755, "end": 277328}, {"filename": "/tests/metron/pass/noconvert.h", "start": 277328, "end": 277658}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 277658, "end": 278011}, {"filename": "/tests/metron/pass/oneliners.h", "start": 278011, "end": 278285}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 278285, "end": 278772}, {"filename": "/tests/metron/pass/preproc.h", "start": 278772, "end": 279100}, {"filename": "/tests/metron/pass/private_getter.h", "start": 279100, "end": 279352}, {"filename": "/tests/metron/pass/self_reference.h", "start": 279352, "end": 279557}, {"filename": "/tests/metron/pass/slice.h", "start": 279557, "end": 280064}, {"filename": "/tests/metron/pass/structs.h", "start": 280064, "end": 280518}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 280518, "end": 281063}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 281063, "end": 283683}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 283683, "end": 284477}, {"filename": "/tests/metron/pass/tock_task.h", "start": 284477, "end": 284929}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 284929, "end": 285101}, {"filename": "/tests/metron/pass/unions.h", "start": 285101, "end": 285307}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 285307, "end": 285976}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 285976, "end": 286645}], "remote_package_size": 286645});

  })();
