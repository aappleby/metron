
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/ibex/README.md", "start": 26876, "end": 26927}, {"filename": "/examples/ibex/main.cpp", "start": 26927, "end": 27068}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 27068, "end": 28653}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 28653, "end": 40751}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 40751, "end": 55161}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 55161, "end": 71185}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 71185, "end": 73686}, {"filename": "/examples/j1/main.cpp", "start": 73686, "end": 73805}, {"filename": "/examples/j1/metron/dpram.h", "start": 73805, "end": 74243}, {"filename": "/examples/j1/metron/j1.h", "start": 74243, "end": 78263}, {"filename": "/examples/pong/README.md", "start": 78263, "end": 78323}, {"filename": "/examples/pong/main.cpp", "start": 78323, "end": 80269}, {"filename": "/examples/pong/main_vl.cpp", "start": 80269, "end": 80420}, {"filename": "/examples/pong/metron/pong.h", "start": 80420, "end": 84284}, {"filename": "/examples/pong/reference/README.md", "start": 84284, "end": 84344}, {"filename": "/examples/rvsimple/README.md", "start": 84344, "end": 84423}, {"filename": "/examples/rvsimple/main.cpp", "start": 84423, "end": 87189}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 87189, "end": 90091}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 90091, "end": 93252}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 93252, "end": 93752}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 93752, "end": 95213}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 95213, "end": 97818}, {"filename": "/examples/rvsimple/metron/config.h", "start": 97818, "end": 99033}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 99033, "end": 104752}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 104752, "end": 105862}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 105862, "end": 107795}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 107795, "end": 109022}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 109022, "end": 110266}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 110266, "end": 110935}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 110935, "end": 111899}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 111899, "end": 114017}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 114017, "end": 114781}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 114781, "end": 115461}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 115461, "end": 116277}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 116277, "end": 117276}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 117276, "end": 118247}, {"filename": "/examples/rvsimple/metron/register.h", "start": 118247, "end": 118932}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 118932, "end": 121975}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 121975, "end": 127549}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 127549, "end": 130042}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 130042, "end": 134769}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 134769, "end": 136757}, {"filename": "/examples/scratch.h", "start": 136757, "end": 139873}, {"filename": "/examples/tutorial/adder.h", "start": 139873, "end": 140053}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 140053, "end": 141034}, {"filename": "/examples/tutorial/blockram.h", "start": 141034, "end": 141551}, {"filename": "/examples/tutorial/checksum.h", "start": 141551, "end": 142274}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 142274, "end": 142810}, {"filename": "/examples/tutorial/counter.h", "start": 142810, "end": 142959}, {"filename": "/examples/tutorial/declaration_order.h", "start": 142959, "end": 143738}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 143738, "end": 145156}, {"filename": "/examples/tutorial/nonblocking.h", "start": 145156, "end": 145278}, {"filename": "/examples/tutorial/submodules.h", "start": 145278, "end": 146395}, {"filename": "/examples/tutorial/templates.h", "start": 146395, "end": 146866}, {"filename": "/examples/tutorial/tutorial2.h", "start": 146866, "end": 146934}, {"filename": "/examples/tutorial/tutorial3.h", "start": 146934, "end": 146975}, {"filename": "/examples/tutorial/tutorial4.h", "start": 146975, "end": 147016}, {"filename": "/examples/tutorial/tutorial5.h", "start": 147016, "end": 147057}, {"filename": "/examples/tutorial/vga.h", "start": 147057, "end": 148204}, {"filename": "/examples/uart/README.md", "start": 148204, "end": 148448}, {"filename": "/examples/uart/main.cpp", "start": 148448, "end": 149747}, {"filename": "/examples/uart/main_vl.cpp", "start": 149747, "end": 152283}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 152283, "end": 154863}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 154863, "end": 157424}, {"filename": "/examples/uart/metron/uart_top.h", "start": 157424, "end": 159188}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 159188, "end": 162207}, {"filename": "/tests/metron_bad/README.md", "start": 162207, "end": 162404}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 162404, "end": 162700}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 162700, "end": 162948}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 162948, "end": 163191}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 163191, "end": 163786}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 163786, "end": 164013}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 164013, "end": 164423}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 164423, "end": 164963}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 164963, "end": 165274}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 165274, "end": 165720}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 165720, "end": 165980}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 165980, "end": 166252}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 166252, "end": 166759}, {"filename": "/tests/metron_good/README.md", "start": 166759, "end": 166840}, {"filename": "/tests/metron_good/all_func_types.h", "start": 166840, "end": 168499}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 168499, "end": 168906}, {"filename": "/tests/metron_good/basic_function.h", "start": 168906, "end": 169185}, {"filename": "/tests/metron_good/basic_increment.h", "start": 169185, "end": 169540}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 169540, "end": 169835}, {"filename": "/tests/metron_good/basic_literals.h", "start": 169835, "end": 170447}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 170447, "end": 170693}, {"filename": "/tests/metron_good/basic_output.h", "start": 170693, "end": 170954}, {"filename": "/tests/metron_good/basic_param.h", "start": 170954, "end": 171213}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 171213, "end": 171444}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 171444, "end": 171624}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 171624, "end": 171887}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 171887, "end": 172107}, {"filename": "/tests/metron_good/basic_submod.h", "start": 172107, "end": 172396}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 172396, "end": 172751}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 172751, "end": 173127}, {"filename": "/tests/metron_good/basic_switch.h", "start": 173127, "end": 173604}, {"filename": "/tests/metron_good/basic_task.h", "start": 173604, "end": 173938}, {"filename": "/tests/metron_good/basic_template.h", "start": 173938, "end": 174424}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 174424, "end": 174583}, {"filename": "/tests/metron_good/bit_casts.h", "start": 174583, "end": 180556}, {"filename": "/tests/metron_good/bit_concat.h", "start": 180556, "end": 180983}, {"filename": "/tests/metron_good/bit_dup.h", "start": 180983, "end": 181642}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 181642, "end": 182480}, {"filename": "/tests/metron_good/builtins.h", "start": 182480, "end": 182811}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 182811, "end": 183118}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 183118, "end": 183672}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 183672, "end": 184565}, {"filename": "/tests/metron_good/constructor_args.h", "start": 184565, "end": 185073}, {"filename": "/tests/metron_good/defines.h", "start": 185073, "end": 185387}, {"filename": "/tests/metron_good/dontcare.h", "start": 185387, "end": 185667}, {"filename": "/tests/metron_good/enum_simple.h", "start": 185667, "end": 187035}, {"filename": "/tests/metron_good/for_loops.h", "start": 187035, "end": 187355}, {"filename": "/tests/metron_good/good_order.h", "start": 187355, "end": 187651}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 187651, "end": 188062}, {"filename": "/tests/metron_good/include_guards.h", "start": 188062, "end": 188259}, {"filename": "/tests/metron_good/init_chain.h", "start": 188259, "end": 188967}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 188967, "end": 189254}, {"filename": "/tests/metron_good/input_signals.h", "start": 189254, "end": 189916}, {"filename": "/tests/metron_good/local_localparam.h", "start": 189916, "end": 190094}, {"filename": "/tests/metron_good/magic_comments.h", "start": 190094, "end": 190397}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 190397, "end": 190711}, {"filename": "/tests/metron_good/minimal.h", "start": 190711, "end": 190786}, {"filename": "/tests/metron_good/multi_tick.h", "start": 190786, "end": 191152}, {"filename": "/tests/metron_good/namespaces.h", "start": 191152, "end": 191502}, {"filename": "/tests/metron_good/nested_structs.h", "start": 191502, "end": 191917}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 191917, "end": 192462}, {"filename": "/tests/metron_good/oneliners.h", "start": 192462, "end": 192725}, {"filename": "/tests/metron_good/plus_equals.h", "start": 192725, "end": 193149}, {"filename": "/tests/metron_good/private_getter.h", "start": 193149, "end": 193373}, {"filename": "/tests/metron_good/structs.h", "start": 193373, "end": 193592}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 193592, "end": 194128}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 194128, "end": 196672}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 196672, "end": 197426}, {"filename": "/tests/metron_good/tock_task.h", "start": 197426, "end": 197782}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 197782, "end": 197947}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 197947, "end": 198606}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 198606, "end": 199265}], "remote_package_size": 199265});

  })();
