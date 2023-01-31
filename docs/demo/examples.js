
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/ibex/README.md", "start": 26876, "end": 26927}, {"filename": "/examples/ibex/main.cpp", "start": 26927, "end": 27068}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 27068, "end": 28653}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 28653, "end": 40751}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 40751, "end": 55161}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 55161, "end": 71185}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 71185, "end": 73686}, {"filename": "/examples/j1/main.cpp", "start": 73686, "end": 73805}, {"filename": "/examples/j1/metron/dpram.h", "start": 73805, "end": 74243}, {"filename": "/examples/j1/metron/j1.h", "start": 74243, "end": 78263}, {"filename": "/examples/pong/README.md", "start": 78263, "end": 78323}, {"filename": "/examples/pong/main.cpp", "start": 78323, "end": 80269}, {"filename": "/examples/pong/main_vl.cpp", "start": 80269, "end": 80420}, {"filename": "/examples/pong/metron/pong.h", "start": 80420, "end": 84284}, {"filename": "/examples/pong/reference/README.md", "start": 84284, "end": 84344}, {"filename": "/examples/rvsimple/README.md", "start": 84344, "end": 84423}, {"filename": "/examples/rvsimple/main.cpp", "start": 84423, "end": 87189}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 87189, "end": 90091}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 90091, "end": 93252}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 93252, "end": 93752}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 93752, "end": 95213}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 95213, "end": 97818}, {"filename": "/examples/rvsimple/metron/config.h", "start": 97818, "end": 99033}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 99033, "end": 104752}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 104752, "end": 105862}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 105862, "end": 107795}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 107795, "end": 109022}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 109022, "end": 110266}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 110266, "end": 110935}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 110935, "end": 111899}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 111899, "end": 114017}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 114017, "end": 114781}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 114781, "end": 115461}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 115461, "end": 116277}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 116277, "end": 117276}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 117276, "end": 118247}, {"filename": "/examples/rvsimple/metron/register.h", "start": 118247, "end": 118932}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 118932, "end": 121975}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 121975, "end": 127549}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 127549, "end": 130042}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 130042, "end": 134769}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 134769, "end": 136757}, {"filename": "/examples/scratch.h", "start": 136757, "end": 137263}, {"filename": "/examples/tutorial/adder.h", "start": 137263, "end": 137443}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 137443, "end": 138424}, {"filename": "/examples/tutorial/blockram.h", "start": 138424, "end": 138941}, {"filename": "/examples/tutorial/checksum.h", "start": 138941, "end": 139664}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 139664, "end": 140200}, {"filename": "/examples/tutorial/counter.h", "start": 140200, "end": 140349}, {"filename": "/examples/tutorial/declaration_order.h", "start": 140349, "end": 141128}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 141128, "end": 142546}, {"filename": "/examples/tutorial/nonblocking.h", "start": 142546, "end": 142668}, {"filename": "/examples/tutorial/submodules.h", "start": 142668, "end": 143785}, {"filename": "/examples/tutorial/templates.h", "start": 143785, "end": 144256}, {"filename": "/examples/tutorial/tutorial2.h", "start": 144256, "end": 144324}, {"filename": "/examples/tutorial/tutorial3.h", "start": 144324, "end": 144365}, {"filename": "/examples/tutorial/tutorial4.h", "start": 144365, "end": 144406}, {"filename": "/examples/tutorial/tutorial5.h", "start": 144406, "end": 144447}, {"filename": "/examples/tutorial/vga.h", "start": 144447, "end": 145594}, {"filename": "/examples/uart/README.md", "start": 145594, "end": 145838}, {"filename": "/examples/uart/main.cpp", "start": 145838, "end": 147137}, {"filename": "/examples/uart/main_vl.cpp", "start": 147137, "end": 149673}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 149673, "end": 152253}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 152253, "end": 154814}, {"filename": "/examples/uart/metron/uart_top.h", "start": 154814, "end": 156578}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 156578, "end": 159597}, {"filename": "/tests/metron_bad/README.md", "start": 159597, "end": 159794}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 159794, "end": 160090}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 160090, "end": 160338}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 160338, "end": 160581}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 160581, "end": 161176}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 161176, "end": 161586}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 161586, "end": 162126}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 162126, "end": 162437}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 162437, "end": 162883}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 162883, "end": 163143}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 163143, "end": 163415}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 163415, "end": 163922}, {"filename": "/tests/metron_good/README.md", "start": 163922, "end": 164003}, {"filename": "/tests/metron_good/all_func_types.h", "start": 164003, "end": 165568}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 165568, "end": 165975}, {"filename": "/tests/metron_good/basic_function.h", "start": 165975, "end": 166254}, {"filename": "/tests/metron_good/basic_increment.h", "start": 166254, "end": 166609}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 166609, "end": 166904}, {"filename": "/tests/metron_good/basic_literals.h", "start": 166904, "end": 167516}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 167516, "end": 167762}, {"filename": "/tests/metron_good/basic_output.h", "start": 167762, "end": 168023}, {"filename": "/tests/metron_good/basic_param.h", "start": 168023, "end": 168282}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 168282, "end": 168513}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 168513, "end": 168693}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 168693, "end": 168956}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 168956, "end": 169176}, {"filename": "/tests/metron_good/basic_submod.h", "start": 169176, "end": 169465}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 169465, "end": 169820}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 169820, "end": 170196}, {"filename": "/tests/metron_good/basic_switch.h", "start": 170196, "end": 170673}, {"filename": "/tests/metron_good/basic_task.h", "start": 170673, "end": 171007}, {"filename": "/tests/metron_good/basic_template.h", "start": 171007, "end": 171493}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 171493, "end": 171652}, {"filename": "/tests/metron_good/bit_casts.h", "start": 171652, "end": 177625}, {"filename": "/tests/metron_good/bit_concat.h", "start": 177625, "end": 178052}, {"filename": "/tests/metron_good/bit_dup.h", "start": 178052, "end": 178711}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 178711, "end": 179549}, {"filename": "/tests/metron_good/builtins.h", "start": 179549, "end": 179880}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 179880, "end": 180187}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 180187, "end": 180741}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 180741, "end": 181634}, {"filename": "/tests/metron_good/constructor_args.h", "start": 181634, "end": 182142}, {"filename": "/tests/metron_good/defines.h", "start": 182142, "end": 182456}, {"filename": "/tests/metron_good/dontcare.h", "start": 182456, "end": 182736}, {"filename": "/tests/metron_good/enum_simple.h", "start": 182736, "end": 184104}, {"filename": "/tests/metron_good/for_loops.h", "start": 184104, "end": 184424}, {"filename": "/tests/metron_good/good_order.h", "start": 184424, "end": 184720}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 184720, "end": 185131}, {"filename": "/tests/metron_good/include_guards.h", "start": 185131, "end": 185328}, {"filename": "/tests/metron_good/init_chain.h", "start": 185328, "end": 186036}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 186036, "end": 186323}, {"filename": "/tests/metron_good/input_signals.h", "start": 186323, "end": 186985}, {"filename": "/tests/metron_good/local_localparam.h", "start": 186985, "end": 187163}, {"filename": "/tests/metron_good/magic_comments.h", "start": 187163, "end": 187466}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 187466, "end": 187780}, {"filename": "/tests/metron_good/minimal.h", "start": 187780, "end": 187855}, {"filename": "/tests/metron_good/multi_tick.h", "start": 187855, "end": 188221}, {"filename": "/tests/metron_good/namespaces.h", "start": 188221, "end": 188571}, {"filename": "/tests/metron_good/nested_structs.h", "start": 188571, "end": 189084}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 189084, "end": 189629}, {"filename": "/tests/metron_good/oneliners.h", "start": 189629, "end": 189892}, {"filename": "/tests/metron_good/plus_equals.h", "start": 189892, "end": 190316}, {"filename": "/tests/metron_good/private_getter.h", "start": 190316, "end": 190540}, {"filename": "/tests/metron_good/structs.h", "start": 190540, "end": 190759}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 190759, "end": 191295}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 191295, "end": 193646}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 193646, "end": 194400}, {"filename": "/tests/metron_good/tock_task.h", "start": 194400, "end": 194756}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 194756, "end": 194921}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 194921, "end": 195580}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 195580, "end": 196239}], "remote_package_size": 196239});

  })();
