
  var Module = typeof Module !== 'undefined' ? Module : {};

  if (!Module.expectedDataFileDownloads) {
    Module.expectedDataFileDownloads = 0;
  }

  Module.expectedDataFileDownloads++;
  (function() {
    // Do not attempt to redownload the virtual filesystem data when in a pthread or a Wasm Worker context.
    if (Module['ENVIRONMENT_IS_PTHREAD'] || Module['$ww']) return;
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
            if (Module['setStatus']) Module['setStatus'](`Downloading data... (${loaded}/${total})`);
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
          Module['addRunDependency'](`fp ${this.name}`);
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
          Module['removeRunDependency'](`fp ${that.name}`);
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/pong/README.md", "start": 71623, "end": 71683}, {"filename": "/examples/pong/metron/pong.h", "start": 71683, "end": 75674}, {"filename": "/examples/pong/reference/README.md", "start": 75674, "end": 75734}, {"filename": "/examples/rvsimple/README.md", "start": 75734, "end": 75813}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 75813, "end": 76320}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 76320, "end": 77788}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 77788, "end": 80400}, {"filename": "/examples/rvsimple/metron/config.h", "start": 80400, "end": 81623}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 81623, "end": 87349}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 87349, "end": 88466}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 88466, "end": 90406}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 90406, "end": 91646}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 91646, "end": 92897}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 92897, "end": 93576}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 93576, "end": 94547}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 94547, "end": 96672}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 96672, "end": 97443}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 97443, "end": 98130}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 98130, "end": 98953}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 98953, "end": 99959}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 99959, "end": 100937}, {"filename": "/examples/rvsimple/metron/register.h", "start": 100937, "end": 101633}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 101633, "end": 104683}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 104683, "end": 110264}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 110264, "end": 112764}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 112764, "end": 117502}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 117502, "end": 119497}, {"filename": "/examples/scratch.h", "start": 119497, "end": 119891}, {"filename": "/examples/tutorial/adder.h", "start": 119891, "end": 120073}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 120073, "end": 121061}, {"filename": "/examples/tutorial/blockram.h", "start": 121061, "end": 121589}, {"filename": "/examples/tutorial/checksum.h", "start": 121589, "end": 122331}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 122331, "end": 122907}, {"filename": "/examples/tutorial/counter.h", "start": 122907, "end": 123058}, {"filename": "/examples/tutorial/declaration_order.h", "start": 123058, "end": 123884}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 123884, "end": 125312}, {"filename": "/examples/tutorial/nonblocking.h", "start": 125312, "end": 125438}, {"filename": "/examples/tutorial/submodules.h", "start": 125438, "end": 126557}, {"filename": "/examples/tutorial/templates.h", "start": 126557, "end": 127040}, {"filename": "/examples/tutorial/tutorial2.h", "start": 127040, "end": 127108}, {"filename": "/examples/tutorial/tutorial3.h", "start": 127108, "end": 127149}, {"filename": "/examples/tutorial/tutorial4.h", "start": 127149, "end": 127190}, {"filename": "/examples/tutorial/tutorial5.h", "start": 127190, "end": 127231}, {"filename": "/examples/tutorial/vga.h", "start": 127231, "end": 128411}, {"filename": "/examples/uart/README.md", "start": 128411, "end": 128655}, {"filename": "/examples/uart/message.txt", "start": 128655, "end": 129167}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 129167, "end": 131805}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 131805, "end": 134496}, {"filename": "/examples/uart/metron/uart_top.h", "start": 134496, "end": 136284}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 136284, "end": 139341}, {"filename": "/tests/metron_bad/README.md", "start": 139341, "end": 139538}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 139538, "end": 139845}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 139845, "end": 140100}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 140100, "end": 140320}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 140320, "end": 140806}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 140806, "end": 141043}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 141043, "end": 141460}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 141460, "end": 141926}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 141926, "end": 142244}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 142244, "end": 142696}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 142696, "end": 142953}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 142953, "end": 143235}, {"filename": "/tests/metron_bad/unorderable_ticks.h", "start": 143235, "end": 143471}, {"filename": "/tests/metron_bad/unorderable_tocks.h", "start": 143471, "end": 143701}, {"filename": "/tests/metron_bad/wrong_submod_call_order.h", "start": 143701, "end": 144070}, {"filename": "/tests/metron_good/README.md", "start": 144070, "end": 144151}, {"filename": "/tests/metron_good/all_func_types.h", "start": 144151, "end": 145652}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 145652, "end": 146069}, {"filename": "/tests/metron_good/basic_function.h", "start": 146069, "end": 146321}, {"filename": "/tests/metron_good/basic_increment.h", "start": 146321, "end": 146648}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 146648, "end": 146953}, {"filename": "/tests/metron_good/basic_literals.h", "start": 146953, "end": 147572}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 147572, "end": 147828}, {"filename": "/tests/metron_good/basic_output.h", "start": 147828, "end": 148100}, {"filename": "/tests/metron_good/basic_param.h", "start": 148100, "end": 148369}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 148369, "end": 148570}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 148570, "end": 148757}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 148757, "end": 148988}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 148988, "end": 149215}, {"filename": "/tests/metron_good/basic_submod.h", "start": 149215, "end": 149531}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 149531, "end": 149896}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 149896, "end": 150282}, {"filename": "/tests/metron_good/basic_switch.h", "start": 150282, "end": 150771}, {"filename": "/tests/metron_good/basic_task.h", "start": 150771, "end": 151106}, {"filename": "/tests/metron_good/basic_template.h", "start": 151106, "end": 151608}, {"filename": "/tests/metron_good/bit_casts.h", "start": 151608, "end": 157588}, {"filename": "/tests/metron_good/bit_concat.h", "start": 157588, "end": 158022}, {"filename": "/tests/metron_good/bit_dup.h", "start": 158022, "end": 158688}, {"filename": "/tests/metron_good/bitfields.h", "start": 158688, "end": 159894}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 159894, "end": 160711}, {"filename": "/tests/metron_good/builtins.h", "start": 160711, "end": 161049}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 161049, "end": 161366}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 161366, "end": 161951}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 161951, "end": 162814}, {"filename": "/tests/metron_good/constructor_args.h", "start": 162814, "end": 163286}, {"filename": "/tests/metron_good/counter.h", "start": 163286, "end": 163933}, {"filename": "/tests/metron_good/defines.h", "start": 163933, "end": 164254}, {"filename": "/tests/metron_good/dontcare.h", "start": 164254, "end": 164541}, {"filename": "/tests/metron_good/enum_simple.h", "start": 164541, "end": 165936}, {"filename": "/tests/metron_good/for_loops.h", "start": 165936, "end": 166263}, {"filename": "/tests/metron_good/good_order.h", "start": 166263, "end": 166466}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 166466, "end": 166887}, {"filename": "/tests/metron_good/include_guards.h", "start": 166887, "end": 167084}, {"filename": "/tests/metron_good/include_test_module.h", "start": 167084, "end": 167275}, {"filename": "/tests/metron_good/include_test_submod.h", "start": 167275, "end": 167452}, {"filename": "/tests/metron_good/init_chain.h", "start": 167452, "end": 168173}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 168173, "end": 168465}, {"filename": "/tests/metron_good/input_signals.h", "start": 168465, "end": 169261}, {"filename": "/tests/metron_good/local_localparam.h", "start": 169261, "end": 169446}, {"filename": "/tests/metron_good/magic_comments.h", "start": 169446, "end": 169759}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 169759, "end": 170080}, {"filename": "/tests/metron_good/minimal.h", "start": 170080, "end": 170260}, {"filename": "/tests/metron_good/multi_tick.h", "start": 170260, "end": 170636}, {"filename": "/tests/metron_good/namespaces.h", "start": 170636, "end": 171017}, {"filename": "/tests/metron_good/nested_structs.h", "start": 171017, "end": 171478}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171478, "end": 172051}, {"filename": "/tests/metron_good/noconvert.h", "start": 172051, "end": 172381}, {"filename": "/tests/metron_good/oneliners.h", "start": 172381, "end": 172655}, {"filename": "/tests/metron_good/plus_equals.h", "start": 172655, "end": 173142}, {"filename": "/tests/metron_good/private_getter.h", "start": 173142, "end": 173394}, {"filename": "/tests/metron_good/self_reference.h", "start": 173394, "end": 173599}, {"filename": "/tests/metron_good/slice.h", "start": 173599, "end": 174106}, {"filename": "/tests/metron_good/structs.h", "start": 174106, "end": 174604}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 174604, "end": 175149}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 175149, "end": 177769}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 177769, "end": 178563}, {"filename": "/tests/metron_good/tock_task.h", "start": 178563, "end": 179015}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 179015, "end": 179187}, {"filename": "/tests/metron_good/unions.h", "start": 179187, "end": 179393}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 179393, "end": 180062}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 180062, "end": 180731}], "remote_package_size": 180731});

  })();
