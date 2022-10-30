
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22810}, {"filename": "/examples/ibex/README.md", "start": 22810, "end": 22861}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22861, "end": 24446}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24446, "end": 36544}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36544, "end": 50954}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50954, "end": 66978}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 66978, "end": 69479}, {"filename": "/examples/j1/metron/dpram.h", "start": 69479, "end": 69917}, {"filename": "/examples/j1/metron/j1.h", "start": 69917, "end": 73942}, {"filename": "/examples/pinwheel/README.md", "start": 73942, "end": 74106}, {"filename": "/examples/pinwheel/metron/pinwheel.h", "start": 74106, "end": 83245}, {"filename": "/examples/pong/README.md", "start": 83245, "end": 83305}, {"filename": "/examples/pong/metron/pong.h", "start": 83305, "end": 87169}, {"filename": "/examples/pong/reference/README.md", "start": 87169, "end": 87229}, {"filename": "/examples/rvsimple/README.md", "start": 87229, "end": 87308}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 87308, "end": 87808}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 87808, "end": 89269}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 89269, "end": 91890}, {"filename": "/examples/rvsimple/metron/config.h", "start": 91890, "end": 93105}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 93105, "end": 98824}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 98824, "end": 99938}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 99938, "end": 101876}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 101876, "end": 103110}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 103110, "end": 104263}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 104263, "end": 104939}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 104939, "end": 105812}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 105812, "end": 107930}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 107930, "end": 108694}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 108694, "end": 109385}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 109385, "end": 110212}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 110212, "end": 111222}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 111222, "end": 112193}, {"filename": "/examples/rvsimple/metron/register.h", "start": 112193, "end": 112878}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 112878, "end": 115921}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 115921, "end": 121515}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 121515, "end": 124008}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 124008, "end": 128735}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 128735, "end": 130583}, {"filename": "/examples/scratch.h", "start": 130583, "end": 130790}, {"filename": "/examples/tutorial/adder.h", "start": 130790, "end": 130970}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 130970, "end": 131951}, {"filename": "/examples/tutorial/blockram.h", "start": 131951, "end": 132468}, {"filename": "/examples/tutorial/checksum.h", "start": 132468, "end": 133191}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 133191, "end": 133727}, {"filename": "/examples/tutorial/counter.h", "start": 133727, "end": 133876}, {"filename": "/examples/tutorial/declaration_order.h", "start": 133876, "end": 134655}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 134655, "end": 136073}, {"filename": "/examples/tutorial/nonblocking.h", "start": 136073, "end": 136195}, {"filename": "/examples/tutorial/submodules.h", "start": 136195, "end": 137312}, {"filename": "/examples/tutorial/templates.h", "start": 137312, "end": 137783}, {"filename": "/examples/tutorial/tutorial2.h", "start": 137783, "end": 137851}, {"filename": "/examples/tutorial/tutorial3.h", "start": 137851, "end": 137892}, {"filename": "/examples/tutorial/tutorial4.h", "start": 137892, "end": 137933}, {"filename": "/examples/tutorial/tutorial5.h", "start": 137933, "end": 137974}, {"filename": "/examples/tutorial/vga.h", "start": 137974, "end": 139121}, {"filename": "/examples/uart/README.md", "start": 139121, "end": 139365}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 139365, "end": 141945}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 141945, "end": 144506}, {"filename": "/examples/uart/metron/uart_top.h", "start": 144506, "end": 146270}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 146270, "end": 149289}, {"filename": "/tests/metron_bad/README.md", "start": 149289, "end": 149486}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 149486, "end": 149782}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 149782, "end": 150030}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 150030, "end": 150273}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 150273, "end": 150868}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 150868, "end": 151008}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 151008, "end": 151418}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 151418, "end": 151958}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 151958, "end": 152269}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 152269, "end": 152715}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 152715, "end": 152975}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 152975, "end": 153247}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 153247, "end": 153754}, {"filename": "/tests/metron_good/README.md", "start": 153754, "end": 153835}, {"filename": "/tests/metron_good/all_func_types.h", "start": 153835, "end": 155400}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 155400, "end": 155807}, {"filename": "/tests/metron_good/basic_function.h", "start": 155807, "end": 156086}, {"filename": "/tests/metron_good/basic_increment.h", "start": 156086, "end": 156441}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 156441, "end": 156736}, {"filename": "/tests/metron_good/basic_literals.h", "start": 156736, "end": 157348}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 157348, "end": 157594}, {"filename": "/tests/metron_good/basic_output.h", "start": 157594, "end": 157855}, {"filename": "/tests/metron_good/basic_param.h", "start": 157855, "end": 158114}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 158114, "end": 158345}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 158345, "end": 158525}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 158525, "end": 158788}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 158788, "end": 159008}, {"filename": "/tests/metron_good/basic_submod.h", "start": 159008, "end": 159297}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 159297, "end": 159652}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 159652, "end": 160028}, {"filename": "/tests/metron_good/basic_switch.h", "start": 160028, "end": 160505}, {"filename": "/tests/metron_good/basic_task.h", "start": 160505, "end": 160839}, {"filename": "/tests/metron_good/basic_template.h", "start": 160839, "end": 161270}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 161270, "end": 161429}, {"filename": "/tests/metron_good/bit_casts.h", "start": 161429, "end": 167402}, {"filename": "/tests/metron_good/bit_concat.h", "start": 167402, "end": 167829}, {"filename": "/tests/metron_good/bit_dup.h", "start": 167829, "end": 168488}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 168488, "end": 169326}, {"filename": "/tests/metron_good/builtins.h", "start": 169326, "end": 169657}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 169657, "end": 169964}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 169964, "end": 170518}, {"filename": "/tests/metron_good/defines.h", "start": 170518, "end": 170832}, {"filename": "/tests/metron_good/dontcare.h", "start": 170832, "end": 171115}, {"filename": "/tests/metron_good/enum_simple.h", "start": 171115, "end": 172483}, {"filename": "/tests/metron_good/for_loops.h", "start": 172483, "end": 172803}, {"filename": "/tests/metron_good/good_order.h", "start": 172803, "end": 173099}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 173099, "end": 173510}, {"filename": "/tests/metron_good/include_guards.h", "start": 173510, "end": 173707}, {"filename": "/tests/metron_good/init_chain.h", "start": 173707, "end": 174415}, {"filename": "/tests/metron_good/input_signals.h", "start": 174415, "end": 175077}, {"filename": "/tests/metron_good/local_localparam.h", "start": 175077, "end": 175255}, {"filename": "/tests/metron_good/magic_comments.h", "start": 175255, "end": 175558}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 175558, "end": 175872}, {"filename": "/tests/metron_good/minimal.h", "start": 175872, "end": 175947}, {"filename": "/tests/metron_good/multi_tick.h", "start": 175947, "end": 176313}, {"filename": "/tests/metron_good/namespaces.h", "start": 176313, "end": 176663}, {"filename": "/tests/metron_good/nested_structs.h", "start": 176663, "end": 177176}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 177176, "end": 177721}, {"filename": "/tests/metron_good/oneliners.h", "start": 177721, "end": 177984}, {"filename": "/tests/metron_good/plus_equals.h", "start": 177984, "end": 178334}, {"filename": "/tests/metron_good/private_getter.h", "start": 178334, "end": 178558}, {"filename": "/tests/metron_good/structs.h", "start": 178558, "end": 178777}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 178777, "end": 179531}, {"filename": "/tests/metron_good/tock_task.h", "start": 179531, "end": 179887}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 179887, "end": 180052}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 180052, "end": 180711}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 180711, "end": 181370}], "remote_package_size": 181370});

  })();
