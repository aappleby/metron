
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22810}, {"filename": "/examples/ibex/README.md", "start": 22810, "end": 22861}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22861, "end": 24446}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24446, "end": 36544}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36544, "end": 50954}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50954, "end": 66978}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 66978, "end": 69479}, {"filename": "/examples/j1/metron/dpram.h", "start": 69479, "end": 69917}, {"filename": "/examples/j1/metron/j1.h", "start": 69917, "end": 73942}, {"filename": "/examples/pong/README.md", "start": 73942, "end": 74002}, {"filename": "/examples/pong/metron/pong.h", "start": 74002, "end": 77866}, {"filename": "/examples/pong/reference/README.md", "start": 77866, "end": 77926}, {"filename": "/examples/rvsimple/README.md", "start": 77926, "end": 78005}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78005, "end": 78505}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78505, "end": 79966}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 79966, "end": 82587}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82587, "end": 83802}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83802, "end": 89521}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89521, "end": 90635}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90635, "end": 92573}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92573, "end": 93813}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93813, "end": 94966}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 94966, "end": 95648}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95648, "end": 96521}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96521, "end": 98639}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 98639, "end": 99403}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99403, "end": 100094}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100094, "end": 100921}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 100921, "end": 101931}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 101931, "end": 102902}, {"filename": "/examples/rvsimple/metron/register.h", "start": 102902, "end": 103587}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 103587, "end": 106630}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 106630, "end": 112224}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112224, "end": 114717}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 114717, "end": 119444}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119444, "end": 121292}, {"filename": "/examples/scratch.h", "start": 121292, "end": 121499}, {"filename": "/examples/tutorial/adder.h", "start": 121499, "end": 121679}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 121679, "end": 122660}, {"filename": "/examples/tutorial/blockram.h", "start": 122660, "end": 123177}, {"filename": "/examples/tutorial/checksum.h", "start": 123177, "end": 123900}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 123900, "end": 124436}, {"filename": "/examples/tutorial/counter.h", "start": 124436, "end": 124585}, {"filename": "/examples/tutorial/declaration_order.h", "start": 124585, "end": 125364}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 125364, "end": 126782}, {"filename": "/examples/tutorial/nonblocking.h", "start": 126782, "end": 126904}, {"filename": "/examples/tutorial/submodules.h", "start": 126904, "end": 128021}, {"filename": "/examples/tutorial/templates.h", "start": 128021, "end": 128492}, {"filename": "/examples/tutorial/tutorial2.h", "start": 128492, "end": 128560}, {"filename": "/examples/tutorial/tutorial3.h", "start": 128560, "end": 128601}, {"filename": "/examples/tutorial/tutorial4.h", "start": 128601, "end": 128642}, {"filename": "/examples/tutorial/tutorial5.h", "start": 128642, "end": 128683}, {"filename": "/examples/tutorial/vga.h", "start": 128683, "end": 129830}, {"filename": "/examples/uart/README.md", "start": 129830, "end": 130074}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 130074, "end": 132654}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 132654, "end": 135215}, {"filename": "/examples/uart/metron/uart_top.h", "start": 135215, "end": 136979}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 136979, "end": 139998}, {"filename": "/tests/metron_bad/README.md", "start": 139998, "end": 140195}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 140195, "end": 140491}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 140491, "end": 140739}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 140739, "end": 140982}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 140982, "end": 141577}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 141577, "end": 141717}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 141717, "end": 142127}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 142127, "end": 142667}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 142667, "end": 142978}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 142978, "end": 143424}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 143424, "end": 143684}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 143684, "end": 143956}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 143956, "end": 144463}, {"filename": "/tests/metron_good/README.md", "start": 144463, "end": 144544}, {"filename": "/tests/metron_good/all_func_types.h", "start": 144544, "end": 146109}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 146109, "end": 146516}, {"filename": "/tests/metron_good/basic_function.h", "start": 146516, "end": 146795}, {"filename": "/tests/metron_good/basic_increment.h", "start": 146795, "end": 147150}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 147150, "end": 147445}, {"filename": "/tests/metron_good/basic_literals.h", "start": 147445, "end": 148057}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 148057, "end": 148303}, {"filename": "/tests/metron_good/basic_output.h", "start": 148303, "end": 148564}, {"filename": "/tests/metron_good/basic_param.h", "start": 148564, "end": 148823}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 148823, "end": 149054}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 149054, "end": 149234}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 149234, "end": 149497}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 149497, "end": 149717}, {"filename": "/tests/metron_good/basic_submod.h", "start": 149717, "end": 150006}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 150006, "end": 150361}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 150361, "end": 150737}, {"filename": "/tests/metron_good/basic_switch.h", "start": 150737, "end": 151214}, {"filename": "/tests/metron_good/basic_task.h", "start": 151214, "end": 151548}, {"filename": "/tests/metron_good/basic_template.h", "start": 151548, "end": 151979}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 151979, "end": 152138}, {"filename": "/tests/metron_good/bit_casts.h", "start": 152138, "end": 158111}, {"filename": "/tests/metron_good/bit_concat.h", "start": 158111, "end": 158538}, {"filename": "/tests/metron_good/bit_dup.h", "start": 158538, "end": 159197}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 159197, "end": 160035}, {"filename": "/tests/metron_good/builtins.h", "start": 160035, "end": 160366}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 160366, "end": 160673}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 160673, "end": 161227}, {"filename": "/tests/metron_good/constructor_args.h", "start": 161227, "end": 161382}, {"filename": "/tests/metron_good/defines.h", "start": 161382, "end": 161696}, {"filename": "/tests/metron_good/dontcare.h", "start": 161696, "end": 161979}, {"filename": "/tests/metron_good/enum_simple.h", "start": 161979, "end": 163347}, {"filename": "/tests/metron_good/for_loops.h", "start": 163347, "end": 163667}, {"filename": "/tests/metron_good/good_order.h", "start": 163667, "end": 163963}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 163963, "end": 164374}, {"filename": "/tests/metron_good/include_guards.h", "start": 164374, "end": 164571}, {"filename": "/tests/metron_good/init_chain.h", "start": 164571, "end": 165279}, {"filename": "/tests/metron_good/input_signals.h", "start": 165279, "end": 165941}, {"filename": "/tests/metron_good/local_localparam.h", "start": 165941, "end": 166119}, {"filename": "/tests/metron_good/magic_comments.h", "start": 166119, "end": 166422}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 166422, "end": 166736}, {"filename": "/tests/metron_good/minimal.h", "start": 166736, "end": 166811}, {"filename": "/tests/metron_good/multi_tick.h", "start": 166811, "end": 167177}, {"filename": "/tests/metron_good/namespaces.h", "start": 167177, "end": 167527}, {"filename": "/tests/metron_good/nested_structs.h", "start": 167527, "end": 168040}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 168040, "end": 168585}, {"filename": "/tests/metron_good/oneliners.h", "start": 168585, "end": 168848}, {"filename": "/tests/metron_good/plus_equals.h", "start": 168848, "end": 169198}, {"filename": "/tests/metron_good/private_getter.h", "start": 169198, "end": 169422}, {"filename": "/tests/metron_good/structs.h", "start": 169422, "end": 169641}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 169641, "end": 170395}, {"filename": "/tests/metron_good/tock_task.h", "start": 170395, "end": 170751}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 170751, "end": 170916}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 170916, "end": 171575}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 171575, "end": 172234}], "remote_package_size": 172234});

  })();
