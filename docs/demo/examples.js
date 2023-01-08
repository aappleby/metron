
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
Module['FS_createPath']("/examples", "gb_spu", true, true);
Module['FS_createPath']("/examples/gb_spu", "metron", true, true);
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples/j1", "metron", true, true);
Module['FS_createPath']("/examples", "pinwheel", true, true);
Module['FS_createPath']("/examples/pinwheel", "metron", true, true);
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22810}, {"filename": "/examples/ibex/README.md", "start": 22810, "end": 22861}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22861, "end": 24446}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24446, "end": 36544}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36544, "end": 50954}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50954, "end": 66978}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 66978, "end": 69479}, {"filename": "/examples/j1/metron/dpram.h", "start": 69479, "end": 69917}, {"filename": "/examples/j1/metron/j1.h", "start": 69917, "end": 73942}, {"filename": "/examples/pinwheel/README.md", "start": 73942, "end": 74106}, {"filename": "/examples/pinwheel/metron/constants.h", "start": 74106, "end": 76664}, {"filename": "/examples/pinwheel/metron/pinwheel.h", "start": 76664, "end": 88167}, {"filename": "/examples/pong/README.md", "start": 88167, "end": 88227}, {"filename": "/examples/pong/metron/pong.h", "start": 88227, "end": 92091}, {"filename": "/examples/pong/reference/README.md", "start": 92091, "end": 92151}, {"filename": "/examples/rvsimple/README.md", "start": 92151, "end": 92230}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 92230, "end": 92730}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 92730, "end": 94191}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 94191, "end": 96812}, {"filename": "/examples/rvsimple/metron/config.h", "start": 96812, "end": 98027}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 98027, "end": 103746}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 103746, "end": 104860}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 104860, "end": 106798}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 106798, "end": 108032}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 108032, "end": 109185}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 109185, "end": 109861}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 109861, "end": 110734}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 110734, "end": 112852}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 112852, "end": 113616}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 113616, "end": 114307}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 114307, "end": 115134}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 115134, "end": 116144}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 116144, "end": 117115}, {"filename": "/examples/rvsimple/metron/register.h", "start": 117115, "end": 117800}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 117800, "end": 120843}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 120843, "end": 126437}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 126437, "end": 128930}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 128930, "end": 133657}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 133657, "end": 135505}, {"filename": "/examples/scratch.h", "start": 135505, "end": 135712}, {"filename": "/examples/tutorial/adder.h", "start": 135712, "end": 135892}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 135892, "end": 136873}, {"filename": "/examples/tutorial/blockram.h", "start": 136873, "end": 137390}, {"filename": "/examples/tutorial/checksum.h", "start": 137390, "end": 138113}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 138113, "end": 138649}, {"filename": "/examples/tutorial/counter.h", "start": 138649, "end": 138798}, {"filename": "/examples/tutorial/declaration_order.h", "start": 138798, "end": 139577}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 139577, "end": 140995}, {"filename": "/examples/tutorial/nonblocking.h", "start": 140995, "end": 141117}, {"filename": "/examples/tutorial/submodules.h", "start": 141117, "end": 142234}, {"filename": "/examples/tutorial/templates.h", "start": 142234, "end": 142705}, {"filename": "/examples/tutorial/tutorial2.h", "start": 142705, "end": 142773}, {"filename": "/examples/tutorial/tutorial3.h", "start": 142773, "end": 142814}, {"filename": "/examples/tutorial/tutorial4.h", "start": 142814, "end": 142855}, {"filename": "/examples/tutorial/tutorial5.h", "start": 142855, "end": 142896}, {"filename": "/examples/tutorial/vga.h", "start": 142896, "end": 144043}, {"filename": "/examples/uart/README.md", "start": 144043, "end": 144287}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 144287, "end": 146867}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 146867, "end": 149428}, {"filename": "/examples/uart/metron/uart_top.h", "start": 149428, "end": 151192}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 151192, "end": 154211}, {"filename": "/tests/metron_bad/README.md", "start": 154211, "end": 154408}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 154408, "end": 154704}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 154704, "end": 154952}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 154952, "end": 155195}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 155195, "end": 155790}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 155790, "end": 155930}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 155930, "end": 156340}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 156340, "end": 156880}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 156880, "end": 157191}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 157191, "end": 157637}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 157637, "end": 157897}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 157897, "end": 158169}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 158169, "end": 158676}, {"filename": "/tests/metron_good/README.md", "start": 158676, "end": 158757}, {"filename": "/tests/metron_good/all_func_types.h", "start": 158757, "end": 160322}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 160322, "end": 160729}, {"filename": "/tests/metron_good/basic_function.h", "start": 160729, "end": 161008}, {"filename": "/tests/metron_good/basic_increment.h", "start": 161008, "end": 161363}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 161363, "end": 161658}, {"filename": "/tests/metron_good/basic_literals.h", "start": 161658, "end": 162270}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 162270, "end": 162516}, {"filename": "/tests/metron_good/basic_output.h", "start": 162516, "end": 162777}, {"filename": "/tests/metron_good/basic_param.h", "start": 162777, "end": 163036}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 163036, "end": 163267}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 163267, "end": 163447}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 163447, "end": 163710}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 163710, "end": 163930}, {"filename": "/tests/metron_good/basic_submod.h", "start": 163930, "end": 164219}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 164219, "end": 164574}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 164574, "end": 164950}, {"filename": "/tests/metron_good/basic_switch.h", "start": 164950, "end": 165427}, {"filename": "/tests/metron_good/basic_task.h", "start": 165427, "end": 165761}, {"filename": "/tests/metron_good/basic_template.h", "start": 165761, "end": 166192}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 166192, "end": 166351}, {"filename": "/tests/metron_good/bit_casts.h", "start": 166351, "end": 172324}, {"filename": "/tests/metron_good/bit_concat.h", "start": 172324, "end": 172751}, {"filename": "/tests/metron_good/bit_dup.h", "start": 172751, "end": 173410}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 173410, "end": 174248}, {"filename": "/tests/metron_good/builtins.h", "start": 174248, "end": 174579}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 174579, "end": 174886}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 174886, "end": 175440}, {"filename": "/tests/metron_good/defines.h", "start": 175440, "end": 175754}, {"filename": "/tests/metron_good/dontcare.h", "start": 175754, "end": 176037}, {"filename": "/tests/metron_good/enum_simple.h", "start": 176037, "end": 177405}, {"filename": "/tests/metron_good/for_loops.h", "start": 177405, "end": 177725}, {"filename": "/tests/metron_good/good_order.h", "start": 177725, "end": 178021}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 178021, "end": 178432}, {"filename": "/tests/metron_good/include_guards.h", "start": 178432, "end": 178629}, {"filename": "/tests/metron_good/init_chain.h", "start": 178629, "end": 179337}, {"filename": "/tests/metron_good/input_signals.h", "start": 179337, "end": 179999}, {"filename": "/tests/metron_good/local_localparam.h", "start": 179999, "end": 180177}, {"filename": "/tests/metron_good/magic_comments.h", "start": 180177, "end": 180480}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 180480, "end": 180794}, {"filename": "/tests/metron_good/minimal.h", "start": 180794, "end": 180869}, {"filename": "/tests/metron_good/multi_tick.h", "start": 180869, "end": 181235}, {"filename": "/tests/metron_good/namespaces.h", "start": 181235, "end": 181585}, {"filename": "/tests/metron_good/nested_structs.h", "start": 181585, "end": 182098}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 182098, "end": 182643}, {"filename": "/tests/metron_good/oneliners.h", "start": 182643, "end": 182906}, {"filename": "/tests/metron_good/plus_equals.h", "start": 182906, "end": 183256}, {"filename": "/tests/metron_good/private_getter.h", "start": 183256, "end": 183480}, {"filename": "/tests/metron_good/structs.h", "start": 183480, "end": 183699}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 183699, "end": 184453}, {"filename": "/tests/metron_good/tock_task.h", "start": 184453, "end": 184809}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 184809, "end": 184974}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 184974, "end": 185633}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 185633, "end": 186292}], "remote_package_size": 186292});

  })();
