
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22817}, {"filename": "/examples/ibex/README.md", "start": 22817, "end": 22868}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22868, "end": 24466}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24466, "end": 36565}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36565, "end": 50989}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50989, "end": 67025}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67025, "end": 69539}, {"filename": "/examples/j1/metron/dpram.h", "start": 69539, "end": 69990}, {"filename": "/examples/j1/metron/j1.h", "start": 69990, "end": 74023}, {"filename": "/examples/pong/README.md", "start": 74023, "end": 74083}, {"filename": "/examples/pong/metron/pong.h", "start": 74083, "end": 77960}, {"filename": "/examples/pong/reference/README.md", "start": 77960, "end": 78020}, {"filename": "/examples/rvsimple/README.md", "start": 78020, "end": 78099}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78099, "end": 78612}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78612, "end": 80086}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80086, "end": 82704}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82704, "end": 83932}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83932, "end": 89664}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89664, "end": 90787}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90787, "end": 92733}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92733, "end": 93973}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93973, "end": 95230}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95230, "end": 95912}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95912, "end": 96889}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96889, "end": 99020}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99020, "end": 99797}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99797, "end": 100490}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100490, "end": 101319}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101319, "end": 102331}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102331, "end": 103315}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103315, "end": 104013}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104013, "end": 107069}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107069, "end": 112656}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112656, "end": 115162}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115162, "end": 119902}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119902, "end": 121903}, {"filename": "/examples/scratch.h", "start": 121903, "end": 122297}, {"filename": "/examples/tutorial/adder.h", "start": 122297, "end": 122477}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122477, "end": 123471}, {"filename": "/examples/tutorial/blockram.h", "start": 123471, "end": 124001}, {"filename": "/examples/tutorial/checksum.h", "start": 124001, "end": 124737}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124737, "end": 125273}, {"filename": "/examples/tutorial/counter.h", "start": 125273, "end": 125422}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125422, "end": 126201}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126201, "end": 127619}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127619, "end": 127741}, {"filename": "/examples/tutorial/submodules.h", "start": 127741, "end": 128858}, {"filename": "/examples/tutorial/templates.h", "start": 128858, "end": 129347}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129347, "end": 129415}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129415, "end": 129456}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129456, "end": 129497}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129497, "end": 129538}, {"filename": "/examples/tutorial/vga.h", "start": 129538, "end": 130698}, {"filename": "/examples/uart/README.md", "start": 130698, "end": 130942}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 130942, "end": 133535}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 133535, "end": 136109}, {"filename": "/examples/uart/metron/uart_top.h", "start": 136109, "end": 137886}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 137886, "end": 140918}, {"filename": "/tests/metron_bad/README.md", "start": 140918, "end": 141115}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141115, "end": 141424}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141424, "end": 141685}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141685, "end": 141941}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 141941, "end": 142549}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 142549, "end": 142789}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 142789, "end": 143212}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 143212, "end": 143765}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 143765, "end": 144089}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144089, "end": 144548}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144548, "end": 144821}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 144821, "end": 145106}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 145106, "end": 145626}, {"filename": "/tests/metron_good/README.md", "start": 145626, "end": 145707}, {"filename": "/tests/metron_good/all_func_types.h", "start": 145707, "end": 147379}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 147379, "end": 147799}, {"filename": "/tests/metron_good/basic_function.h", "start": 147799, "end": 148091}, {"filename": "/tests/metron_good/basic_increment.h", "start": 148091, "end": 148459}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 148459, "end": 148767}, {"filename": "/tests/metron_good/basic_literals.h", "start": 148767, "end": 149392}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 149392, "end": 149651}, {"filename": "/tests/metron_good/basic_output.h", "start": 149651, "end": 149925}, {"filename": "/tests/metron_good/basic_param.h", "start": 149925, "end": 150197}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 150197, "end": 150441}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 150441, "end": 150634}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 150634, "end": 150910}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 150910, "end": 151143}, {"filename": "/tests/metron_good/basic_submod.h", "start": 151143, "end": 151445}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 151445, "end": 151813}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 151813, "end": 152202}, {"filename": "/tests/metron_good/basic_switch.h", "start": 152202, "end": 152692}, {"filename": "/tests/metron_good/basic_task.h", "start": 152692, "end": 153039}, {"filename": "/tests/metron_good/basic_template.h", "start": 153039, "end": 153538}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 153538, "end": 153710}, {"filename": "/tests/metron_good/bit_casts.h", "start": 153710, "end": 159696}, {"filename": "/tests/metron_good/bit_concat.h", "start": 159696, "end": 160136}, {"filename": "/tests/metron_good/bit_dup.h", "start": 160136, "end": 160808}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 160808, "end": 161659}, {"filename": "/tests/metron_good/builtins.h", "start": 161659, "end": 162003}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 162003, "end": 162323}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 162323, "end": 162890}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 162890, "end": 163730}, {"filename": "/tests/metron_good/constructor_args.h", "start": 163730, "end": 164202}, {"filename": "/tests/metron_good/defines.h", "start": 164202, "end": 164529}, {"filename": "/tests/metron_good/dontcare.h", "start": 164529, "end": 164822}, {"filename": "/tests/metron_good/enum_simple.h", "start": 164822, "end": 166264}, {"filename": "/tests/metron_good/for_loops.h", "start": 166264, "end": 166597}, {"filename": "/tests/metron_good/good_order.h", "start": 166597, "end": 166906}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 166906, "end": 167330}, {"filename": "/tests/metron_good/include_guards.h", "start": 167330, "end": 167527}, {"filename": "/tests/metron_good/init_chain.h", "start": 167527, "end": 168248}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 168248, "end": 168548}, {"filename": "/tests/metron_good/input_signals.h", "start": 168548, "end": 169223}, {"filename": "/tests/metron_good/local_localparam.h", "start": 169223, "end": 169414}, {"filename": "/tests/metron_good/magic_comments.h", "start": 169414, "end": 169730}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 169730, "end": 170057}, {"filename": "/tests/metron_good/minimal.h", "start": 170057, "end": 170145}, {"filename": "/tests/metron_good/multi_tick.h", "start": 170145, "end": 170524}, {"filename": "/tests/metron_good/namespaces.h", "start": 170524, "end": 170887}, {"filename": "/tests/metron_good/nested_structs.h", "start": 170887, "end": 171315}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171315, "end": 171873}, {"filename": "/tests/metron_good/oneliners.h", "start": 171873, "end": 172149}, {"filename": "/tests/metron_good/plus_equals.h", "start": 172149, "end": 172573}, {"filename": "/tests/metron_good/private_getter.h", "start": 172573, "end": 172810}, {"filename": "/tests/metron_good/structs.h", "start": 172810, "end": 173042}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 173042, "end": 173591}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 173591, "end": 176148}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 176148, "end": 176915}, {"filename": "/tests/metron_good/tock_task.h", "start": 176915, "end": 177284}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 177284, "end": 177462}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 177462, "end": 178134}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 178134, "end": 178806}], "remote_package_size": 178806});

  })();
