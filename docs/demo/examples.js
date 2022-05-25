
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
      var PACKAGE_UUID = metadata['package_uuid'];

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
Module['FS_createPath']("/examples", "rvtiny", true, true);
Module['FS_createPath']("/examples/rvtiny", "metron", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples", "rvtiny_sync", true, true);
Module['FS_createPath']("/examples/rvtiny_sync", "metron", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/examples/uart", "metron", true, true);
Module['FS_createPath']("/", "tests", true, true);
Module['FS_createPath']("/tests", "metron_good", true, true);
Module['FS_createPath']("/tests", "metron_bad", true, true);

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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 402}, {"filename": "/examples/rvtiny/main_vl.cpp", "start": 402, "end": 3313}, {"filename": "/examples/rvtiny/README.md", "start": 3313, "end": 3477}, {"filename": "/examples/rvtiny/main.cpp", "start": 3477, "end": 6214}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 6214, "end": 12310}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 12310, "end": 15212}, {"filename": "/examples/rvsimple/README.md", "start": 15212, "end": 15291}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 15291, "end": 18193}, {"filename": "/examples/rvsimple/main.cpp", "start": 18193, "end": 20980}, {"filename": "/examples/rvsimple/metron/register.h", "start": 20980, "end": 21665}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 21665, "end": 22818}, {"filename": "/examples/rvsimple/metron/config.h", "start": 22818, "end": 24033}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 24033, "end": 24797}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 24797, "end": 27290}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 27290, "end": 29138}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 29138, "end": 29814}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 29814, "end": 34541}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 34541, "end": 35655}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 35655, "end": 36626}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 36626, "end": 37860}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 37860, "end": 39798}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 39798, "end": 40298}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 40298, "end": 43341}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 43341, "end": 48935}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 48935, "end": 49945}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 49945, "end": 50772}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 50772, "end": 53393}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 53393, "end": 54854}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 54854, "end": 60573}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 60573, "end": 61446}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 61446, "end": 63564}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 63564, "end": 64255}, {"filename": "/examples/pong/main_vl.cpp", "start": 64255, "end": 64406}, {"filename": "/examples/pong/README.md", "start": 64406, "end": 64466}, {"filename": "/examples/pong/main.cpp", "start": 64466, "end": 66412}, {"filename": "/examples/pong/metron/pong.h", "start": 66412, "end": 70276}, {"filename": "/examples/pong/reference/README.md", "start": 70276, "end": 70336}, {"filename": "/examples/ibex/README.md", "start": 70336, "end": 70387}, {"filename": "/examples/ibex/main.cpp", "start": 70387, "end": 70528}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 70528, "end": 84938}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 84938, "end": 97036}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 97036, "end": 98621}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 98621, "end": 114645}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 114645, "end": 117146}, {"filename": "/examples/rvtiny_sync/main_vl.cpp", "start": 117146, "end": 120062}, {"filename": "/examples/rvtiny_sync/README.md", "start": 120062, "end": 120114}, {"filename": "/examples/rvtiny_sync/main.cpp", "start": 120114, "end": 122870}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 122870, "end": 129825}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129825, "end": 129866}, {"filename": "/examples/tutorial/vga.h", "start": 129866, "end": 131013}, {"filename": "/examples/tutorial/declaration_order.h", "start": 131013, "end": 131792}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 131792, "end": 132177}, {"filename": "/examples/tutorial/tutorial3.h", "start": 132177, "end": 132218}, {"filename": "/examples/tutorial/submodules.h", "start": 132218, "end": 133405}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 133405, "end": 134823}, {"filename": "/examples/tutorial/adder.h", "start": 134823, "end": 135003}, {"filename": "/examples/tutorial/tutorial4.h", "start": 135003, "end": 135044}, {"filename": "/examples/tutorial/blockram.h", "start": 135044, "end": 135423}, {"filename": "/examples/tutorial/tutorial2.h", "start": 135423, "end": 135491}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 135491, "end": 136472}, {"filename": "/examples/tutorial/counter.h", "start": 136472, "end": 136621}, {"filename": "/examples/tutorial/nonblocking.h", "start": 136621, "end": 136853}, {"filename": "/examples/uart/main_vl.cpp", "start": 136853, "end": 138347}, {"filename": "/examples/uart/README.md", "start": 138347, "end": 138591}, {"filename": "/examples/uart/main.cpp", "start": 138591, "end": 139355}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 139355, "end": 140648}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 140648, "end": 142011}, {"filename": "/examples/uart/metron/uart_top.h", "start": 142011, "end": 143071}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 143071, "end": 144917}, {"filename": "/tests/metron_good/include_guards.h", "start": 144917, "end": 145114}, {"filename": "/tests/metron_good/oneliners.h", "start": 145114, "end": 145377}, {"filename": "/tests/metron_good/enum_simple.h", "start": 145377, "end": 146745}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 146745, "end": 146910}, {"filename": "/tests/metron_good/basic_increment.h", "start": 146910, "end": 147265}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 147265, "end": 148103}, {"filename": "/tests/metron_good/magic_comments.h", "start": 148103, "end": 148406}, {"filename": "/tests/metron_good/for_loops.h", "start": 148406, "end": 148726}, {"filename": "/tests/metron_good/defines.h", "start": 148726, "end": 149040}, {"filename": "/tests/metron_good/bit_concat.h", "start": 149040, "end": 149467}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 149467, "end": 149647}, {"filename": "/tests/metron_good/force_tick.h", "start": 149647, "end": 149829}, {"filename": "/tests/metron_good/README.md", "start": 149829, "end": 149910}, {"filename": "/tests/metron_good/good_order.h", "start": 149910, "end": 150206}, {"filename": "/tests/metron_good/basic_template.h", "start": 150206, "end": 150637}, {"filename": "/tests/metron_good/bit_casts.h", "start": 150637, "end": 156610}, {"filename": "/tests/metron_good/tick_with_return_value.h", "start": 156610, "end": 156882}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 156882, "end": 157189}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 157189, "end": 157848}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 157848, "end": 158203}, {"filename": "/tests/metron_good/tock_task.h", "start": 158203, "end": 158559}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 158559, "end": 158779}, {"filename": "/tests/metron_good/all_func_types.h", "start": 158779, "end": 160344}, {"filename": "/tests/metron_good/basic_submod.h", "start": 160344, "end": 160633}, {"filename": "/tests/metron_good/dontcare.h", "start": 160633, "end": 160916}, {"filename": "/tests/metron_good/basic_param.h", "start": 160916, "end": 161175}, {"filename": "/tests/metron_good/basic_output.h", "start": 161175, "end": 161436}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 161436, "end": 162190}, {"filename": "/tests/metron_good/basic_function.h", "start": 162190, "end": 162469}, {"filename": "/tests/metron_good/builtins.h", "start": 162469, "end": 162800}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 162800, "end": 163046}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 163046, "end": 163453}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 163453, "end": 163684}, {"filename": "/tests/metron_good/basic_literals.h", "start": 163684, "end": 164296}, {"filename": "/tests/metron_good/basic_switch.h", "start": 164296, "end": 164773}, {"filename": "/tests/metron_good/bit_dup.h", "start": 164773, "end": 165432}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 165432, "end": 166091}, {"filename": "/tests/metron_good/local_localparam.h", "start": 166091, "end": 166269}, {"filename": "/tests/metron_good/minimal.h", "start": 166269, "end": 166344}, {"filename": "/tests/metron_good/multi_tick.h", "start": 166344, "end": 166710}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 166710, "end": 166973}, {"filename": "/tests/metron_good/namespaces.h", "start": 166973, "end": 167323}, {"filename": "/tests/metron_good/structs.h", "start": 167323, "end": 167542}, {"filename": "/tests/metron_good/basic_task.h", "start": 167542, "end": 167876}, {"filename": "/tests/metron_good/nested_structs.h", "start": 167876, "end": 168389}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 168389, "end": 168703}, {"filename": "/tests/metron_good/private_getter.h", "start": 168703, "end": 168927}, {"filename": "/tests/metron_good/init_chain.h", "start": 168927, "end": 169635}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 169635, "end": 170011}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 170011, "end": 170422}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 170422, "end": 170976}, {"filename": "/tests/metron_good/input_signals.h", "start": 170976, "end": 171638}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171638, "end": 172183}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 172183, "end": 172342}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 172342, "end": 172637}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 172637, "end": 172777}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 172777, "end": 173187}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 173187, "end": 173727}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 173727, "end": 173975}, {"filename": "/tests/metron_bad/README.md", "start": 173975, "end": 174172}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 174172, "end": 174483}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 174483, "end": 174990}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 174990, "end": 175585}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 175585, "end": 175845}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 175845, "end": 176088}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 176088, "end": 176384}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 176384, "end": 176830}], "remote_package_size": 176830, "package_uuid": "184b5068-06ab-4db8-a139-20b935c270c3"});

  })();
