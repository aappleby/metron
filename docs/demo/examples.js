
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/pong/README.md", "start": 71623, "end": 71683}, {"filename": "/examples/pong/metron/pong.h", "start": 71683, "end": 75674}, {"filename": "/examples/pong/reference/README.md", "start": 75674, "end": 75734}, {"filename": "/examples/rvsimple/README.md", "start": 75734, "end": 75813}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 75813, "end": 76320}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 76320, "end": 77788}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 77788, "end": 80400}, {"filename": "/examples/rvsimple/metron/config.h", "start": 80400, "end": 81622}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 81622, "end": 87348}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 87348, "end": 88465}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 88465, "end": 90405}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 90405, "end": 91645}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 91645, "end": 92896}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 92896, "end": 93575}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 93575, "end": 94546}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 94546, "end": 96671}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 96671, "end": 97442}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 97442, "end": 98129}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 98129, "end": 98952}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 98952, "end": 99958}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 99958, "end": 100936}, {"filename": "/examples/rvsimple/metron/register.h", "start": 100936, "end": 101632}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 101632, "end": 104682}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 104682, "end": 110263}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 110263, "end": 112763}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 112763, "end": 117501}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 117501, "end": 119496}, {"filename": "/examples/scratch.h", "start": 119496, "end": 119843}, {"filename": "/examples/tutorial/adder.h", "start": 119843, "end": 120025}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 120025, "end": 121013}, {"filename": "/examples/tutorial/blockram.h", "start": 121013, "end": 121541}, {"filename": "/examples/tutorial/checksum.h", "start": 121541, "end": 122283}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 122283, "end": 122690}, {"filename": "/examples/tutorial/counter.h", "start": 122690, "end": 122841}, {"filename": "/examples/tutorial/declaration_order.h", "start": 122841, "end": 123667}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 123667, "end": 125095}, {"filename": "/examples/tutorial/nonblocking.h", "start": 125095, "end": 125221}, {"filename": "/examples/tutorial/submodules.h", "start": 125221, "end": 126340}, {"filename": "/examples/tutorial/templates.h", "start": 126340, "end": 126823}, {"filename": "/examples/tutorial/tutorial2.h", "start": 126823, "end": 126891}, {"filename": "/examples/tutorial/tutorial3.h", "start": 126891, "end": 126932}, {"filename": "/examples/tutorial/tutorial4.h", "start": 126932, "end": 126973}, {"filename": "/examples/tutorial/tutorial5.h", "start": 126973, "end": 127014}, {"filename": "/examples/tutorial/vga.h", "start": 127014, "end": 128194}, {"filename": "/examples/uart/README.md", "start": 128194, "end": 128438}, {"filename": "/examples/uart/message.txt", "start": 128438, "end": 128950}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 128950, "end": 131588}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 131588, "end": 134279}, {"filename": "/examples/uart/metron/uart_top.h", "start": 134279, "end": 136067}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 136067, "end": 139124}, {"filename": "/tests/metron_bad/README.md", "start": 139124, "end": 139321}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 139321, "end": 139628}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 139628, "end": 139883}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 139883, "end": 140103}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 140103, "end": 140589}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 140589, "end": 140826}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 140826, "end": 141243}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 141243, "end": 141709}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 141709, "end": 142027}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 142027, "end": 142479}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 142479, "end": 142736}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 142736, "end": 143018}, {"filename": "/tests/metron_bad/unorderable_ticks.h", "start": 143018, "end": 143254}, {"filename": "/tests/metron_bad/unorderable_tocks.h", "start": 143254, "end": 143484}, {"filename": "/tests/metron_bad/wrong_submod_call_order.h", "start": 143484, "end": 143853}, {"filename": "/tests/metron_good/README.md", "start": 143853, "end": 143934}, {"filename": "/tests/metron_good/all_func_types.h", "start": 143934, "end": 145435}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 145435, "end": 145852}, {"filename": "/tests/metron_good/basic_function.h", "start": 145852, "end": 146104}, {"filename": "/tests/metron_good/basic_increment.h", "start": 146104, "end": 146431}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 146431, "end": 146736}, {"filename": "/tests/metron_good/basic_literals.h", "start": 146736, "end": 147355}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 147355, "end": 147611}, {"filename": "/tests/metron_good/basic_output.h", "start": 147611, "end": 147883}, {"filename": "/tests/metron_good/basic_param.h", "start": 147883, "end": 148152}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 148152, "end": 148353}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 148353, "end": 148540}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 148540, "end": 148771}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 148771, "end": 148998}, {"filename": "/tests/metron_good/basic_submod.h", "start": 148998, "end": 149314}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 149314, "end": 149679}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 149679, "end": 150065}, {"filename": "/tests/metron_good/basic_switch.h", "start": 150065, "end": 150554}, {"filename": "/tests/metron_good/basic_task.h", "start": 150554, "end": 150889}, {"filename": "/tests/metron_good/basic_template.h", "start": 150889, "end": 151391}, {"filename": "/tests/metron_good/bit_casts.h", "start": 151391, "end": 157371}, {"filename": "/tests/metron_good/bit_concat.h", "start": 157371, "end": 157805}, {"filename": "/tests/metron_good/bit_dup.h", "start": 157805, "end": 158471}, {"filename": "/tests/metron_good/bitfields.h", "start": 158471, "end": 159677}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 159677, "end": 160494}, {"filename": "/tests/metron_good/builtins.h", "start": 160494, "end": 160832}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 160832, "end": 161149}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 161149, "end": 161734}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 161734, "end": 162597}, {"filename": "/tests/metron_good/constructor_args.h", "start": 162597, "end": 163069}, {"filename": "/tests/metron_good/counter.h", "start": 163069, "end": 163716}, {"filename": "/tests/metron_good/defines.h", "start": 163716, "end": 164037}, {"filename": "/tests/metron_good/dontcare.h", "start": 164037, "end": 164324}, {"filename": "/tests/metron_good/enum_simple.h", "start": 164324, "end": 165719}, {"filename": "/tests/metron_good/for_loops.h", "start": 165719, "end": 166046}, {"filename": "/tests/metron_good/good_order.h", "start": 166046, "end": 166249}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 166249, "end": 166670}, {"filename": "/tests/metron_good/include_guards.h", "start": 166670, "end": 166867}, {"filename": "/tests/metron_good/include_test_module.h", "start": 166867, "end": 167058}, {"filename": "/tests/metron_good/include_test_submod.h", "start": 167058, "end": 167235}, {"filename": "/tests/metron_good/init_chain.h", "start": 167235, "end": 167956}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 167956, "end": 168248}, {"filename": "/tests/metron_good/input_signals.h", "start": 168248, "end": 169044}, {"filename": "/tests/metron_good/local_localparam.h", "start": 169044, "end": 169229}, {"filename": "/tests/metron_good/magic_comments.h", "start": 169229, "end": 169542}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 169542, "end": 169863}, {"filename": "/tests/metron_good/minimal.h", "start": 169863, "end": 170043}, {"filename": "/tests/metron_good/multi_tick.h", "start": 170043, "end": 170419}, {"filename": "/tests/metron_good/namespaces.h", "start": 170419, "end": 170800}, {"filename": "/tests/metron_good/nested_structs.h", "start": 170800, "end": 171261}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171261, "end": 171834}, {"filename": "/tests/metron_good/noconvert.h", "start": 171834, "end": 172164}, {"filename": "/tests/metron_good/oneliners.h", "start": 172164, "end": 172438}, {"filename": "/tests/metron_good/plus_equals.h", "start": 172438, "end": 172925}, {"filename": "/tests/metron_good/private_getter.h", "start": 172925, "end": 173177}, {"filename": "/tests/metron_good/self_reference.h", "start": 173177, "end": 173382}, {"filename": "/tests/metron_good/slice.h", "start": 173382, "end": 173889}, {"filename": "/tests/metron_good/structs.h", "start": 173889, "end": 174343}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 174343, "end": 174888}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 174888, "end": 177508}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 177508, "end": 178302}, {"filename": "/tests/metron_good/tock_task.h", "start": 178302, "end": 178754}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 178754, "end": 178926}, {"filename": "/tests/metron_good/unions.h", "start": 178926, "end": 179132}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 179132, "end": 179801}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 179801, "end": 180470}], "remote_package_size": 180470});

  })();
