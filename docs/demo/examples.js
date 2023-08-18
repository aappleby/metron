
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22817}, {"filename": "/examples/ibex/README.md", "start": 22817, "end": 22868}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22868, "end": 24466}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24466, "end": 36565}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36565, "end": 50989}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50989, "end": 67025}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67025, "end": 69539}, {"filename": "/examples/j1/metron/dpram.h", "start": 69539, "end": 69990}, {"filename": "/examples/j1/metron/j1.h", "start": 69990, "end": 74023}, {"filename": "/examples/pong/README.md", "start": 74023, "end": 74083}, {"filename": "/examples/pong/metron/pong.h", "start": 74083, "end": 77960}, {"filename": "/examples/pong/reference/README.md", "start": 77960, "end": 78020}, {"filename": "/examples/rvsimple/README.md", "start": 78020, "end": 78099}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78099, "end": 78612}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78612, "end": 80086}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80086, "end": 82704}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82704, "end": 83932}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83932, "end": 89664}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89664, "end": 90787}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90787, "end": 92733}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92733, "end": 93973}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93973, "end": 95230}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95230, "end": 95912}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95912, "end": 96889}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96889, "end": 99020}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99020, "end": 99797}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99797, "end": 100490}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100490, "end": 101319}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101319, "end": 102331}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102331, "end": 103315}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103315, "end": 104013}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104013, "end": 107069}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107069, "end": 112656}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112656, "end": 115162}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115162, "end": 119902}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119902, "end": 121903}, {"filename": "/examples/scratch.h", "start": 121903, "end": 122361}, {"filename": "/examples/tutorial/adder.h", "start": 122361, "end": 122541}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122541, "end": 123535}, {"filename": "/examples/tutorial/blockram.h", "start": 123535, "end": 124065}, {"filename": "/examples/tutorial/checksum.h", "start": 124065, "end": 124801}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124801, "end": 125337}, {"filename": "/examples/tutorial/counter.h", "start": 125337, "end": 125486}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125486, "end": 126265}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126265, "end": 127683}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127683, "end": 127805}, {"filename": "/examples/tutorial/submodules.h", "start": 127805, "end": 128922}, {"filename": "/examples/tutorial/templates.h", "start": 128922, "end": 129411}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129411, "end": 129479}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129479, "end": 129520}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129520, "end": 129561}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129561, "end": 129602}, {"filename": "/examples/tutorial/vga.h", "start": 129602, "end": 130762}, {"filename": "/examples/uart/README.md", "start": 130762, "end": 131006}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 131006, "end": 133599}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 133599, "end": 136173}, {"filename": "/examples/uart/metron/uart_top.h", "start": 136173, "end": 137950}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 137950, "end": 140982}, {"filename": "/tests/metron_bad/README.md", "start": 140982, "end": 141179}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141179, "end": 141488}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141488, "end": 141749}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141749, "end": 142005}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142005, "end": 142613}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 142613, "end": 142853}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 142853, "end": 143276}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 143276, "end": 143829}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 143829, "end": 144153}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144153, "end": 144612}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144612, "end": 144885}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 144885, "end": 145170}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 145170, "end": 145690}, {"filename": "/tests/metron_good/README.md", "start": 145690, "end": 145771}, {"filename": "/tests/metron_good/all_func_types.h", "start": 145771, "end": 147443}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 147443, "end": 147863}, {"filename": "/tests/metron_good/basic_function.h", "start": 147863, "end": 148155}, {"filename": "/tests/metron_good/basic_increment.h", "start": 148155, "end": 148523}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 148523, "end": 148831}, {"filename": "/tests/metron_good/basic_literals.h", "start": 148831, "end": 149456}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 149456, "end": 149715}, {"filename": "/tests/metron_good/basic_output.h", "start": 149715, "end": 149989}, {"filename": "/tests/metron_good/basic_param.h", "start": 149989, "end": 150261}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 150261, "end": 150505}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 150505, "end": 150698}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 150698, "end": 150974}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 150974, "end": 151207}, {"filename": "/tests/metron_good/basic_submod.h", "start": 151207, "end": 151509}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 151509, "end": 151877}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 151877, "end": 152266}, {"filename": "/tests/metron_good/basic_switch.h", "start": 152266, "end": 152756}, {"filename": "/tests/metron_good/basic_task.h", "start": 152756, "end": 153103}, {"filename": "/tests/metron_good/basic_template.h", "start": 153103, "end": 153602}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 153602, "end": 153774}, {"filename": "/tests/metron_good/bit_casts.h", "start": 153774, "end": 159760}, {"filename": "/tests/metron_good/bit_concat.h", "start": 159760, "end": 160200}, {"filename": "/tests/metron_good/bit_dup.h", "start": 160200, "end": 160872}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 160872, "end": 161723}, {"filename": "/tests/metron_good/builtins.h", "start": 161723, "end": 162067}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 162067, "end": 162387}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 162387, "end": 162954}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 162954, "end": 163794}, {"filename": "/tests/metron_good/constructor_args.h", "start": 163794, "end": 164266}, {"filename": "/tests/metron_good/defines.h", "start": 164266, "end": 164593}, {"filename": "/tests/metron_good/dontcare.h", "start": 164593, "end": 164886}, {"filename": "/tests/metron_good/enum_simple.h", "start": 164886, "end": 166328}, {"filename": "/tests/metron_good/for_loops.h", "start": 166328, "end": 166661}, {"filename": "/tests/metron_good/good_order.h", "start": 166661, "end": 166970}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 166970, "end": 167394}, {"filename": "/tests/metron_good/include_guards.h", "start": 167394, "end": 167591}, {"filename": "/tests/metron_good/init_chain.h", "start": 167591, "end": 168312}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 168312, "end": 168612}, {"filename": "/tests/metron_good/input_signals.h", "start": 168612, "end": 169287}, {"filename": "/tests/metron_good/local_localparam.h", "start": 169287, "end": 169478}, {"filename": "/tests/metron_good/magic_comments.h", "start": 169478, "end": 169794}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 169794, "end": 170121}, {"filename": "/tests/metron_good/minimal.h", "start": 170121, "end": 170209}, {"filename": "/tests/metron_good/multi_tick.h", "start": 170209, "end": 170588}, {"filename": "/tests/metron_good/namespaces.h", "start": 170588, "end": 170951}, {"filename": "/tests/metron_good/nested_structs.h", "start": 170951, "end": 171379}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171379, "end": 171937}, {"filename": "/tests/metron_good/oneliners.h", "start": 171937, "end": 172213}, {"filename": "/tests/metron_good/plus_equals.h", "start": 172213, "end": 172637}, {"filename": "/tests/metron_good/private_getter.h", "start": 172637, "end": 172874}, {"filename": "/tests/metron_good/structs.h", "start": 172874, "end": 173106}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 173106, "end": 173655}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 173655, "end": 176212}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 176212, "end": 176979}, {"filename": "/tests/metron_good/tock_task.h", "start": 176979, "end": 177348}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 177348, "end": 177526}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 177526, "end": 178198}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 178198, "end": 178870}], "remote_package_size": 178870});

  })();
