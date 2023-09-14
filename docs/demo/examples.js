
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22817}, {"filename": "/examples/ibex/README.md", "start": 22817, "end": 22868}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22868, "end": 24466}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24466, "end": 36565}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36565, "end": 50989}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50989, "end": 67025}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67025, "end": 69539}, {"filename": "/examples/j1/metron/dpram.h", "start": 69539, "end": 69990}, {"filename": "/examples/j1/metron/j1.h", "start": 69990, "end": 74023}, {"filename": "/examples/pong/README.md", "start": 74023, "end": 74083}, {"filename": "/examples/pong/metron/pong.h", "start": 74083, "end": 77960}, {"filename": "/examples/pong/reference/README.md", "start": 77960, "end": 78020}, {"filename": "/examples/rvsimple/README.md", "start": 78020, "end": 78099}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78099, "end": 78612}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78612, "end": 80086}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80086, "end": 82704}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82704, "end": 83932}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83932, "end": 89664}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89664, "end": 90787}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90787, "end": 92733}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92733, "end": 93973}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93973, "end": 95230}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95230, "end": 95912}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95912, "end": 96889}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96889, "end": 99020}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99020, "end": 99797}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99797, "end": 100490}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100490, "end": 101319}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101319, "end": 102331}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102331, "end": 103315}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103315, "end": 104013}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104013, "end": 107069}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107069, "end": 112656}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112656, "end": 115162}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115162, "end": 119902}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119902, "end": 121903}, {"filename": "/examples/scratch.h", "start": 121903, "end": 122099}, {"filename": "/examples/tutorial/adder.h", "start": 122099, "end": 122279}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122279, "end": 123273}, {"filename": "/examples/tutorial/blockram.h", "start": 123273, "end": 123803}, {"filename": "/examples/tutorial/checksum.h", "start": 123803, "end": 124539}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124539, "end": 125075}, {"filename": "/examples/tutorial/counter.h", "start": 125075, "end": 125224}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125224, "end": 126003}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126003, "end": 127421}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127421, "end": 127543}, {"filename": "/examples/tutorial/submodules.h", "start": 127543, "end": 128660}, {"filename": "/examples/tutorial/templates.h", "start": 128660, "end": 129149}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129149, "end": 129217}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129217, "end": 129258}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129258, "end": 129299}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129299, "end": 129340}, {"filename": "/examples/tutorial/vga.h", "start": 129340, "end": 130500}, {"filename": "/examples/uart/README.md", "start": 130500, "end": 130744}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 130744, "end": 133337}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 133337, "end": 135911}, {"filename": "/examples/uart/metron/uart_top.h", "start": 135911, "end": 137688}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 137688, "end": 140720}, {"filename": "/tests/metron_bad/README.md", "start": 140720, "end": 140917}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 140917, "end": 141226}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141226, "end": 141487}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141487, "end": 141743}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 141743, "end": 142351}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 142351, "end": 142591}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 142591, "end": 143014}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 143014, "end": 143567}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 143567, "end": 143891}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 143891, "end": 144350}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144350, "end": 144623}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 144623, "end": 144908}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 144908, "end": 145428}, {"filename": "/tests/metron_good/README.md", "start": 145428, "end": 145509}, {"filename": "/tests/metron_good/all_func_types.h", "start": 145509, "end": 147181}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 147181, "end": 147601}, {"filename": "/tests/metron_good/basic_function.h", "start": 147601, "end": 147893}, {"filename": "/tests/metron_good/basic_increment.h", "start": 147893, "end": 148261}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 148261, "end": 148569}, {"filename": "/tests/metron_good/basic_literals.h", "start": 148569, "end": 149194}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 149194, "end": 149453}, {"filename": "/tests/metron_good/basic_output.h", "start": 149453, "end": 149727}, {"filename": "/tests/metron_good/basic_param.h", "start": 149727, "end": 149999}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 149999, "end": 150243}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 150243, "end": 150436}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 150436, "end": 150712}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 150712, "end": 150945}, {"filename": "/tests/metron_good/basic_submod.h", "start": 150945, "end": 151264}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 151264, "end": 151632}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 151632, "end": 152021}, {"filename": "/tests/metron_good/basic_switch.h", "start": 152021, "end": 152511}, {"filename": "/tests/metron_good/basic_task.h", "start": 152511, "end": 152858}, {"filename": "/tests/metron_good/basic_template.h", "start": 152858, "end": 153357}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 153357, "end": 153529}, {"filename": "/tests/metron_good/bit_casts.h", "start": 153529, "end": 159515}, {"filename": "/tests/metron_good/bit_concat.h", "start": 159515, "end": 159955}, {"filename": "/tests/metron_good/bit_dup.h", "start": 159955, "end": 160627}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 160627, "end": 161478}, {"filename": "/tests/metron_good/builtins.h", "start": 161478, "end": 161822}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 161822, "end": 162142}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 162142, "end": 162709}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 162709, "end": 163563}, {"filename": "/tests/metron_good/constructor_args.h", "start": 163563, "end": 164035}, {"filename": "/tests/metron_good/defines.h", "start": 164035, "end": 164362}, {"filename": "/tests/metron_good/dontcare.h", "start": 164362, "end": 164655}, {"filename": "/tests/metron_good/enum_simple.h", "start": 164655, "end": 166097}, {"filename": "/tests/metron_good/for_loops.h", "start": 166097, "end": 166430}, {"filename": "/tests/metron_good/good_order.h", "start": 166430, "end": 166739}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 166739, "end": 167163}, {"filename": "/tests/metron_good/include_guards.h", "start": 167163, "end": 167360}, {"filename": "/tests/metron_good/init_chain.h", "start": 167360, "end": 168081}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 168081, "end": 168379}, {"filename": "/tests/metron_good/input_signals.h", "start": 168379, "end": 169054}, {"filename": "/tests/metron_good/local_localparam.h", "start": 169054, "end": 169245}, {"filename": "/tests/metron_good/magic_comments.h", "start": 169245, "end": 169561}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 169561, "end": 169888}, {"filename": "/tests/metron_good/minimal.h", "start": 169888, "end": 170074}, {"filename": "/tests/metron_good/multi_tick.h", "start": 170074, "end": 170453}, {"filename": "/tests/metron_good/namespaces.h", "start": 170453, "end": 170816}, {"filename": "/tests/metron_good/nested_structs.h", "start": 170816, "end": 171244}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171244, "end": 171802}, {"filename": "/tests/metron_good/oneliners.h", "start": 171802, "end": 172078}, {"filename": "/tests/metron_good/plus_equals.h", "start": 172078, "end": 172502}, {"filename": "/tests/metron_good/private_getter.h", "start": 172502, "end": 172739}, {"filename": "/tests/metron_good/structs.h", "start": 172739, "end": 172971}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 172971, "end": 173520}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 173520, "end": 176077}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 176077, "end": 176844}, {"filename": "/tests/metron_good/tock_task.h", "start": 176844, "end": 177213}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 177213, "end": 177391}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 177391, "end": 178063}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 178063, "end": 178735}], "remote_package_size": 178735});

  })();
