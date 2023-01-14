
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/gb_spu/metron_sv/MetroBoySPU2.sv", "start": 26876, "end": 49298}, {"filename": "/examples/ibex/README.md", "start": 49298, "end": 49349}, {"filename": "/examples/ibex/main.cpp", "start": 49349, "end": 49490}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 49490, "end": 51075}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 51075, "end": 63173}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 63173, "end": 77583}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 77583, "end": 93607}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 93607, "end": 96108}, {"filename": "/examples/ibex/metron_ref/ibex_alu.sv", "start": 96108, "end": 148426}, {"filename": "/examples/ibex/metron_ref/ibex_compressed_decoder.sv", "start": 148426, "end": 159268}, {"filename": "/examples/ibex/metron_ref/ibex_multdiv_slow.sv", "start": 159268, "end": 172378}, {"filename": "/examples/ibex/metron_ref/ibex_pkg.sv", "start": 172378, "end": 188617}, {"filename": "/examples/ibex/metron_ref/prim_arbiter_fixed.sv", "start": 188617, "end": 194728}, {"filename": "/examples/j1/main.cpp", "start": 194728, "end": 194847}, {"filename": "/examples/j1/metron/dpram.h", "start": 194847, "end": 195285}, {"filename": "/examples/j1/metron/j1.h", "start": 195285, "end": 199310}, {"filename": "/examples/j1/metron_sv/j1.sv", "start": 199310, "end": 199338}, {"filename": "/examples/pong/README.md", "start": 199338, "end": 199398}, {"filename": "/examples/pong/main.cpp", "start": 199398, "end": 201344}, {"filename": "/examples/pong/main_vl.cpp", "start": 201344, "end": 201495}, {"filename": "/examples/pong/metron/pong.h", "start": 201495, "end": 205359}, {"filename": "/examples/pong/metron_sv/pong.sv", "start": 205359, "end": 209865}, {"filename": "/examples/pong/reference/README.md", "start": 209865, "end": 209925}, {"filename": "/examples/pong/reference/hvsync_generator.sv", "start": 209925, "end": 210864}, {"filename": "/examples/pong/reference/pong.sv", "start": 210864, "end": 214478}, {"filename": "/examples/rvsimple/README.md", "start": 214478, "end": 214557}, {"filename": "/examples/rvsimple/main.cpp", "start": 214557, "end": 217372}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 217372, "end": 220274}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 220274, "end": 223176}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 223176, "end": 223676}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 223676, "end": 225137}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 225137, "end": 227758}, {"filename": "/examples/rvsimple/metron/config.h", "start": 227758, "end": 228973}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 228973, "end": 234692}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 234692, "end": 235806}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 235806, "end": 237744}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 237744, "end": 238984}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 238984, "end": 240137}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 240137, "end": 240819}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 240819, "end": 241692}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 241692, "end": 243810}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 243810, "end": 244574}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 244574, "end": 245265}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 245265, "end": 246092}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 246092, "end": 247102}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 247102, "end": 248073}, {"filename": "/examples/rvsimple/metron/register.h", "start": 248073, "end": 248758}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 248758, "end": 251801}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 251801, "end": 257395}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 257395, "end": 259888}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 259888, "end": 264615}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 264615, "end": 266463}, {"filename": "/examples/rvsimple/metron_sv/adder.sv", "start": 266463, "end": 267067}, {"filename": "/examples/rvsimple/metron_sv/alu.sv", "start": 267067, "end": 268485}, {"filename": "/examples/rvsimple/metron_sv/alu_control.sv", "start": 268485, "end": 270925}, {"filename": "/examples/rvsimple/metron_sv/config.sv", "start": 270925, "end": 272121}, {"filename": "/examples/rvsimple/metron_sv/constants.sv", "start": 272121, "end": 277631}, {"filename": "/examples/rvsimple/metron_sv/control_transfer.sv", "start": 277631, "end": 278738}, {"filename": "/examples/rvsimple/metron_sv/data_memory_interface.sv", "start": 278738, "end": 280746}, {"filename": "/examples/rvsimple/metron_sv/example_data_memory.sv", "start": 280746, "end": 282180}, {"filename": "/examples/rvsimple/metron_sv/example_data_memory_bus.sv", "start": 282180, "end": 283922}, {"filename": "/examples/rvsimple/metron_sv/example_text_memory.sv", "start": 283922, "end": 284689}, {"filename": "/examples/rvsimple/metron_sv/example_text_memory_bus.sv", "start": 284689, "end": 285829}, {"filename": "/examples/rvsimple/metron_sv/immediate_generator.sv", "start": 285829, "end": 287888}, {"filename": "/examples/rvsimple/metron_sv/instruction_decoder.sv", "start": 287888, "end": 288776}, {"filename": "/examples/rvsimple/metron_sv/multiplexer2.sv", "start": 288776, "end": 289514}, {"filename": "/examples/rvsimple/metron_sv/multiplexer4.sv", "start": 289514, "end": 290373}, {"filename": "/examples/rvsimple/metron_sv/multiplexer8.sv", "start": 290373, "end": 291447}, {"filename": "/examples/rvsimple/metron_sv/regfile.sv", "start": 291447, "end": 292626}, {"filename": "/examples/rvsimple/metron_sv/register.sv", "start": 292626, "end": 293486}, {"filename": "/examples/rvsimple/metron_sv/riscv_core.sv", "start": 293486, "end": 299467}, {"filename": "/examples/rvsimple/metron_sv/singlecycle_control.sv", "start": 299467, "end": 304627}, {"filename": "/examples/rvsimple/metron_sv/singlecycle_ctlpath.sv", "start": 304627, "end": 308569}, {"filename": "/examples/rvsimple/metron_sv/singlecycle_datapath.sv", "start": 308569, "end": 317848}, {"filename": "/examples/rvsimple/metron_sv/toplevel.sv", "start": 317848, "end": 321249}, {"filename": "/examples/rvsimple/reference_sv/adder.sv", "start": 321249, "end": 321698}, {"filename": "/examples/rvsimple/reference_sv/alu.sv", "start": 321698, "end": 324819}, {"filename": "/examples/rvsimple/reference_sv/alu_control.sv", "start": 324819, "end": 328272}, {"filename": "/examples/rvsimple/reference_sv/config.sv", "start": 328272, "end": 329679}, {"filename": "/examples/rvsimple/reference_sv/constants.sv", "start": 329679, "end": 334416}, {"filename": "/examples/rvsimple/reference_sv/control_transfer.sv", "start": 334416, "end": 335301}, {"filename": "/examples/rvsimple/reference_sv/data_memory_interface.sv", "start": 335301, "end": 337212}, {"filename": "/examples/rvsimple/reference_sv/example_data_memory.sv", "start": 337212, "end": 338127}, {"filename": "/examples/rvsimple/reference_sv/example_data_memory_bus.sv", "start": 338127, "end": 339141}, {"filename": "/examples/rvsimple/reference_sv/example_memory_bus.sv", "start": 339141, "end": 340449}, {"filename": "/examples/rvsimple/reference_sv/example_text_memory.sv", "start": 340449, "end": 340975}, {"filename": "/examples/rvsimple/reference_sv/example_text_memory_bus.sv", "start": 340975, "end": 341660}, {"filename": "/examples/rvsimple/reference_sv/immediate_generator.sv", "start": 341660, "end": 343600}, {"filename": "/examples/rvsimple/reference_sv/instruction_decoder.sv", "start": 343600, "end": 344313}, {"filename": "/examples/rvsimple/reference_sv/multiplexer.sv", "start": 344313, "end": 345091}, {"filename": "/examples/rvsimple/reference_sv/multiplexer2.sv", "start": 345091, "end": 345679}, {"filename": "/examples/rvsimple/reference_sv/multiplexer4.sv", "start": 345679, "end": 346333}, {"filename": "/examples/rvsimple/reference_sv/multiplexer8.sv", "start": 346333, "end": 347119}, {"filename": "/examples/rvsimple/reference_sv/regfile.sv", "start": 347119, "end": 348028}, {"filename": "/examples/rvsimple/reference_sv/register.sv", "start": 348028, "end": 348661}, {"filename": "/examples/rvsimple/reference_sv/riscv_core.sv", "start": 348661, "end": 352035}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_control.sv", "start": 352035, "end": 356767}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_ctlpath.sv", "start": 356767, "end": 358557}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_datapath.sv", "start": 358557, "end": 362786}, {"filename": "/examples/rvsimple/reference_sv/toplevel.sv", "start": 362786, "end": 364417}, {"filename": "/examples/scratch.h", "start": 364417, "end": 364624}, {"filename": "/examples/scratch.sv", "start": 364624, "end": 366845}, {"filename": "/examples/tutorial/adder.h", "start": 366845, "end": 367025}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 367025, "end": 368006}, {"filename": "/examples/tutorial/blockram.h", "start": 368006, "end": 368523}, {"filename": "/examples/tutorial/checksum.h", "start": 368523, "end": 369246}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 369246, "end": 369782}, {"filename": "/examples/tutorial/counter.h", "start": 369782, "end": 369931}, {"filename": "/examples/tutorial/declaration_order.h", "start": 369931, "end": 370710}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 370710, "end": 372128}, {"filename": "/examples/tutorial/nonblocking.h", "start": 372128, "end": 372250}, {"filename": "/examples/tutorial/submodules.h", "start": 372250, "end": 373367}, {"filename": "/examples/tutorial/templates.h", "start": 373367, "end": 373838}, {"filename": "/examples/tutorial/tutorial2.h", "start": 373838, "end": 373906}, {"filename": "/examples/tutorial/tutorial3.h", "start": 373906, "end": 373947}, {"filename": "/examples/tutorial/tutorial4.h", "start": 373947, "end": 373988}, {"filename": "/examples/tutorial/tutorial5.h", "start": 373988, "end": 374029}, {"filename": "/examples/tutorial/vga.h", "start": 374029, "end": 375176}, {"filename": "/examples/uart/README.md", "start": 375176, "end": 375420}, {"filename": "/examples/uart/main.cpp", "start": 375420, "end": 376752}, {"filename": "/examples/uart/main_vl.cpp", "start": 376752, "end": 379272}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 379272, "end": 381852}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 381852, "end": 384413}, {"filename": "/examples/uart/metron/uart_top.h", "start": 384413, "end": 386177}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 386177, "end": 389196}, {"filename": "/examples/uart/metron_sv/uart_hello.sv", "start": 389196, "end": 392079}, {"filename": "/examples/uart/metron_sv/uart_rx.sv", "start": 392079, "end": 395047}, {"filename": "/examples/uart/metron_sv/uart_top.sv", "start": 395047, "end": 399129}, {"filename": "/examples/uart/metron_sv/uart_tx.sv", "start": 399129, "end": 402583}, {"filename": "/examples/uart/uart_test_ice40.sv", "start": 402583, "end": 404758}, {"filename": "/examples/uart/uart_test_iv.sv", "start": 404758, "end": 406355}, {"filename": "/tests/metron_bad/README.md", "start": 406355, "end": 406552}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 406552, "end": 406848}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 406848, "end": 407096}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 407096, "end": 407339}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 407339, "end": 407934}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 407934, "end": 408074}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 408074, "end": 408484}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 408484, "end": 409024}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 409024, "end": 409335}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 409335, "end": 409781}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 409781, "end": 410041}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 410041, "end": 410313}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 410313, "end": 410820}, {"filename": "/tests/metron_good/README.md", "start": 410820, "end": 410901}, {"filename": "/tests/metron_good/all_func_types.h", "start": 410901, "end": 412466}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 412466, "end": 412873}, {"filename": "/tests/metron_good/basic_function.h", "start": 412873, "end": 413152}, {"filename": "/tests/metron_good/basic_increment.h", "start": 413152, "end": 413507}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 413507, "end": 413802}, {"filename": "/tests/metron_good/basic_literals.h", "start": 413802, "end": 414414}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 414414, "end": 414660}, {"filename": "/tests/metron_good/basic_output.h", "start": 414660, "end": 414921}, {"filename": "/tests/metron_good/basic_param.h", "start": 414921, "end": 415180}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 415180, "end": 415411}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 415411, "end": 415591}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 415591, "end": 415854}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 415854, "end": 416074}, {"filename": "/tests/metron_good/basic_submod.h", "start": 416074, "end": 416363}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 416363, "end": 416718}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 416718, "end": 417094}, {"filename": "/tests/metron_good/basic_switch.h", "start": 417094, "end": 417571}, {"filename": "/tests/metron_good/basic_task.h", "start": 417571, "end": 417905}, {"filename": "/tests/metron_good/basic_template.h", "start": 417905, "end": 418336}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 418336, "end": 418495}, {"filename": "/tests/metron_good/bit_casts.h", "start": 418495, "end": 424468}, {"filename": "/tests/metron_good/bit_concat.h", "start": 424468, "end": 424895}, {"filename": "/tests/metron_good/bit_dup.h", "start": 424895, "end": 425554}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 425554, "end": 426392}, {"filename": "/tests/metron_good/builtins.h", "start": 426392, "end": 426723}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 426723, "end": 427030}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 427030, "end": 427584}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 427584, "end": 428301}, {"filename": "/tests/metron_good/constructor_args.h", "start": 428301, "end": 428755}, {"filename": "/tests/metron_good/defines.h", "start": 428755, "end": 429069}, {"filename": "/tests/metron_good/dontcare.h", "start": 429069, "end": 429352}, {"filename": "/tests/metron_good/enum_simple.h", "start": 429352, "end": 430720}, {"filename": "/tests/metron_good/for_loops.h", "start": 430720, "end": 431040}, {"filename": "/tests/metron_good/good_order.h", "start": 431040, "end": 431336}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 431336, "end": 431747}, {"filename": "/tests/metron_good/include_guards.h", "start": 431747, "end": 431944}, {"filename": "/tests/metron_good/init_chain.h", "start": 431944, "end": 432652}, {"filename": "/tests/metron_good/input_signals.h", "start": 432652, "end": 433314}, {"filename": "/tests/metron_good/local_localparam.h", "start": 433314, "end": 433492}, {"filename": "/tests/metron_good/magic_comments.h", "start": 433492, "end": 433795}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 433795, "end": 434109}, {"filename": "/tests/metron_good/minimal.h", "start": 434109, "end": 434184}, {"filename": "/tests/metron_good/multi_tick.h", "start": 434184, "end": 434550}, {"filename": "/tests/metron_good/namespaces.h", "start": 434550, "end": 434900}, {"filename": "/tests/metron_good/nested_structs.h", "start": 434900, "end": 435413}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 435413, "end": 435958}, {"filename": "/tests/metron_good/oneliners.h", "start": 435958, "end": 436221}, {"filename": "/tests/metron_good/plus_equals.h", "start": 436221, "end": 436571}, {"filename": "/tests/metron_good/private_getter.h", "start": 436571, "end": 436795}, {"filename": "/tests/metron_good/structs.h", "start": 436795, "end": 437014}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 437014, "end": 437768}, {"filename": "/tests/metron_good/tock_task.h", "start": 437768, "end": 438124}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 438124, "end": 438289}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 438289, "end": 438948}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 438948, "end": 439607}], "remote_package_size": 439607});

  })();
