
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
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "gb_spu", true, true);
Module['FS_createPath']("/examples/gb_spu", "metron", true, true);
Module['FS_createPath']("/examples", "rvtiny_sync", true, true);
Module['FS_createPath']("/examples/rvtiny_sync", "metron", true, true);
Module['FS_createPath']("/examples", "rvtiny", true, true);
Module['FS_createPath']("/examples/rvtiny", "metron", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/examples/uart", "metron", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples/j1", "metron", true, true);
Module['FS_createPath']("/", "tests", true, true);
Module['FS_createPath']("/tests", "metron_good", true, true);
Module['FS_createPath']("/tests", "metron_bad", true, true);

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
        assert(arrayBuffer instanceof ArrayBuffer, 'bad input to processPackageData');
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1263}, {"filename": "/examples/rvsimple/README.md", "start": 1263, "end": 1342}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 1342, "end": 3460}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 3460, "end": 9054}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 9054, "end": 14773}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 14773, "end": 15783}, {"filename": "/examples/rvsimple/metron/config.h", "start": 15783, "end": 16998}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 16998, "end": 17969}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 17969, "end": 18733}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 18733, "end": 21776}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 21776, "end": 22603}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 22603, "end": 23103}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23103, "end": 25041}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 25041, "end": 26194}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 26194, "end": 28815}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 28815, "end": 29506}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 29506, "end": 30967}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 30967, "end": 31643}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 31643, "end": 34136}, {"filename": "/examples/rvsimple/metron/register.h", "start": 34136, "end": 34821}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 34821, "end": 36669}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 36669, "end": 37783}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 37783, "end": 39017}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 39017, "end": 39890}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 39890, "end": 44617}, {"filename": "/examples/ibex/README.md", "start": 44617, "end": 44668}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 44668, "end": 59078}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 59078, "end": 61579}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 61579, "end": 77603}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 77603, "end": 79188}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 79188, "end": 91286}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 91286, "end": 92704}, {"filename": "/examples/tutorial/counter.h", "start": 92704, "end": 92853}, {"filename": "/examples/tutorial/tutorial2.h", "start": 92853, "end": 92921}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 92921, "end": 93457}, {"filename": "/examples/tutorial/tutorial5.h", "start": 93457, "end": 93498}, {"filename": "/examples/tutorial/declaration_order.h", "start": 93498, "end": 94277}, {"filename": "/examples/tutorial/nonblocking.h", "start": 94277, "end": 94399}, {"filename": "/examples/tutorial/tutorial4.h", "start": 94399, "end": 94440}, {"filename": "/examples/tutorial/adder.h", "start": 94440, "end": 94620}, {"filename": "/examples/tutorial/vga.h", "start": 94620, "end": 95767}, {"filename": "/examples/tutorial/tutorial3.h", "start": 95767, "end": 95808}, {"filename": "/examples/tutorial/checksum.h", "start": 95808, "end": 96531}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 96531, "end": 97512}, {"filename": "/examples/tutorial/templates.h", "start": 97512, "end": 97983}, {"filename": "/examples/tutorial/submodules.h", "start": 97983, "end": 99100}, {"filename": "/examples/tutorial/blockram.h", "start": 99100, "end": 99617}, {"filename": "/examples/gb_spu/regs.h", "start": 99617, "end": 119970}, {"filename": "/examples/gb_spu/Sch_ApuControl.h", "start": 119970, "end": 135946}, {"filename": "/examples/gb_spu/gates.h", "start": 135946, "end": 139437}, {"filename": "/examples/gb_spu/metron/gb_spu.h", "start": 139437, "end": 160228}, {"filename": "/examples/rvtiny_sync/README.md", "start": 160228, "end": 160280}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 160280, "end": 167235}, {"filename": "/examples/rvtiny/README.md", "start": 167235, "end": 167399}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 167399, "end": 173495}, {"filename": "/examples/pong/README.md", "start": 173495, "end": 173555}, {"filename": "/examples/pong/reference/README.md", "start": 173555, "end": 173615}, {"filename": "/examples/pong/metron/pong.h", "start": 173615, "end": 177479}, {"filename": "/examples/uart/README.md", "start": 177479, "end": 177723}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 177723, "end": 180742}, {"filename": "/examples/uart/metron/uart_top.h", "start": 180742, "end": 182506}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 182506, "end": 185086}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 185086, "end": 187647}, {"filename": "/examples/j1/metron/j1.h", "start": 187647, "end": 191672}, {"filename": "/examples/j1/metron/dpram.h", "start": 191672, "end": 192110}, {"filename": "/tests/metron_good/bit_casts.h", "start": 192110, "end": 198083}, {"filename": "/tests/metron_good/magic_comments.h", "start": 198083, "end": 198386}, {"filename": "/tests/metron_good/tock_task.h", "start": 198386, "end": 198742}, {"filename": "/tests/metron_good/structs.h", "start": 198742, "end": 198961}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 198961, "end": 199192}, {"filename": "/tests/metron_good/basic_increment.h", "start": 199192, "end": 199547}, {"filename": "/tests/metron_good/bit_concat.h", "start": 199547, "end": 199974}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 199974, "end": 200519}, {"filename": "/tests/metron_good/bit_dup.h", "start": 200519, "end": 201178}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 201178, "end": 202016}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 202016, "end": 202423}, {"filename": "/tests/metron_good/basic_param.h", "start": 202423, "end": 202682}, {"filename": "/tests/metron_good/init_chain.h", "start": 202682, "end": 203390}, {"filename": "/tests/metron_good/basic_function.h", "start": 203390, "end": 203669}, {"filename": "/tests/metron_good/all_func_types.h", "start": 203669, "end": 205234}, {"filename": "/tests/metron_good/plus_equals.h", "start": 205234, "end": 205584}, {"filename": "/tests/metron_good/basic_switch.h", "start": 205584, "end": 206061}, {"filename": "/tests/metron_good/dontcare.h", "start": 206061, "end": 206344}, {"filename": "/tests/metron_good/basic_literals.h", "start": 206344, "end": 206956}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 206956, "end": 207115}, {"filename": "/tests/metron_good/README.md", "start": 207115, "end": 207196}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 207196, "end": 207607}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 207607, "end": 207772}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 207772, "end": 208431}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 208431, "end": 208726}, {"filename": "/tests/metron_good/builtins.h", "start": 208726, "end": 209057}, {"filename": "/tests/metron_good/basic_template.h", "start": 209057, "end": 209488}, {"filename": "/tests/metron_good/defines.h", "start": 209488, "end": 209802}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 209802, "end": 210461}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 210461, "end": 211215}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 211215, "end": 211478}, {"filename": "/tests/metron_good/namespaces.h", "start": 211478, "end": 211828}, {"filename": "/tests/metron_good/include_guards.h", "start": 211828, "end": 212025}, {"filename": "/tests/metron_good/basic_task.h", "start": 212025, "end": 212359}, {"filename": "/tests/metron_good/enum_simple.h", "start": 212359, "end": 213727}, {"filename": "/tests/metron_good/oneliners.h", "start": 213727, "end": 213990}, {"filename": "/tests/metron_good/for_loops.h", "start": 213990, "end": 214310}, {"filename": "/tests/metron_good/private_getter.h", "start": 214310, "end": 214534}, {"filename": "/tests/metron_good/multi_tick.h", "start": 214534, "end": 214900}, {"filename": "/tests/metron_good/good_order.h", "start": 214900, "end": 215196}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 215196, "end": 215503}, {"filename": "/tests/metron_good/basic_output.h", "start": 215503, "end": 215764}, {"filename": "/tests/metron_good/basic_submod.h", "start": 215764, "end": 216053}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 216053, "end": 216607}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 216607, "end": 216787}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 216787, "end": 217163}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 217163, "end": 217477}, {"filename": "/tests/metron_good/input_signals.h", "start": 217477, "end": 218139}, {"filename": "/tests/metron_good/minimal.h", "start": 218139, "end": 218214}, {"filename": "/tests/metron_good/nested_structs.h", "start": 218214, "end": 218727}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 218727, "end": 219082}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 219082, "end": 219328}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 219328, "end": 219548}, {"filename": "/tests/metron_good/local_localparam.h", "start": 219548, "end": 219726}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 219726, "end": 220037}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 220037, "end": 220577}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 220577, "end": 220820}, {"filename": "/tests/metron_bad/README.md", "start": 220820, "end": 221017}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 221017, "end": 221612}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 221612, "end": 222058}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 222058, "end": 222198}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 222198, "end": 222458}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 222458, "end": 222706}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 222706, "end": 223002}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 223002, "end": 223509}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 223509, "end": 223919}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 223919, "end": 224191}], "remote_package_size": 224191});

  })();
