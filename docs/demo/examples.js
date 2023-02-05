
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/ibex/README.md", "start": 26876, "end": 26927}, {"filename": "/examples/ibex/main.cpp", "start": 26927, "end": 27068}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 27068, "end": 28653}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 28653, "end": 40751}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 40751, "end": 55161}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 55161, "end": 71185}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 71185, "end": 73686}, {"filename": "/examples/j1/main.cpp", "start": 73686, "end": 73805}, {"filename": "/examples/j1/metron/dpram.h", "start": 73805, "end": 74243}, {"filename": "/examples/j1/metron/j1.h", "start": 74243, "end": 78263}, {"filename": "/examples/pong/README.md", "start": 78263, "end": 78323}, {"filename": "/examples/pong/main.cpp", "start": 78323, "end": 80269}, {"filename": "/examples/pong/main_vl.cpp", "start": 80269, "end": 80420}, {"filename": "/examples/pong/metron/pong.h", "start": 80420, "end": 84284}, {"filename": "/examples/pong/reference/README.md", "start": 84284, "end": 84344}, {"filename": "/examples/rvsimple/README.md", "start": 84344, "end": 84423}, {"filename": "/examples/rvsimple/main.cpp", "start": 84423, "end": 87189}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 87189, "end": 90091}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 90091, "end": 93252}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 93252, "end": 93752}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 93752, "end": 95213}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 95213, "end": 97818}, {"filename": "/examples/rvsimple/metron/config.h", "start": 97818, "end": 99033}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 99033, "end": 104752}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 104752, "end": 105862}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 105862, "end": 107795}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 107795, "end": 109022}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 109022, "end": 110266}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 110266, "end": 110935}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 110935, "end": 111899}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 111899, "end": 114017}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 114017, "end": 114781}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 114781, "end": 115461}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 115461, "end": 116277}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 116277, "end": 117276}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 117276, "end": 118247}, {"filename": "/examples/rvsimple/metron/register.h", "start": 118247, "end": 118932}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 118932, "end": 121975}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 121975, "end": 127549}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 127549, "end": 130042}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 130042, "end": 134769}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 134769, "end": 136757}, {"filename": "/examples/scratch.h", "start": 136757, "end": 136986}, {"filename": "/examples/tutorial/adder.h", "start": 136986, "end": 137166}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 137166, "end": 138147}, {"filename": "/examples/tutorial/blockram.h", "start": 138147, "end": 138664}, {"filename": "/examples/tutorial/checksum.h", "start": 138664, "end": 139387}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 139387, "end": 139923}, {"filename": "/examples/tutorial/counter.h", "start": 139923, "end": 140072}, {"filename": "/examples/tutorial/declaration_order.h", "start": 140072, "end": 140851}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 140851, "end": 142269}, {"filename": "/examples/tutorial/nonblocking.h", "start": 142269, "end": 142391}, {"filename": "/examples/tutorial/submodules.h", "start": 142391, "end": 143508}, {"filename": "/examples/tutorial/templates.h", "start": 143508, "end": 143979}, {"filename": "/examples/tutorial/tutorial2.h", "start": 143979, "end": 144047}, {"filename": "/examples/tutorial/tutorial3.h", "start": 144047, "end": 144088}, {"filename": "/examples/tutorial/tutorial4.h", "start": 144088, "end": 144129}, {"filename": "/examples/tutorial/tutorial5.h", "start": 144129, "end": 144170}, {"filename": "/examples/tutorial/vga.h", "start": 144170, "end": 145317}, {"filename": "/examples/uart/README.md", "start": 145317, "end": 145561}, {"filename": "/examples/uart/main.cpp", "start": 145561, "end": 146860}, {"filename": "/examples/uart/main_vl.cpp", "start": 146860, "end": 149396}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 149396, "end": 151976}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 151976, "end": 154537}, {"filename": "/examples/uart/metron/uart_top.h", "start": 154537, "end": 156301}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 156301, "end": 159320}, {"filename": "/tests/metron_bad/README.md", "start": 159320, "end": 159517}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 159517, "end": 159813}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 159813, "end": 160061}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 160061, "end": 160304}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 160304, "end": 160899}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 160899, "end": 161126}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 161126, "end": 161536}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 161536, "end": 162076}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 162076, "end": 162387}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 162387, "end": 162833}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 162833, "end": 163093}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 163093, "end": 163365}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 163365, "end": 163872}, {"filename": "/tests/metron_good/README.md", "start": 163872, "end": 163953}, {"filename": "/tests/metron_good/all_func_types.h", "start": 163953, "end": 165612}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 165612, "end": 166019}, {"filename": "/tests/metron_good/basic_function.h", "start": 166019, "end": 166298}, {"filename": "/tests/metron_good/basic_increment.h", "start": 166298, "end": 166653}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 166653, "end": 166948}, {"filename": "/tests/metron_good/basic_literals.h", "start": 166948, "end": 167560}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 167560, "end": 167806}, {"filename": "/tests/metron_good/basic_output.h", "start": 167806, "end": 168067}, {"filename": "/tests/metron_good/basic_param.h", "start": 168067, "end": 168326}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 168326, "end": 168557}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 168557, "end": 168737}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 168737, "end": 169000}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 169000, "end": 169220}, {"filename": "/tests/metron_good/basic_submod.h", "start": 169220, "end": 169509}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 169509, "end": 169864}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 169864, "end": 170240}, {"filename": "/tests/metron_good/basic_switch.h", "start": 170240, "end": 170717}, {"filename": "/tests/metron_good/basic_task.h", "start": 170717, "end": 171051}, {"filename": "/tests/metron_good/basic_template.h", "start": 171051, "end": 171537}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 171537, "end": 171696}, {"filename": "/tests/metron_good/bit_casts.h", "start": 171696, "end": 177669}, {"filename": "/tests/metron_good/bit_concat.h", "start": 177669, "end": 178096}, {"filename": "/tests/metron_good/bit_dup.h", "start": 178096, "end": 178755}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 178755, "end": 179593}, {"filename": "/tests/metron_good/builtins.h", "start": 179593, "end": 179924}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 179924, "end": 180231}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 180231, "end": 180785}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 180785, "end": 181678}, {"filename": "/tests/metron_good/constructor_args.h", "start": 181678, "end": 182186}, {"filename": "/tests/metron_good/defines.h", "start": 182186, "end": 182500}, {"filename": "/tests/metron_good/dontcare.h", "start": 182500, "end": 182780}, {"filename": "/tests/metron_good/enum_simple.h", "start": 182780, "end": 184148}, {"filename": "/tests/metron_good/for_loops.h", "start": 184148, "end": 184468}, {"filename": "/tests/metron_good/good_order.h", "start": 184468, "end": 184764}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 184764, "end": 185175}, {"filename": "/tests/metron_good/include_guards.h", "start": 185175, "end": 185372}, {"filename": "/tests/metron_good/init_chain.h", "start": 185372, "end": 186080}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 186080, "end": 186367}, {"filename": "/tests/metron_good/input_signals.h", "start": 186367, "end": 187029}, {"filename": "/tests/metron_good/local_localparam.h", "start": 187029, "end": 187207}, {"filename": "/tests/metron_good/magic_comments.h", "start": 187207, "end": 187510}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 187510, "end": 187824}, {"filename": "/tests/metron_good/minimal.h", "start": 187824, "end": 187899}, {"filename": "/tests/metron_good/multi_tick.h", "start": 187899, "end": 188265}, {"filename": "/tests/metron_good/namespaces.h", "start": 188265, "end": 188615}, {"filename": "/tests/metron_good/nested_structs.h", "start": 188615, "end": 189030}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 189030, "end": 189575}, {"filename": "/tests/metron_good/oneliners.h", "start": 189575, "end": 189838}, {"filename": "/tests/metron_good/plus_equals.h", "start": 189838, "end": 190262}, {"filename": "/tests/metron_good/private_getter.h", "start": 190262, "end": 190486}, {"filename": "/tests/metron_good/structs.h", "start": 190486, "end": 190705}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 190705, "end": 191241}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 191241, "end": 193785}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 193785, "end": 194539}, {"filename": "/tests/metron_good/tock_task.h", "start": 194539, "end": 194895}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 194895, "end": 195060}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 195060, "end": 195719}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 195719, "end": 196378}], "remote_package_size": 196378});

  })();
