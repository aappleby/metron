
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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 383}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 383, "end": 20225}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20225, "end": 20993}, {"filename": "/examples/ibex/README.md", "start": 20993, "end": 21044}, {"filename": "/examples/ibex/ibex.hancho", "start": 21044, "end": 21044}, {"filename": "/examples/ibex/ibex_alu.h", "start": 21044, "end": 22683}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22683, "end": 34868}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34868, "end": 49360}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49360, "end": 65437}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65437, "end": 68022}, {"filename": "/examples/j1/dpram.h", "start": 68022, "end": 68483}, {"filename": "/examples/j1/j1.h", "start": 68483, "end": 72774}, {"filename": "/examples/j1/j1.hancho", "start": 72774, "end": 73467}, {"filename": "/examples/picorv32/picorv32.h", "start": 73467, "end": 168671}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168671, "end": 168671}, {"filename": "/examples/pong/README.md", "start": 168671, "end": 168731}, {"filename": "/examples/pong/pong.h", "start": 168731, "end": 172722}, {"filename": "/examples/pong/pong.hancho", "start": 172722, "end": 173218}, {"filename": "/examples/pong/reference/README.md", "start": 173218, "end": 173278}, {"filename": "/examples/rvsimple/README.md", "start": 173278, "end": 173357}, {"filename": "/examples/rvsimple/adder.h", "start": 173357, "end": 173864}, {"filename": "/examples/rvsimple/alu.h", "start": 173864, "end": 175332}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175332, "end": 177944}, {"filename": "/examples/rvsimple/config.h", "start": 177944, "end": 179166}, {"filename": "/examples/rvsimple/constants.h", "start": 179166, "end": 184892}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 184892, "end": 186009}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 186009, "end": 187949}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 187949, "end": 189189}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 189189, "end": 190440}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190440, "end": 191119}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 191119, "end": 192090}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 192090, "end": 194215}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194215, "end": 194986}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 194986, "end": 195673}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 195673, "end": 196496}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 196496, "end": 197502}, {"filename": "/examples/rvsimple/regfile.h", "start": 197502, "end": 198480}, {"filename": "/examples/rvsimple/register.h", "start": 198480, "end": 199176}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 199176, "end": 202226}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 202226, "end": 203465}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 203465, "end": 209046}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 209046, "end": 211546}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 211546, "end": 216284}, {"filename": "/examples/rvsimple/toplevel.h", "start": 216284, "end": 218279}, {"filename": "/examples/scratch.h", "start": 218279, "end": 218647}, {"filename": "/examples/tutorial/adder.h", "start": 218647, "end": 218827}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 218827, "end": 219815}, {"filename": "/examples/tutorial/blockram.h", "start": 219815, "end": 220347}, {"filename": "/examples/tutorial/checksum.h", "start": 220347, "end": 221089}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 221089, "end": 221496}, {"filename": "/examples/tutorial/counter.h", "start": 221496, "end": 221647}, {"filename": "/examples/tutorial/declaration_order.h", "start": 221647, "end": 222473}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 222473, "end": 223432}, {"filename": "/examples/tutorial/nonblocking.h", "start": 223432, "end": 223558}, {"filename": "/examples/tutorial/submodules.h", "start": 223558, "end": 224658}, {"filename": "/examples/tutorial/templates.h", "start": 224658, "end": 225141}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 225141, "end": 225141}, {"filename": "/examples/tutorial/tutorial2.h", "start": 225141, "end": 225209}, {"filename": "/examples/tutorial/tutorial3.h", "start": 225209, "end": 225250}, {"filename": "/examples/tutorial/tutorial4.h", "start": 225250, "end": 225291}, {"filename": "/examples/tutorial/tutorial5.h", "start": 225291, "end": 225332}, {"filename": "/examples/tutorial/vga.h", "start": 225332, "end": 226512}, {"filename": "/examples/uart/README.md", "start": 226512, "end": 226756}, {"filename": "/examples/uart/uart.hancho", "start": 226756, "end": 229079}, {"filename": "/examples/uart/uart_hello.h", "start": 229079, "end": 231747}, {"filename": "/examples/uart/uart_rx.h", "start": 231747, "end": 234438}, {"filename": "/examples/uart/uart_top.h", "start": 234438, "end": 236314}, {"filename": "/examples/uart/uart_tx.h", "start": 236314, "end": 239371}, {"filename": "/tests/metron/fail/README.md", "start": 239371, "end": 239568}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 239568, "end": 239875}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 239875, "end": 240130}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 240130, "end": 240350}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 240350, "end": 240836}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 240836, "end": 241073}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 241073, "end": 241490}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 241490, "end": 241956}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 241956, "end": 242274}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 242274, "end": 242726}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 242726, "end": 242983}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 242983, "end": 243265}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 243265, "end": 243501}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 243501, "end": 243731}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 243731, "end": 244100}, {"filename": "/tests/metron/pass/README.md", "start": 244100, "end": 244181}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 244181, "end": 245682}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 245682, "end": 246099}, {"filename": "/tests/metron/pass/basic_function.h", "start": 246099, "end": 246351}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 246351, "end": 246678}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 246678, "end": 246983}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 246983, "end": 247602}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 247602, "end": 247858}, {"filename": "/tests/metron/pass/basic_output.h", "start": 247858, "end": 248130}, {"filename": "/tests/metron/pass/basic_param.h", "start": 248130, "end": 248399}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 248399, "end": 248600}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 248600, "end": 248787}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 248787, "end": 249018}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 249018, "end": 249245}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 249245, "end": 249561}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 249561, "end": 249926}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 249926, "end": 250312}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 250312, "end": 250801}, {"filename": "/tests/metron/pass/basic_task.h", "start": 250801, "end": 251136}, {"filename": "/tests/metron/pass/basic_template.h", "start": 251136, "end": 251638}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 251638, "end": 257618}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 257618, "end": 258052}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 258052, "end": 258718}, {"filename": "/tests/metron/pass/bitfields.h", "start": 258718, "end": 259924}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 259924, "end": 260741}, {"filename": "/tests/metron/pass/builtins.h", "start": 260741, "end": 261079}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 261079, "end": 261396}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 261396, "end": 261981}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 261981, "end": 262844}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 262844, "end": 263316}, {"filename": "/tests/metron/pass/counter.h", "start": 263316, "end": 263963}, {"filename": "/tests/metron/pass/defines.h", "start": 263963, "end": 264284}, {"filename": "/tests/metron/pass/dontcare.h", "start": 264284, "end": 264571}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 264571, "end": 265966}, {"filename": "/tests/metron/pass/for_loops.h", "start": 265966, "end": 266293}, {"filename": "/tests/metron/pass/good_order.h", "start": 266293, "end": 266496}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 266496, "end": 266917}, {"filename": "/tests/metron/pass/include_guards.h", "start": 266917, "end": 267114}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 267114, "end": 267305}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 267305, "end": 267482}, {"filename": "/tests/metron/pass/init_chain.h", "start": 267482, "end": 268203}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 268203, "end": 268495}, {"filename": "/tests/metron/pass/input_signals.h", "start": 268495, "end": 269168}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 269168, "end": 269353}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 269353, "end": 269666}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 269666, "end": 269987}, {"filename": "/tests/metron/pass/minimal.h", "start": 269987, "end": 270167}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 270167, "end": 270543}, {"filename": "/tests/metron/pass/namespaces.h", "start": 270543, "end": 270924}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 270924, "end": 271385}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 271385, "end": 271958}, {"filename": "/tests/metron/pass/noconvert.h", "start": 271958, "end": 272288}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 272288, "end": 272641}, {"filename": "/tests/metron/pass/oneliners.h", "start": 272641, "end": 272915}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 272915, "end": 273402}, {"filename": "/tests/metron/pass/preproc.h", "start": 273402, "end": 273730}, {"filename": "/tests/metron/pass/private_getter.h", "start": 273730, "end": 273982}, {"filename": "/tests/metron/pass/self_reference.h", "start": 273982, "end": 274187}, {"filename": "/tests/metron/pass/slice.h", "start": 274187, "end": 274694}, {"filename": "/tests/metron/pass/structs.h", "start": 274694, "end": 275148}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 275148, "end": 275693}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 275693, "end": 278313}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 278313, "end": 279107}, {"filename": "/tests/metron/pass/tock_task.h", "start": 279107, "end": 279559}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 279559, "end": 279731}, {"filename": "/tests/metron/pass/unions.h", "start": 279731, "end": 279937}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 279937, "end": 280606}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 280606, "end": 281275}], "remote_package_size": 281275});

  })();
