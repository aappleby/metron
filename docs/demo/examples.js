
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22879}, {"filename": "/examples/ibex/README.md", "start": 22879, "end": 22930}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22930, "end": 24569}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24569, "end": 36754}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36754, "end": 51246}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 51246, "end": 67323}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67323, "end": 69908}, {"filename": "/examples/j1/metron/dpram.h", "start": 69908, "end": 70353}, {"filename": "/examples/j1/metron/j1.h", "start": 70353, "end": 74380}, {"filename": "/examples/pong/README.md", "start": 74380, "end": 74440}, {"filename": "/examples/pong/metron/pong.h", "start": 74440, "end": 78336}, {"filename": "/examples/pong/reference/README.md", "start": 78336, "end": 78396}, {"filename": "/examples/rvsimple/README.md", "start": 78396, "end": 78475}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78475, "end": 78982}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78982, "end": 80450}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80450, "end": 83062}, {"filename": "/examples/rvsimple/metron/config.h", "start": 83062, "end": 84284}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 84284, "end": 90010}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 90010, "end": 91127}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 91127, "end": 93067}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 93067, "end": 94301}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 94301, "end": 95552}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95552, "end": 96228}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 96228, "end": 97199}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 97199, "end": 99324}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99324, "end": 100095}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 100095, "end": 100782}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100782, "end": 101605}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101605, "end": 102611}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102611, "end": 103589}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103589, "end": 104281}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104281, "end": 107331}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107331, "end": 112912}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112912, "end": 115412}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115412, "end": 120146}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 120146, "end": 122141}, {"filename": "/examples/scratch.h", "start": 122141, "end": 122387}, {"filename": "/examples/tutorial/adder.h", "start": 122387, "end": 122567}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122567, "end": 123555}, {"filename": "/examples/tutorial/blockram.h", "start": 123555, "end": 124079}, {"filename": "/examples/tutorial/checksum.h", "start": 124079, "end": 124809}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124809, "end": 125345}, {"filename": "/examples/tutorial/counter.h", "start": 125345, "end": 125494}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125494, "end": 126273}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126273, "end": 127691}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127691, "end": 127813}, {"filename": "/examples/tutorial/submodules.h", "start": 127813, "end": 128930}, {"filename": "/examples/tutorial/templates.h", "start": 128930, "end": 129413}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129413, "end": 129481}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129481, "end": 129522}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129522, "end": 129563}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129563, "end": 129604}, {"filename": "/examples/tutorial/vga.h", "start": 129604, "end": 130758}, {"filename": "/examples/uart/README.md", "start": 130758, "end": 131002}, {"filename": "/examples/uart/message.txt", "start": 131002, "end": 131514}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 131514, "end": 134101}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 134101, "end": 136669}, {"filename": "/examples/uart/metron/uart_top.h", "start": 136669, "end": 138440}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 138440, "end": 141466}, {"filename": "/tests/metron_bad/README.md", "start": 141466, "end": 141663}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141663, "end": 141966}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141966, "end": 142221}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 142221, "end": 142471}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142471, "end": 143073}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 143073, "end": 143318}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 143318, "end": 143735}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 143735, "end": 144282}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 144282, "end": 144600}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144600, "end": 145053}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 145053, "end": 145310}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 145310, "end": 145589}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 145589, "end": 146103}, {"filename": "/tests/metron_bad/wrong_submod_call_order.h", "start": 146103, "end": 146475}, {"filename": "/tests/metron_good/README.md", "start": 146475, "end": 146556}, {"filename": "/tests/metron_good/all_func_types.h", "start": 146556, "end": 148228}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 148228, "end": 148642}, {"filename": "/tests/metron_good/basic_function.h", "start": 148642, "end": 148928}, {"filename": "/tests/metron_good/basic_increment.h", "start": 148928, "end": 149290}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 149290, "end": 149592}, {"filename": "/tests/metron_good/basic_literals.h", "start": 149592, "end": 150211}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 150211, "end": 150464}, {"filename": "/tests/metron_good/basic_output.h", "start": 150464, "end": 150732}, {"filename": "/tests/metron_good/basic_param.h", "start": 150732, "end": 150998}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 150998, "end": 151236}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 151236, "end": 151423}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 151423, "end": 151693}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 151693, "end": 151920}, {"filename": "/tests/metron_good/basic_submod.h", "start": 151920, "end": 152233}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 152233, "end": 152595}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 152595, "end": 152978}, {"filename": "/tests/metron_good/basic_switch.h", "start": 152978, "end": 153462}, {"filename": "/tests/metron_good/basic_task.h", "start": 153462, "end": 153803}, {"filename": "/tests/metron_good/basic_template.h", "start": 153803, "end": 154296}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 154296, "end": 154462}, {"filename": "/tests/metron_good/bit_casts.h", "start": 154462, "end": 160442}, {"filename": "/tests/metron_good/bit_concat.h", "start": 160442, "end": 160876}, {"filename": "/tests/metron_good/bit_dup.h", "start": 160876, "end": 161542}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 161542, "end": 162444}, {"filename": "/tests/metron_good/builtins.h", "start": 162444, "end": 162782}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 162782, "end": 163096}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 163096, "end": 163657}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 163657, "end": 164505}, {"filename": "/tests/metron_good/constructor_args.h", "start": 164505, "end": 164971}, {"filename": "/tests/metron_good/defines.h", "start": 164971, "end": 165292}, {"filename": "/tests/metron_good/dontcare.h", "start": 165292, "end": 165579}, {"filename": "/tests/metron_good/enum_simple.h", "start": 165579, "end": 166974}, {"filename": "/tests/metron_good/for_loops.h", "start": 166974, "end": 167301}, {"filename": "/tests/metron_good/good_order.h", "start": 167301, "end": 167604}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 167604, "end": 168022}, {"filename": "/tests/metron_good/include_guards.h", "start": 168022, "end": 168219}, {"filename": "/tests/metron_good/init_chain.h", "start": 168219, "end": 168934}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 168934, "end": 169226}, {"filename": "/tests/metron_good/input_signals.h", "start": 169226, "end": 170018}, {"filename": "/tests/metron_good/local_localparam.h", "start": 170018, "end": 170203}, {"filename": "/tests/metron_good/magic_comments.h", "start": 170203, "end": 170513}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 170513, "end": 170834}, {"filename": "/tests/metron_good/minimal.h", "start": 170834, "end": 171014}, {"filename": "/tests/metron_good/multi_tick.h", "start": 171014, "end": 171387}, {"filename": "/tests/metron_good/namespaces.h", "start": 171387, "end": 171744}, {"filename": "/tests/metron_good/nested_structs.h", "start": 171744, "end": 172198}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 172198, "end": 172750}, {"filename": "/tests/metron_good/oneliners.h", "start": 172750, "end": 173020}, {"filename": "/tests/metron_good/plus_equals.h", "start": 173020, "end": 173444}, {"filename": "/tests/metron_good/private_getter.h", "start": 173444, "end": 173675}, {"filename": "/tests/metron_good/structs.h", "start": 173675, "end": 173901}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 173901, "end": 174444}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 174444, "end": 176995}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 176995, "end": 177756}, {"filename": "/tests/metron_good/tock_task.h", "start": 177756, "end": 178131}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 178131, "end": 178303}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 178303, "end": 178969}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 178969, "end": 179635}], "remote_package_size": 179635});

  })();
