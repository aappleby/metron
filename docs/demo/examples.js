
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
      var PACKAGE_UUID = metadata['package_uuid'];

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
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "rvtiny", true, true);
Module['FS_createPath']("/examples/rvtiny", "metron", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/examples/uart", "metron", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "rvtiny_sync", true, true);
Module['FS_createPath']("/examples/rvtiny_sync", "metron", true, true);
Module['FS_createPath']("/examples", "gb_spu", true, true);
Module['FS_createPath']("/examples/gb_spu", "metron", true, true);
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1263}, {"filename": "/examples/ibex/README.md", "start": 1263, "end": 1314}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 1314, "end": 17338}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 17338, "end": 18923}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 18923, "end": 31021}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 31021, "end": 33522}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33522, "end": 47932}, {"filename": "/examples/rvsimple/README.md", "start": 47932, "end": 48011}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 48011, "end": 49125}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 49125, "end": 49889}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 49889, "end": 52382}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 52382, "end": 53392}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 53392, "end": 54219}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 54219, "end": 54910}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 54910, "end": 55586}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 55586, "end": 57704}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 57704, "end": 60325}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 60325, "end": 66044}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 66044, "end": 67505}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 67505, "end": 68739}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 68739, "end": 70677}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 70677, "end": 72525}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 72525, "end": 73678}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 73678, "end": 79272}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 79272, "end": 83999}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 83999, "end": 84499}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 84499, "end": 85372}, {"filename": "/examples/rvsimple/metron/register.h", "start": 85372, "end": 86057}, {"filename": "/examples/rvsimple/metron/config.h", "start": 86057, "end": 87272}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 87272, "end": 88243}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 88243, "end": 91286}, {"filename": "/examples/pong/README.md", "start": 91286, "end": 91346}, {"filename": "/examples/pong/metron/pong.h", "start": 91346, "end": 95210}, {"filename": "/examples/pong/reference/README.md", "start": 95210, "end": 95270}, {"filename": "/examples/rvtiny/README.md", "start": 95270, "end": 95434}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 95434, "end": 101530}, {"filename": "/examples/uart/README.md", "start": 101530, "end": 101774}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 101774, "end": 104793}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 104793, "end": 107354}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 107354, "end": 109934}, {"filename": "/examples/uart/metron/uart_top.h", "start": 109934, "end": 111698}, {"filename": "/examples/tutorial/nonblocking.h", "start": 111698, "end": 111820}, {"filename": "/examples/tutorial/checksum.h", "start": 111820, "end": 112543}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 112543, "end": 113524}, {"filename": "/examples/tutorial/tutorial5.h", "start": 113524, "end": 113565}, {"filename": "/examples/tutorial/tutorial2.h", "start": 113565, "end": 113633}, {"filename": "/examples/tutorial/vga.h", "start": 113633, "end": 114780}, {"filename": "/examples/tutorial/templates.h", "start": 114780, "end": 115251}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 115251, "end": 116669}, {"filename": "/examples/tutorial/tutorial4.h", "start": 116669, "end": 116710}, {"filename": "/examples/tutorial/submodules.h", "start": 116710, "end": 117827}, {"filename": "/examples/tutorial/blockram.h", "start": 117827, "end": 118344}, {"filename": "/examples/tutorial/tutorial3.h", "start": 118344, "end": 118385}, {"filename": "/examples/tutorial/declaration_order.h", "start": 118385, "end": 119164}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 119164, "end": 119700}, {"filename": "/examples/tutorial/adder.h", "start": 119700, "end": 119880}, {"filename": "/examples/tutorial/counter.h", "start": 119880, "end": 120029}, {"filename": "/examples/rvtiny_sync/README.md", "start": 120029, "end": 120081}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 120081, "end": 127036}, {"filename": "/examples/gb_spu/regs.h", "start": 127036, "end": 146799}, {"filename": "/examples/gb_spu/Sch_Channel4.h", "start": 146799, "end": 152155}, {"filename": "/examples/gb_spu/Sch_Channel3.h", "start": 152155, "end": 156266}, {"filename": "/examples/gb_spu/Sch_Channel1.h", "start": 156266, "end": 161650}, {"filename": "/examples/gb_spu/gates.h", "start": 161650, "end": 165050}, {"filename": "/examples/gb_spu/Sch_Channel2.h", "start": 165050, "end": 170692}, {"filename": "/examples/gb_spu/Sch_ApuControl.h", "start": 170692, "end": 172872}, {"filename": "/examples/gb_spu/metron/gb_spu.h", "start": 172872, "end": 193663}, {"filename": "/examples/j1/metron/j1.h", "start": 193663, "end": 197688}, {"filename": "/examples/j1/metron/dpram.h", "start": 197688, "end": 198126}, {"filename": "/tests/metron_good/all_func_types.h", "start": 198126, "end": 199691}, {"filename": "/tests/metron_good/tock_task.h", "start": 199691, "end": 200047}, {"filename": "/tests/metron_good/namespaces.h", "start": 200047, "end": 200397}, {"filename": "/tests/metron_good/private_getter.h", "start": 200397, "end": 200621}, {"filename": "/tests/metron_good/defines.h", "start": 200621, "end": 200935}, {"filename": "/tests/metron_good/minimal.h", "start": 200935, "end": 201010}, {"filename": "/tests/metron_good/basic_increment.h", "start": 201010, "end": 201365}, {"filename": "/tests/metron_good/basic_submod.h", "start": 201365, "end": 201654}, {"filename": "/tests/metron_good/bit_dup.h", "start": 201654, "end": 202313}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 202313, "end": 202689}, {"filename": "/tests/metron_good/magic_comments.h", "start": 202689, "end": 202992}, {"filename": "/tests/metron_good/local_localparam.h", "start": 202992, "end": 203170}, {"filename": "/tests/metron_good/include_guards.h", "start": 203170, "end": 203367}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 203367, "end": 204026}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 204026, "end": 204191}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 204191, "end": 204602}, {"filename": "/tests/metron_good/README.md", "start": 204602, "end": 204683}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 204683, "end": 205228}, {"filename": "/tests/metron_good/basic_function.h", "start": 205228, "end": 205507}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 205507, "end": 205814}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 205814, "end": 206109}, {"filename": "/tests/metron_good/input_signals.h", "start": 206109, "end": 206771}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 206771, "end": 207126}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 207126, "end": 207389}, {"filename": "/tests/metron_good/multi_tick.h", "start": 207389, "end": 207755}, {"filename": "/tests/metron_good/for_loops.h", "start": 207755, "end": 208075}, {"filename": "/tests/metron_good/bit_casts.h", "start": 208075, "end": 214048}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 214048, "end": 214602}, {"filename": "/tests/metron_good/init_chain.h", "start": 214602, "end": 215310}, {"filename": "/tests/metron_good/oneliners.h", "start": 215310, "end": 215573}, {"filename": "/tests/metron_good/plus_equals.h", "start": 215573, "end": 215923}, {"filename": "/tests/metron_good/basic_task.h", "start": 215923, "end": 216257}, {"filename": "/tests/metron_good/basic_output.h", "start": 216257, "end": 216518}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 216518, "end": 217272}, {"filename": "/tests/metron_good/basic_param.h", "start": 217272, "end": 217531}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 217531, "end": 217690}, {"filename": "/tests/metron_good/basic_switch.h", "start": 217690, "end": 218167}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 218167, "end": 218347}, {"filename": "/tests/metron_good/good_order.h", "start": 218347, "end": 218643}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 218643, "end": 219302}, {"filename": "/tests/metron_good/enum_simple.h", "start": 219302, "end": 220670}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 220670, "end": 220916}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 220916, "end": 221754}, {"filename": "/tests/metron_good/builtins.h", "start": 221754, "end": 222085}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 222085, "end": 222305}, {"filename": "/tests/metron_good/bit_concat.h", "start": 222305, "end": 222732}, {"filename": "/tests/metron_good/basic_literals.h", "start": 222732, "end": 223344}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 223344, "end": 223751}, {"filename": "/tests/metron_good/dontcare.h", "start": 223751, "end": 224034}, {"filename": "/tests/metron_good/basic_template.h", "start": 224034, "end": 224465}, {"filename": "/tests/metron_good/structs.h", "start": 224465, "end": 224684}, {"filename": "/tests/metron_good/nested_structs.h", "start": 224684, "end": 225197}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 225197, "end": 225511}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 225511, "end": 225742}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 225742, "end": 225985}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 225985, "end": 226580}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 226580, "end": 226852}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 226852, "end": 227100}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 227100, "end": 227510}, {"filename": "/tests/metron_bad/README.md", "start": 227510, "end": 227707}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 227707, "end": 227967}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 227967, "end": 228107}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 228107, "end": 228418}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 228418, "end": 228958}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 228958, "end": 229404}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 229404, "end": 229700}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 229700, "end": 230207}], "remote_package_size": 230207, "package_uuid": "8f6a8708-980a-4fd2-989d-01e0bfbe8346"});

  })();
