
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
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples/j1", "metron", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/ibex/README.md", "start": 26876, "end": 26927}, {"filename": "/examples/ibex/main.cpp", "start": 26927, "end": 27068}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 27068, "end": 28653}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 28653, "end": 40751}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 40751, "end": 55161}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 55161, "end": 71185}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 71185, "end": 73686}, {"filename": "/examples/j1/main.cpp", "start": 73686, "end": 73805}, {"filename": "/examples/j1/metron/dpram.h", "start": 73805, "end": 74243}, {"filename": "/examples/j1/metron/j1.h", "start": 74243, "end": 78263}, {"filename": "/examples/pong/README.md", "start": 78263, "end": 78323}, {"filename": "/examples/pong/main.cpp", "start": 78323, "end": 80269}, {"filename": "/examples/pong/main_vl.cpp", "start": 80269, "end": 80420}, {"filename": "/examples/pong/metron/pong.h", "start": 80420, "end": 84284}, {"filename": "/examples/pong/reference/README.md", "start": 84284, "end": 84344}, {"filename": "/examples/rvsimple/README.md", "start": 84344, "end": 84423}, {"filename": "/examples/rvsimple/main.cpp", "start": 84423, "end": 87189}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 87189, "end": 90091}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 90091, "end": 93252}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 93252, "end": 93752}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 93752, "end": 95213}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 95213, "end": 97818}, {"filename": "/examples/rvsimple/metron/config.h", "start": 97818, "end": 99033}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 99033, "end": 104752}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 104752, "end": 105862}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 105862, "end": 107795}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 107795, "end": 109022}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 109022, "end": 110266}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 110266, "end": 110935}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 110935, "end": 111899}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 111899, "end": 114017}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 114017, "end": 114781}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 114781, "end": 115461}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 115461, "end": 116277}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 116277, "end": 117276}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 117276, "end": 118247}, {"filename": "/examples/rvsimple/metron/register.h", "start": 118247, "end": 118932}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 118932, "end": 121975}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 121975, "end": 127549}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 127549, "end": 130042}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 130042, "end": 134769}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 134769, "end": 136757}, {"filename": "/examples/scratch.h", "start": 136757, "end": 137282}, {"filename": "/examples/tutorial/adder.h", "start": 137282, "end": 137462}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 137462, "end": 138443}, {"filename": "/examples/tutorial/blockram.h", "start": 138443, "end": 138960}, {"filename": "/examples/tutorial/checksum.h", "start": 138960, "end": 139683}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 139683, "end": 140219}, {"filename": "/examples/tutorial/counter.h", "start": 140219, "end": 140368}, {"filename": "/examples/tutorial/declaration_order.h", "start": 140368, "end": 141147}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 141147, "end": 142565}, {"filename": "/examples/tutorial/nonblocking.h", "start": 142565, "end": 142687}, {"filename": "/examples/tutorial/submodules.h", "start": 142687, "end": 143804}, {"filename": "/examples/tutorial/templates.h", "start": 143804, "end": 144275}, {"filename": "/examples/tutorial/tutorial2.h", "start": 144275, "end": 144343}, {"filename": "/examples/tutorial/tutorial3.h", "start": 144343, "end": 144384}, {"filename": "/examples/tutorial/tutorial4.h", "start": 144384, "end": 144425}, {"filename": "/examples/tutorial/tutorial5.h", "start": 144425, "end": 144466}, {"filename": "/examples/tutorial/vga.h", "start": 144466, "end": 145613}, {"filename": "/examples/uart/README.md", "start": 145613, "end": 145857}, {"filename": "/examples/uart/main.cpp", "start": 145857, "end": 147156}, {"filename": "/examples/uart/main_vl.cpp", "start": 147156, "end": 149692}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 149692, "end": 152272}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 152272, "end": 154833}, {"filename": "/examples/uart/metron/uart_top.h", "start": 154833, "end": 156597}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 156597, "end": 159616}, {"filename": "/tests/metron_bad/README.md", "start": 159616, "end": 159813}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 159813, "end": 160109}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 160109, "end": 160357}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 160357, "end": 160600}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 160600, "end": 161195}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 161195, "end": 161605}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 161605, "end": 162145}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 162145, "end": 162456}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 162456, "end": 162902}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 162902, "end": 163162}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 163162, "end": 163434}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 163434, "end": 163941}, {"filename": "/tests/metron_good/README.md", "start": 163941, "end": 164022}, {"filename": "/tests/metron_good/all_func_types.h", "start": 164022, "end": 165587}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 165587, "end": 165994}, {"filename": "/tests/metron_good/basic_function.h", "start": 165994, "end": 166273}, {"filename": "/tests/metron_good/basic_increment.h", "start": 166273, "end": 166628}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 166628, "end": 166923}, {"filename": "/tests/metron_good/basic_literals.h", "start": 166923, "end": 167535}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 167535, "end": 167781}, {"filename": "/tests/metron_good/basic_output.h", "start": 167781, "end": 168042}, {"filename": "/tests/metron_good/basic_param.h", "start": 168042, "end": 168301}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 168301, "end": 168532}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 168532, "end": 168712}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 168712, "end": 168975}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 168975, "end": 169195}, {"filename": "/tests/metron_good/basic_submod.h", "start": 169195, "end": 169484}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 169484, "end": 169839}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 169839, "end": 170215}, {"filename": "/tests/metron_good/basic_switch.h", "start": 170215, "end": 170692}, {"filename": "/tests/metron_good/basic_task.h", "start": 170692, "end": 171026}, {"filename": "/tests/metron_good/basic_template.h", "start": 171026, "end": 171512}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 171512, "end": 171671}, {"filename": "/tests/metron_good/bit_casts.h", "start": 171671, "end": 177644}, {"filename": "/tests/metron_good/bit_concat.h", "start": 177644, "end": 178071}, {"filename": "/tests/metron_good/bit_dup.h", "start": 178071, "end": 178730}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 178730, "end": 179568}, {"filename": "/tests/metron_good/builtins.h", "start": 179568, "end": 179899}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 179899, "end": 180206}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 180206, "end": 180760}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 180760, "end": 181653}, {"filename": "/tests/metron_good/constructor_args.h", "start": 181653, "end": 182161}, {"filename": "/tests/metron_good/defines.h", "start": 182161, "end": 182475}, {"filename": "/tests/metron_good/dontcare.h", "start": 182475, "end": 182755}, {"filename": "/tests/metron_good/enum_simple.h", "start": 182755, "end": 184123}, {"filename": "/tests/metron_good/for_loops.h", "start": 184123, "end": 184443}, {"filename": "/tests/metron_good/good_order.h", "start": 184443, "end": 184739}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 184739, "end": 185150}, {"filename": "/tests/metron_good/include_guards.h", "start": 185150, "end": 185347}, {"filename": "/tests/metron_good/init_chain.h", "start": 185347, "end": 186055}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 186055, "end": 186342}, {"filename": "/tests/metron_good/input_signals.h", "start": 186342, "end": 187004}, {"filename": "/tests/metron_good/local_localparam.h", "start": 187004, "end": 187182}, {"filename": "/tests/metron_good/magic_comments.h", "start": 187182, "end": 187485}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 187485, "end": 187799}, {"filename": "/tests/metron_good/minimal.h", "start": 187799, "end": 187874}, {"filename": "/tests/metron_good/multi_tick.h", "start": 187874, "end": 188240}, {"filename": "/tests/metron_good/namespaces.h", "start": 188240, "end": 188590}, {"filename": "/tests/metron_good/nested_structs.h", "start": 188590, "end": 189103}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 189103, "end": 189648}, {"filename": "/tests/metron_good/oneliners.h", "start": 189648, "end": 189911}, {"filename": "/tests/metron_good/plus_equals.h", "start": 189911, "end": 190335}, {"filename": "/tests/metron_good/private_getter.h", "start": 190335, "end": 190559}, {"filename": "/tests/metron_good/structs.h", "start": 190559, "end": 190778}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 190778, "end": 191314}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 191314, "end": 193665}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 193665, "end": 194419}, {"filename": "/tests/metron_good/tock_task.h", "start": 194419, "end": 194775}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 194775, "end": 194940}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 194940, "end": 195599}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 195599, "end": 196258}], "remote_package_size": 196258});

  })();
