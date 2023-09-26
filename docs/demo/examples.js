
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
Module['FS_createPath']("/examples/rvsimple", "metron_sv2", true, true);
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22885}, {"filename": "/examples/gb_spu/metron_sv/MetroBoySPU2.sv", "start": 22885, "end": 46311}, {"filename": "/examples/ibex/README.md", "start": 46311, "end": 46362}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 46362, "end": 48007}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 48007, "end": 60198}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 60198, "end": 74696}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 74696, "end": 90779}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 90779, "end": 93370}, {"filename": "/examples/ibex/metron_ref/ibex_alu.sv", "start": 93370, "end": 145688}, {"filename": "/examples/ibex/metron_ref/ibex_compressed_decoder.sv", "start": 145688, "end": 156530}, {"filename": "/examples/ibex/metron_ref/ibex_multdiv_slow.sv", "start": 156530, "end": 169640}, {"filename": "/examples/ibex/metron_ref/ibex_pkg.sv", "start": 169640, "end": 185879}, {"filename": "/examples/ibex/metron_ref/prim_arbiter_fixed.sv", "start": 185879, "end": 191990}, {"filename": "/examples/j1/metron/dpram.h", "start": 191990, "end": 192441}, {"filename": "/examples/j1/metron/j1.h", "start": 192441, "end": 196474}, {"filename": "/examples/j1/metron_sv/j1.sv", "start": 196474, "end": 196514}, {"filename": "/examples/pong/README.md", "start": 196514, "end": 196574}, {"filename": "/examples/pong/metron/pong.h", "start": 196574, "end": 200476}, {"filename": "/examples/pong/metron_sv/pong.sv", "start": 200476, "end": 205111}, {"filename": "/examples/pong/reference/README.md", "start": 205111, "end": 205171}, {"filename": "/examples/pong/reference/hvsync_generator.sv", "start": 205171, "end": 206110}, {"filename": "/examples/pong/reference/pong.sv", "start": 206110, "end": 209724}, {"filename": "/examples/rvsimple/README.md", "start": 209724, "end": 209803}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 209803, "end": 210316}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 210316, "end": 211790}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 211790, "end": 214408}, {"filename": "/examples/rvsimple/metron/config.h", "start": 214408, "end": 215636}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 215636, "end": 221368}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 221368, "end": 222491}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 222491, "end": 224437}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 224437, "end": 225677}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 225677, "end": 226934}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 226934, "end": 227616}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 227616, "end": 228593}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 228593, "end": 230724}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 230724, "end": 231501}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 231501, "end": 232194}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 232194, "end": 233023}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 233023, "end": 234035}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 234035, "end": 235019}, {"filename": "/examples/rvsimple/metron/register.h", "start": 235019, "end": 235717}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 235717, "end": 238773}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 238773, "end": 244360}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 244360, "end": 246866}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 246866, "end": 251606}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 251606, "end": 253607}, {"filename": "/examples/rvsimple/metron_sv/adder.sv", "start": 253607, "end": 254220}, {"filename": "/examples/rvsimple/metron_sv/alu.sv", "start": 254220, "end": 255783}, {"filename": "/examples/rvsimple/metron_sv/alu_control.sv", "start": 255783, "end": 258492}, {"filename": "/examples/rvsimple/metron_sv/config.sv", "start": 258492, "end": 259890}, {"filename": "/examples/rvsimple/metron_sv/constants.sv", "start": 259890, "end": 267681}, {"filename": "/examples/rvsimple/metron_sv/control_transfer.sv", "start": 267681, "end": 268876}, {"filename": "/examples/rvsimple/metron_sv/data_memory_interface.sv", "start": 268876, "end": 270982}, {"filename": "/examples/rvsimple/metron_sv/example_data_memory.sv", "start": 270982, "end": 272442}, {"filename": "/examples/rvsimple/metron_sv/example_data_memory_bus.sv", "start": 272442, "end": 274349}, {"filename": "/examples/rvsimple/metron_sv/example_text_memory.sv", "start": 274349, "end": 275127}, {"filename": "/examples/rvsimple/metron_sv/example_text_memory_bus.sv", "start": 275127, "end": 276434}, {"filename": "/examples/rvsimple/metron_sv/immediate_generator.sv", "start": 276434, "end": 278572}, {"filename": "/examples/rvsimple/metron_sv/instruction_decoder.sv", "start": 278572, "end": 279472}, {"filename": "/examples/rvsimple/metron_sv/multiplexer2.sv", "start": 279472, "end": 280265}, {"filename": "/examples/rvsimple/metron_sv/multiplexer4.sv", "start": 280265, "end": 281217}, {"filename": "/examples/rvsimple/metron_sv/multiplexer8.sv", "start": 281217, "end": 282388}, {"filename": "/examples/rvsimple/metron_sv/regfile.sv", "start": 282388, "end": 283592}, {"filename": "/examples/rvsimple/metron_sv/register.sv", "start": 283592, "end": 284469}, {"filename": "/examples/rvsimple/metron_sv/riscv_core.sv", "start": 284469, "end": 291366}, {"filename": "/examples/rvsimple/metron_sv/singlecycle_control.sv", "start": 291366, "end": 297102}, {"filename": "/examples/rvsimple/metron_sv/singlecycle_ctlpath.sv", "start": 297102, "end": 301515}, {"filename": "/examples/rvsimple/metron_sv/singlecycle_datapath.sv", "start": 301515, "end": 311455}, {"filename": "/examples/rvsimple/metron_sv/toplevel.sv", "start": 311455, "end": 315234}, {"filename": "/examples/rvsimple/metron_sv2/regfile.sv", "start": 315234, "end": 316438}, {"filename": "/examples/rvsimple/reference_sv/adder.sv", "start": 316438, "end": 316887}, {"filename": "/examples/rvsimple/reference_sv/alu.sv", "start": 316887, "end": 320008}, {"filename": "/examples/rvsimple/reference_sv/alu_control.sv", "start": 320008, "end": 323461}, {"filename": "/examples/rvsimple/reference_sv/config.sv", "start": 323461, "end": 324868}, {"filename": "/examples/rvsimple/reference_sv/constants.sv", "start": 324868, "end": 329605}, {"filename": "/examples/rvsimple/reference_sv/control_transfer.sv", "start": 329605, "end": 330490}, {"filename": "/examples/rvsimple/reference_sv/data_memory_interface.sv", "start": 330490, "end": 332401}, {"filename": "/examples/rvsimple/reference_sv/example_data_memory.sv", "start": 332401, "end": 333316}, {"filename": "/examples/rvsimple/reference_sv/example_data_memory_bus.sv", "start": 333316, "end": 334330}, {"filename": "/examples/rvsimple/reference_sv/example_memory_bus.sv", "start": 334330, "end": 335638}, {"filename": "/examples/rvsimple/reference_sv/example_text_memory.sv", "start": 335638, "end": 336164}, {"filename": "/examples/rvsimple/reference_sv/example_text_memory_bus.sv", "start": 336164, "end": 336849}, {"filename": "/examples/rvsimple/reference_sv/immediate_generator.sv", "start": 336849, "end": 338789}, {"filename": "/examples/rvsimple/reference_sv/instruction_decoder.sv", "start": 338789, "end": 339502}, {"filename": "/examples/rvsimple/reference_sv/multiplexer.sv", "start": 339502, "end": 340280}, {"filename": "/examples/rvsimple/reference_sv/multiplexer2.sv", "start": 340280, "end": 340868}, {"filename": "/examples/rvsimple/reference_sv/multiplexer4.sv", "start": 340868, "end": 341522}, {"filename": "/examples/rvsimple/reference_sv/multiplexer8.sv", "start": 341522, "end": 342308}, {"filename": "/examples/rvsimple/reference_sv/regfile.sv", "start": 342308, "end": 343217}, {"filename": "/examples/rvsimple/reference_sv/register.sv", "start": 343217, "end": 343850}, {"filename": "/examples/rvsimple/reference_sv/riscv_core.sv", "start": 343850, "end": 347224}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_control.sv", "start": 347224, "end": 351956}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_ctlpath.sv", "start": 351956, "end": 353746}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_datapath.sv", "start": 353746, "end": 357975}, {"filename": "/examples/rvsimple/reference_sv/toplevel.sv", "start": 357975, "end": 359606}, {"filename": "/examples/scratch.h", "start": 359606, "end": 359767}, {"filename": "/examples/scratch.sv", "start": 359767, "end": 360026}, {"filename": "/examples/scratch_good.sv", "start": 360026, "end": 363382}, {"filename": "/examples/tutorial/adder.h", "start": 363382, "end": 363562}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 363562, "end": 364556}, {"filename": "/examples/tutorial/blockram.h", "start": 364556, "end": 365086}, {"filename": "/examples/tutorial/checksum.h", "start": 365086, "end": 365822}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 365822, "end": 366358}, {"filename": "/examples/tutorial/counter.h", "start": 366358, "end": 366507}, {"filename": "/examples/tutorial/declaration_order.h", "start": 366507, "end": 367286}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 367286, "end": 368704}, {"filename": "/examples/tutorial/nonblocking.h", "start": 368704, "end": 368826}, {"filename": "/examples/tutorial/submodules.h", "start": 368826, "end": 369943}, {"filename": "/examples/tutorial/templates.h", "start": 369943, "end": 370432}, {"filename": "/examples/tutorial/tutorial2.h", "start": 370432, "end": 370500}, {"filename": "/examples/tutorial/tutorial3.h", "start": 370500, "end": 370541}, {"filename": "/examples/tutorial/tutorial4.h", "start": 370541, "end": 370582}, {"filename": "/examples/tutorial/tutorial5.h", "start": 370582, "end": 370623}, {"filename": "/examples/tutorial/vga.h", "start": 370623, "end": 371783}, {"filename": "/examples/uart/README.md", "start": 371783, "end": 372027}, {"filename": "/examples/uart/message.txt", "start": 372027, "end": 372539}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 372539, "end": 375132}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 375132, "end": 377706}, {"filename": "/examples/uart/metron/uart_top.h", "start": 377706, "end": 379483}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 379483, "end": 382515}, {"filename": "/examples/uart/metron_sv/uart_hello.sv", "start": 382515, "end": 385510}, {"filename": "/examples/uart/metron_sv/uart_rx.sv", "start": 385510, "end": 388569}, {"filename": "/examples/uart/metron_sv/uart_top.sv", "start": 388569, "end": 392924}, {"filename": "/examples/uart/metron_sv/uart_tx.sv", "start": 392924, "end": 396512}, {"filename": "/examples/uart/uart_test_ice40.sv", "start": 396512, "end": 398687}, {"filename": "/examples/uart/uart_test_iv.sv", "start": 398687, "end": 400284}, {"filename": "/tests/metron_bad/README.md", "start": 400284, "end": 400481}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 400481, "end": 400790}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 400790, "end": 401051}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 401051, "end": 401307}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 401307, "end": 401915}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 401915, "end": 402155}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 402155, "end": 402578}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 402578, "end": 403131}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 403131, "end": 403455}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 403455, "end": 403914}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 403914, "end": 404187}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 404187, "end": 404472}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 404472, "end": 404992}, {"filename": "/tests/metron_good/README.md", "start": 404992, "end": 405073}, {"filename": "/tests/metron_good/all_func_types.h", "start": 405073, "end": 406751}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 406751, "end": 407171}, {"filename": "/tests/metron_good/basic_function.h", "start": 407171, "end": 407463}, {"filename": "/tests/metron_good/basic_increment.h", "start": 407463, "end": 407831}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 407831, "end": 408139}, {"filename": "/tests/metron_good/basic_literals.h", "start": 408139, "end": 408764}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 408764, "end": 409023}, {"filename": "/tests/metron_good/basic_output.h", "start": 409023, "end": 409297}, {"filename": "/tests/metron_good/basic_param.h", "start": 409297, "end": 409569}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 409569, "end": 409813}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 409813, "end": 410006}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 410006, "end": 410282}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 410282, "end": 410515}, {"filename": "/tests/metron_good/basic_submod.h", "start": 410515, "end": 410834}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 410834, "end": 411202}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 411202, "end": 411591}, {"filename": "/tests/metron_good/basic_switch.h", "start": 411591, "end": 412081}, {"filename": "/tests/metron_good/basic_task.h", "start": 412081, "end": 412428}, {"filename": "/tests/metron_good/basic_template.h", "start": 412428, "end": 412927}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 412927, "end": 413099}, {"filename": "/tests/metron_good/bit_casts.h", "start": 413099, "end": 419085}, {"filename": "/tests/metron_good/bit_concat.h", "start": 419085, "end": 419525}, {"filename": "/tests/metron_good/bit_dup.h", "start": 419525, "end": 420197}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 420197, "end": 421105}, {"filename": "/tests/metron_good/builtins.h", "start": 421105, "end": 421449}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 421449, "end": 421769}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 421769, "end": 422336}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 422336, "end": 423190}, {"filename": "/tests/metron_good/constructor_args.h", "start": 423190, "end": 423662}, {"filename": "/tests/metron_good/defines.h", "start": 423662, "end": 423989}, {"filename": "/tests/metron_good/dontcare.h", "start": 423989, "end": 424282}, {"filename": "/tests/metron_good/enum_simple.h", "start": 424282, "end": 425724}, {"filename": "/tests/metron_good/for_loops.h", "start": 425724, "end": 426057}, {"filename": "/tests/metron_good/good_order.h", "start": 426057, "end": 426366}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 426366, "end": 426790}, {"filename": "/tests/metron_good/include_guards.h", "start": 426790, "end": 426987}, {"filename": "/tests/metron_good/init_chain.h", "start": 426987, "end": 427708}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 427708, "end": 428006}, {"filename": "/tests/metron_good/input_signals.h", "start": 428006, "end": 428804}, {"filename": "/tests/metron_good/local_localparam.h", "start": 428804, "end": 428995}, {"filename": "/tests/metron_good/magic_comments.h", "start": 428995, "end": 429311}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 429311, "end": 429638}, {"filename": "/tests/metron_good/minimal.h", "start": 429638, "end": 429824}, {"filename": "/tests/metron_good/multi_tick.h", "start": 429824, "end": 430203}, {"filename": "/tests/metron_good/namespaces.h", "start": 430203, "end": 430566}, {"filename": "/tests/metron_good/nested_structs.h", "start": 430566, "end": 431026}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 431026, "end": 431584}, {"filename": "/tests/metron_good/oneliners.h", "start": 431584, "end": 431860}, {"filename": "/tests/metron_good/plus_equals.h", "start": 431860, "end": 432284}, {"filename": "/tests/metron_good/private_getter.h", "start": 432284, "end": 432521}, {"filename": "/tests/metron_good/structs.h", "start": 432521, "end": 432753}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 432753, "end": 433302}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 433302, "end": 435859}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 435859, "end": 436626}, {"filename": "/tests/metron_good/tock_task.h", "start": 436626, "end": 437007}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 437007, "end": 437185}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 437185, "end": 437857}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 437857, "end": 438529}], "remote_package_size": 438529});

  })();
