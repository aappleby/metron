
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1263}, {"filename": "/examples/ibex/README.md", "start": 1263, "end": 1314}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 1314, "end": 17338}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 17338, "end": 18923}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 18923, "end": 31021}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 31021, "end": 33522}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33522, "end": 47932}, {"filename": "/examples/rvsimple/README.md", "start": 47932, "end": 48011}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 48011, "end": 49125}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 49125, "end": 49889}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 49889, "end": 52382}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 52382, "end": 53392}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 53392, "end": 54219}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 54219, "end": 54910}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 54910, "end": 55586}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 55586, "end": 57704}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 57704, "end": 60325}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 60325, "end": 66044}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 66044, "end": 67505}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 67505, "end": 68739}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 68739, "end": 70677}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 70677, "end": 72525}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 72525, "end": 73678}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 73678, "end": 79272}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 79272, "end": 83999}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 83999, "end": 84499}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 84499, "end": 85372}, {"filename": "/examples/rvsimple/metron/register.h", "start": 85372, "end": 86057}, {"filename": "/examples/rvsimple/metron/config.h", "start": 86057, "end": 87272}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 87272, "end": 88243}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 88243, "end": 91286}, {"filename": "/examples/pong/README.md", "start": 91286, "end": 91346}, {"filename": "/examples/pong/metron/pong.h", "start": 91346, "end": 95210}, {"filename": "/examples/pong/reference/README.md", "start": 95210, "end": 95270}, {"filename": "/examples/rvtiny/README.md", "start": 95270, "end": 95434}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 95434, "end": 101530}, {"filename": "/examples/uart/README.md", "start": 101530, "end": 101774}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 101774, "end": 104793}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 104793, "end": 107354}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 107354, "end": 109934}, {"filename": "/examples/uart/metron/uart_top.h", "start": 109934, "end": 111698}, {"filename": "/examples/tutorial/nonblocking.h", "start": 111698, "end": 111820}, {"filename": "/examples/tutorial/checksum.h", "start": 111820, "end": 112543}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 112543, "end": 113524}, {"filename": "/examples/tutorial/tutorial5.h", "start": 113524, "end": 113565}, {"filename": "/examples/tutorial/tutorial2.h", "start": 113565, "end": 113633}, {"filename": "/examples/tutorial/vga.h", "start": 113633, "end": 114780}, {"filename": "/examples/tutorial/templates.h", "start": 114780, "end": 115251}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 115251, "end": 116669}, {"filename": "/examples/tutorial/tutorial4.h", "start": 116669, "end": 116710}, {"filename": "/examples/tutorial/submodules.h", "start": 116710, "end": 117827}, {"filename": "/examples/tutorial/blockram.h", "start": 117827, "end": 118344}, {"filename": "/examples/tutorial/tutorial3.h", "start": 118344, "end": 118385}, {"filename": "/examples/tutorial/declaration_order.h", "start": 118385, "end": 119164}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 119164, "end": 119700}, {"filename": "/examples/tutorial/adder.h", "start": 119700, "end": 119880}, {"filename": "/examples/tutorial/counter.h", "start": 119880, "end": 120029}, {"filename": "/examples/rvtiny_sync/README.md", "start": 120029, "end": 120081}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 120081, "end": 127036}, {"filename": "/examples/gb_spu/regs.h", "start": 127036, "end": 147389}, {"filename": "/examples/gb_spu/gates.h", "start": 147389, "end": 150880}, {"filename": "/examples/gb_spu/Sch_ApuControl.h", "start": 150880, "end": 166340}, {"filename": "/examples/gb_spu/metron/gb_spu.h", "start": 166340, "end": 187131}, {"filename": "/examples/j1/metron/j1.h", "start": 187131, "end": 191156}, {"filename": "/examples/j1/metron/dpram.h", "start": 191156, "end": 191594}, {"filename": "/tests/metron_good/all_func_types.h", "start": 191594, "end": 193159}, {"filename": "/tests/metron_good/tock_task.h", "start": 193159, "end": 193515}, {"filename": "/tests/metron_good/namespaces.h", "start": 193515, "end": 193865}, {"filename": "/tests/metron_good/private_getter.h", "start": 193865, "end": 194089}, {"filename": "/tests/metron_good/defines.h", "start": 194089, "end": 194403}, {"filename": "/tests/metron_good/minimal.h", "start": 194403, "end": 194478}, {"filename": "/tests/metron_good/basic_increment.h", "start": 194478, "end": 194833}, {"filename": "/tests/metron_good/basic_submod.h", "start": 194833, "end": 195122}, {"filename": "/tests/metron_good/bit_dup.h", "start": 195122, "end": 195781}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 195781, "end": 196157}, {"filename": "/tests/metron_good/magic_comments.h", "start": 196157, "end": 196460}, {"filename": "/tests/metron_good/local_localparam.h", "start": 196460, "end": 196638}, {"filename": "/tests/metron_good/include_guards.h", "start": 196638, "end": 196835}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 196835, "end": 197494}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 197494, "end": 197659}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 197659, "end": 198070}, {"filename": "/tests/metron_good/README.md", "start": 198070, "end": 198151}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 198151, "end": 198696}, {"filename": "/tests/metron_good/basic_function.h", "start": 198696, "end": 198975}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 198975, "end": 199282}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 199282, "end": 199577}, {"filename": "/tests/metron_good/input_signals.h", "start": 199577, "end": 200239}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 200239, "end": 200594}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 200594, "end": 200857}, {"filename": "/tests/metron_good/multi_tick.h", "start": 200857, "end": 201223}, {"filename": "/tests/metron_good/for_loops.h", "start": 201223, "end": 201543}, {"filename": "/tests/metron_good/bit_casts.h", "start": 201543, "end": 207516}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 207516, "end": 208070}, {"filename": "/tests/metron_good/init_chain.h", "start": 208070, "end": 208778}, {"filename": "/tests/metron_good/oneliners.h", "start": 208778, "end": 209041}, {"filename": "/tests/metron_good/plus_equals.h", "start": 209041, "end": 209391}, {"filename": "/tests/metron_good/basic_task.h", "start": 209391, "end": 209725}, {"filename": "/tests/metron_good/basic_output.h", "start": 209725, "end": 209986}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 209986, "end": 210740}, {"filename": "/tests/metron_good/basic_param.h", "start": 210740, "end": 210999}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 210999, "end": 211158}, {"filename": "/tests/metron_good/basic_switch.h", "start": 211158, "end": 211635}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 211635, "end": 211815}, {"filename": "/tests/metron_good/good_order.h", "start": 211815, "end": 212111}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 212111, "end": 212770}, {"filename": "/tests/metron_good/enum_simple.h", "start": 212770, "end": 214138}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 214138, "end": 214384}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 214384, "end": 215222}, {"filename": "/tests/metron_good/builtins.h", "start": 215222, "end": 215553}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 215553, "end": 215773}, {"filename": "/tests/metron_good/bit_concat.h", "start": 215773, "end": 216200}, {"filename": "/tests/metron_good/basic_literals.h", "start": 216200, "end": 216812}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 216812, "end": 217219}, {"filename": "/tests/metron_good/dontcare.h", "start": 217219, "end": 217502}, {"filename": "/tests/metron_good/basic_template.h", "start": 217502, "end": 217933}, {"filename": "/tests/metron_good/structs.h", "start": 217933, "end": 218152}, {"filename": "/tests/metron_good/nested_structs.h", "start": 218152, "end": 218665}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 218665, "end": 218979}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 218979, "end": 219210}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 219210, "end": 219453}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 219453, "end": 220048}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 220048, "end": 220320}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 220320, "end": 220568}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 220568, "end": 220978}, {"filename": "/tests/metron_bad/README.md", "start": 220978, "end": 221175}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 221175, "end": 221435}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 221435, "end": 221575}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 221575, "end": 221886}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 221886, "end": 222426}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 222426, "end": 222872}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 222872, "end": 223168}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 223168, "end": 223675}], "remote_package_size": 223675, "package_uuid": "a8c5da53-8995-4383-a7b7-020dbac8a21c"});

  })();
