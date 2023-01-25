
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/ibex/README.md", "start": 26876, "end": 26927}, {"filename": "/examples/ibex/main.cpp", "start": 26927, "end": 27068}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 27068, "end": 28653}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 28653, "end": 40751}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 40751, "end": 55161}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 55161, "end": 71185}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 71185, "end": 73686}, {"filename": "/examples/j1/main.cpp", "start": 73686, "end": 73805}, {"filename": "/examples/j1/metron/dpram.h", "start": 73805, "end": 74243}, {"filename": "/examples/j1/metron/j1.h", "start": 74243, "end": 78268}, {"filename": "/examples/pong/README.md", "start": 78268, "end": 78328}, {"filename": "/examples/pong/main.cpp", "start": 78328, "end": 80274}, {"filename": "/examples/pong/main_vl.cpp", "start": 80274, "end": 80425}, {"filename": "/examples/pong/metron/pong.h", "start": 80425, "end": 84289}, {"filename": "/examples/pong/reference/README.md", "start": 84289, "end": 84349}, {"filename": "/examples/rvsimple/README.md", "start": 84349, "end": 84428}, {"filename": "/examples/rvsimple/main.cpp", "start": 84428, "end": 87194}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 87194, "end": 90096}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 90096, "end": 93257}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 93257, "end": 93757}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 93757, "end": 95218}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 95218, "end": 97839}, {"filename": "/examples/rvsimple/metron/config.h", "start": 97839, "end": 99054}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 99054, "end": 104773}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 104773, "end": 105887}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 105887, "end": 107825}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 107825, "end": 109052}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 109052, "end": 110296}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 110296, "end": 110965}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 110965, "end": 111929}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 111929, "end": 114047}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 114047, "end": 114811}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 114811, "end": 115502}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 115502, "end": 116329}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 116329, "end": 117339}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 117339, "end": 118310}, {"filename": "/examples/rvsimple/metron/register.h", "start": 118310, "end": 118995}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 118995, "end": 122038}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 122038, "end": 127632}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 127632, "end": 130125}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 130125, "end": 134852}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 134852, "end": 136866}, {"filename": "/examples/scratch.h", "start": 136866, "end": 137126}, {"filename": "/examples/tutorial/adder.h", "start": 137126, "end": 137306}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 137306, "end": 138287}, {"filename": "/examples/tutorial/blockram.h", "start": 138287, "end": 138804}, {"filename": "/examples/tutorial/checksum.h", "start": 138804, "end": 139527}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 139527, "end": 140063}, {"filename": "/examples/tutorial/counter.h", "start": 140063, "end": 140212}, {"filename": "/examples/tutorial/declaration_order.h", "start": 140212, "end": 140991}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 140991, "end": 142409}, {"filename": "/examples/tutorial/nonblocking.h", "start": 142409, "end": 142531}, {"filename": "/examples/tutorial/submodules.h", "start": 142531, "end": 143648}, {"filename": "/examples/tutorial/templates.h", "start": 143648, "end": 144119}, {"filename": "/examples/tutorial/tutorial2.h", "start": 144119, "end": 144187}, {"filename": "/examples/tutorial/tutorial3.h", "start": 144187, "end": 144228}, {"filename": "/examples/tutorial/tutorial4.h", "start": 144228, "end": 144269}, {"filename": "/examples/tutorial/tutorial5.h", "start": 144269, "end": 144310}, {"filename": "/examples/tutorial/vga.h", "start": 144310, "end": 145457}, {"filename": "/examples/uart/README.md", "start": 145457, "end": 145701}, {"filename": "/examples/uart/main.cpp", "start": 145701, "end": 147000}, {"filename": "/examples/uart/main_vl.cpp", "start": 147000, "end": 149536}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 149536, "end": 152116}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 152116, "end": 154677}, {"filename": "/examples/uart/metron/uart_top.h", "start": 154677, "end": 156441}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 156441, "end": 159460}, {"filename": "/tests/metron_bad/README.md", "start": 159460, "end": 159657}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 159657, "end": 159953}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 159953, "end": 160201}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 160201, "end": 160444}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 160444, "end": 161039}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 161039, "end": 161449}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 161449, "end": 161989}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 161989, "end": 162300}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 162300, "end": 162746}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 162746, "end": 163006}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 163006, "end": 163278}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 163278, "end": 163785}, {"filename": "/tests/metron_good/README.md", "start": 163785, "end": 163866}, {"filename": "/tests/metron_good/all_func_types.h", "start": 163866, "end": 165431}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 165431, "end": 165838}, {"filename": "/tests/metron_good/basic_function.h", "start": 165838, "end": 166117}, {"filename": "/tests/metron_good/basic_increment.h", "start": 166117, "end": 166472}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 166472, "end": 166767}, {"filename": "/tests/metron_good/basic_literals.h", "start": 166767, "end": 167379}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 167379, "end": 167625}, {"filename": "/tests/metron_good/basic_output.h", "start": 167625, "end": 167886}, {"filename": "/tests/metron_good/basic_param.h", "start": 167886, "end": 168145}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 168145, "end": 168376}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 168376, "end": 168556}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 168556, "end": 168819}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 168819, "end": 169039}, {"filename": "/tests/metron_good/basic_submod.h", "start": 169039, "end": 169328}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 169328, "end": 169683}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 169683, "end": 170059}, {"filename": "/tests/metron_good/basic_switch.h", "start": 170059, "end": 170536}, {"filename": "/tests/metron_good/basic_task.h", "start": 170536, "end": 170870}, {"filename": "/tests/metron_good/basic_template.h", "start": 170870, "end": 171356}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 171356, "end": 171515}, {"filename": "/tests/metron_good/bit_casts.h", "start": 171515, "end": 177488}, {"filename": "/tests/metron_good/bit_concat.h", "start": 177488, "end": 177915}, {"filename": "/tests/metron_good/bit_dup.h", "start": 177915, "end": 178574}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 178574, "end": 179412}, {"filename": "/tests/metron_good/builtins.h", "start": 179412, "end": 179743}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 179743, "end": 180050}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 180050, "end": 180604}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 180604, "end": 181497}, {"filename": "/tests/metron_good/constructor_args.h", "start": 181497, "end": 182005}, {"filename": "/tests/metron_good/defines.h", "start": 182005, "end": 182319}, {"filename": "/tests/metron_good/dontcare.h", "start": 182319, "end": 182599}, {"filename": "/tests/metron_good/enum_simple.h", "start": 182599, "end": 183967}, {"filename": "/tests/metron_good/for_loops.h", "start": 183967, "end": 184287}, {"filename": "/tests/metron_good/good_order.h", "start": 184287, "end": 184583}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 184583, "end": 184994}, {"filename": "/tests/metron_good/include_guards.h", "start": 184994, "end": 185191}, {"filename": "/tests/metron_good/init_chain.h", "start": 185191, "end": 185899}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 185899, "end": 186204}, {"filename": "/tests/metron_good/input_signals.h", "start": 186204, "end": 186866}, {"filename": "/tests/metron_good/local_localparam.h", "start": 186866, "end": 187044}, {"filename": "/tests/metron_good/magic_comments.h", "start": 187044, "end": 187347}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 187347, "end": 187661}, {"filename": "/tests/metron_good/minimal.h", "start": 187661, "end": 187736}, {"filename": "/tests/metron_good/multi_tick.h", "start": 187736, "end": 188102}, {"filename": "/tests/metron_good/namespaces.h", "start": 188102, "end": 188452}, {"filename": "/tests/metron_good/nested_structs.h", "start": 188452, "end": 188965}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 188965, "end": 189510}, {"filename": "/tests/metron_good/oneliners.h", "start": 189510, "end": 189773}, {"filename": "/tests/metron_good/plus_equals.h", "start": 189773, "end": 190197}, {"filename": "/tests/metron_good/private_getter.h", "start": 190197, "end": 190421}, {"filename": "/tests/metron_good/structs.h", "start": 190421, "end": 190640}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 190640, "end": 191176}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 191176, "end": 193537}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 193537, "end": 194291}, {"filename": "/tests/metron_good/tock_task.h", "start": 194291, "end": 194647}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 194647, "end": 194812}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 194812, "end": 195471}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 195471, "end": 196130}], "remote_package_size": 196130});

  })();
