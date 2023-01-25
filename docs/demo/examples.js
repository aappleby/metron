
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/ibex/README.md", "start": 26876, "end": 26927}, {"filename": "/examples/ibex/main.cpp", "start": 26927, "end": 27068}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 27068, "end": 28653}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 28653, "end": 40751}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 40751, "end": 55161}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 55161, "end": 71185}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 71185, "end": 73686}, {"filename": "/examples/j1/main.cpp", "start": 73686, "end": 73805}, {"filename": "/examples/j1/metron/dpram.h", "start": 73805, "end": 74243}, {"filename": "/examples/j1/metron/j1.h", "start": 74243, "end": 78268}, {"filename": "/examples/pong/README.md", "start": 78268, "end": 78328}, {"filename": "/examples/pong/main.cpp", "start": 78328, "end": 80274}, {"filename": "/examples/pong/main_vl.cpp", "start": 80274, "end": 80425}, {"filename": "/examples/pong/metron/pong.h", "start": 80425, "end": 84289}, {"filename": "/examples/pong/reference/README.md", "start": 84289, "end": 84349}, {"filename": "/examples/rvsimple/README.md", "start": 84349, "end": 84428}, {"filename": "/examples/rvsimple/main.cpp", "start": 84428, "end": 87243}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 87243, "end": 90145}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 90145, "end": 93047}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 93047, "end": 93547}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 93547, "end": 95008}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 95008, "end": 97629}, {"filename": "/examples/rvsimple/metron/config.h", "start": 97629, "end": 98844}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 98844, "end": 104563}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 104563, "end": 105677}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 105677, "end": 107615}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 107615, "end": 108855}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 108855, "end": 110008}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 110008, "end": 110690}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 110690, "end": 111563}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 111563, "end": 113681}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 113681, "end": 114445}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 114445, "end": 115136}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 115136, "end": 115963}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 115963, "end": 116973}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 116973, "end": 117944}, {"filename": "/examples/rvsimple/metron/register.h", "start": 117944, "end": 118629}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 118629, "end": 121672}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 121672, "end": 127266}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 127266, "end": 129759}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 129759, "end": 134486}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 134486, "end": 136334}, {"filename": "/examples/scratch.h", "start": 136334, "end": 136594}, {"filename": "/examples/tutorial/adder.h", "start": 136594, "end": 136774}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 136774, "end": 137755}, {"filename": "/examples/tutorial/blockram.h", "start": 137755, "end": 138272}, {"filename": "/examples/tutorial/checksum.h", "start": 138272, "end": 138995}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 138995, "end": 139531}, {"filename": "/examples/tutorial/counter.h", "start": 139531, "end": 139680}, {"filename": "/examples/tutorial/declaration_order.h", "start": 139680, "end": 140459}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 140459, "end": 141877}, {"filename": "/examples/tutorial/nonblocking.h", "start": 141877, "end": 141999}, {"filename": "/examples/tutorial/submodules.h", "start": 141999, "end": 143116}, {"filename": "/examples/tutorial/templates.h", "start": 143116, "end": 143587}, {"filename": "/examples/tutorial/tutorial2.h", "start": 143587, "end": 143655}, {"filename": "/examples/tutorial/tutorial3.h", "start": 143655, "end": 143696}, {"filename": "/examples/tutorial/tutorial4.h", "start": 143696, "end": 143737}, {"filename": "/examples/tutorial/tutorial5.h", "start": 143737, "end": 143778}, {"filename": "/examples/tutorial/vga.h", "start": 143778, "end": 144925}, {"filename": "/examples/uart/README.md", "start": 144925, "end": 145169}, {"filename": "/examples/uart/main.cpp", "start": 145169, "end": 146501}, {"filename": "/examples/uart/main_vl.cpp", "start": 146501, "end": 149021}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 149021, "end": 151601}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 151601, "end": 154162}, {"filename": "/examples/uart/metron/uart_top.h", "start": 154162, "end": 155926}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 155926, "end": 158945}, {"filename": "/tests/metron_bad/README.md", "start": 158945, "end": 159142}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 159142, "end": 159438}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 159438, "end": 159686}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 159686, "end": 159929}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 159929, "end": 160524}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 160524, "end": 160934}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 160934, "end": 161474}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 161474, "end": 161785}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 161785, "end": 162231}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 162231, "end": 162491}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 162491, "end": 162763}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 162763, "end": 163270}, {"filename": "/tests/metron_good/README.md", "start": 163270, "end": 163351}, {"filename": "/tests/metron_good/all_func_types.h", "start": 163351, "end": 164916}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 164916, "end": 165323}, {"filename": "/tests/metron_good/basic_function.h", "start": 165323, "end": 165602}, {"filename": "/tests/metron_good/basic_increment.h", "start": 165602, "end": 165957}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 165957, "end": 166252}, {"filename": "/tests/metron_good/basic_literals.h", "start": 166252, "end": 166864}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 166864, "end": 167110}, {"filename": "/tests/metron_good/basic_output.h", "start": 167110, "end": 167371}, {"filename": "/tests/metron_good/basic_param.h", "start": 167371, "end": 167630}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 167630, "end": 167861}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 167861, "end": 168041}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 168041, "end": 168304}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 168304, "end": 168524}, {"filename": "/tests/metron_good/basic_submod.h", "start": 168524, "end": 168813}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 168813, "end": 169168}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 169168, "end": 169544}, {"filename": "/tests/metron_good/basic_switch.h", "start": 169544, "end": 170021}, {"filename": "/tests/metron_good/basic_task.h", "start": 170021, "end": 170355}, {"filename": "/tests/metron_good/basic_template.h", "start": 170355, "end": 170841}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 170841, "end": 171000}, {"filename": "/tests/metron_good/bit_casts.h", "start": 171000, "end": 176973}, {"filename": "/tests/metron_good/bit_concat.h", "start": 176973, "end": 177400}, {"filename": "/tests/metron_good/bit_dup.h", "start": 177400, "end": 178059}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 178059, "end": 178897}, {"filename": "/tests/metron_good/builtins.h", "start": 178897, "end": 179228}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 179228, "end": 179535}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 179535, "end": 180089}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 180089, "end": 180982}, {"filename": "/tests/metron_good/constructor_args.h", "start": 180982, "end": 181490}, {"filename": "/tests/metron_good/defines.h", "start": 181490, "end": 181804}, {"filename": "/tests/metron_good/dontcare.h", "start": 181804, "end": 182087}, {"filename": "/tests/metron_good/enum_simple.h", "start": 182087, "end": 183455}, {"filename": "/tests/metron_good/for_loops.h", "start": 183455, "end": 183775}, {"filename": "/tests/metron_good/good_order.h", "start": 183775, "end": 184071}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 184071, "end": 184482}, {"filename": "/tests/metron_good/include_guards.h", "start": 184482, "end": 184679}, {"filename": "/tests/metron_good/init_chain.h", "start": 184679, "end": 185387}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 185387, "end": 185692}, {"filename": "/tests/metron_good/input_signals.h", "start": 185692, "end": 186354}, {"filename": "/tests/metron_good/local_localparam.h", "start": 186354, "end": 186532}, {"filename": "/tests/metron_good/magic_comments.h", "start": 186532, "end": 186835}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 186835, "end": 187149}, {"filename": "/tests/metron_good/minimal.h", "start": 187149, "end": 187224}, {"filename": "/tests/metron_good/multi_tick.h", "start": 187224, "end": 187590}, {"filename": "/tests/metron_good/namespaces.h", "start": 187590, "end": 187940}, {"filename": "/tests/metron_good/nested_structs.h", "start": 187940, "end": 188453}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 188453, "end": 188998}, {"filename": "/tests/metron_good/oneliners.h", "start": 188998, "end": 189261}, {"filename": "/tests/metron_good/plus_equals.h", "start": 189261, "end": 189685}, {"filename": "/tests/metron_good/private_getter.h", "start": 189685, "end": 189909}, {"filename": "/tests/metron_good/structs.h", "start": 189909, "end": 190128}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 190128, "end": 190664}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 190664, "end": 193025}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 193025, "end": 193779}, {"filename": "/tests/metron_good/tock_task.h", "start": 193779, "end": 194135}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 194135, "end": 194300}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 194300, "end": 194959}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 194959, "end": 195618}], "remote_package_size": 195618});

  })();
