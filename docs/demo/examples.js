
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22810}, {"filename": "/examples/ibex/README.md", "start": 22810, "end": 22861}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22861, "end": 24446}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24446, "end": 36544}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36544, "end": 50954}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50954, "end": 66978}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 66978, "end": 69479}, {"filename": "/examples/j1/metron/dpram.h", "start": 69479, "end": 69917}, {"filename": "/examples/j1/metron/j1.h", "start": 69917, "end": 73937}, {"filename": "/examples/pong/README.md", "start": 73937, "end": 73997}, {"filename": "/examples/pong/metron/pong.h", "start": 73997, "end": 77861}, {"filename": "/examples/pong/reference/README.md", "start": 77861, "end": 77921}, {"filename": "/examples/rvsimple/README.md", "start": 77921, "end": 78000}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78000, "end": 78500}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78500, "end": 79961}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 79961, "end": 82566}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82566, "end": 83781}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83781, "end": 89500}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89500, "end": 90610}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90610, "end": 92543}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92543, "end": 93770}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93770, "end": 95014}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95014, "end": 95683}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95683, "end": 96647}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96647, "end": 98765}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 98765, "end": 99529}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99529, "end": 100209}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100209, "end": 101025}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101025, "end": 102024}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102024, "end": 102995}, {"filename": "/examples/rvsimple/metron/register.h", "start": 102995, "end": 103680}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 103680, "end": 106723}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 106723, "end": 112297}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112297, "end": 114790}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 114790, "end": 119517}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119517, "end": 121505}, {"filename": "/examples/scratch.h", "start": 121505, "end": 122009}, {"filename": "/examples/tutorial/adder.h", "start": 122009, "end": 122189}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122189, "end": 123170}, {"filename": "/examples/tutorial/blockram.h", "start": 123170, "end": 123687}, {"filename": "/examples/tutorial/checksum.h", "start": 123687, "end": 124410}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124410, "end": 124946}, {"filename": "/examples/tutorial/counter.h", "start": 124946, "end": 125095}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125095, "end": 125874}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 125874, "end": 127292}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127292, "end": 127414}, {"filename": "/examples/tutorial/submodules.h", "start": 127414, "end": 128531}, {"filename": "/examples/tutorial/templates.h", "start": 128531, "end": 129002}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129002, "end": 129070}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129070, "end": 129111}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129111, "end": 129152}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129152, "end": 129193}, {"filename": "/examples/tutorial/vga.h", "start": 129193, "end": 130340}, {"filename": "/examples/uart/README.md", "start": 130340, "end": 130584}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 130584, "end": 133164}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 133164, "end": 135725}, {"filename": "/examples/uart/metron/uart_top.h", "start": 135725, "end": 137489}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 137489, "end": 140508}, {"filename": "/tests/metron_bad/README.md", "start": 140508, "end": 140705}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 140705, "end": 141001}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141001, "end": 141249}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141249, "end": 141492}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 141492, "end": 142087}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 142087, "end": 142314}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 142314, "end": 142724}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 142724, "end": 143264}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 143264, "end": 143575}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 143575, "end": 144021}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144021, "end": 144281}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 144281, "end": 144553}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 144553, "end": 145060}, {"filename": "/tests/metron_good/README.md", "start": 145060, "end": 145141}, {"filename": "/tests/metron_good/all_func_types.h", "start": 145141, "end": 146800}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 146800, "end": 147207}, {"filename": "/tests/metron_good/basic_function.h", "start": 147207, "end": 147486}, {"filename": "/tests/metron_good/basic_increment.h", "start": 147486, "end": 147841}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 147841, "end": 148136}, {"filename": "/tests/metron_good/basic_literals.h", "start": 148136, "end": 148748}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 148748, "end": 148994}, {"filename": "/tests/metron_good/basic_output.h", "start": 148994, "end": 149255}, {"filename": "/tests/metron_good/basic_param.h", "start": 149255, "end": 149514}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 149514, "end": 149745}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 149745, "end": 149925}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 149925, "end": 150188}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 150188, "end": 150408}, {"filename": "/tests/metron_good/basic_submod.h", "start": 150408, "end": 150697}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 150697, "end": 151052}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 151052, "end": 151428}, {"filename": "/tests/metron_good/basic_switch.h", "start": 151428, "end": 151905}, {"filename": "/tests/metron_good/basic_task.h", "start": 151905, "end": 152239}, {"filename": "/tests/metron_good/basic_template.h", "start": 152239, "end": 152725}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 152725, "end": 152884}, {"filename": "/tests/metron_good/bit_casts.h", "start": 152884, "end": 158857}, {"filename": "/tests/metron_good/bit_concat.h", "start": 158857, "end": 159284}, {"filename": "/tests/metron_good/bit_dup.h", "start": 159284, "end": 159943}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 159943, "end": 160781}, {"filename": "/tests/metron_good/builtins.h", "start": 160781, "end": 161112}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 161112, "end": 161419}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 161419, "end": 161973}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 161973, "end": 162866}, {"filename": "/tests/metron_good/constructor_args.h", "start": 162866, "end": 163374}, {"filename": "/tests/metron_good/defines.h", "start": 163374, "end": 163688}, {"filename": "/tests/metron_good/dontcare.h", "start": 163688, "end": 163968}, {"filename": "/tests/metron_good/enum_simple.h", "start": 163968, "end": 165333}, {"filename": "/tests/metron_good/for_loops.h", "start": 165333, "end": 165653}, {"filename": "/tests/metron_good/good_order.h", "start": 165653, "end": 165949}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 165949, "end": 166360}, {"filename": "/tests/metron_good/include_guards.h", "start": 166360, "end": 166557}, {"filename": "/tests/metron_good/init_chain.h", "start": 166557, "end": 167265}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 167265, "end": 167552}, {"filename": "/tests/metron_good/input_signals.h", "start": 167552, "end": 168214}, {"filename": "/tests/metron_good/local_localparam.h", "start": 168214, "end": 168392}, {"filename": "/tests/metron_good/magic_comments.h", "start": 168392, "end": 168695}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 168695, "end": 169009}, {"filename": "/tests/metron_good/minimal.h", "start": 169009, "end": 169084}, {"filename": "/tests/metron_good/multi_tick.h", "start": 169084, "end": 169450}, {"filename": "/tests/metron_good/namespaces.h", "start": 169450, "end": 169800}, {"filename": "/tests/metron_good/nested_structs.h", "start": 169800, "end": 170215}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 170215, "end": 170760}, {"filename": "/tests/metron_good/oneliners.h", "start": 170760, "end": 171023}, {"filename": "/tests/metron_good/plus_equals.h", "start": 171023, "end": 171447}, {"filename": "/tests/metron_good/private_getter.h", "start": 171447, "end": 171671}, {"filename": "/tests/metron_good/structs.h", "start": 171671, "end": 171890}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 171890, "end": 172426}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 172426, "end": 174970}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 174970, "end": 175724}, {"filename": "/tests/metron_good/tock_task.h", "start": 175724, "end": 176080}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 176080, "end": 176245}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 176245, "end": 176904}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 176904, "end": 177563}], "remote_package_size": 177563});

  })();
