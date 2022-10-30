
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
Module['FS_createPath']("/examples", "pinwheel", true, true);
Module['FS_createPath']("/examples/pinwheel", "metron", true, true);
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22810}, {"filename": "/examples/ibex/README.md", "start": 22810, "end": 22861}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22861, "end": 24446}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24446, "end": 36544}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36544, "end": 50954}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50954, "end": 66978}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 66978, "end": 69479}, {"filename": "/examples/j1/metron/dpram.h", "start": 69479, "end": 69917}, {"filename": "/examples/j1/metron/j1.h", "start": 69917, "end": 73942}, {"filename": "/examples/pinwheel/README.md", "start": 73942, "end": 74106}, {"filename": "/examples/pinwheel/metron/pinwheel.h", "start": 74106, "end": 83218}, {"filename": "/examples/pong/README.md", "start": 83218, "end": 83278}, {"filename": "/examples/pong/metron/pong.h", "start": 83278, "end": 87142}, {"filename": "/examples/pong/reference/README.md", "start": 87142, "end": 87202}, {"filename": "/examples/rvsimple/README.md", "start": 87202, "end": 87281}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 87281, "end": 87781}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 87781, "end": 89242}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 89242, "end": 91863}, {"filename": "/examples/rvsimple/metron/config.h", "start": 91863, "end": 93078}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 93078, "end": 98797}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 98797, "end": 99911}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 99911, "end": 101849}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 101849, "end": 103083}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 103083, "end": 104236}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 104236, "end": 104912}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 104912, "end": 105785}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 105785, "end": 107903}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 107903, "end": 108667}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 108667, "end": 109358}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 109358, "end": 110185}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 110185, "end": 111195}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 111195, "end": 112166}, {"filename": "/examples/rvsimple/metron/register.h", "start": 112166, "end": 112851}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 112851, "end": 115894}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 115894, "end": 121488}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 121488, "end": 123981}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 123981, "end": 128708}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 128708, "end": 130556}, {"filename": "/examples/scratch.h", "start": 130556, "end": 130763}, {"filename": "/examples/tutorial/adder.h", "start": 130763, "end": 130943}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 130943, "end": 131924}, {"filename": "/examples/tutorial/blockram.h", "start": 131924, "end": 132441}, {"filename": "/examples/tutorial/checksum.h", "start": 132441, "end": 133164}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 133164, "end": 133700}, {"filename": "/examples/tutorial/counter.h", "start": 133700, "end": 133849}, {"filename": "/examples/tutorial/declaration_order.h", "start": 133849, "end": 134628}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 134628, "end": 136046}, {"filename": "/examples/tutorial/nonblocking.h", "start": 136046, "end": 136168}, {"filename": "/examples/tutorial/submodules.h", "start": 136168, "end": 137285}, {"filename": "/examples/tutorial/templates.h", "start": 137285, "end": 137756}, {"filename": "/examples/tutorial/tutorial2.h", "start": 137756, "end": 137824}, {"filename": "/examples/tutorial/tutorial3.h", "start": 137824, "end": 137865}, {"filename": "/examples/tutorial/tutorial4.h", "start": 137865, "end": 137906}, {"filename": "/examples/tutorial/tutorial5.h", "start": 137906, "end": 137947}, {"filename": "/examples/tutorial/vga.h", "start": 137947, "end": 139094}, {"filename": "/examples/uart/README.md", "start": 139094, "end": 139338}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 139338, "end": 141918}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 141918, "end": 144479}, {"filename": "/examples/uart/metron/uart_top.h", "start": 144479, "end": 146243}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 146243, "end": 149262}, {"filename": "/tests/metron_bad/README.md", "start": 149262, "end": 149459}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 149459, "end": 149755}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 149755, "end": 150003}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 150003, "end": 150246}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 150246, "end": 150841}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 150841, "end": 150981}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 150981, "end": 151391}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 151391, "end": 151931}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 151931, "end": 152242}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 152242, "end": 152688}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 152688, "end": 152948}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 152948, "end": 153220}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 153220, "end": 153727}, {"filename": "/tests/metron_good/README.md", "start": 153727, "end": 153808}, {"filename": "/tests/metron_good/all_func_types.h", "start": 153808, "end": 155373}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 155373, "end": 155780}, {"filename": "/tests/metron_good/basic_function.h", "start": 155780, "end": 156059}, {"filename": "/tests/metron_good/basic_increment.h", "start": 156059, "end": 156414}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 156414, "end": 156709}, {"filename": "/tests/metron_good/basic_literals.h", "start": 156709, "end": 157321}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 157321, "end": 157567}, {"filename": "/tests/metron_good/basic_output.h", "start": 157567, "end": 157828}, {"filename": "/tests/metron_good/basic_param.h", "start": 157828, "end": 158087}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 158087, "end": 158318}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 158318, "end": 158498}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 158498, "end": 158761}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 158761, "end": 158981}, {"filename": "/tests/metron_good/basic_submod.h", "start": 158981, "end": 159270}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 159270, "end": 159625}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 159625, "end": 160001}, {"filename": "/tests/metron_good/basic_switch.h", "start": 160001, "end": 160478}, {"filename": "/tests/metron_good/basic_task.h", "start": 160478, "end": 160812}, {"filename": "/tests/metron_good/basic_template.h", "start": 160812, "end": 161243}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 161243, "end": 161402}, {"filename": "/tests/metron_good/bit_casts.h", "start": 161402, "end": 167375}, {"filename": "/tests/metron_good/bit_concat.h", "start": 167375, "end": 167802}, {"filename": "/tests/metron_good/bit_dup.h", "start": 167802, "end": 168461}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 168461, "end": 169299}, {"filename": "/tests/metron_good/builtins.h", "start": 169299, "end": 169630}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 169630, "end": 169937}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 169937, "end": 170491}, {"filename": "/tests/metron_good/defines.h", "start": 170491, "end": 170805}, {"filename": "/tests/metron_good/dontcare.h", "start": 170805, "end": 171088}, {"filename": "/tests/metron_good/enum_simple.h", "start": 171088, "end": 172456}, {"filename": "/tests/metron_good/for_loops.h", "start": 172456, "end": 172776}, {"filename": "/tests/metron_good/good_order.h", "start": 172776, "end": 173072}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 173072, "end": 173483}, {"filename": "/tests/metron_good/include_guards.h", "start": 173483, "end": 173680}, {"filename": "/tests/metron_good/init_chain.h", "start": 173680, "end": 174388}, {"filename": "/tests/metron_good/input_signals.h", "start": 174388, "end": 175050}, {"filename": "/tests/metron_good/local_localparam.h", "start": 175050, "end": 175228}, {"filename": "/tests/metron_good/magic_comments.h", "start": 175228, "end": 175531}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 175531, "end": 175845}, {"filename": "/tests/metron_good/minimal.h", "start": 175845, "end": 175920}, {"filename": "/tests/metron_good/multi_tick.h", "start": 175920, "end": 176286}, {"filename": "/tests/metron_good/namespaces.h", "start": 176286, "end": 176636}, {"filename": "/tests/metron_good/nested_structs.h", "start": 176636, "end": 177149}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 177149, "end": 177694}, {"filename": "/tests/metron_good/oneliners.h", "start": 177694, "end": 177957}, {"filename": "/tests/metron_good/plus_equals.h", "start": 177957, "end": 178307}, {"filename": "/tests/metron_good/private_getter.h", "start": 178307, "end": 178531}, {"filename": "/tests/metron_good/structs.h", "start": 178531, "end": 178750}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 178750, "end": 179504}, {"filename": "/tests/metron_good/tock_task.h", "start": 179504, "end": 179860}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 179860, "end": 180025}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 180025, "end": 180684}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 180684, "end": 181343}], "remote_package_size": 181343});

  })();
