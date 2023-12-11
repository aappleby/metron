
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 19842}, {"filename": "/examples/ibex/README.md", "start": 19842, "end": 19893}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 19893, "end": 21532}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 21532, "end": 33717}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 33717, "end": 48209}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 48209, "end": 64286}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 64286, "end": 66871}, {"filename": "/examples/j1/metron/dpram.h", "start": 66871, "end": 67322}, {"filename": "/examples/j1/metron/j1.h", "start": 67322, "end": 71370}, {"filename": "/examples/pong/README.md", "start": 71370, "end": 71430}, {"filename": "/examples/pong/metron/pong.h", "start": 71430, "end": 75421}, {"filename": "/examples/pong/reference/README.md", "start": 75421, "end": 75481}, {"filename": "/examples/rvsimple/README.md", "start": 75481, "end": 75560}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 75560, "end": 76067}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 76067, "end": 77535}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 77535, "end": 80147}, {"filename": "/examples/rvsimple/metron/config.h", "start": 80147, "end": 81369}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 81369, "end": 87095}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 87095, "end": 88212}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 88212, "end": 90152}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 90152, "end": 91392}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 91392, "end": 92643}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 92643, "end": 93322}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 93322, "end": 94293}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 94293, "end": 96418}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 96418, "end": 97189}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 97189, "end": 97876}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 97876, "end": 98699}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 98699, "end": 99705}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 99705, "end": 100683}, {"filename": "/examples/rvsimple/metron/register.h", "start": 100683, "end": 101379}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 101379, "end": 104429}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 104429, "end": 110010}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 110010, "end": 112510}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 112510, "end": 117248}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 117248, "end": 119243}, {"filename": "/examples/scratch.h", "start": 119243, "end": 119558}, {"filename": "/examples/tutorial/adder.h", "start": 119558, "end": 119740}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 119740, "end": 120728}, {"filename": "/examples/tutorial/blockram.h", "start": 120728, "end": 121256}, {"filename": "/examples/tutorial/checksum.h", "start": 121256, "end": 121998}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 121998, "end": 122574}, {"filename": "/examples/tutorial/counter.h", "start": 122574, "end": 122725}, {"filename": "/examples/tutorial/declaration_order.h", "start": 122725, "end": 123551}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 123551, "end": 124979}, {"filename": "/examples/tutorial/nonblocking.h", "start": 124979, "end": 125105}, {"filename": "/examples/tutorial/submodules.h", "start": 125105, "end": 126224}, {"filename": "/examples/tutorial/templates.h", "start": 126224, "end": 126707}, {"filename": "/examples/tutorial/tutorial2.h", "start": 126707, "end": 126775}, {"filename": "/examples/tutorial/tutorial3.h", "start": 126775, "end": 126816}, {"filename": "/examples/tutorial/tutorial4.h", "start": 126816, "end": 126857}, {"filename": "/examples/tutorial/tutorial5.h", "start": 126857, "end": 126898}, {"filename": "/examples/tutorial/vga.h", "start": 126898, "end": 128078}, {"filename": "/examples/uart/README.md", "start": 128078, "end": 128322}, {"filename": "/examples/uart/message.txt", "start": 128322, "end": 128834}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 128834, "end": 131543}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 131543, "end": 134234}, {"filename": "/examples/uart/metron/uart_top.h", "start": 134234, "end": 136022}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 136022, "end": 139079}, {"filename": "/tests/metron_bad/README.md", "start": 139079, "end": 139276}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 139276, "end": 139583}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 139583, "end": 139838}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 139838, "end": 140088}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 140088, "end": 140694}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 140694, "end": 140942}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 140942, "end": 141359}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 141359, "end": 141910}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 141910, "end": 142228}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 142228, "end": 142681}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 142681, "end": 142938}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 142938, "end": 143220}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 143220, "end": 143738}, {"filename": "/tests/metron_bad/unorderable_ticks.h", "start": 143738, "end": 143974}, {"filename": "/tests/metron_bad/unorderable_tocks.h", "start": 143974, "end": 144204}, {"filename": "/tests/metron_bad/wrong_submod_call_order.h", "start": 144204, "end": 144576}, {"filename": "/tests/metron_good/README.md", "start": 144576, "end": 144657}, {"filename": "/tests/metron_good/all_func_types.h", "start": 144657, "end": 146341}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 146341, "end": 146758}, {"filename": "/tests/metron_good/basic_function.h", "start": 146758, "end": 147047}, {"filename": "/tests/metron_good/basic_increment.h", "start": 147047, "end": 147417}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 147417, "end": 147722}, {"filename": "/tests/metron_good/basic_literals.h", "start": 147722, "end": 148341}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 148341, "end": 148597}, {"filename": "/tests/metron_good/basic_output.h", "start": 148597, "end": 148869}, {"filename": "/tests/metron_good/basic_param.h", "start": 148869, "end": 149138}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 149138, "end": 149379}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 149379, "end": 149566}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 149566, "end": 149840}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 149840, "end": 150067}, {"filename": "/tests/metron_good/basic_submod.h", "start": 150067, "end": 150383}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 150383, "end": 150748}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 150748, "end": 151134}, {"filename": "/tests/metron_good/basic_switch.h", "start": 151134, "end": 151623}, {"filename": "/tests/metron_good/basic_task.h", "start": 151623, "end": 151971}, {"filename": "/tests/metron_good/basic_template.h", "start": 151971, "end": 152452}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 152452, "end": 152618}, {"filename": "/tests/metron_good/bit_casts.h", "start": 152618, "end": 158598}, {"filename": "/tests/metron_good/bit_concat.h", "start": 158598, "end": 159032}, {"filename": "/tests/metron_good/bit_dup.h", "start": 159032, "end": 159698}, {"filename": "/tests/metron_good/bitfields.h", "start": 159698, "end": 160904}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 160904, "end": 161814}, {"filename": "/tests/metron_good/builtins.h", "start": 161814, "end": 162152}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 162152, "end": 162469}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 162469, "end": 163034}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 163034, "end": 163897}, {"filename": "/tests/metron_good/constructor_args.h", "start": 163897, "end": 164369}, {"filename": "/tests/metron_good/counter.h", "start": 164369, "end": 165016}, {"filename": "/tests/metron_good/defines.h", "start": 165016, "end": 165337}, {"filename": "/tests/metron_good/dontcare.h", "start": 165337, "end": 165624}, {"filename": "/tests/metron_good/enum_simple.h", "start": 165624, "end": 167019}, {"filename": "/tests/metron_good/for_loops.h", "start": 167019, "end": 167346}, {"filename": "/tests/metron_good/good_order.h", "start": 167346, "end": 167549}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 167549, "end": 167970}, {"filename": "/tests/metron_good/include_guards.h", "start": 167970, "end": 168167}, {"filename": "/tests/metron_good/init_chain.h", "start": 168167, "end": 168902}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 168902, "end": 169194}, {"filename": "/tests/metron_good/input_signals.h", "start": 169194, "end": 169992}, {"filename": "/tests/metron_good/local_localparam.h", "start": 169992, "end": 170177}, {"filename": "/tests/metron_good/magic_comments.h", "start": 170177, "end": 170490}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 170490, "end": 170811}, {"filename": "/tests/metron_good/minimal.h", "start": 170811, "end": 170991}, {"filename": "/tests/metron_good/multi_tick.h", "start": 170991, "end": 171370}, {"filename": "/tests/metron_good/namespaces.h", "start": 171370, "end": 171730}, {"filename": "/tests/metron_good/nested_structs.h", "start": 171730, "end": 172191}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 172191, "end": 172743}, {"filename": "/tests/metron_good/noconvert.h", "start": 172743, "end": 173073}, {"filename": "/tests/metron_good/oneliners.h", "start": 173073, "end": 173347}, {"filename": "/tests/metron_good/plus_equals.h", "start": 173347, "end": 173834}, {"filename": "/tests/metron_good/private_getter.h", "start": 173834, "end": 174065}, {"filename": "/tests/metron_good/self_reference.h", "start": 174065, "end": 174270}, {"filename": "/tests/metron_good/slice.h", "start": 174270, "end": 174777}, {"filename": "/tests/metron_good/structs.h", "start": 174777, "end": 175242}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 175242, "end": 175787}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 175787, "end": 178407}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 178407, "end": 179168}, {"filename": "/tests/metron_good/tock_task.h", "start": 179168, "end": 179543}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 179543, "end": 179715}, {"filename": "/tests/metron_good/unions.h", "start": 179715, "end": 179921}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 179921, "end": 180590}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 180590, "end": 181259}], "remote_package_size": 181259});

  })();
