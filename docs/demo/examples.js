
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
        assert(arrayBuffer instanceof ArrayBuffer, 'bad input to processPackageData');
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22810}, {"filename": "/examples/ibex/README.md", "start": 22810, "end": 22861}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22861, "end": 24446}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24446, "end": 36544}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36544, "end": 50954}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50954, "end": 66978}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 66978, "end": 69479}, {"filename": "/examples/j1/metron/dpram.h", "start": 69479, "end": 69917}, {"filename": "/examples/j1/metron/j1.h", "start": 69917, "end": 73942}, {"filename": "/examples/pinwheel/README.md", "start": 73942, "end": 74106}, {"filename": "/examples/pinwheel/metron/constants.h", "start": 74106, "end": 76664}, {"filename": "/examples/pinwheel/metron/pinwheel.h", "start": 76664, "end": 88246}, {"filename": "/examples/pong/README.md", "start": 88246, "end": 88306}, {"filename": "/examples/pong/metron/pong.h", "start": 88306, "end": 92170}, {"filename": "/examples/pong/reference/README.md", "start": 92170, "end": 92230}, {"filename": "/examples/rvsimple/README.md", "start": 92230, "end": 92309}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 92309, "end": 92809}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 92809, "end": 94270}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 94270, "end": 96891}, {"filename": "/examples/rvsimple/metron/config.h", "start": 96891, "end": 98106}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 98106, "end": 103825}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 103825, "end": 104939}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 104939, "end": 106877}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 106877, "end": 108111}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 108111, "end": 109264}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 109264, "end": 109940}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 109940, "end": 110813}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 110813, "end": 112931}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 112931, "end": 113695}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 113695, "end": 114386}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 114386, "end": 115213}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 115213, "end": 116223}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 116223, "end": 117194}, {"filename": "/examples/rvsimple/metron/register.h", "start": 117194, "end": 117879}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 117879, "end": 120922}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 120922, "end": 126516}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 126516, "end": 129009}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 129009, "end": 133736}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 133736, "end": 135584}, {"filename": "/examples/scratch.h", "start": 135584, "end": 135791}, {"filename": "/examples/tutorial/adder.h", "start": 135791, "end": 135971}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 135971, "end": 136952}, {"filename": "/examples/tutorial/blockram.h", "start": 136952, "end": 137469}, {"filename": "/examples/tutorial/checksum.h", "start": 137469, "end": 138192}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 138192, "end": 138728}, {"filename": "/examples/tutorial/counter.h", "start": 138728, "end": 138877}, {"filename": "/examples/tutorial/declaration_order.h", "start": 138877, "end": 139656}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 139656, "end": 141074}, {"filename": "/examples/tutorial/nonblocking.h", "start": 141074, "end": 141196}, {"filename": "/examples/tutorial/submodules.h", "start": 141196, "end": 142313}, {"filename": "/examples/tutorial/templates.h", "start": 142313, "end": 142784}, {"filename": "/examples/tutorial/tutorial2.h", "start": 142784, "end": 142852}, {"filename": "/examples/tutorial/tutorial3.h", "start": 142852, "end": 142893}, {"filename": "/examples/tutorial/tutorial4.h", "start": 142893, "end": 142934}, {"filename": "/examples/tutorial/tutorial5.h", "start": 142934, "end": 142975}, {"filename": "/examples/tutorial/vga.h", "start": 142975, "end": 144122}, {"filename": "/examples/uart/README.md", "start": 144122, "end": 144366}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 144366, "end": 146946}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 146946, "end": 149507}, {"filename": "/examples/uart/metron/uart_top.h", "start": 149507, "end": 151271}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 151271, "end": 154290}, {"filename": "/tests/metron_bad/README.md", "start": 154290, "end": 154487}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 154487, "end": 154783}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 154783, "end": 155031}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 155031, "end": 155274}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 155274, "end": 155869}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 155869, "end": 156009}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 156009, "end": 156419}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 156419, "end": 156959}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 156959, "end": 157270}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 157270, "end": 157716}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 157716, "end": 157976}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 157976, "end": 158248}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 158248, "end": 158755}, {"filename": "/tests/metron_good/README.md", "start": 158755, "end": 158836}, {"filename": "/tests/metron_good/all_func_types.h", "start": 158836, "end": 160401}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 160401, "end": 160808}, {"filename": "/tests/metron_good/basic_function.h", "start": 160808, "end": 161087}, {"filename": "/tests/metron_good/basic_increment.h", "start": 161087, "end": 161442}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 161442, "end": 161737}, {"filename": "/tests/metron_good/basic_literals.h", "start": 161737, "end": 162349}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 162349, "end": 162595}, {"filename": "/tests/metron_good/basic_output.h", "start": 162595, "end": 162856}, {"filename": "/tests/metron_good/basic_param.h", "start": 162856, "end": 163115}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 163115, "end": 163346}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 163346, "end": 163526}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 163526, "end": 163789}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 163789, "end": 164009}, {"filename": "/tests/metron_good/basic_submod.h", "start": 164009, "end": 164298}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 164298, "end": 164653}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 164653, "end": 165029}, {"filename": "/tests/metron_good/basic_switch.h", "start": 165029, "end": 165506}, {"filename": "/tests/metron_good/basic_task.h", "start": 165506, "end": 165840}, {"filename": "/tests/metron_good/basic_template.h", "start": 165840, "end": 166271}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 166271, "end": 166430}, {"filename": "/tests/metron_good/bit_casts.h", "start": 166430, "end": 172403}, {"filename": "/tests/metron_good/bit_concat.h", "start": 172403, "end": 172830}, {"filename": "/tests/metron_good/bit_dup.h", "start": 172830, "end": 173489}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 173489, "end": 174327}, {"filename": "/tests/metron_good/builtins.h", "start": 174327, "end": 174658}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 174658, "end": 174965}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 174965, "end": 175519}, {"filename": "/tests/metron_good/defines.h", "start": 175519, "end": 175833}, {"filename": "/tests/metron_good/dontcare.h", "start": 175833, "end": 176116}, {"filename": "/tests/metron_good/enum_simple.h", "start": 176116, "end": 177484}, {"filename": "/tests/metron_good/for_loops.h", "start": 177484, "end": 177804}, {"filename": "/tests/metron_good/good_order.h", "start": 177804, "end": 178100}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 178100, "end": 178511}, {"filename": "/tests/metron_good/include_guards.h", "start": 178511, "end": 178708}, {"filename": "/tests/metron_good/init_chain.h", "start": 178708, "end": 179416}, {"filename": "/tests/metron_good/input_signals.h", "start": 179416, "end": 180078}, {"filename": "/tests/metron_good/local_localparam.h", "start": 180078, "end": 180256}, {"filename": "/tests/metron_good/magic_comments.h", "start": 180256, "end": 180559}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 180559, "end": 180873}, {"filename": "/tests/metron_good/minimal.h", "start": 180873, "end": 180948}, {"filename": "/tests/metron_good/multi_tick.h", "start": 180948, "end": 181314}, {"filename": "/tests/metron_good/namespaces.h", "start": 181314, "end": 181664}, {"filename": "/tests/metron_good/nested_structs.h", "start": 181664, "end": 182177}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 182177, "end": 182722}, {"filename": "/tests/metron_good/oneliners.h", "start": 182722, "end": 182985}, {"filename": "/tests/metron_good/plus_equals.h", "start": 182985, "end": 183335}, {"filename": "/tests/metron_good/private_getter.h", "start": 183335, "end": 183559}, {"filename": "/tests/metron_good/structs.h", "start": 183559, "end": 183778}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 183778, "end": 184532}, {"filename": "/tests/metron_good/tock_task.h", "start": 184532, "end": 184888}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 184888, "end": 185053}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 185053, "end": 185712}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 185712, "end": 186371}], "remote_package_size": 186371});

  })();
