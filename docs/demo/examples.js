
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22817}, {"filename": "/examples/ibex/README.md", "start": 22817, "end": 22868}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22868, "end": 24466}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24466, "end": 36565}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36565, "end": 50989}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50989, "end": 67025}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67025, "end": 69539}, {"filename": "/examples/j1/metron/dpram.h", "start": 69539, "end": 69990}, {"filename": "/examples/j1/metron/j1.h", "start": 69990, "end": 74023}, {"filename": "/examples/pong/README.md", "start": 74023, "end": 74083}, {"filename": "/examples/pong/metron/pong.h", "start": 74083, "end": 77960}, {"filename": "/examples/pong/reference/README.md", "start": 77960, "end": 78020}, {"filename": "/examples/rvsimple/README.md", "start": 78020, "end": 78099}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78099, "end": 78612}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78612, "end": 80086}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80086, "end": 82704}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82704, "end": 83932}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83932, "end": 89664}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89664, "end": 90787}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90787, "end": 92733}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92733, "end": 93973}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93973, "end": 95230}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95230, "end": 95912}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95912, "end": 96889}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96889, "end": 99020}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99020, "end": 99797}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99797, "end": 100490}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100490, "end": 101319}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101319, "end": 102331}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102331, "end": 103315}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103315, "end": 104013}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104013, "end": 107069}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107069, "end": 112656}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112656, "end": 115162}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115162, "end": 119902}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119902, "end": 121903}, {"filename": "/examples/scratch.h", "start": 121903, "end": 122190}, {"filename": "/examples/tutorial/adder.h", "start": 122190, "end": 122370}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122370, "end": 123364}, {"filename": "/examples/tutorial/blockram.h", "start": 123364, "end": 123894}, {"filename": "/examples/tutorial/checksum.h", "start": 123894, "end": 124630}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124630, "end": 125166}, {"filename": "/examples/tutorial/counter.h", "start": 125166, "end": 125315}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125315, "end": 126094}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126094, "end": 127512}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127512, "end": 127634}, {"filename": "/examples/tutorial/submodules.h", "start": 127634, "end": 128751}, {"filename": "/examples/tutorial/templates.h", "start": 128751, "end": 129240}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129240, "end": 129308}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129308, "end": 129349}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129349, "end": 129390}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129390, "end": 129431}, {"filename": "/examples/tutorial/vga.h", "start": 129431, "end": 130591}, {"filename": "/examples/uart/README.md", "start": 130591, "end": 130835}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 130835, "end": 133428}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 133428, "end": 136002}, {"filename": "/examples/uart/metron/uart_top.h", "start": 136002, "end": 137779}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 137779, "end": 140811}, {"filename": "/tests/metron_bad/README.md", "start": 140811, "end": 141008}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141008, "end": 141317}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141317, "end": 141578}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141578, "end": 141834}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 141834, "end": 142442}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 142442, "end": 142682}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 142682, "end": 143105}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 143105, "end": 143658}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 143658, "end": 143982}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 143982, "end": 144441}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144441, "end": 144714}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 144714, "end": 144999}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 144999, "end": 145519}, {"filename": "/tests/metron_good/README.md", "start": 145519, "end": 145600}, {"filename": "/tests/metron_good/all_func_types.h", "start": 145600, "end": 147272}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 147272, "end": 147692}, {"filename": "/tests/metron_good/basic_function.h", "start": 147692, "end": 147984}, {"filename": "/tests/metron_good/basic_increment.h", "start": 147984, "end": 148352}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 148352, "end": 148660}, {"filename": "/tests/metron_good/basic_literals.h", "start": 148660, "end": 149285}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 149285, "end": 149544}, {"filename": "/tests/metron_good/basic_output.h", "start": 149544, "end": 149818}, {"filename": "/tests/metron_good/basic_param.h", "start": 149818, "end": 150090}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 150090, "end": 150334}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 150334, "end": 150527}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 150527, "end": 150803}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 150803, "end": 151036}, {"filename": "/tests/metron_good/basic_submod.h", "start": 151036, "end": 151355}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 151355, "end": 151723}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 151723, "end": 152112}, {"filename": "/tests/metron_good/basic_switch.h", "start": 152112, "end": 152602}, {"filename": "/tests/metron_good/basic_task.h", "start": 152602, "end": 152949}, {"filename": "/tests/metron_good/basic_template.h", "start": 152949, "end": 153448}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 153448, "end": 153620}, {"filename": "/tests/metron_good/bit_casts.h", "start": 153620, "end": 159606}, {"filename": "/tests/metron_good/bit_concat.h", "start": 159606, "end": 160046}, {"filename": "/tests/metron_good/bit_dup.h", "start": 160046, "end": 160718}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 160718, "end": 161569}, {"filename": "/tests/metron_good/builtins.h", "start": 161569, "end": 161913}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 161913, "end": 162233}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 162233, "end": 162800}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 162800, "end": 163654}, {"filename": "/tests/metron_good/constructor_args.h", "start": 163654, "end": 164126}, {"filename": "/tests/metron_good/defines.h", "start": 164126, "end": 164453}, {"filename": "/tests/metron_good/dontcare.h", "start": 164453, "end": 164746}, {"filename": "/tests/metron_good/enum_simple.h", "start": 164746, "end": 166188}, {"filename": "/tests/metron_good/for_loops.h", "start": 166188, "end": 166521}, {"filename": "/tests/metron_good/good_order.h", "start": 166521, "end": 166830}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 166830, "end": 167254}, {"filename": "/tests/metron_good/include_guards.h", "start": 167254, "end": 167451}, {"filename": "/tests/metron_good/init_chain.h", "start": 167451, "end": 168172}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 168172, "end": 168470}, {"filename": "/tests/metron_good/input_signals.h", "start": 168470, "end": 169145}, {"filename": "/tests/metron_good/local_localparam.h", "start": 169145, "end": 169336}, {"filename": "/tests/metron_good/magic_comments.h", "start": 169336, "end": 169652}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 169652, "end": 169979}, {"filename": "/tests/metron_good/minimal.h", "start": 169979, "end": 170067}, {"filename": "/tests/metron_good/multi_tick.h", "start": 170067, "end": 170446}, {"filename": "/tests/metron_good/namespaces.h", "start": 170446, "end": 170809}, {"filename": "/tests/metron_good/nested_structs.h", "start": 170809, "end": 171237}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171237, "end": 171795}, {"filename": "/tests/metron_good/oneliners.h", "start": 171795, "end": 172071}, {"filename": "/tests/metron_good/plus_equals.h", "start": 172071, "end": 172495}, {"filename": "/tests/metron_good/private_getter.h", "start": 172495, "end": 172732}, {"filename": "/tests/metron_good/structs.h", "start": 172732, "end": 172964}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 172964, "end": 173513}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 173513, "end": 176070}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 176070, "end": 176837}, {"filename": "/tests/metron_good/tock_task.h", "start": 176837, "end": 177206}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 177206, "end": 177384}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 177384, "end": 178056}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 178056, "end": 178728}], "remote_package_size": 178728});

  })();
