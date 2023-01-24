
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
<<<<<<< HEAD
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22810}, {"filename": "/examples/ibex/README.md", "start": 22810, "end": 22861}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22861, "end": 24446}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24446, "end": 36544}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36544, "end": 50954}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50954, "end": 66978}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 66978, "end": 69479}, {"filename": "/examples/j1/metron/dpram.h", "start": 69479, "end": 69917}, {"filename": "/examples/j1/metron/j1.h", "start": 69917, "end": 73942}, {"filename": "/examples/pong/README.md", "start": 73942, "end": 74002}, {"filename": "/examples/pong/metron/pong.h", "start": 74002, "end": 77866}, {"filename": "/examples/pong/reference/README.md", "start": 77866, "end": 77926}, {"filename": "/examples/rvsimple/README.md", "start": 77926, "end": 78005}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78005, "end": 78505}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78505, "end": 79966}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 79966, "end": 82587}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82587, "end": 83802}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83802, "end": 89521}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89521, "end": 90635}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90635, "end": 92573}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92573, "end": 93813}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93813, "end": 94966}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 94966, "end": 95648}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95648, "end": 96521}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96521, "end": 98639}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 98639, "end": 99403}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99403, "end": 100094}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100094, "end": 100921}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 100921, "end": 101931}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 101931, "end": 102902}, {"filename": "/examples/rvsimple/metron/register.h", "start": 102902, "end": 103587}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 103587, "end": 106630}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 106630, "end": 112224}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112224, "end": 114717}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 114717, "end": 119444}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119444, "end": 121292}, {"filename": "/examples/scratch.h", "start": 121292, "end": 122052}, {"filename": "/examples/tutorial/adder.h", "start": 122052, "end": 122232}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122232, "end": 123213}, {"filename": "/examples/tutorial/blockram.h", "start": 123213, "end": 123730}, {"filename": "/examples/tutorial/checksum.h", "start": 123730, "end": 124453}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124453, "end": 124989}, {"filename": "/examples/tutorial/counter.h", "start": 124989, "end": 125138}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125138, "end": 125917}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 125917, "end": 127335}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127335, "end": 127457}, {"filename": "/examples/tutorial/submodules.h", "start": 127457, "end": 128574}, {"filename": "/examples/tutorial/templates.h", "start": 128574, "end": 129045}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129045, "end": 129113}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129113, "end": 129154}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129154, "end": 129195}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129195, "end": 129236}, {"filename": "/examples/tutorial/vga.h", "start": 129236, "end": 130383}, {"filename": "/examples/uart/README.md", "start": 130383, "end": 130627}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 130627, "end": 133207}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 133207, "end": 135768}, {"filename": "/examples/uart/metron/uart_top.h", "start": 135768, "end": 137532}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 137532, "end": 140551}, {"filename": "/tests/metron_bad/README.md", "start": 140551, "end": 140748}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 140748, "end": 141044}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141044, "end": 141292}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141292, "end": 141535}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 141535, "end": 142130}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 142130, "end": 142540}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 142540, "end": 143080}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 143080, "end": 143391}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 143391, "end": 143837}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 143837, "end": 144097}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 144097, "end": 144369}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 144369, "end": 144876}, {"filename": "/tests/metron_good/README.md", "start": 144876, "end": 144957}, {"filename": "/tests/metron_good/all_func_types.h", "start": 144957, "end": 146522}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 146522, "end": 146929}, {"filename": "/tests/metron_good/basic_function.h", "start": 146929, "end": 147208}, {"filename": "/tests/metron_good/basic_increment.h", "start": 147208, "end": 147563}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 147563, "end": 147858}, {"filename": "/tests/metron_good/basic_literals.h", "start": 147858, "end": 148470}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 148470, "end": 148716}, {"filename": "/tests/metron_good/basic_output.h", "start": 148716, "end": 148977}, {"filename": "/tests/metron_good/basic_param.h", "start": 148977, "end": 149236}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 149236, "end": 149467}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 149467, "end": 149647}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 149647, "end": 149910}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 149910, "end": 150130}, {"filename": "/tests/metron_good/basic_submod.h", "start": 150130, "end": 150419}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 150419, "end": 150774}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 150774, "end": 151150}, {"filename": "/tests/metron_good/basic_switch.h", "start": 151150, "end": 151627}, {"filename": "/tests/metron_good/basic_task.h", "start": 151627, "end": 151961}, {"filename": "/tests/metron_good/basic_template.h", "start": 151961, "end": 152447}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 152447, "end": 152606}, {"filename": "/tests/metron_good/bit_casts.h", "start": 152606, "end": 158579}, {"filename": "/tests/metron_good/bit_concat.h", "start": 158579, "end": 159006}, {"filename": "/tests/metron_good/bit_dup.h", "start": 159006, "end": 159665}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 159665, "end": 160503}, {"filename": "/tests/metron_good/builtins.h", "start": 160503, "end": 160834}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 160834, "end": 161141}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 161141, "end": 161695}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 161695, "end": 162588}, {"filename": "/tests/metron_good/constructor_args.h", "start": 162588, "end": 163096}, {"filename": "/tests/metron_good/defines.h", "start": 163096, "end": 163410}, {"filename": "/tests/metron_good/dontcare.h", "start": 163410, "end": 163693}, {"filename": "/tests/metron_good/enum_simple.h", "start": 163693, "end": 165061}, {"filename": "/tests/metron_good/for_loops.h", "start": 165061, "end": 165381}, {"filename": "/tests/metron_good/good_order.h", "start": 165381, "end": 165677}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 165677, "end": 166088}, {"filename": "/tests/metron_good/include_guards.h", "start": 166088, "end": 166285}, {"filename": "/tests/metron_good/init_chain.h", "start": 166285, "end": 166993}, {"filename": "/tests/metron_good/input_signals.h", "start": 166993, "end": 167655}, {"filename": "/tests/metron_good/local_localparam.h", "start": 167655, "end": 167833}, {"filename": "/tests/metron_good/magic_comments.h", "start": 167833, "end": 168136}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 168136, "end": 168450}, {"filename": "/tests/metron_good/minimal.h", "start": 168450, "end": 168525}, {"filename": "/tests/metron_good/multi_tick.h", "start": 168525, "end": 168891}, {"filename": "/tests/metron_good/namespaces.h", "start": 168891, "end": 169241}, {"filename": "/tests/metron_good/nested_structs.h", "start": 169241, "end": 169754}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 169754, "end": 170299}, {"filename": "/tests/metron_good/oneliners.h", "start": 170299, "end": 170562}, {"filename": "/tests/metron_good/plus_equals.h", "start": 170562, "end": 170912}, {"filename": "/tests/metron_good/private_getter.h", "start": 170912, "end": 171136}, {"filename": "/tests/metron_good/structs.h", "start": 171136, "end": 171355}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 171355, "end": 171891}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 171891, "end": 174252}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 174252, "end": 175006}, {"filename": "/tests/metron_good/tock_task.h", "start": 175006, "end": 175362}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 175362, "end": 175527}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 175527, "end": 176186}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 176186, "end": 176845}], "remote_package_size": 176845});
=======
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/ibex/README.md", "start": 26876, "end": 26927}, {"filename": "/examples/ibex/main.cpp", "start": 26927, "end": 27068}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 27068, "end": 28653}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 28653, "end": 40751}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 40751, "end": 55161}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 55161, "end": 71185}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 71185, "end": 73686}, {"filename": "/examples/ibex/metron_ref/ibex_alu.sv", "start": 73686, "end": 126004}, {"filename": "/examples/ibex/metron_ref/ibex_compressed_decoder.sv", "start": 126004, "end": 136846}, {"filename": "/examples/ibex/metron_ref/ibex_multdiv_slow.sv", "start": 136846, "end": 149956}, {"filename": "/examples/ibex/metron_ref/ibex_pkg.sv", "start": 149956, "end": 166195}, {"filename": "/examples/ibex/metron_ref/prim_arbiter_fixed.sv", "start": 166195, "end": 172306}, {"filename": "/examples/j1/main.cpp", "start": 172306, "end": 172425}, {"filename": "/examples/j1/metron/dpram.h", "start": 172425, "end": 172863}, {"filename": "/examples/j1/metron/j1.h", "start": 172863, "end": 176888}, {"filename": "/examples/j1/metron_sv/j1.sv", "start": 176888, "end": 176916}, {"filename": "/examples/pong/README.md", "start": 176916, "end": 176976}, {"filename": "/examples/pong/main.cpp", "start": 176976, "end": 178922}, {"filename": "/examples/pong/main_vl.cpp", "start": 178922, "end": 179073}, {"filename": "/examples/pong/metron/pong.h", "start": 179073, "end": 182937}, {"filename": "/examples/pong/reference/README.md", "start": 182937, "end": 182997}, {"filename": "/examples/pong/reference/hvsync_generator.sv", "start": 182997, "end": 183936}, {"filename": "/examples/pong/reference/pong.sv", "start": 183936, "end": 187550}, {"filename": "/examples/rvsimple/README.md", "start": 187550, "end": 187629}, {"filename": "/examples/rvsimple/main.cpp", "start": 187629, "end": 190444}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 190444, "end": 193346}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 193346, "end": 196248}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 196248, "end": 196748}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 196748, "end": 198209}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 198209, "end": 200830}, {"filename": "/examples/rvsimple/metron/config.h", "start": 200830, "end": 202045}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 202045, "end": 207764}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 207764, "end": 208878}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 208878, "end": 210816}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 210816, "end": 212056}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 212056, "end": 213209}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 213209, "end": 213891}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 213891, "end": 214764}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 214764, "end": 216882}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 216882, "end": 217646}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 217646, "end": 218337}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 218337, "end": 219164}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 219164, "end": 220174}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 220174, "end": 221145}, {"filename": "/examples/rvsimple/metron/register.h", "start": 221145, "end": 221830}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 221830, "end": 224873}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 224873, "end": 230467}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 230467, "end": 232960}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 232960, "end": 237687}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 237687, "end": 239535}, {"filename": "/examples/rvsimple/reference_sv/adder.sv", "start": 239535, "end": 239984}, {"filename": "/examples/rvsimple/reference_sv/alu.sv", "start": 239984, "end": 243105}, {"filename": "/examples/rvsimple/reference_sv/alu_control.sv", "start": 243105, "end": 246558}, {"filename": "/examples/rvsimple/reference_sv/config.sv", "start": 246558, "end": 247965}, {"filename": "/examples/rvsimple/reference_sv/constants.sv", "start": 247965, "end": 252702}, {"filename": "/examples/rvsimple/reference_sv/control_transfer.sv", "start": 252702, "end": 253587}, {"filename": "/examples/rvsimple/reference_sv/data_memory_interface.sv", "start": 253587, "end": 255498}, {"filename": "/examples/rvsimple/reference_sv/example_data_memory.sv", "start": 255498, "end": 256413}, {"filename": "/examples/rvsimple/reference_sv/example_data_memory_bus.sv", "start": 256413, "end": 257427}, {"filename": "/examples/rvsimple/reference_sv/example_memory_bus.sv", "start": 257427, "end": 258735}, {"filename": "/examples/rvsimple/reference_sv/example_text_memory.sv", "start": 258735, "end": 259261}, {"filename": "/examples/rvsimple/reference_sv/example_text_memory_bus.sv", "start": 259261, "end": 259946}, {"filename": "/examples/rvsimple/reference_sv/immediate_generator.sv", "start": 259946, "end": 261886}, {"filename": "/examples/rvsimple/reference_sv/instruction_decoder.sv", "start": 261886, "end": 262599}, {"filename": "/examples/rvsimple/reference_sv/multiplexer.sv", "start": 262599, "end": 263377}, {"filename": "/examples/rvsimple/reference_sv/multiplexer2.sv", "start": 263377, "end": 263965}, {"filename": "/examples/rvsimple/reference_sv/multiplexer4.sv", "start": 263965, "end": 264619}, {"filename": "/examples/rvsimple/reference_sv/multiplexer8.sv", "start": 264619, "end": 265405}, {"filename": "/examples/rvsimple/reference_sv/regfile.sv", "start": 265405, "end": 266314}, {"filename": "/examples/rvsimple/reference_sv/register.sv", "start": 266314, "end": 266947}, {"filename": "/examples/rvsimple/reference_sv/riscv_core.sv", "start": 266947, "end": 270321}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_control.sv", "start": 270321, "end": 275053}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_ctlpath.sv", "start": 275053, "end": 276843}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_datapath.sv", "start": 276843, "end": 281072}, {"filename": "/examples/rvsimple/reference_sv/toplevel.sv", "start": 281072, "end": 282703}, {"filename": "/examples/scratch.h", "start": 282703, "end": 282963}, {"filename": "/examples/scratch.sv", "start": 282963, "end": 283671}, {"filename": "/examples/tutorial/adder.h", "start": 283671, "end": 283851}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 283851, "end": 284832}, {"filename": "/examples/tutorial/blockram.h", "start": 284832, "end": 285349}, {"filename": "/examples/tutorial/checksum.h", "start": 285349, "end": 286072}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 286072, "end": 286608}, {"filename": "/examples/tutorial/counter.h", "start": 286608, "end": 286757}, {"filename": "/examples/tutorial/declaration_order.h", "start": 286757, "end": 287536}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 287536, "end": 288954}, {"filename": "/examples/tutorial/nonblocking.h", "start": 288954, "end": 289076}, {"filename": "/examples/tutorial/submodules.h", "start": 289076, "end": 290193}, {"filename": "/examples/tutorial/templates.h", "start": 290193, "end": 290664}, {"filename": "/examples/tutorial/tutorial2.h", "start": 290664, "end": 290732}, {"filename": "/examples/tutorial/tutorial3.h", "start": 290732, "end": 290773}, {"filename": "/examples/tutorial/tutorial4.h", "start": 290773, "end": 290814}, {"filename": "/examples/tutorial/tutorial5.h", "start": 290814, "end": 290855}, {"filename": "/examples/tutorial/vga.h", "start": 290855, "end": 292002}, {"filename": "/examples/uart/README.md", "start": 292002, "end": 292246}, {"filename": "/examples/uart/main.cpp", "start": 292246, "end": 293578}, {"filename": "/examples/uart/main_vl.cpp", "start": 293578, "end": 296098}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 296098, "end": 298678}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 298678, "end": 301239}, {"filename": "/examples/uart/metron/uart_top.h", "start": 301239, "end": 303003}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 303003, "end": 306022}, {"filename": "/examples/uart/uart_test_ice40.sv", "start": 306022, "end": 308197}, {"filename": "/examples/uart/uart_test_iv.sv", "start": 308197, "end": 309794}, {"filename": "/tests/metron_bad/README.md", "start": 309794, "end": 309991}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 309991, "end": 310287}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 310287, "end": 310535}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 310535, "end": 310778}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 310778, "end": 311373}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 311373, "end": 311783}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 311783, "end": 312323}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 312323, "end": 312634}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 312634, "end": 313080}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 313080, "end": 313340}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 313340, "end": 313612}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 313612, "end": 314119}, {"filename": "/tests/metron_good/README.md", "start": 314119, "end": 314200}, {"filename": "/tests/metron_good/all_func_types.h", "start": 314200, "end": 315765}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 315765, "end": 316172}, {"filename": "/tests/metron_good/basic_function.h", "start": 316172, "end": 316451}, {"filename": "/tests/metron_good/basic_increment.h", "start": 316451, "end": 316806}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 316806, "end": 317101}, {"filename": "/tests/metron_good/basic_literals.h", "start": 317101, "end": 317713}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 317713, "end": 317959}, {"filename": "/tests/metron_good/basic_output.h", "start": 317959, "end": 318220}, {"filename": "/tests/metron_good/basic_param.h", "start": 318220, "end": 318479}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 318479, "end": 318710}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 318710, "end": 318890}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 318890, "end": 319153}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 319153, "end": 319373}, {"filename": "/tests/metron_good/basic_submod.h", "start": 319373, "end": 319662}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 319662, "end": 320017}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 320017, "end": 320393}, {"filename": "/tests/metron_good/basic_switch.h", "start": 320393, "end": 320870}, {"filename": "/tests/metron_good/basic_task.h", "start": 320870, "end": 321204}, {"filename": "/tests/metron_good/basic_template.h", "start": 321204, "end": 321690}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 321690, "end": 321849}, {"filename": "/tests/metron_good/bit_casts.h", "start": 321849, "end": 327822}, {"filename": "/tests/metron_good/bit_concat.h", "start": 327822, "end": 328249}, {"filename": "/tests/metron_good/bit_dup.h", "start": 328249, "end": 328908}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 328908, "end": 329746}, {"filename": "/tests/metron_good/builtins.h", "start": 329746, "end": 330077}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 330077, "end": 330384}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 330384, "end": 330938}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 330938, "end": 331831}, {"filename": "/tests/metron_good/constructor_args.h", "start": 331831, "end": 332339}, {"filename": "/tests/metron_good/defines.h", "start": 332339, "end": 332653}, {"filename": "/tests/metron_good/dontcare.h", "start": 332653, "end": 332936}, {"filename": "/tests/metron_good/enum_simple.h", "start": 332936, "end": 334304}, {"filename": "/tests/metron_good/for_loops.h", "start": 334304, "end": 334624}, {"filename": "/tests/metron_good/good_order.h", "start": 334624, "end": 334920}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 334920, "end": 335331}, {"filename": "/tests/metron_good/include_guards.h", "start": 335331, "end": 335528}, {"filename": "/tests/metron_good/init_chain.h", "start": 335528, "end": 336236}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 336236, "end": 336537}, {"filename": "/tests/metron_good/input_signals.h", "start": 336537, "end": 337199}, {"filename": "/tests/metron_good/local_localparam.h", "start": 337199, "end": 337377}, {"filename": "/tests/metron_good/magic_comments.h", "start": 337377, "end": 337680}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 337680, "end": 337994}, {"filename": "/tests/metron_good/minimal.h", "start": 337994, "end": 338069}, {"filename": "/tests/metron_good/multi_tick.h", "start": 338069, "end": 338435}, {"filename": "/tests/metron_good/namespaces.h", "start": 338435, "end": 338785}, {"filename": "/tests/metron_good/nested_structs.h", "start": 338785, "end": 339298}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 339298, "end": 339843}, {"filename": "/tests/metron_good/oneliners.h", "start": 339843, "end": 340106}, {"filename": "/tests/metron_good/plus_equals.h", "start": 340106, "end": 340456}, {"filename": "/tests/metron_good/private_getter.h", "start": 340456, "end": 340680}, {"filename": "/tests/metron_good/structs.h", "start": 340680, "end": 340899}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 340899, "end": 341435}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 341435, "end": 343796}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 343796, "end": 344550}, {"filename": "/tests/metron_good/tock_task.h", "start": 344550, "end": 344906}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 344906, "end": 345071}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 345071, "end": 345730}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 345730, "end": 346389}], "remote_package_size": 346389});
>>>>>>> 0240e96 (cleaning up test suite a bit)

  })();
