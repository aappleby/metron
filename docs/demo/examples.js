
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
Module['FS_createPath']("/examples/gb_spu", "metron_vl", true, true);
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples/j1", "metron", true, true);
Module['FS_createPath']("/examples", "picorv32", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/examples/uart", "metron", true, true);
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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 4240}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4240, "end": 24082}, {"filename": "/examples/gb_spu/metron_vl/VMetroBoySPU2.h", "start": 24082, "end": 27508}, {"filename": "/examples/gb_spu/metron_vl/VMetroBoySPU2.mk", "start": 27508, "end": 29030}, {"filename": "/examples/gb_spu/metron_vl/VMetroBoySPU2__Syms.h", "start": 29030, "end": 30060}, {"filename": "/examples/gb_spu/metron_vl/VMetroBoySPU2___024root.h", "start": 30060, "end": 37505}, {"filename": "/examples/gb_spu/metron_vl/VMetroBoySPU2___024unit.h", "start": 37505, "end": 38222}, {"filename": "/examples/gb_spu/metron_vl/VMetroBoySPU2__pch.h", "start": 38222, "end": 38951}, {"filename": "/examples/gb_spu/metron_vl/VMetroBoySPU2__ver.d", "start": 38951, "end": 40163}, {"filename": "/examples/gb_spu/metron_vl/VMetroBoySPU2__verFiles.dat", "start": 40163, "end": 43220}, {"filename": "/examples/gb_spu/metron_vl/VMetroBoySPU2_classes.mk", "start": 43220, "end": 45000}, {"filename": "/examples/ibex/README.md", "start": 45000, "end": 45051}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 45051, "end": 46690}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 46690, "end": 58875}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 58875, "end": 73367}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 73367, "end": 89444}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 89444, "end": 92029}, {"filename": "/examples/j1/metron/dpram.h", "start": 92029, "end": 92490}, {"filename": "/examples/j1/metron/j1.h", "start": 92490, "end": 96781}, {"filename": "/examples/picorv32/picorv32.h", "start": 96781, "end": 191985}, {"filename": "/examples/pong/README.md", "start": 191985, "end": 192045}, {"filename": "/examples/pong/metron/pong.h", "start": 192045, "end": 196036}, {"filename": "/examples/pong/reference/README.md", "start": 196036, "end": 196096}, {"filename": "/examples/rvsimple/README.md", "start": 196096, "end": 196175}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 196175, "end": 196682}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 196682, "end": 198150}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 198150, "end": 200762}, {"filename": "/examples/rvsimple/metron/config.h", "start": 200762, "end": 201984}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 201984, "end": 207710}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 207710, "end": 208827}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 208827, "end": 210767}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 210767, "end": 212007}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 212007, "end": 213258}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 213258, "end": 213937}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 213937, "end": 214908}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 214908, "end": 217033}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 217033, "end": 217804}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 217804, "end": 218491}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 218491, "end": 219314}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 219314, "end": 220320}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 220320, "end": 221298}, {"filename": "/examples/rvsimple/metron/register.h", "start": 221298, "end": 221994}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 221994, "end": 225044}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 225044, "end": 230625}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 230625, "end": 233125}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 233125, "end": 237863}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 237863, "end": 239858}, {"filename": "/examples/scratch.h", "start": 239858, "end": 240226}, {"filename": "/examples/tutorial/adder.h", "start": 240226, "end": 240406}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 240406, "end": 241394}, {"filename": "/examples/tutorial/blockram.h", "start": 241394, "end": 241926}, {"filename": "/examples/tutorial/checksum.h", "start": 241926, "end": 242668}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 242668, "end": 243075}, {"filename": "/examples/tutorial/counter.h", "start": 243075, "end": 243226}, {"filename": "/examples/tutorial/declaration_order.h", "start": 243226, "end": 244052}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 244052, "end": 245011}, {"filename": "/examples/tutorial/nonblocking.h", "start": 245011, "end": 245137}, {"filename": "/examples/tutorial/submodules.h", "start": 245137, "end": 246237}, {"filename": "/examples/tutorial/templates.h", "start": 246237, "end": 246720}, {"filename": "/examples/tutorial/tutorial2.h", "start": 246720, "end": 246788}, {"filename": "/examples/tutorial/tutorial3.h", "start": 246788, "end": 246829}, {"filename": "/examples/tutorial/tutorial4.h", "start": 246829, "end": 246870}, {"filename": "/examples/tutorial/tutorial5.h", "start": 246870, "end": 246911}, {"filename": "/examples/tutorial/vga.h", "start": 246911, "end": 248091}, {"filename": "/examples/uart/README.md", "start": 248091, "end": 248335}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 248335, "end": 250973}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 250973, "end": 253664}, {"filename": "/examples/uart/metron/uart_top.h", "start": 253664, "end": 255452}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 255452, "end": 258509}, {"filename": "/tests/metron/fail/README.md", "start": 258509, "end": 258706}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 258706, "end": 259013}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 259013, "end": 259268}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 259268, "end": 259488}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 259488, "end": 259974}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 259974, "end": 260211}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 260211, "end": 260628}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 260628, "end": 261094}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 261094, "end": 261412}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 261412, "end": 261864}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 261864, "end": 262121}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 262121, "end": 262403}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 262403, "end": 262639}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 262639, "end": 262869}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 262869, "end": 263238}, {"filename": "/tests/metron/pass/README.md", "start": 263238, "end": 263319}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 263319, "end": 264820}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 264820, "end": 265237}, {"filename": "/tests/metron/pass/basic_function.h", "start": 265237, "end": 265489}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 265489, "end": 265816}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 265816, "end": 266121}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 266121, "end": 266740}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 266740, "end": 266996}, {"filename": "/tests/metron/pass/basic_output.h", "start": 266996, "end": 267268}, {"filename": "/tests/metron/pass/basic_param.h", "start": 267268, "end": 267537}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 267537, "end": 267738}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 267738, "end": 267925}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 267925, "end": 268156}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 268156, "end": 268383}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 268383, "end": 268699}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 268699, "end": 269064}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 269064, "end": 269450}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 269450, "end": 269939}, {"filename": "/tests/metron/pass/basic_task.h", "start": 269939, "end": 270274}, {"filename": "/tests/metron/pass/basic_template.h", "start": 270274, "end": 270776}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 270776, "end": 276756}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 276756, "end": 277190}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 277190, "end": 277856}, {"filename": "/tests/metron/pass/bitfields.h", "start": 277856, "end": 279062}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 279062, "end": 279879}, {"filename": "/tests/metron/pass/builtins.h", "start": 279879, "end": 280217}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 280217, "end": 280534}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 280534, "end": 281119}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 281119, "end": 281982}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 281982, "end": 282454}, {"filename": "/tests/metron/pass/counter.h", "start": 282454, "end": 283101}, {"filename": "/tests/metron/pass/defines.h", "start": 283101, "end": 283422}, {"filename": "/tests/metron/pass/dontcare.h", "start": 283422, "end": 283709}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 283709, "end": 285104}, {"filename": "/tests/metron/pass/for_loops.h", "start": 285104, "end": 285431}, {"filename": "/tests/metron/pass/good_order.h", "start": 285431, "end": 285634}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 285634, "end": 286055}, {"filename": "/tests/metron/pass/include_guards.h", "start": 286055, "end": 286252}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 286252, "end": 286443}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 286443, "end": 286620}, {"filename": "/tests/metron/pass/init_chain.h", "start": 286620, "end": 287341}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 287341, "end": 287633}, {"filename": "/tests/metron/pass/input_signals.h", "start": 287633, "end": 288306}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 288306, "end": 288491}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 288491, "end": 288804}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 288804, "end": 289125}, {"filename": "/tests/metron/pass/minimal.h", "start": 289125, "end": 289305}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 289305, "end": 289681}, {"filename": "/tests/metron/pass/namespaces.h", "start": 289681, "end": 290062}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 290062, "end": 290523}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 290523, "end": 291096}, {"filename": "/tests/metron/pass/noconvert.h", "start": 291096, "end": 291426}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 291426, "end": 291779}, {"filename": "/tests/metron/pass/oneliners.h", "start": 291779, "end": 292053}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 292053, "end": 292540}, {"filename": "/tests/metron/pass/preproc.h", "start": 292540, "end": 292868}, {"filename": "/tests/metron/pass/private_getter.h", "start": 292868, "end": 293120}, {"filename": "/tests/metron/pass/self_reference.h", "start": 293120, "end": 293325}, {"filename": "/tests/metron/pass/slice.h", "start": 293325, "end": 293832}, {"filename": "/tests/metron/pass/structs.h", "start": 293832, "end": 294286}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 294286, "end": 294831}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 294831, "end": 297451}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 297451, "end": 298245}, {"filename": "/tests/metron/pass/tock_task.h", "start": 298245, "end": 298697}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 298697, "end": 298869}, {"filename": "/tests/metron/pass/unions.h", "start": 298869, "end": 299075}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 299075, "end": 299744}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 299744, "end": 300413}], "remote_package_size": 300413});

  })();
