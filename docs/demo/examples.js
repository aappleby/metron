
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22817}, {"filename": "/examples/ibex/README.md", "start": 22817, "end": 22868}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22868, "end": 24466}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24466, "end": 36565}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36565, "end": 50989}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50989, "end": 67025}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67025, "end": 69539}, {"filename": "/examples/j1/metron/dpram.h", "start": 69539, "end": 69990}, {"filename": "/examples/j1/metron/j1.h", "start": 69990, "end": 74023}, {"filename": "/examples/pong/README.md", "start": 74023, "end": 74083}, {"filename": "/examples/pong/metron/pong.h", "start": 74083, "end": 77960}, {"filename": "/examples/pong/reference/README.md", "start": 77960, "end": 78020}, {"filename": "/examples/rvsimple/README.md", "start": 78020, "end": 78099}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78099, "end": 78612}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78612, "end": 80086}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80086, "end": 82704}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82704, "end": 83932}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83932, "end": 89664}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89664, "end": 90787}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90787, "end": 92733}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92733, "end": 93973}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93973, "end": 95230}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95230, "end": 95912}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95912, "end": 96889}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96889, "end": 99020}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99020, "end": 99797}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99797, "end": 100490}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100490, "end": 101319}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101319, "end": 102331}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102331, "end": 103315}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103315, "end": 104013}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104013, "end": 107069}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107069, "end": 112656}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112656, "end": 115162}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115162, "end": 119902}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119902, "end": 121903}, {"filename": "/examples/scratch.h", "start": 121903, "end": 122076}, {"filename": "/examples/tutorial/adder.h", "start": 122076, "end": 122256}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122256, "end": 123250}, {"filename": "/examples/tutorial/blockram.h", "start": 123250, "end": 123780}, {"filename": "/examples/tutorial/checksum.h", "start": 123780, "end": 124516}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124516, "end": 125052}, {"filename": "/examples/tutorial/counter.h", "start": 125052, "end": 125201}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125201, "end": 125980}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 125980, "end": 127398}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127398, "end": 127520}, {"filename": "/examples/tutorial/submodules.h", "start": 127520, "end": 128637}, {"filename": "/examples/tutorial/templates.h", "start": 128637, "end": 129126}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129126, "end": 129194}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129194, "end": 129235}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129235, "end": 129276}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129276, "end": 129317}, {"filename": "/examples/tutorial/vga.h", "start": 129317, "end": 130477}, {"filename": "/examples/uart/README.md", "start": 130477, "end": 130721}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 130721, "end": 133314}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 133314, "end": 135888}, {"filename": "/examples/uart/metron/uart_top.h", "start": 135888, "end": 137665}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 137665, "end": 140697}, {"filename": "/tests/metron_bad/README.md", "start": 140697, "end": 140894}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 140894, "end": 141203}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141203, "end": 141464}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141464, "end": 141720}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 141720, "end": 142328}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 142328, "end": 142568}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 142568, "end": 142991}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 142991, "end": 143544}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 143544, "end": 143868}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 143868, "end": 144327}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144327, "end": 144600}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 144600, "end": 144885}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 144885, "end": 145405}, {"filename": "/tests/metron_good/README.md", "start": 145405, "end": 145486}, {"filename": "/tests/metron_good/all_func_types.h", "start": 145486, "end": 147158}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 147158, "end": 147578}, {"filename": "/tests/metron_good/basic_function.h", "start": 147578, "end": 147870}, {"filename": "/tests/metron_good/basic_increment.h", "start": 147870, "end": 148238}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 148238, "end": 148546}, {"filename": "/tests/metron_good/basic_literals.h", "start": 148546, "end": 149171}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 149171, "end": 149430}, {"filename": "/tests/metron_good/basic_output.h", "start": 149430, "end": 149704}, {"filename": "/tests/metron_good/basic_param.h", "start": 149704, "end": 149976}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 149976, "end": 150220}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 150220, "end": 150413}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 150413, "end": 150689}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 150689, "end": 150922}, {"filename": "/tests/metron_good/basic_submod.h", "start": 150922, "end": 151224}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 151224, "end": 151592}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 151592, "end": 151981}, {"filename": "/tests/metron_good/basic_switch.h", "start": 151981, "end": 152471}, {"filename": "/tests/metron_good/basic_task.h", "start": 152471, "end": 152818}, {"filename": "/tests/metron_good/basic_template.h", "start": 152818, "end": 153317}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 153317, "end": 153489}, {"filename": "/tests/metron_good/bit_casts.h", "start": 153489, "end": 159475}, {"filename": "/tests/metron_good/bit_concat.h", "start": 159475, "end": 159915}, {"filename": "/tests/metron_good/bit_dup.h", "start": 159915, "end": 160587}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 160587, "end": 161438}, {"filename": "/tests/metron_good/builtins.h", "start": 161438, "end": 161782}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 161782, "end": 162102}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 162102, "end": 162669}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 162669, "end": 163509}, {"filename": "/tests/metron_good/constructor_args.h", "start": 163509, "end": 163981}, {"filename": "/tests/metron_good/defines.h", "start": 163981, "end": 164308}, {"filename": "/tests/metron_good/dontcare.h", "start": 164308, "end": 164601}, {"filename": "/tests/metron_good/enum_simple.h", "start": 164601, "end": 166043}, {"filename": "/tests/metron_good/for_loops.h", "start": 166043, "end": 166376}, {"filename": "/tests/metron_good/good_order.h", "start": 166376, "end": 166685}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 166685, "end": 167109}, {"filename": "/tests/metron_good/include_guards.h", "start": 167109, "end": 167306}, {"filename": "/tests/metron_good/init_chain.h", "start": 167306, "end": 168027}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 168027, "end": 168327}, {"filename": "/tests/metron_good/input_signals.h", "start": 168327, "end": 169002}, {"filename": "/tests/metron_good/local_localparam.h", "start": 169002, "end": 169193}, {"filename": "/tests/metron_good/magic_comments.h", "start": 169193, "end": 169509}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 169509, "end": 169836}, {"filename": "/tests/metron_good/minimal.h", "start": 169836, "end": 169924}, {"filename": "/tests/metron_good/multi_tick.h", "start": 169924, "end": 170303}, {"filename": "/tests/metron_good/namespaces.h", "start": 170303, "end": 170666}, {"filename": "/tests/metron_good/nested_structs.h", "start": 170666, "end": 171094}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171094, "end": 171652}, {"filename": "/tests/metron_good/oneliners.h", "start": 171652, "end": 171928}, {"filename": "/tests/metron_good/plus_equals.h", "start": 171928, "end": 172352}, {"filename": "/tests/metron_good/private_getter.h", "start": 172352, "end": 172589}, {"filename": "/tests/metron_good/structs.h", "start": 172589, "end": 172821}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 172821, "end": 173370}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 173370, "end": 175927}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 175927, "end": 176694}, {"filename": "/tests/metron_good/tock_task.h", "start": 176694, "end": 177063}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 177063, "end": 177241}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 177241, "end": 177913}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 177913, "end": 178585}], "remote_package_size": 178585});

  })();
