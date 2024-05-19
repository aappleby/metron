
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
      var PACKAGE_NAME = '/home/aappleby/repos/metron/docs/demo/examples.data';
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
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples", "picorv32", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/", "tests", true, true);
Module['FS_createPath']("/tests", "metron", true, true);
Module['FS_createPath']("/tests/metron", "fail", true, true);
Module['FS_createPath']("/tests/metron", "pass", true, true);

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
          }          Module['removeRunDependency']('datafile_/home/aappleby/repos/metron/docs/demo/examples.data');

      };
      Module['addRunDependency']('datafile_/home/aappleby/repos/metron/docs/demo/examples.data');

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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 383}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 383, "end": 20225}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20225, "end": 20993}, {"filename": "/examples/ibex/README.md", "start": 20993, "end": 21044}, {"filename": "/examples/ibex/ibex.hancho", "start": 21044, "end": 21044}, {"filename": "/examples/ibex/ibex_alu.h", "start": 21044, "end": 22683}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22683, "end": 34868}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34868, "end": 49360}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49360, "end": 65437}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65437, "end": 68022}, {"filename": "/examples/j1/dpram.h", "start": 68022, "end": 68483}, {"filename": "/examples/j1/j1.h", "start": 68483, "end": 72774}, {"filename": "/examples/j1/j1.hancho", "start": 72774, "end": 73467}, {"filename": "/examples/picorv32/picorv32.h", "start": 73467, "end": 168671}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168671, "end": 168671}, {"filename": "/examples/pong/README.md", "start": 168671, "end": 168731}, {"filename": "/examples/pong/pong.h", "start": 168731, "end": 172722}, {"filename": "/examples/pong/pong.hancho", "start": 172722, "end": 173218}, {"filename": "/examples/pong/reference/README.md", "start": 173218, "end": 173278}, {"filename": "/examples/rvsimple/README.md", "start": 173278, "end": 173357}, {"filename": "/examples/rvsimple/adder.h", "start": 173357, "end": 173864}, {"filename": "/examples/rvsimple/alu.h", "start": 173864, "end": 175332}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175332, "end": 177944}, {"filename": "/examples/rvsimple/config.h", "start": 177944, "end": 179166}, {"filename": "/examples/rvsimple/constants.h", "start": 179166, "end": 184892}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 184892, "end": 186009}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 186009, "end": 187949}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 187949, "end": 189189}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 189189, "end": 190440}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190440, "end": 191119}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 191119, "end": 192090}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 192090, "end": 194215}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194215, "end": 194986}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 194986, "end": 195673}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 195673, "end": 196496}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 196496, "end": 197502}, {"filename": "/examples/rvsimple/regfile.h", "start": 197502, "end": 198480}, {"filename": "/examples/rvsimple/register.h", "start": 198480, "end": 199176}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 199176, "end": 202226}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 202226, "end": 205388}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 205388, "end": 210969}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 210969, "end": 213469}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 213469, "end": 218207}, {"filename": "/examples/rvsimple/toplevel.h", "start": 218207, "end": 220202}, {"filename": "/examples/scratch.h", "start": 220202, "end": 220570}, {"filename": "/examples/tutorial/adder.h", "start": 220570, "end": 220750}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 220750, "end": 221738}, {"filename": "/examples/tutorial/blockram.h", "start": 221738, "end": 222270}, {"filename": "/examples/tutorial/checksum.h", "start": 222270, "end": 223012}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 223012, "end": 223419}, {"filename": "/examples/tutorial/counter.h", "start": 223419, "end": 223570}, {"filename": "/examples/tutorial/declaration_order.h", "start": 223570, "end": 224396}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 224396, "end": 225355}, {"filename": "/examples/tutorial/nonblocking.h", "start": 225355, "end": 225481}, {"filename": "/examples/tutorial/submodules.h", "start": 225481, "end": 226581}, {"filename": "/examples/tutorial/templates.h", "start": 226581, "end": 227064}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 227064, "end": 227064}, {"filename": "/examples/tutorial/tutorial2.h", "start": 227064, "end": 227132}, {"filename": "/examples/tutorial/tutorial3.h", "start": 227132, "end": 227173}, {"filename": "/examples/tutorial/tutorial4.h", "start": 227173, "end": 227214}, {"filename": "/examples/tutorial/tutorial5.h", "start": 227214, "end": 227255}, {"filename": "/examples/tutorial/vga.h", "start": 227255, "end": 228435}, {"filename": "/examples/uart/README.md", "start": 228435, "end": 228679}, {"filename": "/examples/uart/uart.hancho", "start": 228679, "end": 231356}, {"filename": "/examples/uart/uart_hello.h", "start": 231356, "end": 234024}, {"filename": "/examples/uart/uart_rx.h", "start": 234024, "end": 236715}, {"filename": "/examples/uart/uart_top.h", "start": 236715, "end": 238591}, {"filename": "/examples/uart/uart_tx.h", "start": 238591, "end": 241648}, {"filename": "/tests/metron/fail/README.md", "start": 241648, "end": 241845}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 241845, "end": 242152}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 242152, "end": 242407}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 242407, "end": 242627}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 242627, "end": 243113}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 243113, "end": 243350}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 243350, "end": 243767}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 243767, "end": 244233}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 244233, "end": 244551}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 244551, "end": 245003}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 245003, "end": 245260}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 245260, "end": 245542}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 245542, "end": 245778}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 245778, "end": 246008}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 246008, "end": 246377}, {"filename": "/tests/metron/pass/README.md", "start": 246377, "end": 246458}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 246458, "end": 247959}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 247959, "end": 248376}, {"filename": "/tests/metron/pass/basic_function.h", "start": 248376, "end": 248628}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 248628, "end": 248955}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 248955, "end": 249260}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 249260, "end": 249879}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 249879, "end": 250135}, {"filename": "/tests/metron/pass/basic_output.h", "start": 250135, "end": 250407}, {"filename": "/tests/metron/pass/basic_param.h", "start": 250407, "end": 250676}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 250676, "end": 250877}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 250877, "end": 251064}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 251064, "end": 251295}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 251295, "end": 251522}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 251522, "end": 251838}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 251838, "end": 252203}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 252203, "end": 252589}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 252589, "end": 253078}, {"filename": "/tests/metron/pass/basic_task.h", "start": 253078, "end": 253413}, {"filename": "/tests/metron/pass/basic_template.h", "start": 253413, "end": 253915}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 253915, "end": 259895}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 259895, "end": 260329}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 260329, "end": 260995}, {"filename": "/tests/metron/pass/bitfields.h", "start": 260995, "end": 262201}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 262201, "end": 263018}, {"filename": "/tests/metron/pass/builtins.h", "start": 263018, "end": 263356}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 263356, "end": 263673}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 263673, "end": 264258}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 264258, "end": 265113}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 265113, "end": 265591}, {"filename": "/tests/metron/pass/counter.h", "start": 265591, "end": 266238}, {"filename": "/tests/metron/pass/defines.h", "start": 266238, "end": 266559}, {"filename": "/tests/metron/pass/dontcare.h", "start": 266559, "end": 266846}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 266846, "end": 268241}, {"filename": "/tests/metron/pass/for_loops.h", "start": 268241, "end": 268568}, {"filename": "/tests/metron/pass/good_order.h", "start": 268568, "end": 268771}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 268771, "end": 269192}, {"filename": "/tests/metron/pass/include_guards.h", "start": 269192, "end": 269389}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 269389, "end": 269580}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 269580, "end": 269757}, {"filename": "/tests/metron/pass/init_chain.h", "start": 269757, "end": 270478}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 270478, "end": 270770}, {"filename": "/tests/metron/pass/input_signals.h", "start": 270770, "end": 271443}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 271443, "end": 271628}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 271628, "end": 271941}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 271941, "end": 272262}, {"filename": "/tests/metron/pass/minimal.h", "start": 272262, "end": 272442}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 272442, "end": 272818}, {"filename": "/tests/metron/pass/namespaces.h", "start": 272818, "end": 273199}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 273199, "end": 273660}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 273660, "end": 274233}, {"filename": "/tests/metron/pass/noconvert.h", "start": 274233, "end": 274563}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 274563, "end": 274916}, {"filename": "/tests/metron/pass/oneliners.h", "start": 274916, "end": 275190}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 275190, "end": 275677}, {"filename": "/tests/metron/pass/preproc.h", "start": 275677, "end": 276005}, {"filename": "/tests/metron/pass/private_getter.h", "start": 276005, "end": 276257}, {"filename": "/tests/metron/pass/self_reference.h", "start": 276257, "end": 276462}, {"filename": "/tests/metron/pass/slice.h", "start": 276462, "end": 276969}, {"filename": "/tests/metron/pass/structs.h", "start": 276969, "end": 277423}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 277423, "end": 277968}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 277968, "end": 280588}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 280588, "end": 281382}, {"filename": "/tests/metron/pass/tock_task.h", "start": 281382, "end": 281834}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 281834, "end": 282006}, {"filename": "/tests/metron/pass/unions.h", "start": 282006, "end": 282212}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 282212, "end": 282881}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 282881, "end": 283550}], "remote_package_size": 283550});

  })();
