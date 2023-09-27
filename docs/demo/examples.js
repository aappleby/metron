
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22885}, {"filename": "/examples/ibex/README.md", "start": 22885, "end": 22936}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22936, "end": 24581}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24581, "end": 36772}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36772, "end": 51270}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 51270, "end": 67353}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67353, "end": 69944}, {"filename": "/examples/j1/metron/dpram.h", "start": 69944, "end": 70395}, {"filename": "/examples/j1/metron/j1.h", "start": 70395, "end": 74428}, {"filename": "/examples/pong/README.md", "start": 74428, "end": 74488}, {"filename": "/examples/pong/metron/pong.h", "start": 74488, "end": 78390}, {"filename": "/examples/pong/reference/README.md", "start": 78390, "end": 78450}, {"filename": "/examples/rvsimple/README.md", "start": 78450, "end": 78529}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78529, "end": 79042}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 79042, "end": 80516}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80516, "end": 83134}, {"filename": "/examples/rvsimple/metron/config.h", "start": 83134, "end": 84362}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 84362, "end": 90094}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 90094, "end": 91217}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 91217, "end": 93163}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 93163, "end": 94403}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 94403, "end": 95660}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95660, "end": 96342}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 96342, "end": 97319}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 97319, "end": 99450}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99450, "end": 100227}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 100227, "end": 100920}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100920, "end": 101749}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101749, "end": 102761}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102761, "end": 103745}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103745, "end": 104443}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104443, "end": 107499}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107499, "end": 113086}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 113086, "end": 115592}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115592, "end": 120332}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 120332, "end": 122333}, {"filename": "/examples/scratch.h", "start": 122333, "end": 122504}, {"filename": "/examples/tutorial/adder.h", "start": 122504, "end": 122684}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122684, "end": 123678}, {"filename": "/examples/tutorial/blockram.h", "start": 123678, "end": 124208}, {"filename": "/examples/tutorial/checksum.h", "start": 124208, "end": 124944}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124944, "end": 125480}, {"filename": "/examples/tutorial/counter.h", "start": 125480, "end": 125629}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125629, "end": 126408}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126408, "end": 127826}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127826, "end": 127948}, {"filename": "/examples/tutorial/submodules.h", "start": 127948, "end": 129065}, {"filename": "/examples/tutorial/templates.h", "start": 129065, "end": 129554}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129554, "end": 129622}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129622, "end": 129663}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129663, "end": 129704}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129704, "end": 129745}, {"filename": "/examples/tutorial/vga.h", "start": 129745, "end": 130905}, {"filename": "/examples/uart/README.md", "start": 130905, "end": 131149}, {"filename": "/examples/uart/message.txt", "start": 131149, "end": 131661}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 131661, "end": 134254}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 134254, "end": 136828}, {"filename": "/examples/uart/metron/uart_top.h", "start": 136828, "end": 138605}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 138605, "end": 141637}, {"filename": "/tests/metron_bad/README.md", "start": 141637, "end": 141834}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141834, "end": 142143}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 142143, "end": 142404}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 142404, "end": 142660}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142660, "end": 143268}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 143268, "end": 143519}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 143519, "end": 143942}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 143942, "end": 144495}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 144495, "end": 144819}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144819, "end": 145278}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 145278, "end": 145541}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 145541, "end": 145826}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 145826, "end": 146346}, {"filename": "/tests/metron_good/README.md", "start": 146346, "end": 146427}, {"filename": "/tests/metron_good/all_func_types.h", "start": 146427, "end": 148105}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 148105, "end": 148525}, {"filename": "/tests/metron_good/basic_function.h", "start": 148525, "end": 148817}, {"filename": "/tests/metron_good/basic_increment.h", "start": 148817, "end": 149185}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 149185, "end": 149493}, {"filename": "/tests/metron_good/basic_literals.h", "start": 149493, "end": 150118}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 150118, "end": 150377}, {"filename": "/tests/metron_good/basic_output.h", "start": 150377, "end": 150651}, {"filename": "/tests/metron_good/basic_param.h", "start": 150651, "end": 150923}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 150923, "end": 151167}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 151167, "end": 151360}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 151360, "end": 151636}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 151636, "end": 151869}, {"filename": "/tests/metron_good/basic_submod.h", "start": 151869, "end": 152188}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 152188, "end": 152556}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 152556, "end": 152945}, {"filename": "/tests/metron_good/basic_switch.h", "start": 152945, "end": 153435}, {"filename": "/tests/metron_good/basic_task.h", "start": 153435, "end": 153782}, {"filename": "/tests/metron_good/basic_template.h", "start": 153782, "end": 154281}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 154281, "end": 154453}, {"filename": "/tests/metron_good/bit_casts.h", "start": 154453, "end": 160439}, {"filename": "/tests/metron_good/bit_concat.h", "start": 160439, "end": 160879}, {"filename": "/tests/metron_good/bit_dup.h", "start": 160879, "end": 161551}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 161551, "end": 162459}, {"filename": "/tests/metron_good/builtins.h", "start": 162459, "end": 162803}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 162803, "end": 163123}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 163123, "end": 163690}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 163690, "end": 164544}, {"filename": "/tests/metron_good/constructor_args.h", "start": 164544, "end": 165016}, {"filename": "/tests/metron_good/defines.h", "start": 165016, "end": 165343}, {"filename": "/tests/metron_good/dontcare.h", "start": 165343, "end": 165636}, {"filename": "/tests/metron_good/enum_simple.h", "start": 165636, "end": 167078}, {"filename": "/tests/metron_good/for_loops.h", "start": 167078, "end": 167411}, {"filename": "/tests/metron_good/good_order.h", "start": 167411, "end": 167720}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 167720, "end": 168144}, {"filename": "/tests/metron_good/include_guards.h", "start": 168144, "end": 168341}, {"filename": "/tests/metron_good/init_chain.h", "start": 168341, "end": 169062}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 169062, "end": 169360}, {"filename": "/tests/metron_good/input_signals.h", "start": 169360, "end": 170158}, {"filename": "/tests/metron_good/local_localparam.h", "start": 170158, "end": 170349}, {"filename": "/tests/metron_good/magic_comments.h", "start": 170349, "end": 170665}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 170665, "end": 170992}, {"filename": "/tests/metron_good/minimal.h", "start": 170992, "end": 171178}, {"filename": "/tests/metron_good/multi_tick.h", "start": 171178, "end": 171557}, {"filename": "/tests/metron_good/namespaces.h", "start": 171557, "end": 171920}, {"filename": "/tests/metron_good/nested_structs.h", "start": 171920, "end": 172380}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 172380, "end": 172938}, {"filename": "/tests/metron_good/oneliners.h", "start": 172938, "end": 173214}, {"filename": "/tests/metron_good/plus_equals.h", "start": 173214, "end": 173638}, {"filename": "/tests/metron_good/private_getter.h", "start": 173638, "end": 173875}, {"filename": "/tests/metron_good/structs.h", "start": 173875, "end": 174107}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 174107, "end": 174656}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 174656, "end": 177213}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 177213, "end": 177980}, {"filename": "/tests/metron_good/tock_task.h", "start": 177980, "end": 178361}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 178361, "end": 178539}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 178539, "end": 179211}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 179211, "end": 179883}], "remote_package_size": 179883});

  })();
