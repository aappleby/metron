
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1263}, {"filename": "/examples/rvsimple/README.md", "start": 1263, "end": 1342}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 1342, "end": 3460}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 3460, "end": 9054}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 9054, "end": 14773}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 14773, "end": 15783}, {"filename": "/examples/rvsimple/metron/config.h", "start": 15783, "end": 16998}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 16998, "end": 17969}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 17969, "end": 18733}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 18733, "end": 21776}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 21776, "end": 22603}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 22603, "end": 23103}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23103, "end": 25041}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 25041, "end": 26194}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 26194, "end": 28815}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 28815, "end": 29506}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 29506, "end": 30967}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 30967, "end": 31643}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 31643, "end": 34136}, {"filename": "/examples/rvsimple/metron/register.h", "start": 34136, "end": 34821}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 34821, "end": 36669}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 36669, "end": 37783}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 37783, "end": 39017}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 39017, "end": 39890}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 39890, "end": 44617}, {"filename": "/examples/ibex/README.md", "start": 44617, "end": 44668}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 44668, "end": 59078}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 59078, "end": 61579}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 61579, "end": 77603}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 77603, "end": 79188}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 79188, "end": 91286}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 91286, "end": 92704}, {"filename": "/examples/tutorial/counter.h", "start": 92704, "end": 92853}, {"filename": "/examples/tutorial/tutorial2.h", "start": 92853, "end": 92921}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 92921, "end": 93457}, {"filename": "/examples/tutorial/tutorial5.h", "start": 93457, "end": 93498}, {"filename": "/examples/tutorial/declaration_order.h", "start": 93498, "end": 94277}, {"filename": "/examples/tutorial/nonblocking.h", "start": 94277, "end": 94399}, {"filename": "/examples/tutorial/tutorial4.h", "start": 94399, "end": 94440}, {"filename": "/examples/tutorial/adder.h", "start": 94440, "end": 94620}, {"filename": "/examples/tutorial/vga.h", "start": 94620, "end": 95767}, {"filename": "/examples/tutorial/tutorial3.h", "start": 95767, "end": 95808}, {"filename": "/examples/tutorial/checksum.h", "start": 95808, "end": 96531}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 96531, "end": 97512}, {"filename": "/examples/tutorial/templates.h", "start": 97512, "end": 97983}, {"filename": "/examples/tutorial/submodules.h", "start": 97983, "end": 99100}, {"filename": "/examples/tutorial/blockram.h", "start": 99100, "end": 99617}, {"filename": "/examples/gb_spu/regs.h", "start": 99617, "end": 119970}, {"filename": "/examples/gb_spu/Sch_ApuControl.h", "start": 119970, "end": 148309}, {"filename": "/examples/gb_spu/gates.h", "start": 148309, "end": 151800}, {"filename": "/examples/gb_spu/metron/gb_spu.h", "start": 151800, "end": 172591}, {"filename": "/examples/rvtiny_sync/README.md", "start": 172591, "end": 172643}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 172643, "end": 179598}, {"filename": "/examples/rvtiny/README.md", "start": 179598, "end": 179762}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 179762, "end": 185858}, {"filename": "/examples/pong/README.md", "start": 185858, "end": 185918}, {"filename": "/examples/pong/reference/README.md", "start": 185918, "end": 185978}, {"filename": "/examples/pong/metron/pong.h", "start": 185978, "end": 189842}, {"filename": "/examples/uart/README.md", "start": 189842, "end": 190086}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 190086, "end": 193105}, {"filename": "/examples/uart/metron/uart_top.h", "start": 193105, "end": 194869}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 194869, "end": 197449}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 197449, "end": 200010}, {"filename": "/examples/j1/metron/j1.h", "start": 200010, "end": 204035}, {"filename": "/examples/j1/metron/dpram.h", "start": 204035, "end": 204473}, {"filename": "/tests/metron_good/bit_casts.h", "start": 204473, "end": 210446}, {"filename": "/tests/metron_good/magic_comments.h", "start": 210446, "end": 210749}, {"filename": "/tests/metron_good/tock_task.h", "start": 210749, "end": 211105}, {"filename": "/tests/metron_good/structs.h", "start": 211105, "end": 211324}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 211324, "end": 211555}, {"filename": "/tests/metron_good/basic_increment.h", "start": 211555, "end": 211910}, {"filename": "/tests/metron_good/bit_concat.h", "start": 211910, "end": 212337}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 212337, "end": 212882}, {"filename": "/tests/metron_good/bit_dup.h", "start": 212882, "end": 213541}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 213541, "end": 214379}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 214379, "end": 214786}, {"filename": "/tests/metron_good/basic_param.h", "start": 214786, "end": 215045}, {"filename": "/tests/metron_good/init_chain.h", "start": 215045, "end": 215753}, {"filename": "/tests/metron_good/basic_function.h", "start": 215753, "end": 216032}, {"filename": "/tests/metron_good/all_func_types.h", "start": 216032, "end": 217597}, {"filename": "/tests/metron_good/plus_equals.h", "start": 217597, "end": 217947}, {"filename": "/tests/metron_good/basic_switch.h", "start": 217947, "end": 218424}, {"filename": "/tests/metron_good/dontcare.h", "start": 218424, "end": 218707}, {"filename": "/tests/metron_good/basic_literals.h", "start": 218707, "end": 219319}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 219319, "end": 219478}, {"filename": "/tests/metron_good/README.md", "start": 219478, "end": 219559}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 219559, "end": 219970}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 219970, "end": 220135}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 220135, "end": 220794}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 220794, "end": 221089}, {"filename": "/tests/metron_good/builtins.h", "start": 221089, "end": 221420}, {"filename": "/tests/metron_good/basic_template.h", "start": 221420, "end": 221851}, {"filename": "/tests/metron_good/defines.h", "start": 221851, "end": 222165}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 222165, "end": 222824}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 222824, "end": 223578}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 223578, "end": 223841}, {"filename": "/tests/metron_good/namespaces.h", "start": 223841, "end": 224191}, {"filename": "/tests/metron_good/include_guards.h", "start": 224191, "end": 224388}, {"filename": "/tests/metron_good/basic_task.h", "start": 224388, "end": 224722}, {"filename": "/tests/metron_good/enum_simple.h", "start": 224722, "end": 226090}, {"filename": "/tests/metron_good/oneliners.h", "start": 226090, "end": 226353}, {"filename": "/tests/metron_good/for_loops.h", "start": 226353, "end": 226673}, {"filename": "/tests/metron_good/private_getter.h", "start": 226673, "end": 226897}, {"filename": "/tests/metron_good/multi_tick.h", "start": 226897, "end": 227263}, {"filename": "/tests/metron_good/good_order.h", "start": 227263, "end": 227559}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 227559, "end": 227866}, {"filename": "/tests/metron_good/basic_output.h", "start": 227866, "end": 228127}, {"filename": "/tests/metron_good/basic_submod.h", "start": 228127, "end": 228416}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 228416, "end": 228970}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 228970, "end": 229150}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 229150, "end": 229526}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 229526, "end": 229840}, {"filename": "/tests/metron_good/input_signals.h", "start": 229840, "end": 230502}, {"filename": "/tests/metron_good/minimal.h", "start": 230502, "end": 230577}, {"filename": "/tests/metron_good/nested_structs.h", "start": 230577, "end": 231090}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 231090, "end": 231445}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 231445, "end": 231691}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 231691, "end": 231911}, {"filename": "/tests/metron_good/local_localparam.h", "start": 231911, "end": 232089}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 232089, "end": 232400}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 232400, "end": 232940}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 232940, "end": 233183}, {"filename": "/tests/metron_bad/README.md", "start": 233183, "end": 233380}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 233380, "end": 233975}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 233975, "end": 234421}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 234421, "end": 234561}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 234561, "end": 234821}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 234821, "end": 235069}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 235069, "end": 235365}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 235365, "end": 235872}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 235872, "end": 236282}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 236282, "end": 236554}], "remote_package_size": 236554});

  })();
