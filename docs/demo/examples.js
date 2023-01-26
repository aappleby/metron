
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/ibex/README.md", "start": 26876, "end": 26927}, {"filename": "/examples/ibex/main.cpp", "start": 26927, "end": 27068}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 27068, "end": 28653}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 28653, "end": 40751}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 40751, "end": 55161}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 55161, "end": 71185}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 71185, "end": 73686}, {"filename": "/examples/j1/main.cpp", "start": 73686, "end": 73805}, {"filename": "/examples/j1/metron/dpram.h", "start": 73805, "end": 74243}, {"filename": "/examples/j1/metron/j1.h", "start": 74243, "end": 78263}, {"filename": "/examples/pong/README.md", "start": 78263, "end": 78323}, {"filename": "/examples/pong/main.cpp", "start": 78323, "end": 80269}, {"filename": "/examples/pong/main_vl.cpp", "start": 80269, "end": 80420}, {"filename": "/examples/pong/metron/pong.h", "start": 80420, "end": 84284}, {"filename": "/examples/pong/reference/README.md", "start": 84284, "end": 84344}, {"filename": "/examples/rvsimple/README.md", "start": 84344, "end": 84423}, {"filename": "/examples/rvsimple/main.cpp", "start": 84423, "end": 87189}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 87189, "end": 90091}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 90091, "end": 93252}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 93252, "end": 93752}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 93752, "end": 95213}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 95213, "end": 97818}, {"filename": "/examples/rvsimple/metron/config.h", "start": 97818, "end": 99033}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 99033, "end": 104752}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 104752, "end": 105862}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 105862, "end": 107795}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 107795, "end": 109022}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 109022, "end": 110266}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 110266, "end": 110935}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 110935, "end": 111899}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 111899, "end": 114017}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 114017, "end": 114781}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 114781, "end": 115461}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 115461, "end": 116277}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 116277, "end": 117276}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 117276, "end": 118247}, {"filename": "/examples/rvsimple/metron/register.h", "start": 118247, "end": 118932}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 118932, "end": 121975}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 121975, "end": 127549}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 127549, "end": 130042}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 130042, "end": 134769}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 134769, "end": 136783}, {"filename": "/examples/scratch.h", "start": 136783, "end": 137336}, {"filename": "/examples/tutorial/adder.h", "start": 137336, "end": 137516}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 137516, "end": 138497}, {"filename": "/examples/tutorial/blockram.h", "start": 138497, "end": 139014}, {"filename": "/examples/tutorial/checksum.h", "start": 139014, "end": 139737}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 139737, "end": 140273}, {"filename": "/examples/tutorial/counter.h", "start": 140273, "end": 140422}, {"filename": "/examples/tutorial/declaration_order.h", "start": 140422, "end": 141201}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 141201, "end": 142619}, {"filename": "/examples/tutorial/nonblocking.h", "start": 142619, "end": 142741}, {"filename": "/examples/tutorial/submodules.h", "start": 142741, "end": 143858}, {"filename": "/examples/tutorial/templates.h", "start": 143858, "end": 144329}, {"filename": "/examples/tutorial/tutorial2.h", "start": 144329, "end": 144397}, {"filename": "/examples/tutorial/tutorial3.h", "start": 144397, "end": 144438}, {"filename": "/examples/tutorial/tutorial4.h", "start": 144438, "end": 144479}, {"filename": "/examples/tutorial/tutorial5.h", "start": 144479, "end": 144520}, {"filename": "/examples/tutorial/vga.h", "start": 144520, "end": 145667}, {"filename": "/examples/uart/README.md", "start": 145667, "end": 145911}, {"filename": "/examples/uart/main.cpp", "start": 145911, "end": 147210}, {"filename": "/examples/uart/main_vl.cpp", "start": 147210, "end": 149746}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 149746, "end": 152326}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 152326, "end": 154887}, {"filename": "/examples/uart/metron/uart_top.h", "start": 154887, "end": 156651}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 156651, "end": 159670}, {"filename": "/tests/metron_bad/README.md", "start": 159670, "end": 159867}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 159867, "end": 160163}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 160163, "end": 160411}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 160411, "end": 160654}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 160654, "end": 161249}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 161249, "end": 161659}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 161659, "end": 162199}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 162199, "end": 162510}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 162510, "end": 162956}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 162956, "end": 163216}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 163216, "end": 163488}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 163488, "end": 163995}, {"filename": "/tests/metron_good/README.md", "start": 163995, "end": 164076}, {"filename": "/tests/metron_good/all_func_types.h", "start": 164076, "end": 165641}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 165641, "end": 166048}, {"filename": "/tests/metron_good/basic_function.h", "start": 166048, "end": 166327}, {"filename": "/tests/metron_good/basic_increment.h", "start": 166327, "end": 166682}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 166682, "end": 166977}, {"filename": "/tests/metron_good/basic_literals.h", "start": 166977, "end": 167589}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 167589, "end": 167835}, {"filename": "/tests/metron_good/basic_output.h", "start": 167835, "end": 168096}, {"filename": "/tests/metron_good/basic_param.h", "start": 168096, "end": 168355}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 168355, "end": 168586}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 168586, "end": 168766}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 168766, "end": 169029}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 169029, "end": 169249}, {"filename": "/tests/metron_good/basic_submod.h", "start": 169249, "end": 169538}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 169538, "end": 169893}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 169893, "end": 170269}, {"filename": "/tests/metron_good/basic_switch.h", "start": 170269, "end": 170746}, {"filename": "/tests/metron_good/basic_task.h", "start": 170746, "end": 171080}, {"filename": "/tests/metron_good/basic_template.h", "start": 171080, "end": 171566}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 171566, "end": 171725}, {"filename": "/tests/metron_good/bit_casts.h", "start": 171725, "end": 177698}, {"filename": "/tests/metron_good/bit_concat.h", "start": 177698, "end": 178125}, {"filename": "/tests/metron_good/bit_dup.h", "start": 178125, "end": 178784}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 178784, "end": 179622}, {"filename": "/tests/metron_good/builtins.h", "start": 179622, "end": 179953}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 179953, "end": 180260}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 180260, "end": 180814}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 180814, "end": 181707}, {"filename": "/tests/metron_good/constructor_args.h", "start": 181707, "end": 182215}, {"filename": "/tests/metron_good/defines.h", "start": 182215, "end": 182529}, {"filename": "/tests/metron_good/dontcare.h", "start": 182529, "end": 182809}, {"filename": "/tests/metron_good/enum_simple.h", "start": 182809, "end": 184177}, {"filename": "/tests/metron_good/for_loops.h", "start": 184177, "end": 184497}, {"filename": "/tests/metron_good/good_order.h", "start": 184497, "end": 184793}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 184793, "end": 185204}, {"filename": "/tests/metron_good/include_guards.h", "start": 185204, "end": 185401}, {"filename": "/tests/metron_good/init_chain.h", "start": 185401, "end": 186109}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 186109, "end": 186396}, {"filename": "/tests/metron_good/input_signals.h", "start": 186396, "end": 187058}, {"filename": "/tests/metron_good/local_localparam.h", "start": 187058, "end": 187236}, {"filename": "/tests/metron_good/magic_comments.h", "start": 187236, "end": 187539}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 187539, "end": 187853}, {"filename": "/tests/metron_good/minimal.h", "start": 187853, "end": 187928}, {"filename": "/tests/metron_good/multi_tick.h", "start": 187928, "end": 188294}, {"filename": "/tests/metron_good/namespaces.h", "start": 188294, "end": 188644}, {"filename": "/tests/metron_good/nested_structs.h", "start": 188644, "end": 189157}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 189157, "end": 189702}, {"filename": "/tests/metron_good/oneliners.h", "start": 189702, "end": 189965}, {"filename": "/tests/metron_good/plus_equals.h", "start": 189965, "end": 190389}, {"filename": "/tests/metron_good/private_getter.h", "start": 190389, "end": 190613}, {"filename": "/tests/metron_good/structs.h", "start": 190613, "end": 190832}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 190832, "end": 191368}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 191368, "end": 193719}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 193719, "end": 194473}, {"filename": "/tests/metron_good/tock_task.h", "start": 194473, "end": 194829}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 194829, "end": 194994}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 194994, "end": 195653}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 195653, "end": 196312}], "remote_package_size": 196312});

  })();
