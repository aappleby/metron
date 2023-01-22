
  var Module = typeof Module !== 'undefined' ? Module : {};

  if (!Module.expectedDataFileDownloads) {
    Module.expectedDataFileDownloads = 0;
  }

  Module.expectedDataFileDownloads++;
  (function() {
    // When running as a pthread, FS operations are proxied to the main thread, so we don't need to
    // fetch the .data bundle on the worker
    if (Module['ENVIRONMENT_IS_PTHREAD']) return;
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
            if (Module['setStatus']) Module['setStatus']('Downloading data... (' + loaded + '/' + total + ')');
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
Module['FS_createPath']("/examples/gb_spu", "metron_sv", true, true);
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples/ibex", "metron_ref", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples/j1", "metron", true, true);
Module['FS_createPath']("/examples/j1", "metron_sv", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples/pong", "metron_sv", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron_sv", true, true);
Module['FS_createPath']("/examples/rvsimple", "reference_sv", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/examples/uart", "metron", true, true);
Module['FS_createPath']("/examples/uart", "metron_sv", true, true);
Module['FS_createPath']("/", "tests", true, true);
Module['FS_createPath']("/tests", "metron_bad", true, true);
Module['FS_createPath']("/tests", "metron_good", true, true);

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
          Module['addRunDependency']('fp ' + this.name);
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
          Module['removeRunDependency']('fp ' + that.name);
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/gb_spu/metron_sv/MetroBoySPU2.sv", "start": 26876, "end": 49298}, {"filename": "/examples/ibex/README.md", "start": 49298, "end": 49349}, {"filename": "/examples/ibex/main.cpp", "start": 49349, "end": 49490}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 49490, "end": 51075}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 51075, "end": 63173}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 63173, "end": 77583}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 77583, "end": 93607}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 93607, "end": 96108}, {"filename": "/examples/ibex/metron_ref/ibex_alu.sv", "start": 96108, "end": 148426}, {"filename": "/examples/ibex/metron_ref/ibex_compressed_decoder.sv", "start": 148426, "end": 159268}, {"filename": "/examples/ibex/metron_ref/ibex_multdiv_slow.sv", "start": 159268, "end": 172378}, {"filename": "/examples/ibex/metron_ref/ibex_pkg.sv", "start": 172378, "end": 188617}, {"filename": "/examples/ibex/metron_ref/prim_arbiter_fixed.sv", "start": 188617, "end": 194728}, {"filename": "/examples/j1/main.cpp", "start": 194728, "end": 194847}, {"filename": "/examples/j1/metron/dpram.h", "start": 194847, "end": 195285}, {"filename": "/examples/j1/metron/j1.h", "start": 195285, "end": 199310}, {"filename": "/examples/j1/metron_sv/j1.sv", "start": 199310, "end": 199338}, {"filename": "/examples/pong/README.md", "start": 199338, "end": 199398}, {"filename": "/examples/pong/main.cpp", "start": 199398, "end": 201344}, {"filename": "/examples/pong/main_vl.cpp", "start": 201344, "end": 201495}, {"filename": "/examples/pong/metron/pong.h", "start": 201495, "end": 205359}, {"filename": "/examples/pong/metron_sv/pong.sv", "start": 205359, "end": 209865}, {"filename": "/examples/pong/reference/README.md", "start": 209865, "end": 209925}, {"filename": "/examples/pong/reference/hvsync_generator.sv", "start": 209925, "end": 210864}, {"filename": "/examples/pong/reference/pong.sv", "start": 210864, "end": 214478}, {"filename": "/examples/rvsimple/README.md", "start": 214478, "end": 214557}, {"filename": "/examples/rvsimple/main.cpp", "start": 214557, "end": 217372}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 217372, "end": 220274}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 220274, "end": 223176}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 223176, "end": 223676}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 223676, "end": 225137}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 225137, "end": 227758}, {"filename": "/examples/rvsimple/metron/config.h", "start": 227758, "end": 228973}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 228973, "end": 234692}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 234692, "end": 235806}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 235806, "end": 237744}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 237744, "end": 238984}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 238984, "end": 240137}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 240137, "end": 240819}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 240819, "end": 241692}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 241692, "end": 243810}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 243810, "end": 244574}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 244574, "end": 245265}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 245265, "end": 246092}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 246092, "end": 247102}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 247102, "end": 248073}, {"filename": "/examples/rvsimple/metron/register.h", "start": 248073, "end": 248758}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 248758, "end": 251801}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 251801, "end": 257395}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 257395, "end": 259888}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 259888, "end": 264615}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 264615, "end": 266463}, {"filename": "/examples/rvsimple/metron_sv/adder.sv", "start": 266463, "end": 267062}, {"filename": "/examples/rvsimple/metron_sv/alu.sv", "start": 267062, "end": 268480}, {"filename": "/examples/rvsimple/metron_sv/alu_control.sv", "start": 268480, "end": 270920}, {"filename": "/examples/rvsimple/metron_sv/config.sv", "start": 270920, "end": 272116}, {"filename": "/examples/rvsimple/metron_sv/constants.sv", "start": 272116, "end": 277626}, {"filename": "/examples/rvsimple/metron_sv/control_transfer.sv", "start": 277626, "end": 278733}, {"filename": "/examples/rvsimple/metron_sv/data_memory_interface.sv", "start": 278733, "end": 280741}, {"filename": "/examples/rvsimple/metron_sv/example_data_memory.sv", "start": 280741, "end": 282175}, {"filename": "/examples/rvsimple/metron_sv/example_data_memory_bus.sv", "start": 282175, "end": 283917}, {"filename": "/examples/rvsimple/metron_sv/example_text_memory.sv", "start": 283917, "end": 284684}, {"filename": "/examples/rvsimple/metron_sv/example_text_memory_bus.sv", "start": 284684, "end": 285824}, {"filename": "/examples/rvsimple/metron_sv/immediate_generator.sv", "start": 285824, "end": 287883}, {"filename": "/examples/rvsimple/metron_sv/instruction_decoder.sv", "start": 287883, "end": 288771}, {"filename": "/examples/rvsimple/metron_sv/multiplexer2.sv", "start": 288771, "end": 289504}, {"filename": "/examples/rvsimple/metron_sv/multiplexer4.sv", "start": 289504, "end": 290358}, {"filename": "/examples/rvsimple/metron_sv/multiplexer8.sv", "start": 290358, "end": 291427}, {"filename": "/examples/rvsimple/metron_sv/regfile.sv", "start": 291427, "end": 292606}, {"filename": "/examples/rvsimple/metron_sv/register.sv", "start": 292606, "end": 293458}, {"filename": "/examples/rvsimple/metron_sv/riscv_core.sv", "start": 293458, "end": 299439}, {"filename": "/examples/rvsimple/metron_sv/singlecycle_control.sv", "start": 299439, "end": 304599}, {"filename": "/examples/rvsimple/metron_sv/singlecycle_ctlpath.sv", "start": 304599, "end": 308541}, {"filename": "/examples/rvsimple/metron_sv/singlecycle_datapath.sv", "start": 308541, "end": 318135}, {"filename": "/examples/rvsimple/metron_sv/toplevel.sv", "start": 318135, "end": 321531}, {"filename": "/examples/rvsimple/reference_sv/adder.sv", "start": 321531, "end": 321980}, {"filename": "/examples/rvsimple/reference_sv/alu.sv", "start": 321980, "end": 325101}, {"filename": "/examples/rvsimple/reference_sv/alu_control.sv", "start": 325101, "end": 328554}, {"filename": "/examples/rvsimple/reference_sv/config.sv", "start": 328554, "end": 329961}, {"filename": "/examples/rvsimple/reference_sv/constants.sv", "start": 329961, "end": 334698}, {"filename": "/examples/rvsimple/reference_sv/control_transfer.sv", "start": 334698, "end": 335583}, {"filename": "/examples/rvsimple/reference_sv/data_memory_interface.sv", "start": 335583, "end": 337494}, {"filename": "/examples/rvsimple/reference_sv/example_data_memory.sv", "start": 337494, "end": 338409}, {"filename": "/examples/rvsimple/reference_sv/example_data_memory_bus.sv", "start": 338409, "end": 339423}, {"filename": "/examples/rvsimple/reference_sv/example_memory_bus.sv", "start": 339423, "end": 340731}, {"filename": "/examples/rvsimple/reference_sv/example_text_memory.sv", "start": 340731, "end": 341257}, {"filename": "/examples/rvsimple/reference_sv/example_text_memory_bus.sv", "start": 341257, "end": 341942}, {"filename": "/examples/rvsimple/reference_sv/immediate_generator.sv", "start": 341942, "end": 343882}, {"filename": "/examples/rvsimple/reference_sv/instruction_decoder.sv", "start": 343882, "end": 344595}, {"filename": "/examples/rvsimple/reference_sv/multiplexer.sv", "start": 344595, "end": 345373}, {"filename": "/examples/rvsimple/reference_sv/multiplexer2.sv", "start": 345373, "end": 345961}, {"filename": "/examples/rvsimple/reference_sv/multiplexer4.sv", "start": 345961, "end": 346615}, {"filename": "/examples/rvsimple/reference_sv/multiplexer8.sv", "start": 346615, "end": 347401}, {"filename": "/examples/rvsimple/reference_sv/regfile.sv", "start": 347401, "end": 348310}, {"filename": "/examples/rvsimple/reference_sv/register.sv", "start": 348310, "end": 348943}, {"filename": "/examples/rvsimple/reference_sv/riscv_core.sv", "start": 348943, "end": 352317}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_control.sv", "start": 352317, "end": 357049}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_ctlpath.sv", "start": 357049, "end": 358839}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_datapath.sv", "start": 358839, "end": 363068}, {"filename": "/examples/rvsimple/reference_sv/toplevel.sv", "start": 363068, "end": 364699}, {"filename": "/examples/scratch.h", "start": 364699, "end": 365279}, {"filename": "/examples/scratch.sv", "start": 365279, "end": 366080}, {"filename": "/examples/tutorial/adder.h", "start": 366080, "end": 366260}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 366260, "end": 367241}, {"filename": "/examples/tutorial/blockram.h", "start": 367241, "end": 367758}, {"filename": "/examples/tutorial/checksum.h", "start": 367758, "end": 368481}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 368481, "end": 369017}, {"filename": "/examples/tutorial/counter.h", "start": 369017, "end": 369166}, {"filename": "/examples/tutorial/declaration_order.h", "start": 369166, "end": 369945}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 369945, "end": 371363}, {"filename": "/examples/tutorial/nonblocking.h", "start": 371363, "end": 371485}, {"filename": "/examples/tutorial/submodules.h", "start": 371485, "end": 372602}, {"filename": "/examples/tutorial/templates.h", "start": 372602, "end": 373073}, {"filename": "/examples/tutorial/tutorial2.h", "start": 373073, "end": 373141}, {"filename": "/examples/tutorial/tutorial3.h", "start": 373141, "end": 373182}, {"filename": "/examples/tutorial/tutorial4.h", "start": 373182, "end": 373223}, {"filename": "/examples/tutorial/tutorial5.h", "start": 373223, "end": 373264}, {"filename": "/examples/tutorial/vga.h", "start": 373264, "end": 374411}, {"filename": "/examples/uart/README.md", "start": 374411, "end": 374655}, {"filename": "/examples/uart/main.cpp", "start": 374655, "end": 375987}, {"filename": "/examples/uart/main_vl.cpp", "start": 375987, "end": 378507}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 378507, "end": 381087}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 381087, "end": 383648}, {"filename": "/examples/uart/metron/uart_top.h", "start": 383648, "end": 385412}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 385412, "end": 388431}, {"filename": "/examples/uart/metron_sv/uart_hello.sv", "start": 388431, "end": 391309}, {"filename": "/examples/uart/metron_sv/uart_rx.sv", "start": 391309, "end": 394272}, {"filename": "/examples/uart/metron_sv/uart_top.sv", "start": 394272, "end": 398498}, {"filename": "/examples/uart/metron_sv/uart_tx.sv", "start": 398498, "end": 401947}, {"filename": "/examples/uart/uart_test_ice40.sv", "start": 401947, "end": 404122}, {"filename": "/examples/uart/uart_test_iv.sv", "start": 404122, "end": 405719}, {"filename": "/tests/metron_bad/README.md", "start": 405719, "end": 405916}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 405916, "end": 406212}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 406212, "end": 406460}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 406460, "end": 406703}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 406703, "end": 407298}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 407298, "end": 407708}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 407708, "end": 408248}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 408248, "end": 408559}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 408559, "end": 409005}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 409005, "end": 409265}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 409265, "end": 409537}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 409537, "end": 410044}, {"filename": "/tests/metron_good/README.md", "start": 410044, "end": 410125}, {"filename": "/tests/metron_good/all_func_types.h", "start": 410125, "end": 411690}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 411690, "end": 412097}, {"filename": "/tests/metron_good/basic_function.h", "start": 412097, "end": 412376}, {"filename": "/tests/metron_good/basic_increment.h", "start": 412376, "end": 412731}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 412731, "end": 413026}, {"filename": "/tests/metron_good/basic_literals.h", "start": 413026, "end": 413638}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 413638, "end": 413884}, {"filename": "/tests/metron_good/basic_output.h", "start": 413884, "end": 414145}, {"filename": "/tests/metron_good/basic_param.h", "start": 414145, "end": 414404}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 414404, "end": 414635}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 414635, "end": 414815}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 414815, "end": 415078}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 415078, "end": 415298}, {"filename": "/tests/metron_good/basic_submod.h", "start": 415298, "end": 415587}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 415587, "end": 415942}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 415942, "end": 416318}, {"filename": "/tests/metron_good/basic_switch.h", "start": 416318, "end": 416795}, {"filename": "/tests/metron_good/basic_task.h", "start": 416795, "end": 417129}, {"filename": "/tests/metron_good/basic_template.h", "start": 417129, "end": 417615}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 417615, "end": 417774}, {"filename": "/tests/metron_good/bit_casts.h", "start": 417774, "end": 423747}, {"filename": "/tests/metron_good/bit_concat.h", "start": 423747, "end": 424174}, {"filename": "/tests/metron_good/bit_dup.h", "start": 424174, "end": 424833}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 424833, "end": 425671}, {"filename": "/tests/metron_good/builtins.h", "start": 425671, "end": 426002}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 426002, "end": 426309}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 426309, "end": 426863}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 426863, "end": 427756}, {"filename": "/tests/metron_good/constructor_args.h", "start": 427756, "end": 428264}, {"filename": "/tests/metron_good/defines.h", "start": 428264, "end": 428578}, {"filename": "/tests/metron_good/dontcare.h", "start": 428578, "end": 428861}, {"filename": "/tests/metron_good/enum_simple.h", "start": 428861, "end": 430229}, {"filename": "/tests/metron_good/for_loops.h", "start": 430229, "end": 430549}, {"filename": "/tests/metron_good/good_order.h", "start": 430549, "end": 430845}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 430845, "end": 431256}, {"filename": "/tests/metron_good/include_guards.h", "start": 431256, "end": 431453}, {"filename": "/tests/metron_good/init_chain.h", "start": 431453, "end": 432161}, {"filename": "/tests/metron_good/input_signals.h", "start": 432161, "end": 432823}, {"filename": "/tests/metron_good/local_localparam.h", "start": 432823, "end": 433001}, {"filename": "/tests/metron_good/magic_comments.h", "start": 433001, "end": 433304}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 433304, "end": 433618}, {"filename": "/tests/metron_good/minimal.h", "start": 433618, "end": 433693}, {"filename": "/tests/metron_good/multi_tick.h", "start": 433693, "end": 434059}, {"filename": "/tests/metron_good/namespaces.h", "start": 434059, "end": 434409}, {"filename": "/tests/metron_good/nested_structs.h", "start": 434409, "end": 434922}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 434922, "end": 435467}, {"filename": "/tests/metron_good/oneliners.h", "start": 435467, "end": 435730}, {"filename": "/tests/metron_good/plus_equals.h", "start": 435730, "end": 436080}, {"filename": "/tests/metron_good/private_getter.h", "start": 436080, "end": 436304}, {"filename": "/tests/metron_good/structs.h", "start": 436304, "end": 436523}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 436523, "end": 437059}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 437059, "end": 439420}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 439420, "end": 440174}, {"filename": "/tests/metron_good/tock_task.h", "start": 440174, "end": 440530}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 440530, "end": 440695}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 440695, "end": 441354}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 441354, "end": 442013}], "remote_package_size": 442013});

  })();
