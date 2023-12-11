
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67332}, {"filename": "/examples/j1/metron/j1.h", "start": 67332, "end": 71623}, {"filename": "/examples/pong/README.md", "start": 71623, "end": 71683}, {"filename": "/examples/pong/metron/pong.h", "start": 71683, "end": 75674}, {"filename": "/examples/pong/reference/README.md", "start": 75674, "end": 75734}, {"filename": "/examples/rvsimple/README.md", "start": 75734, "end": 75813}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 75813, "end": 76320}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 76320, "end": 77788}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 77788, "end": 80400}, {"filename": "/examples/rvsimple/metron/config.h", "start": 80400, "end": 81622}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 81622, "end": 87348}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 87348, "end": 88465}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 88465, "end": 90405}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 90405, "end": 91645}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 91645, "end": 92896}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 92896, "end": 93575}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 93575, "end": 94546}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 94546, "end": 96671}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 96671, "end": 97442}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 97442, "end": 98129}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 98129, "end": 98952}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 98952, "end": 99958}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 99958, "end": 100936}, {"filename": "/examples/rvsimple/metron/register.h", "start": 100936, "end": 101632}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 101632, "end": 104682}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 104682, "end": 110263}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 110263, "end": 112763}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 112763, "end": 117501}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 117501, "end": 119496}, {"filename": "/examples/scratch.h", "start": 119496, "end": 119765}, {"filename": "/examples/tutorial/adder.h", "start": 119765, "end": 119947}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 119947, "end": 120935}, {"filename": "/examples/tutorial/blockram.h", "start": 120935, "end": 121463}, {"filename": "/examples/tutorial/checksum.h", "start": 121463, "end": 122205}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 122205, "end": 122781}, {"filename": "/examples/tutorial/counter.h", "start": 122781, "end": 122932}, {"filename": "/examples/tutorial/declaration_order.h", "start": 122932, "end": 123758}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 123758, "end": 125186}, {"filename": "/examples/tutorial/nonblocking.h", "start": 125186, "end": 125312}, {"filename": "/examples/tutorial/submodules.h", "start": 125312, "end": 126431}, {"filename": "/examples/tutorial/templates.h", "start": 126431, "end": 126914}, {"filename": "/examples/tutorial/tutorial2.h", "start": 126914, "end": 126982}, {"filename": "/examples/tutorial/tutorial3.h", "start": 126982, "end": 127023}, {"filename": "/examples/tutorial/tutorial4.h", "start": 127023, "end": 127064}, {"filename": "/examples/tutorial/tutorial5.h", "start": 127064, "end": 127105}, {"filename": "/examples/tutorial/vga.h", "start": 127105, "end": 128285}, {"filename": "/examples/uart/README.md", "start": 128285, "end": 128529}, {"filename": "/examples/uart/message.txt", "start": 128529, "end": 129041}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 129041, "end": 131750}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 131750, "end": 134441}, {"filename": "/examples/uart/metron/uart_top.h", "start": 134441, "end": 136229}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 136229, "end": 139286}, {"filename": "/tests/metron_bad/README.md", "start": 139286, "end": 139483}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 139483, "end": 139790}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 139790, "end": 140045}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 140045, "end": 140265}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 140265, "end": 140751}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 140751, "end": 140988}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 140988, "end": 141405}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 141405, "end": 141871}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 141871, "end": 142189}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 142189, "end": 142641}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 142641, "end": 142898}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 142898, "end": 143180}, {"filename": "/tests/metron_bad/unorderable_ticks.h", "start": 143180, "end": 143416}, {"filename": "/tests/metron_bad/unorderable_tocks.h", "start": 143416, "end": 143646}, {"filename": "/tests/metron_bad/wrong_submod_call_order.h", "start": 143646, "end": 144015}, {"filename": "/tests/metron_good/README.md", "start": 144015, "end": 144096}, {"filename": "/tests/metron_good/all_func_types.h", "start": 144096, "end": 145816}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 145816, "end": 146233}, {"filename": "/tests/metron_good/basic_function.h", "start": 146233, "end": 146485}, {"filename": "/tests/metron_good/basic_increment.h", "start": 146485, "end": 146812}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 146812, "end": 147117}, {"filename": "/tests/metron_good/basic_literals.h", "start": 147117, "end": 147736}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 147736, "end": 147992}, {"filename": "/tests/metron_good/basic_output.h", "start": 147992, "end": 148264}, {"filename": "/tests/metron_good/basic_param.h", "start": 148264, "end": 148533}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 148533, "end": 148734}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 148734, "end": 148921}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 148921, "end": 149152}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 149152, "end": 149379}, {"filename": "/tests/metron_good/basic_submod.h", "start": 149379, "end": 149695}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 149695, "end": 150060}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 150060, "end": 150446}, {"filename": "/tests/metron_good/basic_switch.h", "start": 150446, "end": 150935}, {"filename": "/tests/metron_good/basic_task.h", "start": 150935, "end": 151283}, {"filename": "/tests/metron_good/basic_template.h", "start": 151283, "end": 151764}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 151764, "end": 151930}, {"filename": "/tests/metron_good/bit_casts.h", "start": 151930, "end": 157910}, {"filename": "/tests/metron_good/bit_concat.h", "start": 157910, "end": 158344}, {"filename": "/tests/metron_good/bit_dup.h", "start": 158344, "end": 159010}, {"filename": "/tests/metron_good/bitfields.h", "start": 159010, "end": 160216}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 160216, "end": 161126}, {"filename": "/tests/metron_good/builtins.h", "start": 161126, "end": 161464}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 161464, "end": 161781}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 161781, "end": 162346}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 162346, "end": 163209}, {"filename": "/tests/metron_good/constructor_args.h", "start": 163209, "end": 163681}, {"filename": "/tests/metron_good/counter.h", "start": 163681, "end": 164328}, {"filename": "/tests/metron_good/defines.h", "start": 164328, "end": 164649}, {"filename": "/tests/metron_good/dontcare.h", "start": 164649, "end": 164936}, {"filename": "/tests/metron_good/enum_simple.h", "start": 164936, "end": 166331}, {"filename": "/tests/metron_good/for_loops.h", "start": 166331, "end": 166658}, {"filename": "/tests/metron_good/good_order.h", "start": 166658, "end": 166861}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 166861, "end": 167282}, {"filename": "/tests/metron_good/include_guards.h", "start": 167282, "end": 167479}, {"filename": "/tests/metron_good/init_chain.h", "start": 167479, "end": 168214}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 168214, "end": 168506}, {"filename": "/tests/metron_good/input_signals.h", "start": 168506, "end": 169304}, {"filename": "/tests/metron_good/local_localparam.h", "start": 169304, "end": 169489}, {"filename": "/tests/metron_good/magic_comments.h", "start": 169489, "end": 169802}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 169802, "end": 170123}, {"filename": "/tests/metron_good/minimal.h", "start": 170123, "end": 170303}, {"filename": "/tests/metron_good/multi_tick.h", "start": 170303, "end": 170682}, {"filename": "/tests/metron_good/namespaces.h", "start": 170682, "end": 171042}, {"filename": "/tests/metron_good/nested_structs.h", "start": 171042, "end": 171503}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171503, "end": 172055}, {"filename": "/tests/metron_good/noconvert.h", "start": 172055, "end": 172385}, {"filename": "/tests/metron_good/oneliners.h", "start": 172385, "end": 172659}, {"filename": "/tests/metron_good/plus_equals.h", "start": 172659, "end": 173146}, {"filename": "/tests/metron_good/private_getter.h", "start": 173146, "end": 173377}, {"filename": "/tests/metron_good/self_reference.h", "start": 173377, "end": 173582}, {"filename": "/tests/metron_good/slice.h", "start": 173582, "end": 174089}, {"filename": "/tests/metron_good/structs.h", "start": 174089, "end": 174554}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 174554, "end": 175099}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 175099, "end": 177719}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 177719, "end": 178480}, {"filename": "/tests/metron_good/tock_task.h", "start": 178480, "end": 178855}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 178855, "end": 179027}, {"filename": "/tests/metron_good/unions.h", "start": 179027, "end": 179233}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 179233, "end": 179902}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 179902, "end": 180571}], "remote_package_size": 180571});

  })();
