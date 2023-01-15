
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/gb_spu/metron_sv/MetroBoySPU2.sv", "start": 26876, "end": 49299}, {"filename": "/examples/ibex/README.md", "start": 49299, "end": 49350}, {"filename": "/examples/ibex/main.cpp", "start": 49350, "end": 49491}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 49491, "end": 51076}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 51076, "end": 63174}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 63174, "end": 77584}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 77584, "end": 93608}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 93608, "end": 96109}, {"filename": "/examples/ibex/metron_ref/ibex_alu.sv", "start": 96109, "end": 148427}, {"filename": "/examples/ibex/metron_ref/ibex_compressed_decoder.sv", "start": 148427, "end": 159269}, {"filename": "/examples/ibex/metron_ref/ibex_multdiv_slow.sv", "start": 159269, "end": 172379}, {"filename": "/examples/ibex/metron_ref/ibex_pkg.sv", "start": 172379, "end": 188618}, {"filename": "/examples/ibex/metron_ref/prim_arbiter_fixed.sv", "start": 188618, "end": 194729}, {"filename": "/examples/j1/main.cpp", "start": 194729, "end": 194848}, {"filename": "/examples/j1/metron/dpram.h", "start": 194848, "end": 195286}, {"filename": "/examples/j1/metron/j1.h", "start": 195286, "end": 199311}, {"filename": "/examples/j1/metron_sv/j1.sv", "start": 199311, "end": 199339}, {"filename": "/examples/pong/README.md", "start": 199339, "end": 199399}, {"filename": "/examples/pong/main.cpp", "start": 199399, "end": 201345}, {"filename": "/examples/pong/main_vl.cpp", "start": 201345, "end": 201496}, {"filename": "/examples/pong/metron/pong.h", "start": 201496, "end": 205360}, {"filename": "/examples/pong/metron_sv/pong.sv", "start": 205360, "end": 209867}, {"filename": "/examples/pong/reference/README.md", "start": 209867, "end": 209927}, {"filename": "/examples/pong/reference/hvsync_generator.sv", "start": 209927, "end": 210866}, {"filename": "/examples/pong/reference/pong.sv", "start": 210866, "end": 214480}, {"filename": "/examples/rvsimple/README.md", "start": 214480, "end": 214559}, {"filename": "/examples/rvsimple/main.cpp", "start": 214559, "end": 217374}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 217374, "end": 220276}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 220276, "end": 223178}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 223178, "end": 223678}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 223678, "end": 225139}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 225139, "end": 227760}, {"filename": "/examples/rvsimple/metron/config.h", "start": 227760, "end": 228975}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 228975, "end": 234694}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 234694, "end": 235808}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 235808, "end": 237746}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 237746, "end": 238986}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 238986, "end": 240139}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 240139, "end": 240821}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 240821, "end": 241694}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 241694, "end": 243812}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 243812, "end": 244576}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 244576, "end": 245267}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 245267, "end": 246094}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 246094, "end": 247104}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 247104, "end": 248075}, {"filename": "/examples/rvsimple/metron/register.h", "start": 248075, "end": 248760}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 248760, "end": 251803}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 251803, "end": 257397}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 257397, "end": 259890}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 259890, "end": 264617}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 264617, "end": 266465}, {"filename": "/examples/rvsimple/metron_sv/adder.sv", "start": 266465, "end": 267064}, {"filename": "/examples/rvsimple/metron_sv/alu.sv", "start": 267064, "end": 268483}, {"filename": "/examples/rvsimple/metron_sv/alu_control.sv", "start": 268483, "end": 270924}, {"filename": "/examples/rvsimple/metron_sv/config.sv", "start": 270924, "end": 272120}, {"filename": "/examples/rvsimple/metron_sv/constants.sv", "start": 272120, "end": 277630}, {"filename": "/examples/rvsimple/metron_sv/control_transfer.sv", "start": 277630, "end": 278738}, {"filename": "/examples/rvsimple/metron_sv/data_memory_interface.sv", "start": 278738, "end": 280747}, {"filename": "/examples/rvsimple/metron_sv/example_data_memory.sv", "start": 280747, "end": 282182}, {"filename": "/examples/rvsimple/metron_sv/example_data_memory_bus.sv", "start": 282182, "end": 283925}, {"filename": "/examples/rvsimple/metron_sv/example_text_memory.sv", "start": 283925, "end": 284693}, {"filename": "/examples/rvsimple/metron_sv/example_text_memory_bus.sv", "start": 284693, "end": 285834}, {"filename": "/examples/rvsimple/metron_sv/immediate_generator.sv", "start": 285834, "end": 287894}, {"filename": "/examples/rvsimple/metron_sv/instruction_decoder.sv", "start": 287894, "end": 288783}, {"filename": "/examples/rvsimple/metron_sv/multiplexer2.sv", "start": 288783, "end": 289516}, {"filename": "/examples/rvsimple/metron_sv/multiplexer4.sv", "start": 289516, "end": 290370}, {"filename": "/examples/rvsimple/metron_sv/multiplexer8.sv", "start": 290370, "end": 291439}, {"filename": "/examples/rvsimple/metron_sv/regfile.sv", "start": 291439, "end": 292619}, {"filename": "/examples/rvsimple/metron_sv/register.sv", "start": 292619, "end": 293471}, {"filename": "/examples/rvsimple/metron_sv/riscv_core.sv", "start": 293471, "end": 299453}, {"filename": "/examples/rvsimple/metron_sv/singlecycle_control.sv", "start": 299453, "end": 304614}, {"filename": "/examples/rvsimple/metron_sv/singlecycle_ctlpath.sv", "start": 304614, "end": 308557}, {"filename": "/examples/rvsimple/metron_sv/singlecycle_datapath.sv", "start": 308557, "end": 318152}, {"filename": "/examples/rvsimple/metron_sv/toplevel.sv", "start": 318152, "end": 321548}, {"filename": "/examples/rvsimple/reference_sv/adder.sv", "start": 321548, "end": 321997}, {"filename": "/examples/rvsimple/reference_sv/alu.sv", "start": 321997, "end": 325118}, {"filename": "/examples/rvsimple/reference_sv/alu_control.sv", "start": 325118, "end": 328571}, {"filename": "/examples/rvsimple/reference_sv/config.sv", "start": 328571, "end": 329978}, {"filename": "/examples/rvsimple/reference_sv/constants.sv", "start": 329978, "end": 334715}, {"filename": "/examples/rvsimple/reference_sv/control_transfer.sv", "start": 334715, "end": 335600}, {"filename": "/examples/rvsimple/reference_sv/data_memory_interface.sv", "start": 335600, "end": 337511}, {"filename": "/examples/rvsimple/reference_sv/example_data_memory.sv", "start": 337511, "end": 338426}, {"filename": "/examples/rvsimple/reference_sv/example_data_memory_bus.sv", "start": 338426, "end": 339440}, {"filename": "/examples/rvsimple/reference_sv/example_memory_bus.sv", "start": 339440, "end": 340748}, {"filename": "/examples/rvsimple/reference_sv/example_text_memory.sv", "start": 340748, "end": 341274}, {"filename": "/examples/rvsimple/reference_sv/example_text_memory_bus.sv", "start": 341274, "end": 341959}, {"filename": "/examples/rvsimple/reference_sv/immediate_generator.sv", "start": 341959, "end": 343899}, {"filename": "/examples/rvsimple/reference_sv/instruction_decoder.sv", "start": 343899, "end": 344612}, {"filename": "/examples/rvsimple/reference_sv/multiplexer.sv", "start": 344612, "end": 345390}, {"filename": "/examples/rvsimple/reference_sv/multiplexer2.sv", "start": 345390, "end": 345978}, {"filename": "/examples/rvsimple/reference_sv/multiplexer4.sv", "start": 345978, "end": 346632}, {"filename": "/examples/rvsimple/reference_sv/multiplexer8.sv", "start": 346632, "end": 347418}, {"filename": "/examples/rvsimple/reference_sv/regfile.sv", "start": 347418, "end": 348327}, {"filename": "/examples/rvsimple/reference_sv/register.sv", "start": 348327, "end": 348960}, {"filename": "/examples/rvsimple/reference_sv/riscv_core.sv", "start": 348960, "end": 352334}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_control.sv", "start": 352334, "end": 357066}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_ctlpath.sv", "start": 357066, "end": 358856}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_datapath.sv", "start": 358856, "end": 363085}, {"filename": "/examples/rvsimple/reference_sv/toplevel.sv", "start": 363085, "end": 364716}, {"filename": "/examples/scratch.h", "start": 364716, "end": 364923}, {"filename": "/examples/scratch.sv", "start": 364923, "end": 367144}, {"filename": "/examples/tutorial/adder.h", "start": 367144, "end": 367324}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 367324, "end": 368305}, {"filename": "/examples/tutorial/blockram.h", "start": 368305, "end": 368822}, {"filename": "/examples/tutorial/checksum.h", "start": 368822, "end": 369545}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 369545, "end": 370081}, {"filename": "/examples/tutorial/counter.h", "start": 370081, "end": 370230}, {"filename": "/examples/tutorial/declaration_order.h", "start": 370230, "end": 371009}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 371009, "end": 372427}, {"filename": "/examples/tutorial/nonblocking.h", "start": 372427, "end": 372549}, {"filename": "/examples/tutorial/submodules.h", "start": 372549, "end": 373666}, {"filename": "/examples/tutorial/templates.h", "start": 373666, "end": 374137}, {"filename": "/examples/tutorial/tutorial2.h", "start": 374137, "end": 374205}, {"filename": "/examples/tutorial/tutorial3.h", "start": 374205, "end": 374246}, {"filename": "/examples/tutorial/tutorial4.h", "start": 374246, "end": 374287}, {"filename": "/examples/tutorial/tutorial5.h", "start": 374287, "end": 374328}, {"filename": "/examples/tutorial/vga.h", "start": 374328, "end": 375475}, {"filename": "/examples/uart/README.md", "start": 375475, "end": 375719}, {"filename": "/examples/uart/main.cpp", "start": 375719, "end": 377051}, {"filename": "/examples/uart/main_vl.cpp", "start": 377051, "end": 379571}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 379571, "end": 382151}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 382151, "end": 384712}, {"filename": "/examples/uart/metron/uart_top.h", "start": 384712, "end": 386476}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 386476, "end": 389495}, {"filename": "/examples/uart/metron_sv/uart_hello.sv", "start": 389495, "end": 392373}, {"filename": "/examples/uart/metron_sv/uart_rx.sv", "start": 392373, "end": 395336}, {"filename": "/examples/uart/metron_sv/uart_top.sv", "start": 395336, "end": 399562}, {"filename": "/examples/uart/metron_sv/uart_tx.sv", "start": 399562, "end": 403011}, {"filename": "/examples/uart/uart_test_ice40.sv", "start": 403011, "end": 405186}, {"filename": "/examples/uart/uart_test_iv.sv", "start": 405186, "end": 406783}, {"filename": "/tests/metron_bad/README.md", "start": 406783, "end": 406980}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 406980, "end": 407276}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 407276, "end": 407524}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 407524, "end": 407767}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 407767, "end": 408362}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 408362, "end": 408772}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 408772, "end": 409312}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 409312, "end": 409623}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 409623, "end": 410069}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 410069, "end": 410329}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 410329, "end": 410601}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 410601, "end": 411108}, {"filename": "/tests/metron_good/README.md", "start": 411108, "end": 411189}, {"filename": "/tests/metron_good/all_func_types.h", "start": 411189, "end": 412754}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 412754, "end": 413161}, {"filename": "/tests/metron_good/basic_function.h", "start": 413161, "end": 413440}, {"filename": "/tests/metron_good/basic_increment.h", "start": 413440, "end": 413795}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 413795, "end": 414090}, {"filename": "/tests/metron_good/basic_literals.h", "start": 414090, "end": 414702}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 414702, "end": 414948}, {"filename": "/tests/metron_good/basic_output.h", "start": 414948, "end": 415209}, {"filename": "/tests/metron_good/basic_param.h", "start": 415209, "end": 415468}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 415468, "end": 415699}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 415699, "end": 415879}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 415879, "end": 416142}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 416142, "end": 416362}, {"filename": "/tests/metron_good/basic_submod.h", "start": 416362, "end": 416651}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 416651, "end": 417006}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 417006, "end": 417382}, {"filename": "/tests/metron_good/basic_switch.h", "start": 417382, "end": 417859}, {"filename": "/tests/metron_good/basic_task.h", "start": 417859, "end": 418193}, {"filename": "/tests/metron_good/basic_template.h", "start": 418193, "end": 418679}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 418679, "end": 418838}, {"filename": "/tests/metron_good/bit_casts.h", "start": 418838, "end": 424811}, {"filename": "/tests/metron_good/bit_concat.h", "start": 424811, "end": 425238}, {"filename": "/tests/metron_good/bit_dup.h", "start": 425238, "end": 425897}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 425897, "end": 426735}, {"filename": "/tests/metron_good/builtins.h", "start": 426735, "end": 427066}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 427066, "end": 427373}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 427373, "end": 427927}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 427927, "end": 428820}, {"filename": "/tests/metron_good/constructor_args.h", "start": 428820, "end": 429328}, {"filename": "/tests/metron_good/defines.h", "start": 429328, "end": 429642}, {"filename": "/tests/metron_good/dontcare.h", "start": 429642, "end": 429925}, {"filename": "/tests/metron_good/enum_simple.h", "start": 429925, "end": 431293}, {"filename": "/tests/metron_good/for_loops.h", "start": 431293, "end": 431613}, {"filename": "/tests/metron_good/good_order.h", "start": 431613, "end": 431909}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 431909, "end": 432320}, {"filename": "/tests/metron_good/include_guards.h", "start": 432320, "end": 432517}, {"filename": "/tests/metron_good/init_chain.h", "start": 432517, "end": 433225}, {"filename": "/tests/metron_good/input_signals.h", "start": 433225, "end": 433887}, {"filename": "/tests/metron_good/local_localparam.h", "start": 433887, "end": 434065}, {"filename": "/tests/metron_good/magic_comments.h", "start": 434065, "end": 434368}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 434368, "end": 434682}, {"filename": "/tests/metron_good/minimal.h", "start": 434682, "end": 434757}, {"filename": "/tests/metron_good/multi_tick.h", "start": 434757, "end": 435123}, {"filename": "/tests/metron_good/namespaces.h", "start": 435123, "end": 435473}, {"filename": "/tests/metron_good/nested_structs.h", "start": 435473, "end": 435986}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 435986, "end": 436531}, {"filename": "/tests/metron_good/oneliners.h", "start": 436531, "end": 436794}, {"filename": "/tests/metron_good/plus_equals.h", "start": 436794, "end": 437144}, {"filename": "/tests/metron_good/private_getter.h", "start": 437144, "end": 437368}, {"filename": "/tests/metron_good/structs.h", "start": 437368, "end": 437587}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 437587, "end": 438341}, {"filename": "/tests/metron_good/tock_task.h", "start": 438341, "end": 438697}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 438697, "end": 438862}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 438862, "end": 439521}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 439521, "end": 440180}], "remote_package_size": 440180});

  })();
