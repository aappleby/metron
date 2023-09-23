
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22885}, {"filename": "/examples/ibex/README.md", "start": 22885, "end": 22936}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22936, "end": 24534}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24534, "end": 36633}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36633, "end": 51057}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 51057, "end": 67093}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67093, "end": 69607}, {"filename": "/examples/j1/metron/dpram.h", "start": 69607, "end": 70058}, {"filename": "/examples/j1/metron/j1.h", "start": 70058, "end": 74091}, {"filename": "/examples/pong/README.md", "start": 74091, "end": 74151}, {"filename": "/examples/pong/metron/pong.h", "start": 74151, "end": 78053}, {"filename": "/examples/pong/reference/README.md", "start": 78053, "end": 78113}, {"filename": "/examples/rvsimple/README.md", "start": 78113, "end": 78192}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78192, "end": 78705}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78705, "end": 80179}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80179, "end": 82797}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82797, "end": 84025}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 84025, "end": 89757}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89757, "end": 90880}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90880, "end": 92826}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92826, "end": 94066}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 94066, "end": 95323}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95323, "end": 96005}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 96005, "end": 96982}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96982, "end": 99113}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99113, "end": 99890}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99890, "end": 100583}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100583, "end": 101412}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101412, "end": 102424}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102424, "end": 103408}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103408, "end": 104106}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104106, "end": 107162}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107162, "end": 112749}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112749, "end": 115255}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115255, "end": 119995}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119995, "end": 121996}, {"filename": "/examples/scratch.h", "start": 121996, "end": 122282}, {"filename": "/examples/tutorial/adder.h", "start": 122282, "end": 122462}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122462, "end": 123456}, {"filename": "/examples/tutorial/blockram.h", "start": 123456, "end": 123986}, {"filename": "/examples/tutorial/checksum.h", "start": 123986, "end": 124722}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124722, "end": 125258}, {"filename": "/examples/tutorial/counter.h", "start": 125258, "end": 125407}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125407, "end": 126186}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126186, "end": 127604}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127604, "end": 127726}, {"filename": "/examples/tutorial/submodules.h", "start": 127726, "end": 128843}, {"filename": "/examples/tutorial/templates.h", "start": 128843, "end": 129332}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129332, "end": 129400}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129400, "end": 129441}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129441, "end": 129482}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129482, "end": 129523}, {"filename": "/examples/tutorial/vga.h", "start": 129523, "end": 130683}, {"filename": "/examples/uart/README.md", "start": 130683, "end": 130927}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 130927, "end": 133520}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 133520, "end": 136094}, {"filename": "/examples/uart/metron/uart_top.h", "start": 136094, "end": 137871}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 137871, "end": 140903}, {"filename": "/tests/metron_bad/README.md", "start": 140903, "end": 141100}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141100, "end": 141409}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141409, "end": 141670}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141670, "end": 141926}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 141926, "end": 142534}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 142534, "end": 142774}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 142774, "end": 143197}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 143197, "end": 143750}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 143750, "end": 144074}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144074, "end": 144533}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144533, "end": 144806}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 144806, "end": 145091}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 145091, "end": 145611}, {"filename": "/tests/metron_good/README.md", "start": 145611, "end": 145692}, {"filename": "/tests/metron_good/all_func_types.h", "start": 145692, "end": 147370}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 147370, "end": 147790}, {"filename": "/tests/metron_good/basic_function.h", "start": 147790, "end": 148082}, {"filename": "/tests/metron_good/basic_increment.h", "start": 148082, "end": 148450}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 148450, "end": 148758}, {"filename": "/tests/metron_good/basic_literals.h", "start": 148758, "end": 149383}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 149383, "end": 149642}, {"filename": "/tests/metron_good/basic_output.h", "start": 149642, "end": 149916}, {"filename": "/tests/metron_good/basic_param.h", "start": 149916, "end": 150188}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 150188, "end": 150432}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 150432, "end": 150625}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 150625, "end": 150901}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 150901, "end": 151134}, {"filename": "/tests/metron_good/basic_submod.h", "start": 151134, "end": 151453}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 151453, "end": 151821}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 151821, "end": 152210}, {"filename": "/tests/metron_good/basic_switch.h", "start": 152210, "end": 152700}, {"filename": "/tests/metron_good/basic_task.h", "start": 152700, "end": 153047}, {"filename": "/tests/metron_good/basic_template.h", "start": 153047, "end": 153546}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 153546, "end": 153718}, {"filename": "/tests/metron_good/bit_casts.h", "start": 153718, "end": 159704}, {"filename": "/tests/metron_good/bit_concat.h", "start": 159704, "end": 160144}, {"filename": "/tests/metron_good/bit_dup.h", "start": 160144, "end": 160816}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 160816, "end": 161724}, {"filename": "/tests/metron_good/builtins.h", "start": 161724, "end": 162068}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 162068, "end": 162388}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 162388, "end": 162955}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 162955, "end": 163809}, {"filename": "/tests/metron_good/constructor_args.h", "start": 163809, "end": 164281}, {"filename": "/tests/metron_good/defines.h", "start": 164281, "end": 164608}, {"filename": "/tests/metron_good/dontcare.h", "start": 164608, "end": 164901}, {"filename": "/tests/metron_good/enum_simple.h", "start": 164901, "end": 166343}, {"filename": "/tests/metron_good/for_loops.h", "start": 166343, "end": 166676}, {"filename": "/tests/metron_good/good_order.h", "start": 166676, "end": 166985}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 166985, "end": 167409}, {"filename": "/tests/metron_good/include_guards.h", "start": 167409, "end": 167606}, {"filename": "/tests/metron_good/init_chain.h", "start": 167606, "end": 168327}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 168327, "end": 168625}, {"filename": "/tests/metron_good/input_signals.h", "start": 168625, "end": 169423}, {"filename": "/tests/metron_good/local_localparam.h", "start": 169423, "end": 169614}, {"filename": "/tests/metron_good/magic_comments.h", "start": 169614, "end": 169930}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 169930, "end": 170257}, {"filename": "/tests/metron_good/minimal.h", "start": 170257, "end": 170443}, {"filename": "/tests/metron_good/multi_tick.h", "start": 170443, "end": 170822}, {"filename": "/tests/metron_good/namespaces.h", "start": 170822, "end": 171185}, {"filename": "/tests/metron_good/nested_structs.h", "start": 171185, "end": 171645}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171645, "end": 172203}, {"filename": "/tests/metron_good/oneliners.h", "start": 172203, "end": 172479}, {"filename": "/tests/metron_good/plus_equals.h", "start": 172479, "end": 172903}, {"filename": "/tests/metron_good/private_getter.h", "start": 172903, "end": 173140}, {"filename": "/tests/metron_good/structs.h", "start": 173140, "end": 173372}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 173372, "end": 173921}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 173921, "end": 176478}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 176478, "end": 177245}, {"filename": "/tests/metron_good/tock_task.h", "start": 177245, "end": 177626}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 177626, "end": 177804}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 177804, "end": 178476}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 178476, "end": 179148}], "remote_package_size": 179148});

  })();
