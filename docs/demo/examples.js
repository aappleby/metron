
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
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples/ibex", "metron_ref", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples/j1", "metron", true, true);
Module['FS_createPath']("/examples/j1", "metron_sv", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
Module['FS_createPath']("/examples/rvsimple", "reference_sv", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/examples/uart", "metron", true, true);
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/ibex/README.md", "start": 26876, "end": 26927}, {"filename": "/examples/ibex/main.cpp", "start": 26927, "end": 27068}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 27068, "end": 28653}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 28653, "end": 40751}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 40751, "end": 55161}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 55161, "end": 71185}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 71185, "end": 73686}, {"filename": "/examples/ibex/metron_ref/ibex_alu.sv", "start": 73686, "end": 126004}, {"filename": "/examples/ibex/metron_ref/ibex_compressed_decoder.sv", "start": 126004, "end": 136846}, {"filename": "/examples/ibex/metron_ref/ibex_multdiv_slow.sv", "start": 136846, "end": 149956}, {"filename": "/examples/ibex/metron_ref/ibex_pkg.sv", "start": 149956, "end": 166195}, {"filename": "/examples/ibex/metron_ref/prim_arbiter_fixed.sv", "start": 166195, "end": 172306}, {"filename": "/examples/j1/main.cpp", "start": 172306, "end": 172425}, {"filename": "/examples/j1/metron/dpram.h", "start": 172425, "end": 172863}, {"filename": "/examples/j1/metron/j1.h", "start": 172863, "end": 176888}, {"filename": "/examples/j1/metron_sv/j1.sv", "start": 176888, "end": 176916}, {"filename": "/examples/pong/README.md", "start": 176916, "end": 176976}, {"filename": "/examples/pong/main.cpp", "start": 176976, "end": 178922}, {"filename": "/examples/pong/main_vl.cpp", "start": 178922, "end": 179073}, {"filename": "/examples/pong/metron/pong.h", "start": 179073, "end": 182937}, {"filename": "/examples/pong/reference/README.md", "start": 182937, "end": 182997}, {"filename": "/examples/pong/reference/hvsync_generator.sv", "start": 182997, "end": 183936}, {"filename": "/examples/pong/reference/pong.sv", "start": 183936, "end": 187550}, {"filename": "/examples/rvsimple/README.md", "start": 187550, "end": 187629}, {"filename": "/examples/rvsimple/main.cpp", "start": 187629, "end": 190444}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 190444, "end": 193346}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 193346, "end": 196248}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 196248, "end": 196748}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 196748, "end": 198209}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 198209, "end": 200830}, {"filename": "/examples/rvsimple/metron/config.h", "start": 200830, "end": 202045}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 202045, "end": 207764}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 207764, "end": 208878}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 208878, "end": 210816}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 210816, "end": 212056}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 212056, "end": 213209}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 213209, "end": 213891}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 213891, "end": 214764}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 214764, "end": 216882}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 216882, "end": 217646}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 217646, "end": 218337}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 218337, "end": 219164}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 219164, "end": 220174}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 220174, "end": 221145}, {"filename": "/examples/rvsimple/metron/register.h", "start": 221145, "end": 221830}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 221830, "end": 224873}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 224873, "end": 230467}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 230467, "end": 232960}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 232960, "end": 237687}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 237687, "end": 239535}, {"filename": "/examples/rvsimple/reference_sv/adder.sv", "start": 239535, "end": 239984}, {"filename": "/examples/rvsimple/reference_sv/alu.sv", "start": 239984, "end": 243105}, {"filename": "/examples/rvsimple/reference_sv/alu_control.sv", "start": 243105, "end": 246558}, {"filename": "/examples/rvsimple/reference_sv/config.sv", "start": 246558, "end": 247965}, {"filename": "/examples/rvsimple/reference_sv/constants.sv", "start": 247965, "end": 252702}, {"filename": "/examples/rvsimple/reference_sv/control_transfer.sv", "start": 252702, "end": 253587}, {"filename": "/examples/rvsimple/reference_sv/data_memory_interface.sv", "start": 253587, "end": 255498}, {"filename": "/examples/rvsimple/reference_sv/example_data_memory.sv", "start": 255498, "end": 256413}, {"filename": "/examples/rvsimple/reference_sv/example_data_memory_bus.sv", "start": 256413, "end": 257427}, {"filename": "/examples/rvsimple/reference_sv/example_memory_bus.sv", "start": 257427, "end": 258735}, {"filename": "/examples/rvsimple/reference_sv/example_text_memory.sv", "start": 258735, "end": 259261}, {"filename": "/examples/rvsimple/reference_sv/example_text_memory_bus.sv", "start": 259261, "end": 259946}, {"filename": "/examples/rvsimple/reference_sv/immediate_generator.sv", "start": 259946, "end": 261886}, {"filename": "/examples/rvsimple/reference_sv/instruction_decoder.sv", "start": 261886, "end": 262599}, {"filename": "/examples/rvsimple/reference_sv/multiplexer.sv", "start": 262599, "end": 263377}, {"filename": "/examples/rvsimple/reference_sv/multiplexer2.sv", "start": 263377, "end": 263965}, {"filename": "/examples/rvsimple/reference_sv/multiplexer4.sv", "start": 263965, "end": 264619}, {"filename": "/examples/rvsimple/reference_sv/multiplexer8.sv", "start": 264619, "end": 265405}, {"filename": "/examples/rvsimple/reference_sv/regfile.sv", "start": 265405, "end": 266314}, {"filename": "/examples/rvsimple/reference_sv/register.sv", "start": 266314, "end": 266947}, {"filename": "/examples/rvsimple/reference_sv/riscv_core.sv", "start": 266947, "end": 270321}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_control.sv", "start": 270321, "end": 275053}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_ctlpath.sv", "start": 275053, "end": 276843}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_datapath.sv", "start": 276843, "end": 281072}, {"filename": "/examples/rvsimple/reference_sv/toplevel.sv", "start": 281072, "end": 282703}, {"filename": "/examples/scratch.h", "start": 282703, "end": 283223}, {"filename": "/examples/tutorial/adder.h", "start": 283223, "end": 283403}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 283403, "end": 284384}, {"filename": "/examples/tutorial/blockram.h", "start": 284384, "end": 284901}, {"filename": "/examples/tutorial/checksum.h", "start": 284901, "end": 285624}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 285624, "end": 286160}, {"filename": "/examples/tutorial/counter.h", "start": 286160, "end": 286309}, {"filename": "/examples/tutorial/declaration_order.h", "start": 286309, "end": 287088}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 287088, "end": 288506}, {"filename": "/examples/tutorial/nonblocking.h", "start": 288506, "end": 288628}, {"filename": "/examples/tutorial/submodules.h", "start": 288628, "end": 289745}, {"filename": "/examples/tutorial/templates.h", "start": 289745, "end": 290216}, {"filename": "/examples/tutorial/tutorial2.h", "start": 290216, "end": 290284}, {"filename": "/examples/tutorial/tutorial3.h", "start": 290284, "end": 290325}, {"filename": "/examples/tutorial/tutorial4.h", "start": 290325, "end": 290366}, {"filename": "/examples/tutorial/tutorial5.h", "start": 290366, "end": 290407}, {"filename": "/examples/tutorial/vga.h", "start": 290407, "end": 291554}, {"filename": "/examples/uart/README.md", "start": 291554, "end": 291798}, {"filename": "/examples/uart/main.cpp", "start": 291798, "end": 293130}, {"filename": "/examples/uart/main_vl.cpp", "start": 293130, "end": 295650}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 295650, "end": 298230}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 298230, "end": 300791}, {"filename": "/examples/uart/metron/uart_top.h", "start": 300791, "end": 302555}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 302555, "end": 305574}, {"filename": "/examples/uart/uart_test_ice40.sv", "start": 305574, "end": 307749}, {"filename": "/examples/uart/uart_test_iv.sv", "start": 307749, "end": 309346}, {"filename": "/tests/metron_bad/README.md", "start": 309346, "end": 309543}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 309543, "end": 309839}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 309839, "end": 310087}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 310087, "end": 310330}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 310330, "end": 310925}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 310925, "end": 311335}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 311335, "end": 311875}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 311875, "end": 312186}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 312186, "end": 312632}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 312632, "end": 312892}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 312892, "end": 313164}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 313164, "end": 313671}, {"filename": "/tests/metron_good/README.md", "start": 313671, "end": 313752}, {"filename": "/tests/metron_good/all_func_types.h", "start": 313752, "end": 315317}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 315317, "end": 315724}, {"filename": "/tests/metron_good/basic_function.h", "start": 315724, "end": 316003}, {"filename": "/tests/metron_good/basic_increment.h", "start": 316003, "end": 316358}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 316358, "end": 316653}, {"filename": "/tests/metron_good/basic_literals.h", "start": 316653, "end": 317265}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 317265, "end": 317511}, {"filename": "/tests/metron_good/basic_output.h", "start": 317511, "end": 317772}, {"filename": "/tests/metron_good/basic_param.h", "start": 317772, "end": 318031}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 318031, "end": 318262}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 318262, "end": 318442}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 318442, "end": 318705}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 318705, "end": 318925}, {"filename": "/tests/metron_good/basic_submod.h", "start": 318925, "end": 319214}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 319214, "end": 319569}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 319569, "end": 319945}, {"filename": "/tests/metron_good/basic_switch.h", "start": 319945, "end": 320422}, {"filename": "/tests/metron_good/basic_task.h", "start": 320422, "end": 320756}, {"filename": "/tests/metron_good/basic_template.h", "start": 320756, "end": 321242}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 321242, "end": 321401}, {"filename": "/tests/metron_good/bit_casts.h", "start": 321401, "end": 327374}, {"filename": "/tests/metron_good/bit_concat.h", "start": 327374, "end": 327801}, {"filename": "/tests/metron_good/bit_dup.h", "start": 327801, "end": 328460}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 328460, "end": 329298}, {"filename": "/tests/metron_good/builtins.h", "start": 329298, "end": 329629}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 329629, "end": 329936}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 329936, "end": 330490}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 330490, "end": 331383}, {"filename": "/tests/metron_good/constructor_args.h", "start": 331383, "end": 331891}, {"filename": "/tests/metron_good/defines.h", "start": 331891, "end": 332205}, {"filename": "/tests/metron_good/dontcare.h", "start": 332205, "end": 332488}, {"filename": "/tests/metron_good/enum_simple.h", "start": 332488, "end": 333856}, {"filename": "/tests/metron_good/for_loops.h", "start": 333856, "end": 334176}, {"filename": "/tests/metron_good/good_order.h", "start": 334176, "end": 334472}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 334472, "end": 334883}, {"filename": "/tests/metron_good/include_guards.h", "start": 334883, "end": 335080}, {"filename": "/tests/metron_good/init_chain.h", "start": 335080, "end": 335788}, {"filename": "/tests/metron_good/input_signals.h", "start": 335788, "end": 336450}, {"filename": "/tests/metron_good/local_localparam.h", "start": 336450, "end": 336628}, {"filename": "/tests/metron_good/magic_comments.h", "start": 336628, "end": 336931}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 336931, "end": 337245}, {"filename": "/tests/metron_good/minimal.h", "start": 337245, "end": 337320}, {"filename": "/tests/metron_good/multi_tick.h", "start": 337320, "end": 337686}, {"filename": "/tests/metron_good/namespaces.h", "start": 337686, "end": 338036}, {"filename": "/tests/metron_good/nested_structs.h", "start": 338036, "end": 338549}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 338549, "end": 339094}, {"filename": "/tests/metron_good/oneliners.h", "start": 339094, "end": 339357}, {"filename": "/tests/metron_good/plus_equals.h", "start": 339357, "end": 339707}, {"filename": "/tests/metron_good/private_getter.h", "start": 339707, "end": 339931}, {"filename": "/tests/metron_good/structs.h", "start": 339931, "end": 340150}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 340150, "end": 340686}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 340686, "end": 343047}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 343047, "end": 343801}, {"filename": "/tests/metron_good/tock_task.h", "start": 343801, "end": 344157}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 344157, "end": 344322}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 344322, "end": 344981}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 344981, "end": 345640}], "remote_package_size": 345640});

  })();
