
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
Module['FS_createPath']("/examples", "rvtiny", true, true);
Module['FS_createPath']("/examples/rvtiny", "metron", true, true);
Module['FS_createPath']("/examples", "rvtiny_sync", true, true);
Module['FS_createPath']("/examples/rvtiny_sync", "metron", true, true);
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
        assert(arrayBuffer instanceof ArrayBuffer, 'bad input to processPackageData');
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/Sch_ApuControl.h", "start": 0, "end": 40149}, {"filename": "/examples/gb_spu/gates.h", "start": 40149, "end": 43640}, {"filename": "/examples/gb_spu/metron/gb_spu.h", "start": 43640, "end": 64431}, {"filename": "/examples/gb_spu/regs.h", "start": 64431, "end": 84796}, {"filename": "/examples/ibex/README.md", "start": 84796, "end": 84847}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 84847, "end": 86432}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 86432, "end": 98530}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 98530, "end": 112940}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 112940, "end": 128964}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 128964, "end": 131465}, {"filename": "/examples/j1/metron/dpram.h", "start": 131465, "end": 131903}, {"filename": "/examples/j1/metron/j1.h", "start": 131903, "end": 135928}, {"filename": "/examples/pong/README.md", "start": 135928, "end": 135988}, {"filename": "/examples/pong/metron/pong.h", "start": 135988, "end": 139852}, {"filename": "/examples/pong/reference/README.md", "start": 139852, "end": 139912}, {"filename": "/examples/rvsimple/README.md", "start": 139912, "end": 139991}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 139991, "end": 140491}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 140491, "end": 141952}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 141952, "end": 144573}, {"filename": "/examples/rvsimple/metron/config.h", "start": 144573, "end": 145788}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 145788, "end": 151507}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 151507, "end": 152621}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 152621, "end": 154559}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 154559, "end": 155793}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 155793, "end": 156946}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 156946, "end": 157622}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 157622, "end": 158495}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 158495, "end": 160613}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 160613, "end": 161377}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 161377, "end": 162068}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 162068, "end": 162895}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 162895, "end": 163905}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 163905, "end": 164876}, {"filename": "/examples/rvsimple/metron/register.h", "start": 164876, "end": 165561}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 165561, "end": 168604}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 168604, "end": 174198}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 174198, "end": 176691}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 176691, "end": 181418}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 181418, "end": 183266}, {"filename": "/examples/rvtiny/README.md", "start": 183266, "end": 183430}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 183430, "end": 189526}, {"filename": "/examples/rvtiny_sync/README.md", "start": 189526, "end": 189578}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 189578, "end": 196533}, {"filename": "/examples/scratch.h", "start": 196533, "end": 196740}, {"filename": "/examples/tutorial/adder.h", "start": 196740, "end": 196920}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 196920, "end": 197901}, {"filename": "/examples/tutorial/blockram.h", "start": 197901, "end": 198418}, {"filename": "/examples/tutorial/checksum.h", "start": 198418, "end": 199141}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 199141, "end": 199677}, {"filename": "/examples/tutorial/counter.h", "start": 199677, "end": 199826}, {"filename": "/examples/tutorial/declaration_order.h", "start": 199826, "end": 200605}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 200605, "end": 202023}, {"filename": "/examples/tutorial/nonblocking.h", "start": 202023, "end": 202145}, {"filename": "/examples/tutorial/submodules.h", "start": 202145, "end": 203262}, {"filename": "/examples/tutorial/templates.h", "start": 203262, "end": 203733}, {"filename": "/examples/tutorial/tutorial2.h", "start": 203733, "end": 203801}, {"filename": "/examples/tutorial/tutorial3.h", "start": 203801, "end": 203842}, {"filename": "/examples/tutorial/tutorial4.h", "start": 203842, "end": 203883}, {"filename": "/examples/tutorial/tutorial5.h", "start": 203883, "end": 203924}, {"filename": "/examples/tutorial/vga.h", "start": 203924, "end": 205071}, {"filename": "/examples/uart/README.md", "start": 205071, "end": 205315}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 205315, "end": 207895}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 207895, "end": 210456}, {"filename": "/examples/uart/metron/uart_top.h", "start": 210456, "end": 212220}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 212220, "end": 215239}, {"filename": "/tests/metron_bad/README.md", "start": 215239, "end": 215436}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 215436, "end": 215732}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 215732, "end": 215980}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 215980, "end": 216223}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 216223, "end": 216818}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 216818, "end": 216958}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 216958, "end": 217368}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 217368, "end": 217908}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 217908, "end": 218219}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 218219, "end": 218665}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 218665, "end": 218925}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 218925, "end": 219197}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 219197, "end": 219704}, {"filename": "/tests/metron_good/README.md", "start": 219704, "end": 219785}, {"filename": "/tests/metron_good/all_func_types.h", "start": 219785, "end": 221350}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 221350, "end": 221757}, {"filename": "/tests/metron_good/basic_function.h", "start": 221757, "end": 222036}, {"filename": "/tests/metron_good/basic_increment.h", "start": 222036, "end": 222391}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 222391, "end": 222686}, {"filename": "/tests/metron_good/basic_literals.h", "start": 222686, "end": 223298}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 223298, "end": 223544}, {"filename": "/tests/metron_good/basic_output.h", "start": 223544, "end": 223805}, {"filename": "/tests/metron_good/basic_param.h", "start": 223805, "end": 224064}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 224064, "end": 224295}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 224295, "end": 224475}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 224475, "end": 224738}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 224738, "end": 224958}, {"filename": "/tests/metron_good/basic_submod.h", "start": 224958, "end": 225247}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 225247, "end": 225602}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 225602, "end": 225978}, {"filename": "/tests/metron_good/basic_switch.h", "start": 225978, "end": 226455}, {"filename": "/tests/metron_good/basic_task.h", "start": 226455, "end": 226789}, {"filename": "/tests/metron_good/basic_template.h", "start": 226789, "end": 227220}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 227220, "end": 227379}, {"filename": "/tests/metron_good/bit_casts.h", "start": 227379, "end": 233352}, {"filename": "/tests/metron_good/bit_concat.h", "start": 233352, "end": 233779}, {"filename": "/tests/metron_good/bit_dup.h", "start": 233779, "end": 234438}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 234438, "end": 235276}, {"filename": "/tests/metron_good/builtins.h", "start": 235276, "end": 235607}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 235607, "end": 235914}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 235914, "end": 236468}, {"filename": "/tests/metron_good/defines.h", "start": 236468, "end": 236782}, {"filename": "/tests/metron_good/dontcare.h", "start": 236782, "end": 237065}, {"filename": "/tests/metron_good/enum_simple.h", "start": 237065, "end": 238433}, {"filename": "/tests/metron_good/for_loops.h", "start": 238433, "end": 238753}, {"filename": "/tests/metron_good/good_order.h", "start": 238753, "end": 239049}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 239049, "end": 239460}, {"filename": "/tests/metron_good/include_guards.h", "start": 239460, "end": 239657}, {"filename": "/tests/metron_good/init_chain.h", "start": 239657, "end": 240365}, {"filename": "/tests/metron_good/input_signals.h", "start": 240365, "end": 241027}, {"filename": "/tests/metron_good/local_localparam.h", "start": 241027, "end": 241205}, {"filename": "/tests/metron_good/magic_comments.h", "start": 241205, "end": 241508}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 241508, "end": 241822}, {"filename": "/tests/metron_good/minimal.h", "start": 241822, "end": 241897}, {"filename": "/tests/metron_good/multi_tick.h", "start": 241897, "end": 242263}, {"filename": "/tests/metron_good/namespaces.h", "start": 242263, "end": 242613}, {"filename": "/tests/metron_good/nested_structs.h", "start": 242613, "end": 243126}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 243126, "end": 243671}, {"filename": "/tests/metron_good/oneliners.h", "start": 243671, "end": 243934}, {"filename": "/tests/metron_good/plus_equals.h", "start": 243934, "end": 244284}, {"filename": "/tests/metron_good/private_getter.h", "start": 244284, "end": 244508}, {"filename": "/tests/metron_good/structs.h", "start": 244508, "end": 244727}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 244727, "end": 245481}, {"filename": "/tests/metron_good/tock_task.h", "start": 245481, "end": 245837}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 245837, "end": 246002}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 246002, "end": 246661}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 246661, "end": 247320}], "remote_package_size": 247320});

  })();
