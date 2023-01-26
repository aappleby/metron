
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/ibex/README.md", "start": 26876, "end": 26927}, {"filename": "/examples/ibex/main.cpp", "start": 26927, "end": 27068}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 27068, "end": 28653}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 28653, "end": 40751}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 40751, "end": 55161}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 55161, "end": 71185}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 71185, "end": 73686}, {"filename": "/examples/j1/main.cpp", "start": 73686, "end": 73805}, {"filename": "/examples/j1/metron/dpram.h", "start": 73805, "end": 74243}, {"filename": "/examples/j1/metron/j1.h", "start": 74243, "end": 78263}, {"filename": "/examples/pong/README.md", "start": 78263, "end": 78323}, {"filename": "/examples/pong/main.cpp", "start": 78323, "end": 80269}, {"filename": "/examples/pong/main_vl.cpp", "start": 80269, "end": 80420}, {"filename": "/examples/pong/metron/pong.h", "start": 80420, "end": 84284}, {"filename": "/examples/pong/reference/README.md", "start": 84284, "end": 84344}, {"filename": "/examples/rvsimple/README.md", "start": 84344, "end": 84423}, {"filename": "/examples/rvsimple/main.cpp", "start": 84423, "end": 87189}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 87189, "end": 90091}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 90091, "end": 93252}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 93252, "end": 93752}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 93752, "end": 95213}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 95213, "end": 97818}, {"filename": "/examples/rvsimple/metron/config.h", "start": 97818, "end": 99033}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 99033, "end": 104752}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 104752, "end": 105862}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 105862, "end": 107795}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 107795, "end": 109022}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 109022, "end": 110266}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 110266, "end": 110935}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 110935, "end": 111899}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 111899, "end": 114017}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 114017, "end": 114781}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 114781, "end": 115461}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 115461, "end": 116277}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 116277, "end": 117276}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 117276, "end": 118247}, {"filename": "/examples/rvsimple/metron/register.h", "start": 118247, "end": 118932}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 118932, "end": 121975}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 121975, "end": 127549}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 127549, "end": 130042}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 130042, "end": 134769}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 134769, "end": 136783}, {"filename": "/examples/scratch.h", "start": 136783, "end": 137340}, {"filename": "/examples/tutorial/adder.h", "start": 137340, "end": 137520}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 137520, "end": 138501}, {"filename": "/examples/tutorial/blockram.h", "start": 138501, "end": 139018}, {"filename": "/examples/tutorial/checksum.h", "start": 139018, "end": 139741}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 139741, "end": 140277}, {"filename": "/examples/tutorial/counter.h", "start": 140277, "end": 140426}, {"filename": "/examples/tutorial/declaration_order.h", "start": 140426, "end": 141205}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 141205, "end": 142623}, {"filename": "/examples/tutorial/nonblocking.h", "start": 142623, "end": 142745}, {"filename": "/examples/tutorial/submodules.h", "start": 142745, "end": 143862}, {"filename": "/examples/tutorial/templates.h", "start": 143862, "end": 144333}, {"filename": "/examples/tutorial/tutorial2.h", "start": 144333, "end": 144401}, {"filename": "/examples/tutorial/tutorial3.h", "start": 144401, "end": 144442}, {"filename": "/examples/tutorial/tutorial4.h", "start": 144442, "end": 144483}, {"filename": "/examples/tutorial/tutorial5.h", "start": 144483, "end": 144524}, {"filename": "/examples/tutorial/vga.h", "start": 144524, "end": 145671}, {"filename": "/examples/uart/README.md", "start": 145671, "end": 145915}, {"filename": "/examples/uart/main.cpp", "start": 145915, "end": 147214}, {"filename": "/examples/uart/main_vl.cpp", "start": 147214, "end": 149750}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 149750, "end": 152330}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 152330, "end": 154891}, {"filename": "/examples/uart/metron/uart_top.h", "start": 154891, "end": 156655}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 156655, "end": 159674}, {"filename": "/tests/metron_bad/README.md", "start": 159674, "end": 159871}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 159871, "end": 160167}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 160167, "end": 160415}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 160415, "end": 160658}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 160658, "end": 161253}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 161253, "end": 161663}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 161663, "end": 162203}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 162203, "end": 162514}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 162514, "end": 162960}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 162960, "end": 163220}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 163220, "end": 163492}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 163492, "end": 163999}, {"filename": "/tests/metron_good/README.md", "start": 163999, "end": 164080}, {"filename": "/tests/metron_good/all_func_types.h", "start": 164080, "end": 165645}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 165645, "end": 166052}, {"filename": "/tests/metron_good/basic_function.h", "start": 166052, "end": 166331}, {"filename": "/tests/metron_good/basic_increment.h", "start": 166331, "end": 166686}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 166686, "end": 166981}, {"filename": "/tests/metron_good/basic_literals.h", "start": 166981, "end": 167593}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 167593, "end": 167839}, {"filename": "/tests/metron_good/basic_output.h", "start": 167839, "end": 168100}, {"filename": "/tests/metron_good/basic_param.h", "start": 168100, "end": 168359}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 168359, "end": 168590}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 168590, "end": 168770}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 168770, "end": 169033}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 169033, "end": 169253}, {"filename": "/tests/metron_good/basic_submod.h", "start": 169253, "end": 169542}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 169542, "end": 169897}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 169897, "end": 170273}, {"filename": "/tests/metron_good/basic_switch.h", "start": 170273, "end": 170750}, {"filename": "/tests/metron_good/basic_task.h", "start": 170750, "end": 171084}, {"filename": "/tests/metron_good/basic_template.h", "start": 171084, "end": 171570}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 171570, "end": 171729}, {"filename": "/tests/metron_good/bit_casts.h", "start": 171729, "end": 177702}, {"filename": "/tests/metron_good/bit_concat.h", "start": 177702, "end": 178129}, {"filename": "/tests/metron_good/bit_dup.h", "start": 178129, "end": 178788}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 178788, "end": 179626}, {"filename": "/tests/metron_good/builtins.h", "start": 179626, "end": 179957}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 179957, "end": 180264}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 180264, "end": 180818}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 180818, "end": 181711}, {"filename": "/tests/metron_good/constructor_args.h", "start": 181711, "end": 182219}, {"filename": "/tests/metron_good/defines.h", "start": 182219, "end": 182533}, {"filename": "/tests/metron_good/dontcare.h", "start": 182533, "end": 182813}, {"filename": "/tests/metron_good/enum_simple.h", "start": 182813, "end": 184181}, {"filename": "/tests/metron_good/for_loops.h", "start": 184181, "end": 184501}, {"filename": "/tests/metron_good/good_order.h", "start": 184501, "end": 184797}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 184797, "end": 185208}, {"filename": "/tests/metron_good/include_guards.h", "start": 185208, "end": 185405}, {"filename": "/tests/metron_good/init_chain.h", "start": 185405, "end": 186113}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 186113, "end": 186400}, {"filename": "/tests/metron_good/input_signals.h", "start": 186400, "end": 187062}, {"filename": "/tests/metron_good/local_localparam.h", "start": 187062, "end": 187240}, {"filename": "/tests/metron_good/magic_comments.h", "start": 187240, "end": 187543}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 187543, "end": 187857}, {"filename": "/tests/metron_good/minimal.h", "start": 187857, "end": 187932}, {"filename": "/tests/metron_good/multi_tick.h", "start": 187932, "end": 188298}, {"filename": "/tests/metron_good/namespaces.h", "start": 188298, "end": 188648}, {"filename": "/tests/metron_good/nested_structs.h", "start": 188648, "end": 189161}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 189161, "end": 189706}, {"filename": "/tests/metron_good/oneliners.h", "start": 189706, "end": 189969}, {"filename": "/tests/metron_good/plus_equals.h", "start": 189969, "end": 190393}, {"filename": "/tests/metron_good/private_getter.h", "start": 190393, "end": 190617}, {"filename": "/tests/metron_good/structs.h", "start": 190617, "end": 190836}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 190836, "end": 191372}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 191372, "end": 193723}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 193723, "end": 194477}, {"filename": "/tests/metron_good/tock_task.h", "start": 194477, "end": 194833}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 194833, "end": 194998}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 194998, "end": 195657}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 195657, "end": 196316}], "remote_package_size": 196316});

  })();
