
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
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "gb_spu", true, true);
Module['FS_createPath']("/examples/gb_spu", "metron", true, true);
Module['FS_createPath']("/examples", "rvtiny_sync", true, true);
Module['FS_createPath']("/examples/rvtiny_sync", "metron", true, true);
Module['FS_createPath']("/examples", "rvtiny", true, true);
Module['FS_createPath']("/examples/rvtiny", "metron", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/examples/uart", "metron", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples/j1", "metron", true, true);
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1263}, {"filename": "/examples/rvsimple/README.md", "start": 1263, "end": 1342}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 1342, "end": 3460}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 3460, "end": 9054}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 9054, "end": 14773}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 14773, "end": 15783}, {"filename": "/examples/rvsimple/metron/config.h", "start": 15783, "end": 16998}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 16998, "end": 17969}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 17969, "end": 18733}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 18733, "end": 21776}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 21776, "end": 22603}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 22603, "end": 23103}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23103, "end": 25041}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 25041, "end": 26194}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 26194, "end": 28815}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 28815, "end": 29506}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 29506, "end": 30967}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 30967, "end": 31643}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 31643, "end": 34136}, {"filename": "/examples/rvsimple/metron/register.h", "start": 34136, "end": 34821}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 34821, "end": 36669}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 36669, "end": 37783}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 37783, "end": 39017}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 39017, "end": 39890}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 39890, "end": 44617}, {"filename": "/examples/ibex/README.md", "start": 44617, "end": 44668}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 44668, "end": 59078}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 59078, "end": 61579}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 61579, "end": 77603}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 77603, "end": 79188}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 79188, "end": 91286}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 91286, "end": 92704}, {"filename": "/examples/tutorial/counter.h", "start": 92704, "end": 92853}, {"filename": "/examples/tutorial/tutorial2.h", "start": 92853, "end": 92921}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 92921, "end": 93457}, {"filename": "/examples/tutorial/tutorial5.h", "start": 93457, "end": 93498}, {"filename": "/examples/tutorial/declaration_order.h", "start": 93498, "end": 94277}, {"filename": "/examples/tutorial/nonblocking.h", "start": 94277, "end": 94399}, {"filename": "/examples/tutorial/tutorial4.h", "start": 94399, "end": 94440}, {"filename": "/examples/tutorial/adder.h", "start": 94440, "end": 94620}, {"filename": "/examples/tutorial/vga.h", "start": 94620, "end": 95767}, {"filename": "/examples/tutorial/tutorial3.h", "start": 95767, "end": 95808}, {"filename": "/examples/tutorial/checksum.h", "start": 95808, "end": 96531}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 96531, "end": 97512}, {"filename": "/examples/tutorial/templates.h", "start": 97512, "end": 97983}, {"filename": "/examples/tutorial/submodules.h", "start": 97983, "end": 99100}, {"filename": "/examples/tutorial/blockram.h", "start": 99100, "end": 99617}, {"filename": "/examples/gb_spu/regs.h", "start": 99617, "end": 119970}, {"filename": "/examples/gb_spu/Sch_ApuControl.h", "start": 119970, "end": 145256}, {"filename": "/examples/gb_spu/gates.h", "start": 145256, "end": 148747}, {"filename": "/examples/gb_spu/metron/gb_spu.h", "start": 148747, "end": 169538}, {"filename": "/examples/rvtiny_sync/README.md", "start": 169538, "end": 169590}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 169590, "end": 176545}, {"filename": "/examples/rvtiny/README.md", "start": 176545, "end": 176709}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 176709, "end": 182805}, {"filename": "/examples/pong/README.md", "start": 182805, "end": 182865}, {"filename": "/examples/pong/reference/README.md", "start": 182865, "end": 182925}, {"filename": "/examples/pong/metron/pong.h", "start": 182925, "end": 186789}, {"filename": "/examples/uart/README.md", "start": 186789, "end": 187033}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 187033, "end": 190052}, {"filename": "/examples/uart/metron/uart_top.h", "start": 190052, "end": 191816}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 191816, "end": 194396}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 194396, "end": 196957}, {"filename": "/examples/j1/metron/j1.h", "start": 196957, "end": 200982}, {"filename": "/examples/j1/metron/dpram.h", "start": 200982, "end": 201420}, {"filename": "/tests/metron_good/bit_casts.h", "start": 201420, "end": 207393}, {"filename": "/tests/metron_good/magic_comments.h", "start": 207393, "end": 207696}, {"filename": "/tests/metron_good/tock_task.h", "start": 207696, "end": 208052}, {"filename": "/tests/metron_good/structs.h", "start": 208052, "end": 208271}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 208271, "end": 208502}, {"filename": "/tests/metron_good/basic_increment.h", "start": 208502, "end": 208857}, {"filename": "/tests/metron_good/bit_concat.h", "start": 208857, "end": 209284}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 209284, "end": 209829}, {"filename": "/tests/metron_good/bit_dup.h", "start": 209829, "end": 210488}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 210488, "end": 211326}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 211326, "end": 211733}, {"filename": "/tests/metron_good/basic_param.h", "start": 211733, "end": 211992}, {"filename": "/tests/metron_good/init_chain.h", "start": 211992, "end": 212700}, {"filename": "/tests/metron_good/basic_function.h", "start": 212700, "end": 212979}, {"filename": "/tests/metron_good/all_func_types.h", "start": 212979, "end": 214544}, {"filename": "/tests/metron_good/plus_equals.h", "start": 214544, "end": 214894}, {"filename": "/tests/metron_good/basic_switch.h", "start": 214894, "end": 215371}, {"filename": "/tests/metron_good/dontcare.h", "start": 215371, "end": 215654}, {"filename": "/tests/metron_good/basic_literals.h", "start": 215654, "end": 216266}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 216266, "end": 216425}, {"filename": "/tests/metron_good/README.md", "start": 216425, "end": 216506}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 216506, "end": 216917}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 216917, "end": 217082}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 217082, "end": 217741}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 217741, "end": 218036}, {"filename": "/tests/metron_good/builtins.h", "start": 218036, "end": 218367}, {"filename": "/tests/metron_good/basic_template.h", "start": 218367, "end": 218798}, {"filename": "/tests/metron_good/defines.h", "start": 218798, "end": 219112}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 219112, "end": 219771}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 219771, "end": 220525}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 220525, "end": 220788}, {"filename": "/tests/metron_good/namespaces.h", "start": 220788, "end": 221138}, {"filename": "/tests/metron_good/include_guards.h", "start": 221138, "end": 221335}, {"filename": "/tests/metron_good/basic_task.h", "start": 221335, "end": 221669}, {"filename": "/tests/metron_good/enum_simple.h", "start": 221669, "end": 223037}, {"filename": "/tests/metron_good/oneliners.h", "start": 223037, "end": 223300}, {"filename": "/tests/metron_good/for_loops.h", "start": 223300, "end": 223620}, {"filename": "/tests/metron_good/private_getter.h", "start": 223620, "end": 223844}, {"filename": "/tests/metron_good/multi_tick.h", "start": 223844, "end": 224210}, {"filename": "/tests/metron_good/good_order.h", "start": 224210, "end": 224506}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 224506, "end": 224813}, {"filename": "/tests/metron_good/basic_output.h", "start": 224813, "end": 225074}, {"filename": "/tests/metron_good/basic_submod.h", "start": 225074, "end": 225363}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 225363, "end": 225917}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 225917, "end": 226097}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 226097, "end": 226473}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 226473, "end": 226787}, {"filename": "/tests/metron_good/input_signals.h", "start": 226787, "end": 227449}, {"filename": "/tests/metron_good/minimal.h", "start": 227449, "end": 227524}, {"filename": "/tests/metron_good/nested_structs.h", "start": 227524, "end": 228037}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 228037, "end": 228392}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 228392, "end": 228638}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 228638, "end": 228858}, {"filename": "/tests/metron_good/local_localparam.h", "start": 228858, "end": 229036}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 229036, "end": 229347}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 229347, "end": 229887}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 229887, "end": 230130}, {"filename": "/tests/metron_bad/README.md", "start": 230130, "end": 230327}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 230327, "end": 230922}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 230922, "end": 231368}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 231368, "end": 231508}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 231508, "end": 231768}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 231768, "end": 232016}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 232016, "end": 232312}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 232312, "end": 232819}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 232819, "end": 233229}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 233229, "end": 233501}], "remote_package_size": 233501});

  })();
