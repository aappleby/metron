
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
    loadPackage({"files": [{"filename": "/examples/examples.hancho", "start": 0, "end": 535}, {"filename": "/examples/gb_spu/MetroBoySPU2.h", "start": 535, "end": 20377}, {"filename": "/examples/gb_spu/gb_spu.hancho", "start": 20377, "end": 21007}, {"filename": "/examples/ibex/README.md", "start": 21007, "end": 21058}, {"filename": "/examples/ibex/ibex.hancho", "start": 21058, "end": 21058}, {"filename": "/examples/ibex/ibex_alu.h", "start": 21058, "end": 22697}, {"filename": "/examples/ibex/ibex_compressed_decoder.h", "start": 22697, "end": 34882}, {"filename": "/examples/ibex/ibex_multdiv_slow.h", "start": 34882, "end": 49374}, {"filename": "/examples/ibex/ibex_pkg.h", "start": 49374, "end": 65451}, {"filename": "/examples/ibex/prim_arbiter_fixed.h", "start": 65451, "end": 68036}, {"filename": "/examples/j1/dpram.h", "start": 68036, "end": 68497}, {"filename": "/examples/j1/j1.h", "start": 68497, "end": 72788}, {"filename": "/examples/j1/j1.hancho", "start": 72788, "end": 73481}, {"filename": "/examples/picorv32/picorv32.h", "start": 73481, "end": 168685}, {"filename": "/examples/picorv32/picorv32.hancho", "start": 168685, "end": 168685}, {"filename": "/examples/pong/README.md", "start": 168685, "end": 168745}, {"filename": "/examples/pong/pong.h", "start": 168745, "end": 172736}, {"filename": "/examples/pong/pong.hancho", "start": 172736, "end": 173232}, {"filename": "/examples/pong/reference/README.md", "start": 173232, "end": 173292}, {"filename": "/examples/rvsimple/README.md", "start": 173292, "end": 173371}, {"filename": "/examples/rvsimple/adder.h", "start": 173371, "end": 173878}, {"filename": "/examples/rvsimple/alu.h", "start": 173878, "end": 175346}, {"filename": "/examples/rvsimple/alu_control.h", "start": 175346, "end": 177958}, {"filename": "/examples/rvsimple/config.h", "start": 177958, "end": 179180}, {"filename": "/examples/rvsimple/constants.h", "start": 179180, "end": 184906}, {"filename": "/examples/rvsimple/control_transfer.h", "start": 184906, "end": 186023}, {"filename": "/examples/rvsimple/data_memory_interface.h", "start": 186023, "end": 187963}, {"filename": "/examples/rvsimple/example_data_memory.h", "start": 187963, "end": 189203}, {"filename": "/examples/rvsimple/example_data_memory_bus.h", "start": 189203, "end": 190454}, {"filename": "/examples/rvsimple/example_text_memory.h", "start": 190454, "end": 191133}, {"filename": "/examples/rvsimple/example_text_memory_bus.h", "start": 191133, "end": 192104}, {"filename": "/examples/rvsimple/immediate_generator.h", "start": 192104, "end": 194229}, {"filename": "/examples/rvsimple/instruction_decoder.h", "start": 194229, "end": 195000}, {"filename": "/examples/rvsimple/multiplexer2.h", "start": 195000, "end": 195687}, {"filename": "/examples/rvsimple/multiplexer4.h", "start": 195687, "end": 196510}, {"filename": "/examples/rvsimple/multiplexer8.h", "start": 196510, "end": 197516}, {"filename": "/examples/rvsimple/regfile.h", "start": 197516, "end": 198494}, {"filename": "/examples/rvsimple/register.h", "start": 198494, "end": 199190}, {"filename": "/examples/rvsimple/riscv_core.h", "start": 199190, "end": 202240}, {"filename": "/examples/rvsimple/rvsimple.hancho", "start": 202240, "end": 204797}, {"filename": "/examples/rvsimple/singlecycle_control.h", "start": 204797, "end": 210378}, {"filename": "/examples/rvsimple/singlecycle_ctlpath.h", "start": 210378, "end": 212878}, {"filename": "/examples/rvsimple/singlecycle_datapath.h", "start": 212878, "end": 217616}, {"filename": "/examples/rvsimple/toplevel.h", "start": 217616, "end": 219611}, {"filename": "/examples/scratch.h", "start": 219611, "end": 219979}, {"filename": "/examples/tutorial/adder.h", "start": 219979, "end": 220159}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 220159, "end": 221147}, {"filename": "/examples/tutorial/blockram.h", "start": 221147, "end": 221679}, {"filename": "/examples/tutorial/checksum.h", "start": 221679, "end": 222421}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 222421, "end": 222828}, {"filename": "/examples/tutorial/counter.h", "start": 222828, "end": 222979}, {"filename": "/examples/tutorial/declaration_order.h", "start": 222979, "end": 223805}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 223805, "end": 224764}, {"filename": "/examples/tutorial/nonblocking.h", "start": 224764, "end": 224890}, {"filename": "/examples/tutorial/submodules.h", "start": 224890, "end": 225990}, {"filename": "/examples/tutorial/templates.h", "start": 225990, "end": 226473}, {"filename": "/examples/tutorial/tutorial.hancho", "start": 226473, "end": 226473}, {"filename": "/examples/tutorial/tutorial2.h", "start": 226473, "end": 226541}, {"filename": "/examples/tutorial/tutorial3.h", "start": 226541, "end": 226582}, {"filename": "/examples/tutorial/tutorial4.h", "start": 226582, "end": 226623}, {"filename": "/examples/tutorial/tutorial5.h", "start": 226623, "end": 226664}, {"filename": "/examples/tutorial/vga.h", "start": 226664, "end": 227844}, {"filename": "/examples/uart/README.md", "start": 227844, "end": 228088}, {"filename": "/examples/uart/uart.hancho", "start": 228088, "end": 230368}, {"filename": "/examples/uart/uart_hello.h", "start": 230368, "end": 233036}, {"filename": "/examples/uart/uart_rx.h", "start": 233036, "end": 235727}, {"filename": "/examples/uart/uart_top.h", "start": 235727, "end": 237603}, {"filename": "/examples/uart/uart_tx.h", "start": 237603, "end": 240660}, {"filename": "/tests/metron/fail/README.md", "start": 240660, "end": 240857}, {"filename": "/tests/metron/fail/basic_reg_rwr.h", "start": 240857, "end": 241164}, {"filename": "/tests/metron/fail/basic_sig_wrw.h", "start": 241164, "end": 241419}, {"filename": "/tests/metron/fail/bowtied_signals.h", "start": 241419, "end": 241639}, {"filename": "/tests/metron/fail/case_without_break.h", "start": 241639, "end": 242125}, {"filename": "/tests/metron/fail/func_writes_sig_and_reg.h", "start": 242125, "end": 242362}, {"filename": "/tests/metron/fail/if_with_no_compound.h", "start": 242362, "end": 242779}, {"filename": "/tests/metron/fail/mid_block_break.h", "start": 242779, "end": 243245}, {"filename": "/tests/metron/fail/mid_block_return.h", "start": 243245, "end": 243563}, {"filename": "/tests/metron/fail/multiple_submod_function_bindings.h", "start": 243563, "end": 244015}, {"filename": "/tests/metron/fail/multiple_tock_returns.h", "start": 244015, "end": 244272}, {"filename": "/tests/metron/fail/tick_with_return_value.h", "start": 244272, "end": 244554}, {"filename": "/tests/metron/fail/unorderable_ticks.h", "start": 244554, "end": 244790}, {"filename": "/tests/metron/fail/unorderable_tocks.h", "start": 244790, "end": 245020}, {"filename": "/tests/metron/fail/wrong_submod_call_order.h", "start": 245020, "end": 245389}, {"filename": "/tests/metron/pass/README.md", "start": 245389, "end": 245470}, {"filename": "/tests/metron/pass/all_func_types.h", "start": 245470, "end": 246971}, {"filename": "/tests/metron/pass/basic_constructor.h", "start": 246971, "end": 247388}, {"filename": "/tests/metron/pass/basic_function.h", "start": 247388, "end": 247640}, {"filename": "/tests/metron/pass/basic_increment.h", "start": 247640, "end": 247967}, {"filename": "/tests/metron/pass/basic_inputs.h", "start": 247967, "end": 248272}, {"filename": "/tests/metron/pass/basic_literals.h", "start": 248272, "end": 248891}, {"filename": "/tests/metron/pass/basic_localparam.h", "start": 248891, "end": 249147}, {"filename": "/tests/metron/pass/basic_output.h", "start": 249147, "end": 249419}, {"filename": "/tests/metron/pass/basic_param.h", "start": 249419, "end": 249688}, {"filename": "/tests/metron/pass/basic_public_reg.h", "start": 249688, "end": 249889}, {"filename": "/tests/metron/pass/basic_public_sig.h", "start": 249889, "end": 250076}, {"filename": "/tests/metron/pass/basic_reg_rww.h", "start": 250076, "end": 250307}, {"filename": "/tests/metron/pass/basic_sig_wwr.h", "start": 250307, "end": 250534}, {"filename": "/tests/metron/pass/basic_submod.h", "start": 250534, "end": 250850}, {"filename": "/tests/metron/pass/basic_submod_param.h", "start": 250850, "end": 251215}, {"filename": "/tests/metron/pass/basic_submod_public_reg.h", "start": 251215, "end": 251601}, {"filename": "/tests/metron/pass/basic_switch.h", "start": 251601, "end": 252090}, {"filename": "/tests/metron/pass/basic_task.h", "start": 252090, "end": 252425}, {"filename": "/tests/metron/pass/basic_template.h", "start": 252425, "end": 252927}, {"filename": "/tests/metron/pass/bit_casts.h", "start": 252927, "end": 258907}, {"filename": "/tests/metron/pass/bit_concat.h", "start": 258907, "end": 259341}, {"filename": "/tests/metron/pass/bit_dup.h", "start": 259341, "end": 260007}, {"filename": "/tests/metron/pass/bitfields.h", "start": 260007, "end": 261213}, {"filename": "/tests/metron/pass/both_tock_and_tick_use_tasks_and_funcs.h", "start": 261213, "end": 262030}, {"filename": "/tests/metron/pass/builtins.h", "start": 262030, "end": 262368}, {"filename": "/tests/metron/pass/call_tick_from_tock.h", "start": 262368, "end": 262685}, {"filename": "/tests/metron/pass/case_with_fallthrough.h", "start": 262685, "end": 263270}, {"filename": "/tests/metron/pass/constructor_arg_passing.h", "start": 263270, "end": 264125}, {"filename": "/tests/metron/pass/constructor_args.h", "start": 264125, "end": 264603}, {"filename": "/tests/metron/pass/counter.h", "start": 264603, "end": 265250}, {"filename": "/tests/metron/pass/defines.h", "start": 265250, "end": 265571}, {"filename": "/tests/metron/pass/dontcare.h", "start": 265571, "end": 265858}, {"filename": "/tests/metron/pass/enum_simple.h", "start": 265858, "end": 267253}, {"filename": "/tests/metron/pass/for_loops.h", "start": 267253, "end": 267580}, {"filename": "/tests/metron/pass/good_order.h", "start": 267580, "end": 267783}, {"filename": "/tests/metron/pass/if_with_compound.h", "start": 267783, "end": 268204}, {"filename": "/tests/metron/pass/include_guards.h", "start": 268204, "end": 268401}, {"filename": "/tests/metron/pass/include_test_module.h", "start": 268401, "end": 268592}, {"filename": "/tests/metron/pass/include_test_submod.h", "start": 268592, "end": 268769}, {"filename": "/tests/metron/pass/init_chain.h", "start": 268769, "end": 269490}, {"filename": "/tests/metron/pass/initialize_struct_to_zero.h", "start": 269490, "end": 269782}, {"filename": "/tests/metron/pass/input_signals.h", "start": 269782, "end": 270455}, {"filename": "/tests/metron/pass/local_localparam.h", "start": 270455, "end": 270640}, {"filename": "/tests/metron/pass/magic_comments.h", "start": 270640, "end": 270953}, {"filename": "/tests/metron/pass/matching_port_and_arg_names.h", "start": 270953, "end": 271274}, {"filename": "/tests/metron/pass/minimal.h", "start": 271274, "end": 271454}, {"filename": "/tests/metron/pass/multi_tick.h", "start": 271454, "end": 271830}, {"filename": "/tests/metron/pass/namespaces.h", "start": 271830, "end": 272211}, {"filename": "/tests/metron/pass/nested_structs.h", "start": 272211, "end": 272672}, {"filename": "/tests/metron/pass/nested_submod_calls.h", "start": 272672, "end": 273245}, {"filename": "/tests/metron/pass/noconvert.h", "start": 273245, "end": 273575}, {"filename": "/tests/metron/pass/nonblocking_assign_to_struct_union.h", "start": 273575, "end": 273928}, {"filename": "/tests/metron/pass/oneliners.h", "start": 273928, "end": 274202}, {"filename": "/tests/metron/pass/plus_equals.h", "start": 274202, "end": 274689}, {"filename": "/tests/metron/pass/preproc.h", "start": 274689, "end": 275017}, {"filename": "/tests/metron/pass/private_getter.h", "start": 275017, "end": 275269}, {"filename": "/tests/metron/pass/self_reference.h", "start": 275269, "end": 275474}, {"filename": "/tests/metron/pass/slice.h", "start": 275474, "end": 275981}, {"filename": "/tests/metron/pass/structs.h", "start": 275981, "end": 276435}, {"filename": "/tests/metron/pass/structs_as_args.h", "start": 276435, "end": 276980}, {"filename": "/tests/metron/pass/structs_as_ports.h", "start": 276980, "end": 279600}, {"filename": "/tests/metron/pass/submod_bindings.h", "start": 279600, "end": 280394}, {"filename": "/tests/metron/pass/tock_task.h", "start": 280394, "end": 280846}, {"filename": "/tests/metron/pass/trivial_adder.h", "start": 280846, "end": 281018}, {"filename": "/tests/metron/pass/unions.h", "start": 281018, "end": 281224}, {"filename": "/tests/metron/pass/utf8-mod.bom.h", "start": 281224, "end": 281893}, {"filename": "/tests/metron/pass/utf8-mod.h", "start": 281893, "end": 282562}], "remote_package_size": 282562});

  })();
