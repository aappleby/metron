
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1263}, {"filename": "/examples/rvsimple/README.md", "start": 1263, "end": 1342}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 1342, "end": 3460}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 3460, "end": 9054}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 9054, "end": 14773}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 14773, "end": 15783}, {"filename": "/examples/rvsimple/metron/config.h", "start": 15783, "end": 16998}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 16998, "end": 17969}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 17969, "end": 18733}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 18733, "end": 21776}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 21776, "end": 22603}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 22603, "end": 23103}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23103, "end": 25041}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 25041, "end": 26194}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 26194, "end": 28815}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 28815, "end": 29506}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 29506, "end": 30967}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 30967, "end": 31643}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 31643, "end": 34136}, {"filename": "/examples/rvsimple/metron/register.h", "start": 34136, "end": 34821}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 34821, "end": 36669}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 36669, "end": 37783}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 37783, "end": 39017}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 39017, "end": 39890}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 39890, "end": 44617}, {"filename": "/examples/ibex/README.md", "start": 44617, "end": 44668}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 44668, "end": 59078}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 59078, "end": 61579}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 61579, "end": 77603}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 77603, "end": 79188}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 79188, "end": 91286}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 91286, "end": 92704}, {"filename": "/examples/tutorial/counter.h", "start": 92704, "end": 92853}, {"filename": "/examples/tutorial/tutorial2.h", "start": 92853, "end": 92921}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 92921, "end": 93457}, {"filename": "/examples/tutorial/tutorial5.h", "start": 93457, "end": 93498}, {"filename": "/examples/tutorial/declaration_order.h", "start": 93498, "end": 94277}, {"filename": "/examples/tutorial/nonblocking.h", "start": 94277, "end": 94399}, {"filename": "/examples/tutorial/tutorial4.h", "start": 94399, "end": 94440}, {"filename": "/examples/tutorial/adder.h", "start": 94440, "end": 94620}, {"filename": "/examples/tutorial/vga.h", "start": 94620, "end": 95767}, {"filename": "/examples/tutorial/tutorial3.h", "start": 95767, "end": 95808}, {"filename": "/examples/tutorial/checksum.h", "start": 95808, "end": 96531}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 96531, "end": 97512}, {"filename": "/examples/tutorial/templates.h", "start": 97512, "end": 97983}, {"filename": "/examples/tutorial/submodules.h", "start": 97983, "end": 99100}, {"filename": "/examples/tutorial/blockram.h", "start": 99100, "end": 99617}, {"filename": "/examples/gb_spu/regs.h", "start": 99617, "end": 119970}, {"filename": "/examples/gb_spu/Sch_ApuControl.h", "start": 119970, "end": 153714}, {"filename": "/examples/gb_spu/gates.h", "start": 153714, "end": 157205}, {"filename": "/examples/gb_spu/metron/gb_spu.h", "start": 157205, "end": 177996}, {"filename": "/examples/rvtiny_sync/README.md", "start": 177996, "end": 178048}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 178048, "end": 185003}, {"filename": "/examples/rvtiny/README.md", "start": 185003, "end": 185167}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 185167, "end": 191263}, {"filename": "/examples/pong/README.md", "start": 191263, "end": 191323}, {"filename": "/examples/pong/reference/README.md", "start": 191323, "end": 191383}, {"filename": "/examples/pong/metron/pong.h", "start": 191383, "end": 195247}, {"filename": "/examples/uart/README.md", "start": 195247, "end": 195491}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 195491, "end": 198510}, {"filename": "/examples/uart/metron/uart_top.h", "start": 198510, "end": 200274}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 200274, "end": 202854}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 202854, "end": 205415}, {"filename": "/examples/j1/metron/j1.h", "start": 205415, "end": 209440}, {"filename": "/examples/j1/metron/dpram.h", "start": 209440, "end": 209878}, {"filename": "/tests/metron_good/bit_casts.h", "start": 209878, "end": 215851}, {"filename": "/tests/metron_good/magic_comments.h", "start": 215851, "end": 216154}, {"filename": "/tests/metron_good/tock_task.h", "start": 216154, "end": 216510}, {"filename": "/tests/metron_good/structs.h", "start": 216510, "end": 216729}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 216729, "end": 216960}, {"filename": "/tests/metron_good/basic_increment.h", "start": 216960, "end": 217315}, {"filename": "/tests/metron_good/bit_concat.h", "start": 217315, "end": 217742}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 217742, "end": 218287}, {"filename": "/tests/metron_good/bit_dup.h", "start": 218287, "end": 218946}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 218946, "end": 219784}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 219784, "end": 220191}, {"filename": "/tests/metron_good/basic_param.h", "start": 220191, "end": 220450}, {"filename": "/tests/metron_good/init_chain.h", "start": 220450, "end": 221158}, {"filename": "/tests/metron_good/basic_function.h", "start": 221158, "end": 221437}, {"filename": "/tests/metron_good/all_func_types.h", "start": 221437, "end": 223002}, {"filename": "/tests/metron_good/plus_equals.h", "start": 223002, "end": 223352}, {"filename": "/tests/metron_good/basic_switch.h", "start": 223352, "end": 223829}, {"filename": "/tests/metron_good/dontcare.h", "start": 223829, "end": 224112}, {"filename": "/tests/metron_good/basic_literals.h", "start": 224112, "end": 224724}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 224724, "end": 224883}, {"filename": "/tests/metron_good/README.md", "start": 224883, "end": 224964}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 224964, "end": 225375}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 225375, "end": 225540}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 225540, "end": 226199}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 226199, "end": 226494}, {"filename": "/tests/metron_good/builtins.h", "start": 226494, "end": 226825}, {"filename": "/tests/metron_good/basic_template.h", "start": 226825, "end": 227256}, {"filename": "/tests/metron_good/defines.h", "start": 227256, "end": 227570}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 227570, "end": 228229}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 228229, "end": 228983}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 228983, "end": 229246}, {"filename": "/tests/metron_good/namespaces.h", "start": 229246, "end": 229596}, {"filename": "/tests/metron_good/include_guards.h", "start": 229596, "end": 229793}, {"filename": "/tests/metron_good/basic_task.h", "start": 229793, "end": 230127}, {"filename": "/tests/metron_good/enum_simple.h", "start": 230127, "end": 231495}, {"filename": "/tests/metron_good/oneliners.h", "start": 231495, "end": 231758}, {"filename": "/tests/metron_good/for_loops.h", "start": 231758, "end": 232078}, {"filename": "/tests/metron_good/private_getter.h", "start": 232078, "end": 232302}, {"filename": "/tests/metron_good/multi_tick.h", "start": 232302, "end": 232668}, {"filename": "/tests/metron_good/good_order.h", "start": 232668, "end": 232964}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 232964, "end": 233271}, {"filename": "/tests/metron_good/basic_output.h", "start": 233271, "end": 233532}, {"filename": "/tests/metron_good/basic_submod.h", "start": 233532, "end": 233821}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 233821, "end": 234375}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 234375, "end": 234555}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 234555, "end": 234931}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 234931, "end": 235245}, {"filename": "/tests/metron_good/input_signals.h", "start": 235245, "end": 235907}, {"filename": "/tests/metron_good/minimal.h", "start": 235907, "end": 235982}, {"filename": "/tests/metron_good/nested_structs.h", "start": 235982, "end": 236495}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 236495, "end": 236850}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 236850, "end": 237096}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 237096, "end": 237316}, {"filename": "/tests/metron_good/local_localparam.h", "start": 237316, "end": 237494}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 237494, "end": 237805}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 237805, "end": 238345}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 238345, "end": 238588}, {"filename": "/tests/metron_bad/README.md", "start": 238588, "end": 238785}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 238785, "end": 239380}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 239380, "end": 239826}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 239826, "end": 239966}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 239966, "end": 240226}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 240226, "end": 240474}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 240474, "end": 240770}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 240770, "end": 241277}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 241277, "end": 241687}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 241687, "end": 241959}], "remote_package_size": 241959});

  })();
