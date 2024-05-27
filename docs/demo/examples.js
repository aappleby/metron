
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
Module['FS_createPath']("/examples/ibex", "reference", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples", "picorv32", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "reference_sv", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/", "tests", true, true);
Module['FS_createPath']("/tests", "metron", true, true);
Module['FS_createPath']("/tests/metron", "fail", true, true);
Module['FS_createPath']("/tests/metron", "golden", true, true);
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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 535}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 535, "end": 20377}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20377, "end": 21007}, {"filename": "/examples/ibex/README.md", "start": 21007, "end": 21058}, {"filename": "/examples/ibex/ibex.hancho", "start": 21058, "end": 21058}, {"filename": "/examples/ibex/ibex_alu.h", "start": 21058, "end": 22697}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22697, "end": 34882}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34882, "end": 49374}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49374, "end": 65451}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65451, "end": 68036}, {"filename": "/examples/ibex/reference/ibex_alu.sv", "start": 68036, "end": 120354}, {"filename": "/examples/ibex/reference/ibex_compressed_decoder.sv", "start": 120354, "end": 131196}, {"filename": "/examples/ibex/reference/ibex_multdiv_slow.sv", "start": 131196, "end": 144306}, {"filename": "/examples/ibex/reference/ibex_pkg.sv", "start": 144306, "end": 160545}, {"filename": "/examples/ibex/reference/prim_arbiter_fixed.sv", "start": 160545, "end": 166656}, {"filename": "/examples/j1/dpram.h", "start": 166656, "end": 167117}, {"filename": "/examples/j1/j1.h", "start": 167117, "end": 171408}, {"filename": "/examples/j1/j1.hancho", "start": 171408, "end": 172101}, {"filename": "/examples/picorv32/picorv32.h", "start": 172101, "end": 267305}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 267305, "end": 267305}, {"filename": "/examples/pong/README.md", "start": 267305, "end": 267365}, {"filename": "/examples/pong/pong.h", "start": 267365, "end": 271356}, {"filename": "/examples/pong/pong.hancho", "start": 271356, "end": 271852}, {"filename": "/examples/pong/reference/README.md", "start": 271852, "end": 271912}, {"filename": "/examples/pong/reference/hvsync_generator.sv", "start": 271912, "end": 272851}, {"filename": "/examples/pong/reference/pong.sv", "start": 272851, "end": 276465}, {"filename": "/examples/rvsimple/README.md", "start": 276465, "end": 276544}, {"filename": "/examples/rvsimple/adder.h", "start": 276544, "end": 277051}, {"filename": "/examples/rvsimple/alu.h", "start": 277051, "end": 278519}, {"filename": "/examples/rvsimple/alu_control.h", "start": 278519, "end": 281131}, {"filename": "/examples/rvsimple/config.h", "start": 281131, "end": 282353}, {"filename": "/examples/rvsimple/constants.h", "start": 282353, "end": 288079}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 288079, "end": 289196}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 289196, "end": 291136}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 291136, "end": 292376}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 292376, "end": 293627}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 293627, "end": 294306}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 294306, "end": 295277}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 295277, "end": 297402}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 297402, "end": 298173}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 298173, "end": 298860}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 298860, "end": 299683}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 299683, "end": 300689}, {"filename": "/examples/rvsimple/reference_sv/adder.sv", "start": 300689, "end": 301138}, {"filename": "/examples/rvsimple/reference_sv/alu.sv", "start": 301138, "end": 304259}, {"filename": "/examples/rvsimple/reference_sv/alu_control.sv", "start": 304259, "end": 307712}, {"filename": "/examples/rvsimple/reference_sv/config.sv", "start": 307712, "end": 309119}, {"filename": "/examples/rvsimple/reference_sv/constants.sv", "start": 309119, "end": 313856}, {"filename": "/examples/rvsimple/reference_sv/control_transfer.sv", "start": 313856, "end": 314741}, {"filename": "/examples/rvsimple/reference_sv/data_memory_interface.sv", "start": 314741, "end": 316652}, {"filename": "/examples/rvsimple/reference_sv/example_data_memory.sv", "start": 316652, "end": 317567}, {"filename": "/examples/rvsimple/reference_sv/example_data_memory_bus.sv", "start": 317567, "end": 318581}, {"filename": "/examples/rvsimple/reference_sv/example_memory_bus.sv", "start": 318581, "end": 319889}, {"filename": "/examples/rvsimple/reference_sv/example_text_memory.sv", "start": 319889, "end": 320414}, {"filename": "/examples/rvsimple/reference_sv/example_text_memory_bus.sv", "start": 320414, "end": 321099}, {"filename": "/examples/rvsimple/reference_sv/immediate_generator.sv", "start": 321099, "end": 323039}, {"filename": "/examples/rvsimple/reference_sv/instruction_decoder.sv", "start": 323039, "end": 323752}, {"filename": "/examples/rvsimple/reference_sv/multiplexer.sv", "start": 323752, "end": 324530}, {"filename": "/examples/rvsimple/reference_sv/multiplexer2.sv", "start": 324530, "end": 325118}, {"filename": "/examples/rvsimple/reference_sv/multiplexer4.sv", "start": 325118, "end": 325772}, {"filename": "/examples/rvsimple/reference_sv/multiplexer8.sv", "start": 325772, "end": 326558}, {"filename": "/examples/rvsimple/reference_sv/regfile.sv", "start": 326558, "end": 327467}, {"filename": "/examples/rvsimple/reference_sv/register.sv", "start": 327467, "end": 328100}, {"filename": "/examples/rvsimple/reference_sv/riscv_core.sv", "start": 328100, "end": 331474}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_control.sv", "start": 331474, "end": 336206}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_ctlpath.sv", "start": 336206, "end": 337996}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_datapath.sv", "start": 337996, "end": 342225}, {"filename": "/examples/rvsimple/reference_sv/toplevel.sv", "start": 342225, "end": 343856}, {"filename": "/examples/rvsimple/regfile.h", "start": 343856, "end": 344834}, {"filename": "/examples/rvsimple/register.h", "start": 344834, "end": 345530}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 345530, "end": 348580}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 348580, "end": 351137}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 351137, "end": 356718}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 356718, "end": 359218}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 359218, "end": 363956}, {"filename": "/examples/rvsimple/toplevel.h", "start": 363956, "end": 365951}, {"filename": "/examples/scratch.h", "start": 365951, "end": 366319}, {"filename": "/examples/scratch.sv", "start": 366319, "end": 366842}, {"filename": "/examples/tutorial/adder.h", "start": 366842, "end": 367022}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 367022, "end": 368010}, {"filename": "/examples/tutorial/blockram.h", "start": 368010, "end": 368542}, {"filename": "/examples/tutorial/checksum.h", "start": 368542, "end": 369284}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 369284, "end": 369691}, {"filename": "/examples/tutorial/counter.h", "start": 369691, "end": 369842}, {"filename": "/examples/tutorial/declaration_order.h", "start": 369842, "end": 370668}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 370668, "end": 371627}, {"filename": "/examples/tutorial/nonblocking.h", "start": 371627, "end": 371753}, {"filename": "/examples/tutorial/submodules.h", "start": 371753, "end": 372853}, {"filename": "/examples/tutorial/templates.h", "start": 372853, "end": 373336}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 373336, "end": 373336}, {"filename": "/examples/tutorial/tutorial2.h", "start": 373336, "end": 373404}, {"filename": "/examples/tutorial/tutorial3.h", "start": 373404, "end": 373445}, {"filename": "/examples/tutorial/tutorial4.h", "start": 373445, "end": 373486}, {"filename": "/examples/tutorial/tutorial5.h", "start": 373486, "end": 373527}, {"filename": "/examples/tutorial/vga.h", "start": 373527, "end": 374707}, {"filename": "/examples/uart/README.md", "start": 374707, "end": 374951}, {"filename": "/examples/uart/uart.hancho", "start": 374951, "end": 377871}, {"filename": "/examples/uart/uart_hello.h", "start": 377871, "end": 380539}, {"filename": "/examples/uart/uart_rx.h", "start": 380539, "end": 383230}, {"filename": "/examples/uart/uart_top.h", "start": 383230, "end": 385106}, {"filename": "/examples/uart/uart_tx.h", "start": 385106, "end": 388163}, {"filename": "/tests/metron/fail/README.md", "start": 388163, "end": 388360}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 388360, "end": 388667}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 388667, "end": 388922}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 388922, "end": 389142}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 389142, "end": 389628}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 389628, "end": 389865}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 389865, "end": 390282}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 390282, "end": 390748}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 390748, "end": 391066}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 391066, "end": 391518}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 391518, "end": 391775}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 391775, "end": 392057}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 392057, "end": 392293}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 392293, "end": 392523}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 392523, "end": 392892}, {"filename": "/tests/metron/golden/all_func_types.sv", "start": 392892, "end": 395489}, {"filename": "/tests/metron/golden/basic_constructor.sv", "start": 395489, "end": 395996}, {"filename": "/tests/metron/golden/basic_function.sv", "start": 395996, "end": 396391}, {"filename": "/tests/metron/golden/basic_increment.sv", "start": 396391, "end": 396906}, {"filename": "/tests/metron/golden/basic_inputs.sv", "start": 396906, "end": 397357}, {"filename": "/tests/metron/golden/basic_literals.sv", "start": 397357, "end": 398275}, {"filename": "/tests/metron/golden/basic_localparam.sv", "start": 398275, "end": 398654}, {"filename": "/tests/metron/golden/basic_output.sv", "start": 398654, "end": 399094}, {"filename": "/tests/metron/golden/basic_param.sv", "start": 399094, "end": 399470}, {"filename": "/tests/metron/golden/basic_public_reg.sv", "start": 399470, "end": 399780}, {"filename": "/tests/metron/golden/basic_public_sig.sv", "start": 399780, "end": 400019}, {"filename": "/tests/metron/golden/basic_reg_rww.sv", "start": 400019, "end": 400367}, {"filename": "/tests/metron/golden/basic_sig_wwr.sv", "start": 400367, "end": 400653}, {"filename": "/tests/metron/golden/basic_submod.sv", "start": 400653, "end": 401354}, {"filename": "/tests/metron/golden/basic_submod_param.sv", "start": 401354, "end": 401992}, {"filename": "/tests/metron/golden/basic_submod_public_reg.sv", "start": 401992, "end": 402792}, {"filename": "/tests/metron/golden/basic_switch.sv", "start": 402792, "end": 403413}, {"filename": "/tests/metron/golden/basic_task.sv", "start": 403413, "end": 403870}, {"filename": "/tests/metron/golden/basic_template.sv", "start": 403870, "end": 404981}, {"filename": "/tests/metron/golden/basic_tock_with_return.sv", "start": 404981, "end": 405248}, {"filename": "/tests/metron/golden/bit_casts.sv", "start": 405248, "end": 413515}, {"filename": "/tests/metron/golden/bit_concat.sv", "start": 413515, "end": 414077}, {"filename": "/tests/metron/golden/bit_dup.sv", "start": 414077, "end": 415062}, {"filename": "/tests/metron/golden/bitfields.sv", "start": 415062, "end": 416398}, {"filename": "/tests/metron/golden/both_tock_and_tick_use_tasks_and_funcs.sv", "start": 416398, "end": 417476}, {"filename": "/tests/metron/golden/builtins.sv", "start": 417476, "end": 417920}, {"filename": "/tests/metron/golden/call_tick_from_tock.sv", "start": 417920, "end": 418383}, {"filename": "/tests/metron/golden/case_with_fallthrough.sv", "start": 418383, "end": 419147}, {"filename": "/tests/metron/golden/constructor_arg_passing.sv", "start": 419147, "end": 420741}, {"filename": "/tests/metron/golden/constructor_args.sv", "start": 420741, "end": 421466}, {"filename": "/tests/metron/golden/counter.sv", "start": 421466, "end": 422470}, {"filename": "/tests/metron/golden/defines.sv", "start": 422470, "end": 422870}, {"filename": "/tests/metron/golden/dontcare.sv", "start": 422870, "end": 423339}, {"filename": "/tests/metron/golden/enum_simple.sv", "start": 423339, "end": 424874}, {"filename": "/tests/metron/golden/for_loops.sv", "start": 424874, "end": 425385}, {"filename": "/tests/metron/golden/good_order.sv", "start": 425385, "end": 425686}, {"filename": "/tests/metron/golden/if_with_compound.sv", "start": 425686, "end": 426520}, {"filename": "/tests/metron/golden/include_guards.sv", "start": 426520, "end": 426790}, {"filename": "/tests/metron/golden/include_test_module.sv", "start": 426790, "end": 427229}, {"filename": "/tests/metron/golden/include_test_submod.sv", "start": 427229, "end": 427535}, {"filename": "/tests/metron/golden/init_chain.sv", "start": 427535, "end": 428397}, {"filename": "/tests/metron/golden/initialize_struct_to_zero.sv", "start": 428397, "end": 428761}, {"filename": "/tests/metron/golden/input_signals.sv", "start": 428761, "end": 430290}, {"filename": "/tests/metron/golden/local_localparam.sv", "start": 430290, "end": 430573}, {"filename": "/tests/metron/golden/magic_comments.sv", "start": 430573, "end": 430992}, {"filename": "/tests/metron/golden/matching_port_and_arg_names.sv", "start": 430992, "end": 431454}, {"filename": "/tests/metron/golden/minimal.sv", "start": 431454, "end": 431646}, {"filename": "/tests/metron/golden/multi_tick.sv", "start": 431646, "end": 432192}, {"filename": "/tests/metron/golden/namespaces.sv", "start": 432192, "end": 432755}, {"filename": "/tests/metron/golden/nested_structs.sv", "start": 432755, "end": 433437}, {"filename": "/tests/metron/golden/nested_submod_calls.sv", "start": 433437, "end": 434732}, {"filename": "/tests/metron/golden/noconvert.sv", "start": 434732, "end": 435146}, {"filename": "/tests/metron/golden/nonblocking_assign_to_struct_union.sv", "start": 435146, "end": 435625}, {"filename": "/tests/metron/golden/oneliners.sv", "start": 435625, "end": 436084}, {"filename": "/tests/metron/golden/plus_equals.sv", "start": 436084, "end": 436847}, {"filename": "/tests/metron/golden/preproc.sv", "start": 436847, "end": 437187}, {"filename": "/tests/metron/golden/private_getter.sv", "start": 437187, "end": 437532}, {"filename": "/tests/metron/golden/self_reference.sv", "start": 437532, "end": 437757}, {"filename": "/tests/metron/golden/slice.sv", "start": 437757, "end": 438445}, {"filename": "/tests/metron/golden/structs.sv", "start": 438445, "end": 439099}, {"filename": "/tests/metron/golden/structs_as_args.sv", "start": 439099, "end": 439909}, {"filename": "/tests/metron/golden/structs_as_ports.sv", "start": 439909, "end": 443434}, {"filename": "/tests/metron/golden/submod_bindings.sv", "start": 443434, "end": 445119}, {"filename": "/tests/metron/golden/tock_task.sv", "start": 445119, "end": 445658}, {"filename": "/tests/metron/golden/trivial_adder.sv", "start": 445658, "end": 445929}, {"filename": "/tests/metron/golden/unions.sv", "start": 445929, "end": 446269}, {"filename": "/tests/metron/golden/utf8-mod.bom.sv", "start": 446269, "end": 447034}, {"filename": "/tests/metron/golden/utf8-mod.sv", "start": 447034, "end": 447799}, {"filename": "/tests/metron/pass/README.md", "start": 447799, "end": 447880}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 447880, "end": 449381}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 449381, "end": 449798}, {"filename": "/tests/metron/pass/basic_function.h", "start": 449798, "end": 450050}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 450050, "end": 450377}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 450377, "end": 450682}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 450682, "end": 451301}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 451301, "end": 451557}, {"filename": "/tests/metron/pass/basic_output.h", "start": 451557, "end": 451829}, {"filename": "/tests/metron/pass/basic_param.h", "start": 451829, "end": 452098}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 452098, "end": 452299}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 452299, "end": 452486}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 452486, "end": 452717}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 452717, "end": 452944}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 452944, "end": 453260}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 453260, "end": 453625}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 453625, "end": 454011}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 454011, "end": 454500}, {"filename": "/tests/metron/pass/basic_task.h", "start": 454500, "end": 454835}, {"filename": "/tests/metron/pass/basic_template.h", "start": 454835, "end": 455337}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 455337, "end": 461317}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 461317, "end": 461751}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 461751, "end": 462417}, {"filename": "/tests/metron/pass/bitfields.h", "start": 462417, "end": 463623}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 463623, "end": 464440}, {"filename": "/tests/metron/pass/builtins.h", "start": 464440, "end": 464778}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 464778, "end": 465095}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 465095, "end": 465680}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 465680, "end": 466535}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 466535, "end": 467013}, {"filename": "/tests/metron/pass/counter.h", "start": 467013, "end": 467660}, {"filename": "/tests/metron/pass/defines.h", "start": 467660, "end": 467981}, {"filename": "/tests/metron/pass/dontcare.h", "start": 467981, "end": 468268}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 468268, "end": 469663}, {"filename": "/tests/metron/pass/for_loops.h", "start": 469663, "end": 469990}, {"filename": "/tests/metron/pass/good_order.h", "start": 469990, "end": 470193}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 470193, "end": 470614}, {"filename": "/tests/metron/pass/include_guards.h", "start": 470614, "end": 470811}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 470811, "end": 471002}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 471002, "end": 471179}, {"filename": "/tests/metron/pass/init_chain.h", "start": 471179, "end": 471900}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 471900, "end": 472192}, {"filename": "/tests/metron/pass/input_signals.h", "start": 472192, "end": 472865}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 472865, "end": 473050}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 473050, "end": 473363}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 473363, "end": 473684}, {"filename": "/tests/metron/pass/minimal.h", "start": 473684, "end": 473864}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 473864, "end": 474240}, {"filename": "/tests/metron/pass/namespaces.h", "start": 474240, "end": 474621}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 474621, "end": 475082}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 475082, "end": 475655}, {"filename": "/tests/metron/pass/noconvert.h", "start": 475655, "end": 475985}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 475985, "end": 476338}, {"filename": "/tests/metron/pass/oneliners.h", "start": 476338, "end": 476612}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 476612, "end": 477099}, {"filename": "/tests/metron/pass/preproc.h", "start": 477099, "end": 477427}, {"filename": "/tests/metron/pass/private_getter.h", "start": 477427, "end": 477679}, {"filename": "/tests/metron/pass/self_reference.h", "start": 477679, "end": 477884}, {"filename": "/tests/metron/pass/slice.h", "start": 477884, "end": 478391}, {"filename": "/tests/metron/pass/structs.h", "start": 478391, "end": 478845}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 478845, "end": 479390}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 479390, "end": 482010}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 482010, "end": 482804}, {"filename": "/tests/metron/pass/tock_task.h", "start": 482804, "end": 483256}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 483256, "end": 483428}, {"filename": "/tests/metron/pass/unions.h", "start": 483428, "end": 483634}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 483634, "end": 484303}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 484303, "end": 484972}], "remote_package_size": 484972});

  })();
