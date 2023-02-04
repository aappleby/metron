
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/ibex/README.md", "start": 26876, "end": 26927}, {"filename": "/examples/ibex/main.cpp", "start": 26927, "end": 27068}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 27068, "end": 28653}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 28653, "end": 40751}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 40751, "end": 55161}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 55161, "end": 71185}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 71185, "end": 73686}, {"filename": "/examples/j1/main.cpp", "start": 73686, "end": 73805}, {"filename": "/examples/j1/metron/dpram.h", "start": 73805, "end": 74243}, {"filename": "/examples/j1/metron/j1.h", "start": 74243, "end": 78263}, {"filename": "/examples/pong/README.md", "start": 78263, "end": 78323}, {"filename": "/examples/pong/main.cpp", "start": 78323, "end": 80269}, {"filename": "/examples/pong/main_vl.cpp", "start": 80269, "end": 80420}, {"filename": "/examples/pong/metron/pong.h", "start": 80420, "end": 84284}, {"filename": "/examples/pong/reference/README.md", "start": 84284, "end": 84344}, {"filename": "/examples/rvsimple/README.md", "start": 84344, "end": 84423}, {"filename": "/examples/rvsimple/main.cpp", "start": 84423, "end": 87189}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 87189, "end": 90091}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 90091, "end": 93252}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 93252, "end": 93752}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 93752, "end": 95213}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 95213, "end": 97818}, {"filename": "/examples/rvsimple/metron/config.h", "start": 97818, "end": 99033}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 99033, "end": 104752}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 104752, "end": 105862}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 105862, "end": 107795}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 107795, "end": 109022}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 109022, "end": 110266}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 110266, "end": 110935}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 110935, "end": 111899}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 111899, "end": 114017}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 114017, "end": 114781}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 114781, "end": 115461}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 115461, "end": 116277}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 116277, "end": 117276}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 117276, "end": 118247}, {"filename": "/examples/rvsimple/metron/register.h", "start": 118247, "end": 118932}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 118932, "end": 121975}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 121975, "end": 127549}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 127549, "end": 130042}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 130042, "end": 134769}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 134769, "end": 136757}, {"filename": "/examples/scratch.h", "start": 136757, "end": 137065}, {"filename": "/examples/tutorial/adder.h", "start": 137065, "end": 137245}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 137245, "end": 138226}, {"filename": "/examples/tutorial/blockram.h", "start": 138226, "end": 138743}, {"filename": "/examples/tutorial/checksum.h", "start": 138743, "end": 139466}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 139466, "end": 140002}, {"filename": "/examples/tutorial/counter.h", "start": 140002, "end": 140151}, {"filename": "/examples/tutorial/declaration_order.h", "start": 140151, "end": 140930}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 140930, "end": 142348}, {"filename": "/examples/tutorial/nonblocking.h", "start": 142348, "end": 142470}, {"filename": "/examples/tutorial/submodules.h", "start": 142470, "end": 143587}, {"filename": "/examples/tutorial/templates.h", "start": 143587, "end": 144058}, {"filename": "/examples/tutorial/tutorial2.h", "start": 144058, "end": 144126}, {"filename": "/examples/tutorial/tutorial3.h", "start": 144126, "end": 144167}, {"filename": "/examples/tutorial/tutorial4.h", "start": 144167, "end": 144208}, {"filename": "/examples/tutorial/tutorial5.h", "start": 144208, "end": 144249}, {"filename": "/examples/tutorial/vga.h", "start": 144249, "end": 145396}, {"filename": "/examples/uart/README.md", "start": 145396, "end": 145640}, {"filename": "/examples/uart/main.cpp", "start": 145640, "end": 146939}, {"filename": "/examples/uart/main_vl.cpp", "start": 146939, "end": 149475}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 149475, "end": 152055}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 152055, "end": 154616}, {"filename": "/examples/uart/metron/uart_top.h", "start": 154616, "end": 156380}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 156380, "end": 159399}, {"filename": "/tests/metron_bad/README.md", "start": 159399, "end": 159596}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 159596, "end": 159892}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 159892, "end": 160140}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 160140, "end": 160383}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 160383, "end": 160978}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 160978, "end": 161205}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 161205, "end": 161615}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 161615, "end": 162155}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 162155, "end": 162466}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 162466, "end": 162912}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 162912, "end": 163172}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 163172, "end": 163444}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 163444, "end": 163951}, {"filename": "/tests/metron_good/README.md", "start": 163951, "end": 164032}, {"filename": "/tests/metron_good/all_func_types.h", "start": 164032, "end": 165691}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 165691, "end": 166098}, {"filename": "/tests/metron_good/basic_function.h", "start": 166098, "end": 166377}, {"filename": "/tests/metron_good/basic_increment.h", "start": 166377, "end": 166732}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 166732, "end": 167027}, {"filename": "/tests/metron_good/basic_literals.h", "start": 167027, "end": 167639}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 167639, "end": 167885}, {"filename": "/tests/metron_good/basic_output.h", "start": 167885, "end": 168146}, {"filename": "/tests/metron_good/basic_param.h", "start": 168146, "end": 168405}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 168405, "end": 168636}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 168636, "end": 168816}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 168816, "end": 169079}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 169079, "end": 169299}, {"filename": "/tests/metron_good/basic_submod.h", "start": 169299, "end": 169588}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 169588, "end": 169943}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 169943, "end": 170319}, {"filename": "/tests/metron_good/basic_switch.h", "start": 170319, "end": 170796}, {"filename": "/tests/metron_good/basic_task.h", "start": 170796, "end": 171130}, {"filename": "/tests/metron_good/basic_template.h", "start": 171130, "end": 171616}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 171616, "end": 171775}, {"filename": "/tests/metron_good/bit_casts.h", "start": 171775, "end": 177748}, {"filename": "/tests/metron_good/bit_concat.h", "start": 177748, "end": 178175}, {"filename": "/tests/metron_good/bit_dup.h", "start": 178175, "end": 178834}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 178834, "end": 179672}, {"filename": "/tests/metron_good/builtins.h", "start": 179672, "end": 180003}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 180003, "end": 180310}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 180310, "end": 180864}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 180864, "end": 181757}, {"filename": "/tests/metron_good/constructor_args.h", "start": 181757, "end": 182265}, {"filename": "/tests/metron_good/defines.h", "start": 182265, "end": 182579}, {"filename": "/tests/metron_good/dontcare.h", "start": 182579, "end": 182859}, {"filename": "/tests/metron_good/enum_simple.h", "start": 182859, "end": 184227}, {"filename": "/tests/metron_good/for_loops.h", "start": 184227, "end": 184547}, {"filename": "/tests/metron_good/good_order.h", "start": 184547, "end": 184843}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 184843, "end": 185254}, {"filename": "/tests/metron_good/include_guards.h", "start": 185254, "end": 185451}, {"filename": "/tests/metron_good/init_chain.h", "start": 185451, "end": 186159}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 186159, "end": 186446}, {"filename": "/tests/metron_good/input_signals.h", "start": 186446, "end": 187108}, {"filename": "/tests/metron_good/local_localparam.h", "start": 187108, "end": 187286}, {"filename": "/tests/metron_good/magic_comments.h", "start": 187286, "end": 187589}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 187589, "end": 187903}, {"filename": "/tests/metron_good/minimal.h", "start": 187903, "end": 187978}, {"filename": "/tests/metron_good/multi_tick.h", "start": 187978, "end": 188344}, {"filename": "/tests/metron_good/namespaces.h", "start": 188344, "end": 188694}, {"filename": "/tests/metron_good/nested_structs.h", "start": 188694, "end": 189109}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 189109, "end": 189654}, {"filename": "/tests/metron_good/oneliners.h", "start": 189654, "end": 189917}, {"filename": "/tests/metron_good/plus_equals.h", "start": 189917, "end": 190341}, {"filename": "/tests/metron_good/private_getter.h", "start": 190341, "end": 190565}, {"filename": "/tests/metron_good/structs.h", "start": 190565, "end": 190784}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 190784, "end": 191320}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 191320, "end": 193864}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 193864, "end": 194618}, {"filename": "/tests/metron_good/tock_task.h", "start": 194618, "end": 194974}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 194974, "end": 195139}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 195139, "end": 195798}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 195798, "end": 196457}], "remote_package_size": 196457});

  })();
