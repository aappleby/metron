
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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 383}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 383, "end": 20225}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20225, "end": 20993}, {"filename": "/examples/ibex/README.md", "start": 20993, "end": 21044}, {"filename": "/examples/ibex/ibex.hancho", "start": 21044, "end": 21044}, {"filename": "/examples/ibex/ibex_alu.h", "start": 21044, "end": 22683}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22683, "end": 34868}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34868, "end": 49360}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49360, "end": 65437}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65437, "end": 68022}, {"filename": "/examples/j1/dpram.h", "start": 68022, "end": 68483}, {"filename": "/examples/j1/j1.h", "start": 68483, "end": 72774}, {"filename": "/examples/j1/j1.hancho", "start": 72774, "end": 73467}, {"filename": "/examples/picorv32/picorv32.h", "start": 73467, "end": 168671}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168671, "end": 168671}, {"filename": "/examples/pong/README.md", "start": 168671, "end": 168731}, {"filename": "/examples/pong/pong.h", "start": 168731, "end": 172722}, {"filename": "/examples/pong/pong.hancho", "start": 172722, "end": 173218}, {"filename": "/examples/pong/reference/README.md", "start": 173218, "end": 173278}, {"filename": "/examples/rvsimple/README.md", "start": 173278, "end": 173357}, {"filename": "/examples/rvsimple/adder.h", "start": 173357, "end": 173864}, {"filename": "/examples/rvsimple/alu.h", "start": 173864, "end": 175332}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175332, "end": 177944}, {"filename": "/examples/rvsimple/config.h", "start": 177944, "end": 179166}, {"filename": "/examples/rvsimple/constants.h", "start": 179166, "end": 184892}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 184892, "end": 186009}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 186009, "end": 187949}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 187949, "end": 189189}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 189189, "end": 190440}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190440, "end": 191119}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 191119, "end": 192090}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 192090, "end": 194215}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194215, "end": 194986}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 194986, "end": 195673}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 195673, "end": 196496}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 196496, "end": 197502}, {"filename": "/examples/rvsimple/regfile.h", "start": 197502, "end": 198480}, {"filename": "/examples/rvsimple/register.h", "start": 198480, "end": 199176}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 199176, "end": 202226}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 202226, "end": 205585}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 205585, "end": 211166}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 211166, "end": 213666}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 213666, "end": 218404}, {"filename": "/examples/rvsimple/toplevel.h", "start": 218404, "end": 220399}, {"filename": "/examples/scratch.h", "start": 220399, "end": 220767}, {"filename": "/examples/tutorial/adder.h", "start": 220767, "end": 220947}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 220947, "end": 221935}, {"filename": "/examples/tutorial/blockram.h", "start": 221935, "end": 222467}, {"filename": "/examples/tutorial/checksum.h", "start": 222467, "end": 223209}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 223209, "end": 223616}, {"filename": "/examples/tutorial/counter.h", "start": 223616, "end": 223767}, {"filename": "/examples/tutorial/declaration_order.h", "start": 223767, "end": 224593}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 224593, "end": 225552}, {"filename": "/examples/tutorial/nonblocking.h", "start": 225552, "end": 225678}, {"filename": "/examples/tutorial/submodules.h", "start": 225678, "end": 226778}, {"filename": "/examples/tutorial/templates.h", "start": 226778, "end": 227261}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 227261, "end": 227261}, {"filename": "/examples/tutorial/tutorial2.h", "start": 227261, "end": 227329}, {"filename": "/examples/tutorial/tutorial3.h", "start": 227329, "end": 227370}, {"filename": "/examples/tutorial/tutorial4.h", "start": 227370, "end": 227411}, {"filename": "/examples/tutorial/tutorial5.h", "start": 227411, "end": 227452}, {"filename": "/examples/tutorial/vga.h", "start": 227452, "end": 228632}, {"filename": "/examples/uart/README.md", "start": 228632, "end": 228876}, {"filename": "/examples/uart/uart.hancho", "start": 228876, "end": 231553}, {"filename": "/examples/uart/uart_hello.h", "start": 231553, "end": 234221}, {"filename": "/examples/uart/uart_rx.h", "start": 234221, "end": 236912}, {"filename": "/examples/uart/uart_top.h", "start": 236912, "end": 238788}, {"filename": "/examples/uart/uart_tx.h", "start": 238788, "end": 241845}, {"filename": "/tests/metron/fail/README.md", "start": 241845, "end": 242042}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 242042, "end": 242349}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 242349, "end": 242604}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 242604, "end": 242824}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 242824, "end": 243310}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 243310, "end": 243547}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 243547, "end": 243964}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 243964, "end": 244430}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 244430, "end": 244748}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 244748, "end": 245200}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 245200, "end": 245457}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 245457, "end": 245739}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 245739, "end": 245975}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 245975, "end": 246205}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 246205, "end": 246574}, {"filename": "/tests/metron/pass/README.md", "start": 246574, "end": 246655}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 246655, "end": 248156}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 248156, "end": 248573}, {"filename": "/tests/metron/pass/basic_function.h", "start": 248573, "end": 248825}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 248825, "end": 249152}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 249152, "end": 249457}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 249457, "end": 250076}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 250076, "end": 250332}, {"filename": "/tests/metron/pass/basic_output.h", "start": 250332, "end": 250604}, {"filename": "/tests/metron/pass/basic_param.h", "start": 250604, "end": 250873}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 250873, "end": 251074}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 251074, "end": 251261}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 251261, "end": 251492}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 251492, "end": 251719}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 251719, "end": 252035}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 252035, "end": 252400}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 252400, "end": 252786}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 252786, "end": 253275}, {"filename": "/tests/metron/pass/basic_task.h", "start": 253275, "end": 253610}, {"filename": "/tests/metron/pass/basic_template.h", "start": 253610, "end": 254112}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 254112, "end": 260092}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 260092, "end": 260526}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 260526, "end": 261192}, {"filename": "/tests/metron/pass/bitfields.h", "start": 261192, "end": 262398}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 262398, "end": 263215}, {"filename": "/tests/metron/pass/builtins.h", "start": 263215, "end": 263553}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 263553, "end": 263870}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 263870, "end": 264455}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 264455, "end": 265310}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 265310, "end": 265788}, {"filename": "/tests/metron/pass/counter.h", "start": 265788, "end": 266435}, {"filename": "/tests/metron/pass/defines.h", "start": 266435, "end": 266756}, {"filename": "/tests/metron/pass/dontcare.h", "start": 266756, "end": 267043}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 267043, "end": 268438}, {"filename": "/tests/metron/pass/for_loops.h", "start": 268438, "end": 268765}, {"filename": "/tests/metron/pass/good_order.h", "start": 268765, "end": 268968}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 268968, "end": 269389}, {"filename": "/tests/metron/pass/include_guards.h", "start": 269389, "end": 269586}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 269586, "end": 269777}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 269777, "end": 269954}, {"filename": "/tests/metron/pass/init_chain.h", "start": 269954, "end": 270675}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 270675, "end": 270967}, {"filename": "/tests/metron/pass/input_signals.h", "start": 270967, "end": 271640}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 271640, "end": 271825}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 271825, "end": 272138}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 272138, "end": 272459}, {"filename": "/tests/metron/pass/minimal.h", "start": 272459, "end": 272639}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 272639, "end": 273015}, {"filename": "/tests/metron/pass/namespaces.h", "start": 273015, "end": 273396}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 273396, "end": 273857}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 273857, "end": 274430}, {"filename": "/tests/metron/pass/noconvert.h", "start": 274430, "end": 274760}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 274760, "end": 275113}, {"filename": "/tests/metron/pass/oneliners.h", "start": 275113, "end": 275387}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 275387, "end": 275874}, {"filename": "/tests/metron/pass/preproc.h", "start": 275874, "end": 276202}, {"filename": "/tests/metron/pass/private_getter.h", "start": 276202, "end": 276454}, {"filename": "/tests/metron/pass/self_reference.h", "start": 276454, "end": 276659}, {"filename": "/tests/metron/pass/slice.h", "start": 276659, "end": 277166}, {"filename": "/tests/metron/pass/structs.h", "start": 277166, "end": 277620}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 277620, "end": 278165}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 278165, "end": 280785}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 280785, "end": 281579}, {"filename": "/tests/metron/pass/tock_task.h", "start": 281579, "end": 282031}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 282031, "end": 282203}, {"filename": "/tests/metron/pass/unions.h", "start": 282203, "end": 282409}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 282409, "end": 283078}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 283078, "end": 283747}], "remote_package_size": 283747});

  })();
