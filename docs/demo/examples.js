
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22810}, {"filename": "/examples/ibex/README.md", "start": 22810, "end": 22861}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22861, "end": 24446}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24446, "end": 36544}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36544, "end": 50954}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50954, "end": 66978}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 66978, "end": 69479}, {"filename": "/examples/j1/metron/dpram.h", "start": 69479, "end": 69917}, {"filename": "/examples/j1/metron/j1.h", "start": 69917, "end": 73942}, {"filename": "/examples/pong/README.md", "start": 73942, "end": 74002}, {"filename": "/examples/pong/metron/pong.h", "start": 74002, "end": 77866}, {"filename": "/examples/pong/reference/README.md", "start": 77866, "end": 77926}, {"filename": "/examples/rvsimple/README.md", "start": 77926, "end": 78005}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78005, "end": 78505}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78505, "end": 79966}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 79966, "end": 82587}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82587, "end": 83802}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83802, "end": 89521}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89521, "end": 90635}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90635, "end": 92573}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92573, "end": 93813}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93813, "end": 94966}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 94966, "end": 95648}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95648, "end": 96521}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96521, "end": 98639}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 98639, "end": 99403}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99403, "end": 100094}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100094, "end": 100921}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 100921, "end": 101931}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 101931, "end": 102902}, {"filename": "/examples/rvsimple/metron/register.h", "start": 102902, "end": 103587}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 103587, "end": 106630}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 106630, "end": 112224}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112224, "end": 114717}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 114717, "end": 119444}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119444, "end": 121292}, {"filename": "/examples/scratch.h", "start": 121292, "end": 122052}, {"filename": "/examples/tutorial/adder.h", "start": 122052, "end": 122232}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122232, "end": 123213}, {"filename": "/examples/tutorial/blockram.h", "start": 123213, "end": 123730}, {"filename": "/examples/tutorial/checksum.h", "start": 123730, "end": 124453}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124453, "end": 124989}, {"filename": "/examples/tutorial/counter.h", "start": 124989, "end": 125138}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125138, "end": 125917}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 125917, "end": 127335}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127335, "end": 127457}, {"filename": "/examples/tutorial/submodules.h", "start": 127457, "end": 128574}, {"filename": "/examples/tutorial/templates.h", "start": 128574, "end": 129045}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129045, "end": 129113}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129113, "end": 129154}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129154, "end": 129195}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129195, "end": 129236}, {"filename": "/examples/tutorial/vga.h", "start": 129236, "end": 130383}, {"filename": "/examples/uart/README.md", "start": 130383, "end": 130627}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 130627, "end": 133207}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 133207, "end": 135768}, {"filename": "/examples/uart/metron/uart_top.h", "start": 135768, "end": 137532}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 137532, "end": 140551}, {"filename": "/tests/metron_bad/README.md", "start": 140551, "end": 140748}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 140748, "end": 141044}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141044, "end": 141292}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141292, "end": 141535}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 141535, "end": 142130}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 142130, "end": 142540}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 142540, "end": 143080}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 143080, "end": 143391}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 143391, "end": 143837}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 143837, "end": 144097}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 144097, "end": 144369}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 144369, "end": 144876}, {"filename": "/tests/metron_good/README.md", "start": 144876, "end": 144957}, {"filename": "/tests/metron_good/all_func_types.h", "start": 144957, "end": 146522}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 146522, "end": 146929}, {"filename": "/tests/metron_good/basic_function.h", "start": 146929, "end": 147208}, {"filename": "/tests/metron_good/basic_increment.h", "start": 147208, "end": 147563}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 147563, "end": 147858}, {"filename": "/tests/metron_good/basic_literals.h", "start": 147858, "end": 148470}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 148470, "end": 148716}, {"filename": "/tests/metron_good/basic_output.h", "start": 148716, "end": 148977}, {"filename": "/tests/metron_good/basic_param.h", "start": 148977, "end": 149236}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 149236, "end": 149467}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 149467, "end": 149647}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 149647, "end": 149910}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 149910, "end": 150130}, {"filename": "/tests/metron_good/basic_submod.h", "start": 150130, "end": 150419}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 150419, "end": 150774}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 150774, "end": 151150}, {"filename": "/tests/metron_good/basic_switch.h", "start": 151150, "end": 151627}, {"filename": "/tests/metron_good/basic_task.h", "start": 151627, "end": 151961}, {"filename": "/tests/metron_good/basic_template.h", "start": 151961, "end": 152447}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 152447, "end": 152606}, {"filename": "/tests/metron_good/bit_casts.h", "start": 152606, "end": 158579}, {"filename": "/tests/metron_good/bit_concat.h", "start": 158579, "end": 159006}, {"filename": "/tests/metron_good/bit_dup.h", "start": 159006, "end": 159665}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 159665, "end": 160503}, {"filename": "/tests/metron_good/builtins.h", "start": 160503, "end": 160834}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 160834, "end": 161141}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 161141, "end": 161695}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 161695, "end": 162588}, {"filename": "/tests/metron_good/constructor_args.h", "start": 162588, "end": 163096}, {"filename": "/tests/metron_good/defines.h", "start": 163096, "end": 163410}, {"filename": "/tests/metron_good/dontcare.h", "start": 163410, "end": 163693}, {"filename": "/tests/metron_good/enum_simple.h", "start": 163693, "end": 165061}, {"filename": "/tests/metron_good/for_loops.h", "start": 165061, "end": 165381}, {"filename": "/tests/metron_good/good_order.h", "start": 165381, "end": 165677}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 165677, "end": 166088}, {"filename": "/tests/metron_good/include_guards.h", "start": 166088, "end": 166285}, {"filename": "/tests/metron_good/init_chain.h", "start": 166285, "end": 166993}, {"filename": "/tests/metron_good/input_signals.h", "start": 166993, "end": 167655}, {"filename": "/tests/metron_good/local_localparam.h", "start": 167655, "end": 167833}, {"filename": "/tests/metron_good/magic_comments.h", "start": 167833, "end": 168136}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 168136, "end": 168450}, {"filename": "/tests/metron_good/minimal.h", "start": 168450, "end": 168525}, {"filename": "/tests/metron_good/multi_tick.h", "start": 168525, "end": 168891}, {"filename": "/tests/metron_good/namespaces.h", "start": 168891, "end": 169241}, {"filename": "/tests/metron_good/nested_structs.h", "start": 169241, "end": 169754}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 169754, "end": 170299}, {"filename": "/tests/metron_good/oneliners.h", "start": 170299, "end": 170562}, {"filename": "/tests/metron_good/plus_equals.h", "start": 170562, "end": 170912}, {"filename": "/tests/metron_good/private_getter.h", "start": 170912, "end": 171136}, {"filename": "/tests/metron_good/structs.h", "start": 171136, "end": 171355}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 171355, "end": 171891}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 171891, "end": 174252}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 174252, "end": 175006}, {"filename": "/tests/metron_good/tock_task.h", "start": 175006, "end": 175362}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 175362, "end": 175527}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 175527, "end": 176186}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 176186, "end": 176845}], "remote_package_size": 176845});

  })();
