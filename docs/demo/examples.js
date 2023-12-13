
  var Module = typeof Module !== 'undefined' ? Module : {};

  if (!Module.expectedDataFileDownloads) {
    Module.expectedDataFileDownloads = 0;
  }

  Module.expectedDataFileDownloads++;
  (function() {
    // Do not attempt to redownload the virtual filesystem data when in a pthread or a Wasm Worker context.
    if (Module['ENVIRONMENT_IS_PTHREAD'] || Module['$ww']) return;
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
            if (Module['setStatus']) Module['setStatus'](`Downloading data... (${loaded}/${total})`);
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
          Module['addRunDependency'](`fp ${this.name}`);
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
          Module['removeRunDependency'](`fp ${that.name}`);
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/pong/README.md", "start": 71623, "end": 71683}, {"filename": "/examples/pong/metron/pong.h", "start": 71683, "end": 75674}, {"filename": "/examples/pong/reference/README.md", "start": 75674, "end": 75734}, {"filename": "/examples/rvsimple/README.md", "start": 75734, "end": 75813}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 75813, "end": 76320}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 76320, "end": 77788}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 77788, "end": 80400}, {"filename": "/examples/rvsimple/metron/config.h", "start": 80400, "end": 81622}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 81622, "end": 87348}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 87348, "end": 88465}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 88465, "end": 90405}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 90405, "end": 91645}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 91645, "end": 92896}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 92896, "end": 93575}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 93575, "end": 94546}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 94546, "end": 96671}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 96671, "end": 97442}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 97442, "end": 98129}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 98129, "end": 98952}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 98952, "end": 99958}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 99958, "end": 100936}, {"filename": "/examples/rvsimple/metron/register.h", "start": 100936, "end": 101632}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 101632, "end": 104682}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 104682, "end": 110263}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 110263, "end": 112763}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 112763, "end": 117501}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 117501, "end": 119496}, {"filename": "/examples/scratch.h", "start": 119496, "end": 119890}, {"filename": "/examples/tutorial/adder.h", "start": 119890, "end": 120072}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 120072, "end": 121060}, {"filename": "/examples/tutorial/blockram.h", "start": 121060, "end": 121588}, {"filename": "/examples/tutorial/checksum.h", "start": 121588, "end": 122330}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 122330, "end": 122906}, {"filename": "/examples/tutorial/counter.h", "start": 122906, "end": 123057}, {"filename": "/examples/tutorial/declaration_order.h", "start": 123057, "end": 123883}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 123883, "end": 125311}, {"filename": "/examples/tutorial/nonblocking.h", "start": 125311, "end": 125437}, {"filename": "/examples/tutorial/submodules.h", "start": 125437, "end": 126556}, {"filename": "/examples/tutorial/templates.h", "start": 126556, "end": 127039}, {"filename": "/examples/tutorial/tutorial2.h", "start": 127039, "end": 127107}, {"filename": "/examples/tutorial/tutorial3.h", "start": 127107, "end": 127148}, {"filename": "/examples/tutorial/tutorial4.h", "start": 127148, "end": 127189}, {"filename": "/examples/tutorial/tutorial5.h", "start": 127189, "end": 127230}, {"filename": "/examples/tutorial/vga.h", "start": 127230, "end": 128410}, {"filename": "/examples/uart/README.md", "start": 128410, "end": 128654}, {"filename": "/examples/uart/message.txt", "start": 128654, "end": 129166}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 129166, "end": 131804}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 131804, "end": 134495}, {"filename": "/examples/uart/metron/uart_top.h", "start": 134495, "end": 136283}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 136283, "end": 139340}, {"filename": "/tests/metron_bad/README.md", "start": 139340, "end": 139537}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 139537, "end": 139844}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 139844, "end": 140099}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 140099, "end": 140319}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 140319, "end": 140805}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 140805, "end": 141042}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 141042, "end": 141459}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 141459, "end": 141925}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 141925, "end": 142243}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 142243, "end": 142695}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 142695, "end": 142952}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 142952, "end": 143234}, {"filename": "/tests/metron_bad/unorderable_ticks.h", "start": 143234, "end": 143470}, {"filename": "/tests/metron_bad/unorderable_tocks.h", "start": 143470, "end": 143700}, {"filename": "/tests/metron_bad/wrong_submod_call_order.h", "start": 143700, "end": 144069}, {"filename": "/tests/metron_good/README.md", "start": 144069, "end": 144150}, {"filename": "/tests/metron_good/all_func_types.h", "start": 144150, "end": 145651}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 145651, "end": 146068}, {"filename": "/tests/metron_good/basic_function.h", "start": 146068, "end": 146320}, {"filename": "/tests/metron_good/basic_increment.h", "start": 146320, "end": 146647}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 146647, "end": 146952}, {"filename": "/tests/metron_good/basic_literals.h", "start": 146952, "end": 147571}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 147571, "end": 147827}, {"filename": "/tests/metron_good/basic_output.h", "start": 147827, "end": 148099}, {"filename": "/tests/metron_good/basic_param.h", "start": 148099, "end": 148368}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 148368, "end": 148569}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 148569, "end": 148756}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 148756, "end": 148987}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 148987, "end": 149214}, {"filename": "/tests/metron_good/basic_submod.h", "start": 149214, "end": 149530}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 149530, "end": 149895}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 149895, "end": 150281}, {"filename": "/tests/metron_good/basic_switch.h", "start": 150281, "end": 150770}, {"filename": "/tests/metron_good/basic_task.h", "start": 150770, "end": 151105}, {"filename": "/tests/metron_good/basic_template.h", "start": 151105, "end": 151607}, {"filename": "/tests/metron_good/bit_casts.h", "start": 151607, "end": 157587}, {"filename": "/tests/metron_good/bit_concat.h", "start": 157587, "end": 158021}, {"filename": "/tests/metron_good/bit_dup.h", "start": 158021, "end": 158687}, {"filename": "/tests/metron_good/bitfields.h", "start": 158687, "end": 159893}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 159893, "end": 160710}, {"filename": "/tests/metron_good/builtins.h", "start": 160710, "end": 161048}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 161048, "end": 161365}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 161365, "end": 161950}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 161950, "end": 162813}, {"filename": "/tests/metron_good/constructor_args.h", "start": 162813, "end": 163285}, {"filename": "/tests/metron_good/counter.h", "start": 163285, "end": 163932}, {"filename": "/tests/metron_good/defines.h", "start": 163932, "end": 164253}, {"filename": "/tests/metron_good/dontcare.h", "start": 164253, "end": 164540}, {"filename": "/tests/metron_good/enum_simple.h", "start": 164540, "end": 165935}, {"filename": "/tests/metron_good/for_loops.h", "start": 165935, "end": 166262}, {"filename": "/tests/metron_good/good_order.h", "start": 166262, "end": 166465}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 166465, "end": 166886}, {"filename": "/tests/metron_good/include_guards.h", "start": 166886, "end": 167083}, {"filename": "/tests/metron_good/init_chain.h", "start": 167083, "end": 167804}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 167804, "end": 168096}, {"filename": "/tests/metron_good/input_signals.h", "start": 168096, "end": 168892}, {"filename": "/tests/metron_good/local_localparam.h", "start": 168892, "end": 169077}, {"filename": "/tests/metron_good/magic_comments.h", "start": 169077, "end": 169390}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 169390, "end": 169711}, {"filename": "/tests/metron_good/minimal.h", "start": 169711, "end": 169891}, {"filename": "/tests/metron_good/multi_tick.h", "start": 169891, "end": 170267}, {"filename": "/tests/metron_good/namespaces.h", "start": 170267, "end": 170648}, {"filename": "/tests/metron_good/nested_structs.h", "start": 170648, "end": 171109}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171109, "end": 171682}, {"filename": "/tests/metron_good/noconvert.h", "start": 171682, "end": 172012}, {"filename": "/tests/metron_good/oneliners.h", "start": 172012, "end": 172286}, {"filename": "/tests/metron_good/plus_equals.h", "start": 172286, "end": 172773}, {"filename": "/tests/metron_good/private_getter.h", "start": 172773, "end": 173025}, {"filename": "/tests/metron_good/self_reference.h", "start": 173025, "end": 173230}, {"filename": "/tests/metron_good/slice.h", "start": 173230, "end": 173737}, {"filename": "/tests/metron_good/structs.h", "start": 173737, "end": 174235}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 174235, "end": 174780}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 174780, "end": 177400}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 177400, "end": 178194}, {"filename": "/tests/metron_good/tock_task.h", "start": 178194, "end": 178646}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 178646, "end": 178818}, {"filename": "/tests/metron_good/unions.h", "start": 178818, "end": 179024}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 179024, "end": 179693}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 179693, "end": 180362}], "remote_package_size": 180362});

  })();
