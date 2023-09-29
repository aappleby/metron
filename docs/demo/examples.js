
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22879}, {"filename": "/examples/ibex/README.md", "start": 22879, "end": 22930}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22930, "end": 24569}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24569, "end": 36754}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36754, "end": 51246}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 51246, "end": 67323}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67323, "end": 69908}, {"filename": "/examples/j1/metron/dpram.h", "start": 69908, "end": 70353}, {"filename": "/examples/j1/metron/j1.h", "start": 70353, "end": 74380}, {"filename": "/examples/pong/README.md", "start": 74380, "end": 74440}, {"filename": "/examples/pong/metron/pong.h", "start": 74440, "end": 78336}, {"filename": "/examples/pong/reference/README.md", "start": 78336, "end": 78396}, {"filename": "/examples/rvsimple/README.md", "start": 78396, "end": 78475}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78475, "end": 78982}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78982, "end": 80450}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80450, "end": 83062}, {"filename": "/examples/rvsimple/metron/config.h", "start": 83062, "end": 84284}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 84284, "end": 90010}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 90010, "end": 91127}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 91127, "end": 93067}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 93067, "end": 94301}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 94301, "end": 95552}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95552, "end": 96228}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 96228, "end": 97199}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 97199, "end": 99324}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99324, "end": 100095}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 100095, "end": 100782}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100782, "end": 101605}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101605, "end": 102611}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102611, "end": 103589}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103589, "end": 104281}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104281, "end": 107331}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107331, "end": 112912}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112912, "end": 115412}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115412, "end": 120146}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 120146, "end": 122141}, {"filename": "/examples/scratch.h", "start": 122141, "end": 122387}, {"filename": "/examples/tutorial/adder.h", "start": 122387, "end": 122567}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122567, "end": 123555}, {"filename": "/examples/tutorial/blockram.h", "start": 123555, "end": 124079}, {"filename": "/examples/tutorial/checksum.h", "start": 124079, "end": 124809}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124809, "end": 125345}, {"filename": "/examples/tutorial/counter.h", "start": 125345, "end": 125494}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125494, "end": 126273}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126273, "end": 127691}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127691, "end": 127813}, {"filename": "/examples/tutorial/submodules.h", "start": 127813, "end": 128930}, {"filename": "/examples/tutorial/templates.h", "start": 128930, "end": 129413}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129413, "end": 129481}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129481, "end": 129522}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129522, "end": 129563}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129563, "end": 129604}, {"filename": "/examples/tutorial/vga.h", "start": 129604, "end": 130758}, {"filename": "/examples/uart/README.md", "start": 130758, "end": 131002}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 131002, "end": 133589}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 133589, "end": 136157}, {"filename": "/examples/uart/metron/uart_top.h", "start": 136157, "end": 137928}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 137928, "end": 140954}, {"filename": "/tests/metron_bad/README.md", "start": 140954, "end": 141151}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141151, "end": 141454}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141454, "end": 141709}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141709, "end": 141959}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 141959, "end": 142561}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 142561, "end": 142806}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 142806, "end": 143223}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 143223, "end": 143770}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 143770, "end": 144088}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144088, "end": 144541}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144541, "end": 144798}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 144798, "end": 145077}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 145077, "end": 145591}, {"filename": "/tests/metron_bad/wrong_submod_call_order.h", "start": 145591, "end": 145963}, {"filename": "/tests/metron_good/README.md", "start": 145963, "end": 146044}, {"filename": "/tests/metron_good/all_func_types.h", "start": 146044, "end": 147716}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 147716, "end": 148130}, {"filename": "/tests/metron_good/basic_function.h", "start": 148130, "end": 148416}, {"filename": "/tests/metron_good/basic_increment.h", "start": 148416, "end": 148778}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 148778, "end": 149080}, {"filename": "/tests/metron_good/basic_literals.h", "start": 149080, "end": 149699}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 149699, "end": 149952}, {"filename": "/tests/metron_good/basic_output.h", "start": 149952, "end": 150220}, {"filename": "/tests/metron_good/basic_param.h", "start": 150220, "end": 150486}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 150486, "end": 150724}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 150724, "end": 150911}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 150911, "end": 151181}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 151181, "end": 151408}, {"filename": "/tests/metron_good/basic_submod.h", "start": 151408, "end": 151721}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 151721, "end": 152083}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 152083, "end": 152466}, {"filename": "/tests/metron_good/basic_switch.h", "start": 152466, "end": 152950}, {"filename": "/tests/metron_good/basic_task.h", "start": 152950, "end": 153291}, {"filename": "/tests/metron_good/basic_template.h", "start": 153291, "end": 153784}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 153784, "end": 153950}, {"filename": "/tests/metron_good/bit_casts.h", "start": 153950, "end": 159930}, {"filename": "/tests/metron_good/bit_concat.h", "start": 159930, "end": 160364}, {"filename": "/tests/metron_good/bit_dup.h", "start": 160364, "end": 161030}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 161030, "end": 161932}, {"filename": "/tests/metron_good/builtins.h", "start": 161932, "end": 162270}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 162270, "end": 162584}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 162584, "end": 163145}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 163145, "end": 163993}, {"filename": "/tests/metron_good/constructor_args.h", "start": 163993, "end": 164459}, {"filename": "/tests/metron_good/defines.h", "start": 164459, "end": 164780}, {"filename": "/tests/metron_good/dontcare.h", "start": 164780, "end": 165067}, {"filename": "/tests/metron_good/enum_simple.h", "start": 165067, "end": 166462}, {"filename": "/tests/metron_good/for_loops.h", "start": 166462, "end": 166789}, {"filename": "/tests/metron_good/good_order.h", "start": 166789, "end": 167092}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 167092, "end": 167510}, {"filename": "/tests/metron_good/include_guards.h", "start": 167510, "end": 167707}, {"filename": "/tests/metron_good/init_chain.h", "start": 167707, "end": 168422}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 168422, "end": 168714}, {"filename": "/tests/metron_good/input_signals.h", "start": 168714, "end": 169506}, {"filename": "/tests/metron_good/local_localparam.h", "start": 169506, "end": 169691}, {"filename": "/tests/metron_good/magic_comments.h", "start": 169691, "end": 170001}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 170001, "end": 170322}, {"filename": "/tests/metron_good/minimal.h", "start": 170322, "end": 170502}, {"filename": "/tests/metron_good/multi_tick.h", "start": 170502, "end": 170875}, {"filename": "/tests/metron_good/namespaces.h", "start": 170875, "end": 171232}, {"filename": "/tests/metron_good/nested_structs.h", "start": 171232, "end": 171686}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171686, "end": 172238}, {"filename": "/tests/metron_good/oneliners.h", "start": 172238, "end": 172508}, {"filename": "/tests/metron_good/plus_equals.h", "start": 172508, "end": 172932}, {"filename": "/tests/metron_good/private_getter.h", "start": 172932, "end": 173163}, {"filename": "/tests/metron_good/structs.h", "start": 173163, "end": 173389}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 173389, "end": 173932}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 173932, "end": 176483}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 176483, "end": 177244}, {"filename": "/tests/metron_good/tock_task.h", "start": 177244, "end": 177619}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 177619, "end": 177791}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 177791, "end": 178457}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 178457, "end": 179123}], "remote_package_size": 179123});

  })();
