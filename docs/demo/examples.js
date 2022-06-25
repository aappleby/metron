
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1263}, {"filename": "/examples/rvsimple/README.md", "start": 1263, "end": 1342}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 1342, "end": 3460}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 3460, "end": 9054}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 9054, "end": 14773}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 14773, "end": 15783}, {"filename": "/examples/rvsimple/metron/config.h", "start": 15783, "end": 16998}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 16998, "end": 17969}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 17969, "end": 18733}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 18733, "end": 21776}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 21776, "end": 22603}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 22603, "end": 23103}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23103, "end": 25041}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 25041, "end": 26194}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 26194, "end": 28815}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 28815, "end": 29506}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 29506, "end": 30967}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 30967, "end": 31643}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 31643, "end": 34136}, {"filename": "/examples/rvsimple/metron/register.h", "start": 34136, "end": 34821}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 34821, "end": 36669}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 36669, "end": 37783}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 37783, "end": 39017}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 39017, "end": 39890}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 39890, "end": 44617}, {"filename": "/examples/ibex/README.md", "start": 44617, "end": 44668}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 44668, "end": 59078}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 59078, "end": 61579}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 61579, "end": 77603}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 77603, "end": 79188}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 79188, "end": 91286}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 91286, "end": 92704}, {"filename": "/examples/tutorial/counter.h", "start": 92704, "end": 92853}, {"filename": "/examples/tutorial/tutorial2.h", "start": 92853, "end": 92921}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 92921, "end": 93457}, {"filename": "/examples/tutorial/tutorial5.h", "start": 93457, "end": 93498}, {"filename": "/examples/tutorial/declaration_order.h", "start": 93498, "end": 94277}, {"filename": "/examples/tutorial/nonblocking.h", "start": 94277, "end": 94399}, {"filename": "/examples/tutorial/tutorial4.h", "start": 94399, "end": 94440}, {"filename": "/examples/tutorial/adder.h", "start": 94440, "end": 94620}, {"filename": "/examples/tutorial/vga.h", "start": 94620, "end": 95767}, {"filename": "/examples/tutorial/tutorial3.h", "start": 95767, "end": 95808}, {"filename": "/examples/tutorial/checksum.h", "start": 95808, "end": 96531}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 96531, "end": 97512}, {"filename": "/examples/tutorial/templates.h", "start": 97512, "end": 97983}, {"filename": "/examples/tutorial/submodules.h", "start": 97983, "end": 99100}, {"filename": "/examples/tutorial/blockram.h", "start": 99100, "end": 99617}, {"filename": "/examples/gb_spu/regs.h", "start": 99617, "end": 119970}, {"filename": "/examples/gb_spu/Sch_ApuControl.h", "start": 119970, "end": 136395}, {"filename": "/examples/gb_spu/gates.h", "start": 136395, "end": 139886}, {"filename": "/examples/gb_spu/metron/gb_spu.h", "start": 139886, "end": 160677}, {"filename": "/examples/rvtiny_sync/README.md", "start": 160677, "end": 160729}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 160729, "end": 167684}, {"filename": "/examples/rvtiny/README.md", "start": 167684, "end": 167848}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 167848, "end": 173944}, {"filename": "/examples/pong/README.md", "start": 173944, "end": 174004}, {"filename": "/examples/pong/reference/README.md", "start": 174004, "end": 174064}, {"filename": "/examples/pong/metron/pong.h", "start": 174064, "end": 177928}, {"filename": "/examples/uart/README.md", "start": 177928, "end": 178172}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 178172, "end": 181191}, {"filename": "/examples/uart/metron/uart_top.h", "start": 181191, "end": 182955}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 182955, "end": 185535}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 185535, "end": 188096}, {"filename": "/examples/j1/metron/j1.h", "start": 188096, "end": 192121}, {"filename": "/examples/j1/metron/dpram.h", "start": 192121, "end": 192559}, {"filename": "/tests/metron_good/bit_casts.h", "start": 192559, "end": 198532}, {"filename": "/tests/metron_good/magic_comments.h", "start": 198532, "end": 198835}, {"filename": "/tests/metron_good/tock_task.h", "start": 198835, "end": 199191}, {"filename": "/tests/metron_good/structs.h", "start": 199191, "end": 199410}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 199410, "end": 199641}, {"filename": "/tests/metron_good/basic_increment.h", "start": 199641, "end": 199996}, {"filename": "/tests/metron_good/bit_concat.h", "start": 199996, "end": 200423}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 200423, "end": 200968}, {"filename": "/tests/metron_good/bit_dup.h", "start": 200968, "end": 201627}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 201627, "end": 202465}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 202465, "end": 202872}, {"filename": "/tests/metron_good/basic_param.h", "start": 202872, "end": 203131}, {"filename": "/tests/metron_good/init_chain.h", "start": 203131, "end": 203839}, {"filename": "/tests/metron_good/basic_function.h", "start": 203839, "end": 204118}, {"filename": "/tests/metron_good/all_func_types.h", "start": 204118, "end": 205683}, {"filename": "/tests/metron_good/plus_equals.h", "start": 205683, "end": 206033}, {"filename": "/tests/metron_good/basic_switch.h", "start": 206033, "end": 206510}, {"filename": "/tests/metron_good/dontcare.h", "start": 206510, "end": 206793}, {"filename": "/tests/metron_good/basic_literals.h", "start": 206793, "end": 207405}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 207405, "end": 207564}, {"filename": "/tests/metron_good/README.md", "start": 207564, "end": 207645}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 207645, "end": 208056}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 208056, "end": 208221}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 208221, "end": 208880}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 208880, "end": 209175}, {"filename": "/tests/metron_good/builtins.h", "start": 209175, "end": 209506}, {"filename": "/tests/metron_good/basic_template.h", "start": 209506, "end": 209937}, {"filename": "/tests/metron_good/defines.h", "start": 209937, "end": 210251}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 210251, "end": 210910}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 210910, "end": 211664}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 211664, "end": 211927}, {"filename": "/tests/metron_good/namespaces.h", "start": 211927, "end": 212277}, {"filename": "/tests/metron_good/include_guards.h", "start": 212277, "end": 212474}, {"filename": "/tests/metron_good/basic_task.h", "start": 212474, "end": 212808}, {"filename": "/tests/metron_good/enum_simple.h", "start": 212808, "end": 214176}, {"filename": "/tests/metron_good/oneliners.h", "start": 214176, "end": 214439}, {"filename": "/tests/metron_good/for_loops.h", "start": 214439, "end": 214759}, {"filename": "/tests/metron_good/private_getter.h", "start": 214759, "end": 214983}, {"filename": "/tests/metron_good/multi_tick.h", "start": 214983, "end": 215349}, {"filename": "/tests/metron_good/good_order.h", "start": 215349, "end": 215645}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 215645, "end": 215952}, {"filename": "/tests/metron_good/basic_output.h", "start": 215952, "end": 216213}, {"filename": "/tests/metron_good/basic_submod.h", "start": 216213, "end": 216502}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 216502, "end": 217056}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 217056, "end": 217236}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 217236, "end": 217612}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 217612, "end": 217926}, {"filename": "/tests/metron_good/input_signals.h", "start": 217926, "end": 218588}, {"filename": "/tests/metron_good/minimal.h", "start": 218588, "end": 218663}, {"filename": "/tests/metron_good/nested_structs.h", "start": 218663, "end": 219176}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 219176, "end": 219531}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 219531, "end": 219777}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 219777, "end": 219997}, {"filename": "/tests/metron_good/local_localparam.h", "start": 219997, "end": 220175}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 220175, "end": 220486}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 220486, "end": 221026}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 221026, "end": 221269}, {"filename": "/tests/metron_bad/README.md", "start": 221269, "end": 221466}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 221466, "end": 222061}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 222061, "end": 222507}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 222507, "end": 222647}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 222647, "end": 222907}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 222907, "end": 223155}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 223155, "end": 223451}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 223451, "end": 223958}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 223958, "end": 224368}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 224368, "end": 224640}], "remote_package_size": 224640});

  })();
