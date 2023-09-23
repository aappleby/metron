
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22885}, {"filename": "/examples/ibex/README.md", "start": 22885, "end": 22936}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22936, "end": 24581}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24581, "end": 36772}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36772, "end": 51270}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 51270, "end": 67353}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67353, "end": 69944}, {"filename": "/examples/j1/metron/dpram.h", "start": 69944, "end": 70395}, {"filename": "/examples/j1/metron/j1.h", "start": 70395, "end": 74428}, {"filename": "/examples/pong/README.md", "start": 74428, "end": 74488}, {"filename": "/examples/pong/metron/pong.h", "start": 74488, "end": 78390}, {"filename": "/examples/pong/reference/README.md", "start": 78390, "end": 78450}, {"filename": "/examples/rvsimple/README.md", "start": 78450, "end": 78529}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78529, "end": 79042}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 79042, "end": 80516}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80516, "end": 83134}, {"filename": "/examples/rvsimple/metron/config.h", "start": 83134, "end": 84362}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 84362, "end": 90094}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 90094, "end": 91217}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 91217, "end": 93163}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 93163, "end": 94403}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 94403, "end": 95660}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95660, "end": 96342}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 96342, "end": 97319}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 97319, "end": 99450}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99450, "end": 100227}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 100227, "end": 100920}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100920, "end": 101749}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101749, "end": 102761}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102761, "end": 103745}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103745, "end": 104443}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104443, "end": 107499}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107499, "end": 113086}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 113086, "end": 115592}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115592, "end": 120332}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 120332, "end": 122333}, {"filename": "/examples/scratch.h", "start": 122333, "end": 122619}, {"filename": "/examples/tutorial/adder.h", "start": 122619, "end": 122799}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122799, "end": 123793}, {"filename": "/examples/tutorial/blockram.h", "start": 123793, "end": 124323}, {"filename": "/examples/tutorial/checksum.h", "start": 124323, "end": 125059}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 125059, "end": 125595}, {"filename": "/examples/tutorial/counter.h", "start": 125595, "end": 125744}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125744, "end": 126523}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126523, "end": 127941}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127941, "end": 128063}, {"filename": "/examples/tutorial/submodules.h", "start": 128063, "end": 129180}, {"filename": "/examples/tutorial/templates.h", "start": 129180, "end": 129669}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129669, "end": 129737}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129737, "end": 129778}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129778, "end": 129819}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129819, "end": 129860}, {"filename": "/examples/tutorial/vga.h", "start": 129860, "end": 131020}, {"filename": "/examples/uart/README.md", "start": 131020, "end": 131264}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 131264, "end": 133857}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 133857, "end": 136431}, {"filename": "/examples/uart/metron/uart_top.h", "start": 136431, "end": 138208}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 138208, "end": 141240}, {"filename": "/tests/metron_bad/README.md", "start": 141240, "end": 141437}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141437, "end": 141746}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141746, "end": 142007}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 142007, "end": 142263}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142263, "end": 142871}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 142871, "end": 143111}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 143111, "end": 143534}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 143534, "end": 144087}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 144087, "end": 144411}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144411, "end": 144870}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144870, "end": 145143}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 145143, "end": 145428}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 145428, "end": 145948}, {"filename": "/tests/metron_good/README.md", "start": 145948, "end": 146029}, {"filename": "/tests/metron_good/all_func_types.h", "start": 146029, "end": 147707}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 147707, "end": 148127}, {"filename": "/tests/metron_good/basic_function.h", "start": 148127, "end": 148419}, {"filename": "/tests/metron_good/basic_increment.h", "start": 148419, "end": 148787}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 148787, "end": 149095}, {"filename": "/tests/metron_good/basic_literals.h", "start": 149095, "end": 149720}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 149720, "end": 149979}, {"filename": "/tests/metron_good/basic_output.h", "start": 149979, "end": 150253}, {"filename": "/tests/metron_good/basic_param.h", "start": 150253, "end": 150525}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 150525, "end": 150769}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 150769, "end": 150962}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 150962, "end": 151238}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 151238, "end": 151471}, {"filename": "/tests/metron_good/basic_submod.h", "start": 151471, "end": 151790}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 151790, "end": 152158}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 152158, "end": 152547}, {"filename": "/tests/metron_good/basic_switch.h", "start": 152547, "end": 153037}, {"filename": "/tests/metron_good/basic_task.h", "start": 153037, "end": 153384}, {"filename": "/tests/metron_good/basic_template.h", "start": 153384, "end": 153883}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 153883, "end": 154055}, {"filename": "/tests/metron_good/bit_casts.h", "start": 154055, "end": 160041}, {"filename": "/tests/metron_good/bit_concat.h", "start": 160041, "end": 160481}, {"filename": "/tests/metron_good/bit_dup.h", "start": 160481, "end": 161153}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 161153, "end": 162061}, {"filename": "/tests/metron_good/builtins.h", "start": 162061, "end": 162405}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 162405, "end": 162725}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 162725, "end": 163292}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 163292, "end": 164146}, {"filename": "/tests/metron_good/constructor_args.h", "start": 164146, "end": 164618}, {"filename": "/tests/metron_good/defines.h", "start": 164618, "end": 164945}, {"filename": "/tests/metron_good/dontcare.h", "start": 164945, "end": 165238}, {"filename": "/tests/metron_good/enum_simple.h", "start": 165238, "end": 166680}, {"filename": "/tests/metron_good/for_loops.h", "start": 166680, "end": 167013}, {"filename": "/tests/metron_good/good_order.h", "start": 167013, "end": 167322}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 167322, "end": 167746}, {"filename": "/tests/metron_good/include_guards.h", "start": 167746, "end": 167943}, {"filename": "/tests/metron_good/init_chain.h", "start": 167943, "end": 168664}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 168664, "end": 168962}, {"filename": "/tests/metron_good/input_signals.h", "start": 168962, "end": 169760}, {"filename": "/tests/metron_good/local_localparam.h", "start": 169760, "end": 169951}, {"filename": "/tests/metron_good/magic_comments.h", "start": 169951, "end": 170267}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 170267, "end": 170594}, {"filename": "/tests/metron_good/minimal.h", "start": 170594, "end": 170780}, {"filename": "/tests/metron_good/multi_tick.h", "start": 170780, "end": 171159}, {"filename": "/tests/metron_good/namespaces.h", "start": 171159, "end": 171522}, {"filename": "/tests/metron_good/nested_structs.h", "start": 171522, "end": 171982}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171982, "end": 172540}, {"filename": "/tests/metron_good/oneliners.h", "start": 172540, "end": 172816}, {"filename": "/tests/metron_good/plus_equals.h", "start": 172816, "end": 173240}, {"filename": "/tests/metron_good/private_getter.h", "start": 173240, "end": 173477}, {"filename": "/tests/metron_good/structs.h", "start": 173477, "end": 173709}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 173709, "end": 174258}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 174258, "end": 176815}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 176815, "end": 177582}, {"filename": "/tests/metron_good/tock_task.h", "start": 177582, "end": 177963}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 177963, "end": 178141}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 178141, "end": 178813}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 178813, "end": 179485}], "remote_package_size": 179485});

  })();
