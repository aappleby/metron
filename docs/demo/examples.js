
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22817}, {"filename": "/examples/ibex/README.md", "start": 22817, "end": 22868}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22868, "end": 24466}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24466, "end": 36565}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36565, "end": 50989}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50989, "end": 67025}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67025, "end": 69539}, {"filename": "/examples/j1/metron/dpram.h", "start": 69539, "end": 69990}, {"filename": "/examples/j1/metron/j1.h", "start": 69990, "end": 74023}, {"filename": "/examples/pong/README.md", "start": 74023, "end": 74083}, {"filename": "/examples/pong/metron/pong.h", "start": 74083, "end": 77960}, {"filename": "/examples/pong/reference/README.md", "start": 77960, "end": 78020}, {"filename": "/examples/rvsimple/README.md", "start": 78020, "end": 78099}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78099, "end": 78612}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78612, "end": 80086}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80086, "end": 82704}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82704, "end": 83932}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83932, "end": 89664}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89664, "end": 90787}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90787, "end": 92733}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92733, "end": 93973}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93973, "end": 95230}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95230, "end": 95912}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95912, "end": 96889}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96889, "end": 99020}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99020, "end": 99797}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99797, "end": 100490}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100490, "end": 101319}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101319, "end": 102331}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102331, "end": 103315}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103315, "end": 104013}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104013, "end": 107069}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107069, "end": 112656}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112656, "end": 115162}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115162, "end": 119902}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119902, "end": 121903}, {"filename": "/examples/scratch.h", "start": 121903, "end": 123786}, {"filename": "/examples/tutorial/adder.h", "start": 123786, "end": 123966}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 123966, "end": 124960}, {"filename": "/examples/tutorial/blockram.h", "start": 124960, "end": 125490}, {"filename": "/examples/tutorial/checksum.h", "start": 125490, "end": 126226}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 126226, "end": 126762}, {"filename": "/examples/tutorial/counter.h", "start": 126762, "end": 126911}, {"filename": "/examples/tutorial/declaration_order.h", "start": 126911, "end": 127690}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 127690, "end": 129108}, {"filename": "/examples/tutorial/nonblocking.h", "start": 129108, "end": 129230}, {"filename": "/examples/tutorial/submodules.h", "start": 129230, "end": 130347}, {"filename": "/examples/tutorial/templates.h", "start": 130347, "end": 130836}, {"filename": "/examples/tutorial/tutorial2.h", "start": 130836, "end": 130904}, {"filename": "/examples/tutorial/tutorial3.h", "start": 130904, "end": 130945}, {"filename": "/examples/tutorial/tutorial4.h", "start": 130945, "end": 130986}, {"filename": "/examples/tutorial/tutorial5.h", "start": 130986, "end": 131027}, {"filename": "/examples/tutorial/vga.h", "start": 131027, "end": 132187}, {"filename": "/examples/uart/README.md", "start": 132187, "end": 132431}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 132431, "end": 135024}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 135024, "end": 137598}, {"filename": "/examples/uart/metron/uart_top.h", "start": 137598, "end": 139375}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 139375, "end": 142407}, {"filename": "/tests/metron_bad/README.md", "start": 142407, "end": 142604}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 142604, "end": 142913}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 142913, "end": 143174}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 143174, "end": 143430}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 143430, "end": 144038}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 144038, "end": 144278}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 144278, "end": 144701}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 144701, "end": 145254}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 145254, "end": 145578}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 145578, "end": 146037}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 146037, "end": 146310}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 146310, "end": 146595}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 146595, "end": 147115}, {"filename": "/tests/metron_good/README.md", "start": 147115, "end": 147196}, {"filename": "/tests/metron_good/all_func_types.h", "start": 147196, "end": 148868}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 148868, "end": 149288}, {"filename": "/tests/metron_good/basic_function.h", "start": 149288, "end": 149580}, {"filename": "/tests/metron_good/basic_increment.h", "start": 149580, "end": 149948}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 149948, "end": 150256}, {"filename": "/tests/metron_good/basic_literals.h", "start": 150256, "end": 150881}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 150881, "end": 151140}, {"filename": "/tests/metron_good/basic_output.h", "start": 151140, "end": 151414}, {"filename": "/tests/metron_good/basic_param.h", "start": 151414, "end": 151686}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 151686, "end": 151930}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 151930, "end": 152123}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 152123, "end": 152399}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 152399, "end": 152632}, {"filename": "/tests/metron_good/basic_submod.h", "start": 152632, "end": 152934}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 152934, "end": 153302}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 153302, "end": 153691}, {"filename": "/tests/metron_good/basic_switch.h", "start": 153691, "end": 154181}, {"filename": "/tests/metron_good/basic_task.h", "start": 154181, "end": 154528}, {"filename": "/tests/metron_good/basic_template.h", "start": 154528, "end": 155027}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 155027, "end": 155199}, {"filename": "/tests/metron_good/bit_casts.h", "start": 155199, "end": 161185}, {"filename": "/tests/metron_good/bit_concat.h", "start": 161185, "end": 161625}, {"filename": "/tests/metron_good/bit_dup.h", "start": 161625, "end": 162297}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 162297, "end": 163148}, {"filename": "/tests/metron_good/builtins.h", "start": 163148, "end": 163492}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 163492, "end": 163812}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 163812, "end": 164379}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 164379, "end": 165219}, {"filename": "/tests/metron_good/constructor_args.h", "start": 165219, "end": 165691}, {"filename": "/tests/metron_good/defines.h", "start": 165691, "end": 166018}, {"filename": "/tests/metron_good/dontcare.h", "start": 166018, "end": 166311}, {"filename": "/tests/metron_good/enum_simple.h", "start": 166311, "end": 167753}, {"filename": "/tests/metron_good/for_loops.h", "start": 167753, "end": 168086}, {"filename": "/tests/metron_good/good_order.h", "start": 168086, "end": 168395}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 168395, "end": 168819}, {"filename": "/tests/metron_good/include_guards.h", "start": 168819, "end": 169016}, {"filename": "/tests/metron_good/init_chain.h", "start": 169016, "end": 169737}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 169737, "end": 170037}, {"filename": "/tests/metron_good/input_signals.h", "start": 170037, "end": 170712}, {"filename": "/tests/metron_good/local_localparam.h", "start": 170712, "end": 170903}, {"filename": "/tests/metron_good/magic_comments.h", "start": 170903, "end": 171219}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 171219, "end": 171546}, {"filename": "/tests/metron_good/minimal.h", "start": 171546, "end": 171634}, {"filename": "/tests/metron_good/multi_tick.h", "start": 171634, "end": 172013}, {"filename": "/tests/metron_good/namespaces.h", "start": 172013, "end": 172376}, {"filename": "/tests/metron_good/nested_structs.h", "start": 172376, "end": 172804}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 172804, "end": 173362}, {"filename": "/tests/metron_good/oneliners.h", "start": 173362, "end": 173638}, {"filename": "/tests/metron_good/plus_equals.h", "start": 173638, "end": 174062}, {"filename": "/tests/metron_good/private_getter.h", "start": 174062, "end": 174299}, {"filename": "/tests/metron_good/structs.h", "start": 174299, "end": 174531}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 174531, "end": 175080}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 175080, "end": 177637}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 177637, "end": 178404}, {"filename": "/tests/metron_good/tock_task.h", "start": 178404, "end": 178773}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 178773, "end": 178951}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 178951, "end": 179623}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 179623, "end": 180295}], "remote_package_size": 180295});

  })();
