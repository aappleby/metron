
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22810}, {"filename": "/examples/ibex/README.md", "start": 22810, "end": 22861}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22861, "end": 24446}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24446, "end": 36544}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36544, "end": 50954}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50954, "end": 66978}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 66978, "end": 69479}, {"filename": "/examples/j1/metron/dpram.h", "start": 69479, "end": 69917}, {"filename": "/examples/j1/metron/j1.h", "start": 69917, "end": 73942}, {"filename": "/examples/pong/README.md", "start": 73942, "end": 74002}, {"filename": "/examples/pong/metron/pong.h", "start": 74002, "end": 77866}, {"filename": "/examples/pong/reference/README.md", "start": 77866, "end": 77926}, {"filename": "/examples/rvsimple/README.md", "start": 77926, "end": 78005}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78005, "end": 78505}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78505, "end": 79966}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 79966, "end": 82587}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82587, "end": 83802}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83802, "end": 89521}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89521, "end": 90635}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90635, "end": 92573}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92573, "end": 93813}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93813, "end": 94966}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 94966, "end": 95648}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95648, "end": 96521}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96521, "end": 98639}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 98639, "end": 99403}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99403, "end": 100094}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100094, "end": 100921}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 100921, "end": 101931}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 101931, "end": 102902}, {"filename": "/examples/rvsimple/metron/register.h", "start": 102902, "end": 103587}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 103587, "end": 106630}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 106630, "end": 112224}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112224, "end": 114717}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 114717, "end": 119444}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119444, "end": 121292}, {"filename": "/examples/scratch.h", "start": 121292, "end": 121499}, {"filename": "/examples/tutorial/adder.h", "start": 121499, "end": 121679}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 121679, "end": 122660}, {"filename": "/examples/tutorial/blockram.h", "start": 122660, "end": 123177}, {"filename": "/examples/tutorial/checksum.h", "start": 123177, "end": 123900}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 123900, "end": 124436}, {"filename": "/examples/tutorial/counter.h", "start": 124436, "end": 124585}, {"filename": "/examples/tutorial/declaration_order.h", "start": 124585, "end": 125364}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 125364, "end": 126782}, {"filename": "/examples/tutorial/nonblocking.h", "start": 126782, "end": 126904}, {"filename": "/examples/tutorial/submodules.h", "start": 126904, "end": 128021}, {"filename": "/examples/tutorial/templates.h", "start": 128021, "end": 128492}, {"filename": "/examples/tutorial/tutorial2.h", "start": 128492, "end": 128560}, {"filename": "/examples/tutorial/tutorial3.h", "start": 128560, "end": 128601}, {"filename": "/examples/tutorial/tutorial4.h", "start": 128601, "end": 128642}, {"filename": "/examples/tutorial/tutorial5.h", "start": 128642, "end": 128683}, {"filename": "/examples/tutorial/vga.h", "start": 128683, "end": 129830}, {"filename": "/examples/uart/README.md", "start": 129830, "end": 130074}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 130074, "end": 132654}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 132654, "end": 135215}, {"filename": "/examples/uart/metron/uart_top.h", "start": 135215, "end": 136979}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 136979, "end": 139998}, {"filename": "/tests/metron_bad/README.md", "start": 139998, "end": 140195}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 140195, "end": 140491}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 140491, "end": 140739}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 140739, "end": 140982}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 140982, "end": 141577}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 141577, "end": 141987}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 141987, "end": 142527}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 142527, "end": 142838}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 142838, "end": 143284}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 143284, "end": 143544}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 143544, "end": 143816}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 143816, "end": 144323}, {"filename": "/tests/metron_good/README.md", "start": 144323, "end": 144404}, {"filename": "/tests/metron_good/all_func_types.h", "start": 144404, "end": 145969}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 145969, "end": 146376}, {"filename": "/tests/metron_good/basic_function.h", "start": 146376, "end": 146655}, {"filename": "/tests/metron_good/basic_increment.h", "start": 146655, "end": 147010}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 147010, "end": 147305}, {"filename": "/tests/metron_good/basic_literals.h", "start": 147305, "end": 147917}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 147917, "end": 148163}, {"filename": "/tests/metron_good/basic_output.h", "start": 148163, "end": 148424}, {"filename": "/tests/metron_good/basic_param.h", "start": 148424, "end": 148683}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 148683, "end": 148914}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 148914, "end": 149094}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 149094, "end": 149357}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 149357, "end": 149577}, {"filename": "/tests/metron_good/basic_submod.h", "start": 149577, "end": 149866}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 149866, "end": 150221}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 150221, "end": 150597}, {"filename": "/tests/metron_good/basic_switch.h", "start": 150597, "end": 151074}, {"filename": "/tests/metron_good/basic_task.h", "start": 151074, "end": 151408}, {"filename": "/tests/metron_good/basic_template.h", "start": 151408, "end": 151894}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 151894, "end": 152053}, {"filename": "/tests/metron_good/bit_casts.h", "start": 152053, "end": 158026}, {"filename": "/tests/metron_good/bit_concat.h", "start": 158026, "end": 158453}, {"filename": "/tests/metron_good/bit_dup.h", "start": 158453, "end": 159112}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 159112, "end": 159950}, {"filename": "/tests/metron_good/builtins.h", "start": 159950, "end": 160281}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 160281, "end": 160588}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 160588, "end": 161142}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 161142, "end": 162035}, {"filename": "/tests/metron_good/constructor_args.h", "start": 162035, "end": 162543}, {"filename": "/tests/metron_good/defines.h", "start": 162543, "end": 162857}, {"filename": "/tests/metron_good/dontcare.h", "start": 162857, "end": 163140}, {"filename": "/tests/metron_good/enum_simple.h", "start": 163140, "end": 164508}, {"filename": "/tests/metron_good/for_loops.h", "start": 164508, "end": 164828}, {"filename": "/tests/metron_good/good_order.h", "start": 164828, "end": 165124}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 165124, "end": 165535}, {"filename": "/tests/metron_good/include_guards.h", "start": 165535, "end": 165732}, {"filename": "/tests/metron_good/init_chain.h", "start": 165732, "end": 166440}, {"filename": "/tests/metron_good/input_signals.h", "start": 166440, "end": 167102}, {"filename": "/tests/metron_good/local_localparam.h", "start": 167102, "end": 167280}, {"filename": "/tests/metron_good/magic_comments.h", "start": 167280, "end": 167583}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 167583, "end": 167897}, {"filename": "/tests/metron_good/minimal.h", "start": 167897, "end": 167972}, {"filename": "/tests/metron_good/multi_tick.h", "start": 167972, "end": 168338}, {"filename": "/tests/metron_good/namespaces.h", "start": 168338, "end": 168688}, {"filename": "/tests/metron_good/nested_structs.h", "start": 168688, "end": 169201}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 169201, "end": 169746}, {"filename": "/tests/metron_good/oneliners.h", "start": 169746, "end": 170009}, {"filename": "/tests/metron_good/plus_equals.h", "start": 170009, "end": 170359}, {"filename": "/tests/metron_good/private_getter.h", "start": 170359, "end": 170583}, {"filename": "/tests/metron_good/structs.h", "start": 170583, "end": 170802}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 170802, "end": 173163}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 173163, "end": 173917}, {"filename": "/tests/metron_good/tock_task.h", "start": 173917, "end": 174273}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 174273, "end": 174438}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 174438, "end": 175097}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 175097, "end": 175756}], "remote_package_size": 175756});

  })();
