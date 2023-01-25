
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
Module['FS_createPath']("/examples/rvsimple", "metron_sv", true, true);
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/ibex/README.md", "start": 26876, "end": 26927}, {"filename": "/examples/ibex/main.cpp", "start": 26927, "end": 27068}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 27068, "end": 28653}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 28653, "end": 40751}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 40751, "end": 55161}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 55161, "end": 71185}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 71185, "end": 73686}, {"filename": "/examples/j1/main.cpp", "start": 73686, "end": 73805}, {"filename": "/examples/j1/metron/dpram.h", "start": 73805, "end": 74243}, {"filename": "/examples/j1/metron/j1.h", "start": 74243, "end": 78268}, {"filename": "/examples/pong/README.md", "start": 78268, "end": 78328}, {"filename": "/examples/pong/main.cpp", "start": 78328, "end": 80274}, {"filename": "/examples/pong/main_vl.cpp", "start": 80274, "end": 80425}, {"filename": "/examples/pong/metron/pong.h", "start": 80425, "end": 84289}, {"filename": "/examples/pong/reference/README.md", "start": 84289, "end": 84349}, {"filename": "/examples/rvsimple/README.md", "start": 84349, "end": 84428}, {"filename": "/examples/rvsimple/main.cpp", "start": 84428, "end": 87194}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 87194, "end": 90096}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 90096, "end": 93257}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 93257, "end": 93757}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 93757, "end": 95218}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 95218, "end": 97839}, {"filename": "/examples/rvsimple/metron/config.h", "start": 97839, "end": 99054}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 99054, "end": 104773}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 104773, "end": 105887}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 105887, "end": 107825}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 107825, "end": 109052}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 109052, "end": 110296}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 110296, "end": 110965}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 110965, "end": 111929}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 111929, "end": 114047}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 114047, "end": 114811}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 114811, "end": 115502}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 115502, "end": 116329}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 116329, "end": 117339}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 117339, "end": 118310}, {"filename": "/examples/rvsimple/metron/register.h", "start": 118310, "end": 118995}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 118995, "end": 122038}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 122038, "end": 127632}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 127632, "end": 130125}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 130125, "end": 134852}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 134852, "end": 136866}, {"filename": "/examples/rvsimple/metron_sv/example_text_memory.s", "start": 136866, "end": 137618}, {"filename": "/examples/scratch.h", "start": 137618, "end": 137878}, {"filename": "/examples/tutorial/adder.h", "start": 137878, "end": 138058}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 138058, "end": 139039}, {"filename": "/examples/tutorial/blockram.h", "start": 139039, "end": 139556}, {"filename": "/examples/tutorial/checksum.h", "start": 139556, "end": 140279}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 140279, "end": 140815}, {"filename": "/examples/tutorial/counter.h", "start": 140815, "end": 140964}, {"filename": "/examples/tutorial/declaration_order.h", "start": 140964, "end": 141743}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 141743, "end": 143161}, {"filename": "/examples/tutorial/nonblocking.h", "start": 143161, "end": 143283}, {"filename": "/examples/tutorial/submodules.h", "start": 143283, "end": 144400}, {"filename": "/examples/tutorial/templates.h", "start": 144400, "end": 144871}, {"filename": "/examples/tutorial/tutorial2.h", "start": 144871, "end": 144939}, {"filename": "/examples/tutorial/tutorial3.h", "start": 144939, "end": 144980}, {"filename": "/examples/tutorial/tutorial4.h", "start": 144980, "end": 145021}, {"filename": "/examples/tutorial/tutorial5.h", "start": 145021, "end": 145062}, {"filename": "/examples/tutorial/vga.h", "start": 145062, "end": 146209}, {"filename": "/examples/uart/README.md", "start": 146209, "end": 146453}, {"filename": "/examples/uart/main.cpp", "start": 146453, "end": 147752}, {"filename": "/examples/uart/main_vl.cpp", "start": 147752, "end": 150288}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 150288, "end": 152868}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 152868, "end": 155429}, {"filename": "/examples/uart/metron/uart_top.h", "start": 155429, "end": 157193}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 157193, "end": 160212}, {"filename": "/tests/metron_bad/README.md", "start": 160212, "end": 160409}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 160409, "end": 160705}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 160705, "end": 160953}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 160953, "end": 161196}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 161196, "end": 161791}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 161791, "end": 162201}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 162201, "end": 162741}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 162741, "end": 163052}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 163052, "end": 163498}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 163498, "end": 163758}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 163758, "end": 164030}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 164030, "end": 164537}, {"filename": "/tests/metron_good/README.md", "start": 164537, "end": 164618}, {"filename": "/tests/metron_good/all_func_types.h", "start": 164618, "end": 166183}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 166183, "end": 166590}, {"filename": "/tests/metron_good/basic_function.h", "start": 166590, "end": 166869}, {"filename": "/tests/metron_good/basic_increment.h", "start": 166869, "end": 167224}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 167224, "end": 167519}, {"filename": "/tests/metron_good/basic_literals.h", "start": 167519, "end": 168131}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 168131, "end": 168377}, {"filename": "/tests/metron_good/basic_output.h", "start": 168377, "end": 168638}, {"filename": "/tests/metron_good/basic_param.h", "start": 168638, "end": 168897}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 168897, "end": 169128}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 169128, "end": 169308}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 169308, "end": 169571}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 169571, "end": 169791}, {"filename": "/tests/metron_good/basic_submod.h", "start": 169791, "end": 170080}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 170080, "end": 170435}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 170435, "end": 170811}, {"filename": "/tests/metron_good/basic_switch.h", "start": 170811, "end": 171288}, {"filename": "/tests/metron_good/basic_task.h", "start": 171288, "end": 171622}, {"filename": "/tests/metron_good/basic_template.h", "start": 171622, "end": 172108}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 172108, "end": 172267}, {"filename": "/tests/metron_good/bit_casts.h", "start": 172267, "end": 178240}, {"filename": "/tests/metron_good/bit_concat.h", "start": 178240, "end": 178667}, {"filename": "/tests/metron_good/bit_dup.h", "start": 178667, "end": 179326}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 179326, "end": 180164}, {"filename": "/tests/metron_good/builtins.h", "start": 180164, "end": 180495}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 180495, "end": 180802}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 180802, "end": 181356}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 181356, "end": 182249}, {"filename": "/tests/metron_good/constructor_args.h", "start": 182249, "end": 182757}, {"filename": "/tests/metron_good/defines.h", "start": 182757, "end": 183071}, {"filename": "/tests/metron_good/dontcare.h", "start": 183071, "end": 183354}, {"filename": "/tests/metron_good/enum_simple.h", "start": 183354, "end": 184722}, {"filename": "/tests/metron_good/for_loops.h", "start": 184722, "end": 185042}, {"filename": "/tests/metron_good/good_order.h", "start": 185042, "end": 185338}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 185338, "end": 185749}, {"filename": "/tests/metron_good/include_guards.h", "start": 185749, "end": 185946}, {"filename": "/tests/metron_good/init_chain.h", "start": 185946, "end": 186654}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 186654, "end": 186959}, {"filename": "/tests/metron_good/input_signals.h", "start": 186959, "end": 187621}, {"filename": "/tests/metron_good/local_localparam.h", "start": 187621, "end": 187799}, {"filename": "/tests/metron_good/magic_comments.h", "start": 187799, "end": 188102}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 188102, "end": 188416}, {"filename": "/tests/metron_good/minimal.h", "start": 188416, "end": 188491}, {"filename": "/tests/metron_good/multi_tick.h", "start": 188491, "end": 188857}, {"filename": "/tests/metron_good/namespaces.h", "start": 188857, "end": 189207}, {"filename": "/tests/metron_good/nested_structs.h", "start": 189207, "end": 189720}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 189720, "end": 190265}, {"filename": "/tests/metron_good/oneliners.h", "start": 190265, "end": 190528}, {"filename": "/tests/metron_good/plus_equals.h", "start": 190528, "end": 190952}, {"filename": "/tests/metron_good/private_getter.h", "start": 190952, "end": 191176}, {"filename": "/tests/metron_good/structs.h", "start": 191176, "end": 191395}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 191395, "end": 191931}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 191931, "end": 194292}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 194292, "end": 195046}, {"filename": "/tests/metron_good/tock_task.h", "start": 195046, "end": 195402}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 195402, "end": 195567}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 195567, "end": 196226}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 196226, "end": 196885}], "remote_package_size": 196885});

  })();
