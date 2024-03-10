
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/picorv32/picorv32.h", "start": 71623, "end": 166827}, {"filename": "/examples/pong/README.md", "start": 166827, "end": 166887}, {"filename": "/examples/pong/metron/pong.h", "start": 166887, "end": 170878}, {"filename": "/examples/pong/reference/README.md", "start": 170878, "end": 170938}, {"filename": "/examples/rvsimple/README.md", "start": 170938, "end": 171017}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 171017, "end": 171524}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 171524, "end": 172992}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 172992, "end": 175604}, {"filename": "/examples/rvsimple/metron/config.h", "start": 175604, "end": 176826}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 176826, "end": 182552}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 182552, "end": 183669}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 183669, "end": 185609}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 185609, "end": 186849}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 186849, "end": 188100}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 188100, "end": 188779}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 188779, "end": 189750}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 189750, "end": 191875}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 191875, "end": 192646}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 192646, "end": 193333}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 193333, "end": 194156}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 194156, "end": 195162}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 195162, "end": 196140}, {"filename": "/examples/rvsimple/metron/register.h", "start": 196140, "end": 196836}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 196836, "end": 199886}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 199886, "end": 205467}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 205467, "end": 207967}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 207967, "end": 212705}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 212705, "end": 214700}, {"filename": "/examples/scratch.h", "start": 214700, "end": 215068}, {"filename": "/examples/tutorial/adder.h", "start": 215068, "end": 215248}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 215248, "end": 216236}, {"filename": "/examples/tutorial/blockram.h", "start": 216236, "end": 216768}, {"filename": "/examples/tutorial/checksum.h", "start": 216768, "end": 217510}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 217510, "end": 217917}, {"filename": "/examples/tutorial/counter.h", "start": 217917, "end": 218068}, {"filename": "/examples/tutorial/declaration_order.h", "start": 218068, "end": 218894}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 218894, "end": 219853}, {"filename": "/examples/tutorial/nonblocking.h", "start": 219853, "end": 219979}, {"filename": "/examples/tutorial/submodules.h", "start": 219979, "end": 221079}, {"filename": "/examples/tutorial/templates.h", "start": 221079, "end": 221562}, {"filename": "/examples/tutorial/tutorial2.h", "start": 221562, "end": 221630}, {"filename": "/examples/tutorial/tutorial3.h", "start": 221630, "end": 221671}, {"filename": "/examples/tutorial/tutorial4.h", "start": 221671, "end": 221712}, {"filename": "/examples/tutorial/tutorial5.h", "start": 221712, "end": 221753}, {"filename": "/examples/tutorial/vga.h", "start": 221753, "end": 222933}, {"filename": "/examples/uart/README.md", "start": 222933, "end": 223177}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 223177, "end": 225815}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 225815, "end": 228506}, {"filename": "/examples/uart/metron/uart_top.h", "start": 228506, "end": 230294}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 230294, "end": 233351}, {"filename": "/tests/metron/fail/README.md", "start": 233351, "end": 233548}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 233548, "end": 233855}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 233855, "end": 234110}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 234110, "end": 234330}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 234330, "end": 234816}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 234816, "end": 235053}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 235053, "end": 235470}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 235470, "end": 235936}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 235936, "end": 236254}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 236254, "end": 236706}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 236706, "end": 236963}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 236963, "end": 237245}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 237245, "end": 237481}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 237481, "end": 237711}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 237711, "end": 238080}, {"filename": "/tests/metron/pass/README.md", "start": 238080, "end": 238161}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 238161, "end": 239662}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 239662, "end": 240079}, {"filename": "/tests/metron/pass/basic_function.h", "start": 240079, "end": 240331}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 240331, "end": 240658}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 240658, "end": 240963}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 240963, "end": 241582}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 241582, "end": 241838}, {"filename": "/tests/metron/pass/basic_output.h", "start": 241838, "end": 242110}, {"filename": "/tests/metron/pass/basic_param.h", "start": 242110, "end": 242379}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 242379, "end": 242580}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 242580, "end": 242767}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 242767, "end": 242998}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 242998, "end": 243225}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 243225, "end": 243541}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 243541, "end": 243906}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 243906, "end": 244292}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 244292, "end": 244781}, {"filename": "/tests/metron/pass/basic_task.h", "start": 244781, "end": 245116}, {"filename": "/tests/metron/pass/basic_template.h", "start": 245116, "end": 245618}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 245618, "end": 251598}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 251598, "end": 252032}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 252032, "end": 252698}, {"filename": "/tests/metron/pass/bitfields.h", "start": 252698, "end": 253904}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 253904, "end": 254721}, {"filename": "/tests/metron/pass/builtins.h", "start": 254721, "end": 255059}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 255059, "end": 255376}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 255376, "end": 255961}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 255961, "end": 256824}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 256824, "end": 257296}, {"filename": "/tests/metron/pass/counter.h", "start": 257296, "end": 257943}, {"filename": "/tests/metron/pass/defines.h", "start": 257943, "end": 258264}, {"filename": "/tests/metron/pass/dontcare.h", "start": 258264, "end": 258551}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 258551, "end": 259946}, {"filename": "/tests/metron/pass/for_loops.h", "start": 259946, "end": 260273}, {"filename": "/tests/metron/pass/good_order.h", "start": 260273, "end": 260476}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 260476, "end": 260897}, {"filename": "/tests/metron/pass/include_guards.h", "start": 260897, "end": 261094}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 261094, "end": 261285}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 261285, "end": 261462}, {"filename": "/tests/metron/pass/init_chain.h", "start": 261462, "end": 262183}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 262183, "end": 262475}, {"filename": "/tests/metron/pass/input_signals.h", "start": 262475, "end": 263271}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 263271, "end": 263456}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 263456, "end": 263769}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 263769, "end": 264090}, {"filename": "/tests/metron/pass/minimal.h", "start": 264090, "end": 264270}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 264270, "end": 264646}, {"filename": "/tests/metron/pass/namespaces.h", "start": 264646, "end": 265027}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 265027, "end": 265488}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 265488, "end": 266061}, {"filename": "/tests/metron/pass/noconvert.h", "start": 266061, "end": 266391}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 266391, "end": 266744}, {"filename": "/tests/metron/pass/oneliners.h", "start": 266744, "end": 267018}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 267018, "end": 267505}, {"filename": "/tests/metron/pass/preproc.h", "start": 267505, "end": 267833}, {"filename": "/tests/metron/pass/private_getter.h", "start": 267833, "end": 268085}, {"filename": "/tests/metron/pass/self_reference.h", "start": 268085, "end": 268290}, {"filename": "/tests/metron/pass/slice.h", "start": 268290, "end": 268797}, {"filename": "/tests/metron/pass/structs.h", "start": 268797, "end": 269251}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 269251, "end": 269796}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 269796, "end": 272416}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 272416, "end": 273210}, {"filename": "/tests/metron/pass/tock_task.h", "start": 273210, "end": 273662}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 273662, "end": 273834}, {"filename": "/tests/metron/pass/unions.h", "start": 273834, "end": 274040}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 274040, "end": 274709}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 274709, "end": 275378}], "remote_package_size": 275378});

  })();
