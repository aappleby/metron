
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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 207}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 207, "end": 20049}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20049, "end": 20773}, {"filename": "/examples/ibex/README.md", "start": 20773, "end": 20824}, {"filename": "/examples/ibex/ibex.hancho", "start": 20824, "end": 20824}, {"filename": "/examples/ibex/ibex_alu.h", "start": 20824, "end": 22463}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22463, "end": 34648}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34648, "end": 49140}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49140, "end": 65217}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65217, "end": 67802}, {"filename": "/examples/j1/dpram.h", "start": 67802, "end": 68263}, {"filename": "/examples/j1/j1.h", "start": 68263, "end": 72554}, {"filename": "/examples/j1/j1.hancho", "start": 72554, "end": 73218}, {"filename": "/examples/picorv32/picorv32.h", "start": 73218, "end": 168422}, {"filename": "/examples/pong/README.md", "start": 168422, "end": 168482}, {"filename": "/examples/pong/pong.h", "start": 168482, "end": 172473}, {"filename": "/examples/pong/pong.hancho", "start": 172473, "end": 173025}, {"filename": "/examples/pong/reference/README.md", "start": 173025, "end": 173085}, {"filename": "/examples/rvsimple/README.md", "start": 173085, "end": 173164}, {"filename": "/examples/rvsimple/adder.h", "start": 173164, "end": 173671}, {"filename": "/examples/rvsimple/alu.h", "start": 173671, "end": 175139}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175139, "end": 177751}, {"filename": "/examples/rvsimple/config.h", "start": 177751, "end": 178973}, {"filename": "/examples/rvsimple/constants.h", "start": 178973, "end": 184699}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 184699, "end": 185816}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 185816, "end": 187756}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 187756, "end": 188996}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 188996, "end": 190247}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190247, "end": 190926}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 190926, "end": 191897}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 191897, "end": 194022}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194022, "end": 194793}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 194793, "end": 195480}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 195480, "end": 196303}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 196303, "end": 197309}, {"filename": "/examples/rvsimple/regfile.h", "start": 197309, "end": 198287}, {"filename": "/examples/rvsimple/register.h", "start": 198287, "end": 198983}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 198983, "end": 202033}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 202033, "end": 203591}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 203591, "end": 209172}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 209172, "end": 211672}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 211672, "end": 216410}, {"filename": "/examples/rvsimple/toplevel.h", "start": 216410, "end": 218405}, {"filename": "/examples/scratch.h", "start": 218405, "end": 218773}, {"filename": "/examples/tutorial/adder.h", "start": 218773, "end": 218953}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 218953, "end": 219941}, {"filename": "/examples/tutorial/blockram.h", "start": 219941, "end": 220473}, {"filename": "/examples/tutorial/checksum.h", "start": 220473, "end": 221215}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 221215, "end": 221622}, {"filename": "/examples/tutorial/counter.h", "start": 221622, "end": 221773}, {"filename": "/examples/tutorial/declaration_order.h", "start": 221773, "end": 222599}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 222599, "end": 223558}, {"filename": "/examples/tutorial/nonblocking.h", "start": 223558, "end": 223684}, {"filename": "/examples/tutorial/submodules.h", "start": 223684, "end": 224784}, {"filename": "/examples/tutorial/templates.h", "start": 224784, "end": 225267}, {"filename": "/examples/tutorial/tutorial2.h", "start": 225267, "end": 225335}, {"filename": "/examples/tutorial/tutorial3.h", "start": 225335, "end": 225376}, {"filename": "/examples/tutorial/tutorial4.h", "start": 225376, "end": 225417}, {"filename": "/examples/tutorial/tutorial5.h", "start": 225417, "end": 225458}, {"filename": "/examples/tutorial/vga.h", "start": 225458, "end": 226638}, {"filename": "/examples/uart/README.md", "start": 226638, "end": 226882}, {"filename": "/examples/uart/uart.hancho", "start": 226882, "end": 228870}, {"filename": "/examples/uart/uart_hello.h", "start": 228870, "end": 231508}, {"filename": "/examples/uart/uart_rx.h", "start": 231508, "end": 234199}, {"filename": "/examples/uart/uart_top.h", "start": 234199, "end": 235987}, {"filename": "/examples/uart/uart_tx.h", "start": 235987, "end": 239044}, {"filename": "/tests/metron/fail/README.md", "start": 239044, "end": 239241}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 239241, "end": 239548}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 239548, "end": 239803}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 239803, "end": 240023}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 240023, "end": 240509}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 240509, "end": 240746}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 240746, "end": 241163}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 241163, "end": 241629}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 241629, "end": 241947}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 241947, "end": 242399}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 242399, "end": 242656}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 242656, "end": 242938}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 242938, "end": 243174}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 243174, "end": 243404}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 243404, "end": 243773}, {"filename": "/tests/metron/pass/README.md", "start": 243773, "end": 243854}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 243854, "end": 245355}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 245355, "end": 245772}, {"filename": "/tests/metron/pass/basic_function.h", "start": 245772, "end": 246024}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 246024, "end": 246351}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 246351, "end": 246656}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 246656, "end": 247275}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 247275, "end": 247531}, {"filename": "/tests/metron/pass/basic_output.h", "start": 247531, "end": 247803}, {"filename": "/tests/metron/pass/basic_param.h", "start": 247803, "end": 248072}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 248072, "end": 248273}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 248273, "end": 248460}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 248460, "end": 248691}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 248691, "end": 248918}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 248918, "end": 249234}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 249234, "end": 249599}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 249599, "end": 249985}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 249985, "end": 250474}, {"filename": "/tests/metron/pass/basic_task.h", "start": 250474, "end": 250809}, {"filename": "/tests/metron/pass/basic_template.h", "start": 250809, "end": 251311}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 251311, "end": 257291}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 257291, "end": 257725}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 257725, "end": 258391}, {"filename": "/tests/metron/pass/bitfields.h", "start": 258391, "end": 259597}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 259597, "end": 260414}, {"filename": "/tests/metron/pass/builtins.h", "start": 260414, "end": 260752}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 260752, "end": 261069}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 261069, "end": 261654}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 261654, "end": 262517}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 262517, "end": 262989}, {"filename": "/tests/metron/pass/counter.h", "start": 262989, "end": 263636}, {"filename": "/tests/metron/pass/defines.h", "start": 263636, "end": 263957}, {"filename": "/tests/metron/pass/dontcare.h", "start": 263957, "end": 264244}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 264244, "end": 265639}, {"filename": "/tests/metron/pass/for_loops.h", "start": 265639, "end": 265966}, {"filename": "/tests/metron/pass/good_order.h", "start": 265966, "end": 266169}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 266169, "end": 266590}, {"filename": "/tests/metron/pass/include_guards.h", "start": 266590, "end": 266787}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 266787, "end": 266978}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 266978, "end": 267155}, {"filename": "/tests/metron/pass/init_chain.h", "start": 267155, "end": 267876}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 267876, "end": 268168}, {"filename": "/tests/metron/pass/input_signals.h", "start": 268168, "end": 268841}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 268841, "end": 269026}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 269026, "end": 269339}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 269339, "end": 269660}, {"filename": "/tests/metron/pass/minimal.h", "start": 269660, "end": 269840}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 269840, "end": 270216}, {"filename": "/tests/metron/pass/namespaces.h", "start": 270216, "end": 270597}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 270597, "end": 271058}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 271058, "end": 271631}, {"filename": "/tests/metron/pass/noconvert.h", "start": 271631, "end": 271961}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 271961, "end": 272314}, {"filename": "/tests/metron/pass/oneliners.h", "start": 272314, "end": 272588}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 272588, "end": 273075}, {"filename": "/tests/metron/pass/preproc.h", "start": 273075, "end": 273403}, {"filename": "/tests/metron/pass/private_getter.h", "start": 273403, "end": 273655}, {"filename": "/tests/metron/pass/self_reference.h", "start": 273655, "end": 273860}, {"filename": "/tests/metron/pass/slice.h", "start": 273860, "end": 274367}, {"filename": "/tests/metron/pass/structs.h", "start": 274367, "end": 274821}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 274821, "end": 275366}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 275366, "end": 277986}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 277986, "end": 278780}, {"filename": "/tests/metron/pass/tock_task.h", "start": 278780, "end": 279232}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 279232, "end": 279404}, {"filename": "/tests/metron/pass/unions.h", "start": 279404, "end": 279610}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 279610, "end": 280279}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 280279, "end": 280948}], "remote_package_size": 280948});

  })();
