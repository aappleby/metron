
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/ibex/README.md", "start": 26876, "end": 26927}, {"filename": "/examples/ibex/main.cpp", "start": 26927, "end": 27068}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 27068, "end": 28653}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 28653, "end": 40751}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 40751, "end": 55161}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 55161, "end": 71185}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 71185, "end": 73686}, {"filename": "/examples/j1/main.cpp", "start": 73686, "end": 73805}, {"filename": "/examples/j1/metron/dpram.h", "start": 73805, "end": 74243}, {"filename": "/examples/j1/metron/j1.h", "start": 74243, "end": 78263}, {"filename": "/examples/pong/README.md", "start": 78263, "end": 78323}, {"filename": "/examples/pong/main.cpp", "start": 78323, "end": 80269}, {"filename": "/examples/pong/main_vl.cpp", "start": 80269, "end": 80420}, {"filename": "/examples/pong/metron/pong.h", "start": 80420, "end": 84284}, {"filename": "/examples/pong/reference/README.md", "start": 84284, "end": 84344}, {"filename": "/examples/rvsimple/README.md", "start": 84344, "end": 84423}, {"filename": "/examples/rvsimple/main.cpp", "start": 84423, "end": 87189}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 87189, "end": 90091}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 90091, "end": 93252}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 93252, "end": 93752}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 93752, "end": 95213}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 95213, "end": 97818}, {"filename": "/examples/rvsimple/metron/config.h", "start": 97818, "end": 99033}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 99033, "end": 104752}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 104752, "end": 105862}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 105862, "end": 107795}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 107795, "end": 109022}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 109022, "end": 110266}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 110266, "end": 110935}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 110935, "end": 111899}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 111899, "end": 114017}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 114017, "end": 114781}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 114781, "end": 115461}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 115461, "end": 116277}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 116277, "end": 117276}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 117276, "end": 118247}, {"filename": "/examples/rvsimple/metron/register.h", "start": 118247, "end": 118932}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 118932, "end": 121975}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 121975, "end": 127549}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 127549, "end": 130042}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 130042, "end": 134769}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 134769, "end": 136783}, {"filename": "/examples/scratch.h", "start": 136783, "end": 137043}, {"filename": "/examples/tutorial/adder.h", "start": 137043, "end": 137223}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 137223, "end": 138204}, {"filename": "/examples/tutorial/blockram.h", "start": 138204, "end": 138721}, {"filename": "/examples/tutorial/checksum.h", "start": 138721, "end": 139444}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 139444, "end": 139980}, {"filename": "/examples/tutorial/counter.h", "start": 139980, "end": 140129}, {"filename": "/examples/tutorial/declaration_order.h", "start": 140129, "end": 140908}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 140908, "end": 142326}, {"filename": "/examples/tutorial/nonblocking.h", "start": 142326, "end": 142448}, {"filename": "/examples/tutorial/submodules.h", "start": 142448, "end": 143565}, {"filename": "/examples/tutorial/templates.h", "start": 143565, "end": 144036}, {"filename": "/examples/tutorial/tutorial2.h", "start": 144036, "end": 144104}, {"filename": "/examples/tutorial/tutorial3.h", "start": 144104, "end": 144145}, {"filename": "/examples/tutorial/tutorial4.h", "start": 144145, "end": 144186}, {"filename": "/examples/tutorial/tutorial5.h", "start": 144186, "end": 144227}, {"filename": "/examples/tutorial/vga.h", "start": 144227, "end": 145374}, {"filename": "/examples/uart/README.md", "start": 145374, "end": 145618}, {"filename": "/examples/uart/main.cpp", "start": 145618, "end": 146917}, {"filename": "/examples/uart/main_vl.cpp", "start": 146917, "end": 149453}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 149453, "end": 152033}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 152033, "end": 154594}, {"filename": "/examples/uart/metron/uart_top.h", "start": 154594, "end": 156358}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 156358, "end": 159377}, {"filename": "/tests/metron_bad/README.md", "start": 159377, "end": 159574}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 159574, "end": 159870}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 159870, "end": 160118}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 160118, "end": 160361}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 160361, "end": 160956}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 160956, "end": 161366}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 161366, "end": 161906}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 161906, "end": 162217}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 162217, "end": 162663}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 162663, "end": 162923}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 162923, "end": 163195}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 163195, "end": 163702}, {"filename": "/tests/metron_good/README.md", "start": 163702, "end": 163783}, {"filename": "/tests/metron_good/all_func_types.h", "start": 163783, "end": 165348}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 165348, "end": 165755}, {"filename": "/tests/metron_good/basic_function.h", "start": 165755, "end": 166034}, {"filename": "/tests/metron_good/basic_increment.h", "start": 166034, "end": 166389}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 166389, "end": 166684}, {"filename": "/tests/metron_good/basic_literals.h", "start": 166684, "end": 167296}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 167296, "end": 167542}, {"filename": "/tests/metron_good/basic_output.h", "start": 167542, "end": 167803}, {"filename": "/tests/metron_good/basic_param.h", "start": 167803, "end": 168062}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 168062, "end": 168293}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 168293, "end": 168473}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 168473, "end": 168736}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 168736, "end": 168956}, {"filename": "/tests/metron_good/basic_submod.h", "start": 168956, "end": 169245}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 169245, "end": 169600}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 169600, "end": 169976}, {"filename": "/tests/metron_good/basic_switch.h", "start": 169976, "end": 170453}, {"filename": "/tests/metron_good/basic_task.h", "start": 170453, "end": 170787}, {"filename": "/tests/metron_good/basic_template.h", "start": 170787, "end": 171273}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 171273, "end": 171432}, {"filename": "/tests/metron_good/bit_casts.h", "start": 171432, "end": 177405}, {"filename": "/tests/metron_good/bit_concat.h", "start": 177405, "end": 177832}, {"filename": "/tests/metron_good/bit_dup.h", "start": 177832, "end": 178491}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 178491, "end": 179329}, {"filename": "/tests/metron_good/builtins.h", "start": 179329, "end": 179660}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 179660, "end": 179967}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 179967, "end": 180521}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 180521, "end": 181414}, {"filename": "/tests/metron_good/constructor_args.h", "start": 181414, "end": 181922}, {"filename": "/tests/metron_good/defines.h", "start": 181922, "end": 182236}, {"filename": "/tests/metron_good/dontcare.h", "start": 182236, "end": 182516}, {"filename": "/tests/metron_good/enum_simple.h", "start": 182516, "end": 183884}, {"filename": "/tests/metron_good/for_loops.h", "start": 183884, "end": 184204}, {"filename": "/tests/metron_good/good_order.h", "start": 184204, "end": 184500}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 184500, "end": 184911}, {"filename": "/tests/metron_good/include_guards.h", "start": 184911, "end": 185108}, {"filename": "/tests/metron_good/init_chain.h", "start": 185108, "end": 185816}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 185816, "end": 186103}, {"filename": "/tests/metron_good/input_signals.h", "start": 186103, "end": 186765}, {"filename": "/tests/metron_good/local_localparam.h", "start": 186765, "end": 186943}, {"filename": "/tests/metron_good/magic_comments.h", "start": 186943, "end": 187246}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 187246, "end": 187560}, {"filename": "/tests/metron_good/minimal.h", "start": 187560, "end": 187635}, {"filename": "/tests/metron_good/multi_tick.h", "start": 187635, "end": 188001}, {"filename": "/tests/metron_good/namespaces.h", "start": 188001, "end": 188351}, {"filename": "/tests/metron_good/nested_structs.h", "start": 188351, "end": 188864}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 188864, "end": 189409}, {"filename": "/tests/metron_good/oneliners.h", "start": 189409, "end": 189672}, {"filename": "/tests/metron_good/plus_equals.h", "start": 189672, "end": 190096}, {"filename": "/tests/metron_good/private_getter.h", "start": 190096, "end": 190320}, {"filename": "/tests/metron_good/structs.h", "start": 190320, "end": 190539}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 190539, "end": 191075}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 191075, "end": 193426}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 193426, "end": 194180}, {"filename": "/tests/metron_good/tock_task.h", "start": 194180, "end": 194536}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 194536, "end": 194701}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 194701, "end": 195360}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 195360, "end": 196019}], "remote_package_size": 196019});

  })();
