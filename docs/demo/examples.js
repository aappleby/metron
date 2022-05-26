
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
      var PACKAGE_UUID = metadata['package_uuid'];

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
Module['FS_createPath']("/examples", "rvtiny", true, true);
Module['FS_createPath']("/examples/rvtiny", "metron", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples", "rvtiny_sync", true, true);
Module['FS_createPath']("/examples/rvtiny_sync", "metron", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/examples/uart", "metron", true, true);
Module['FS_createPath']("/", "tests", true, true);
Module['FS_createPath']("/tests", "metron_good", true, true);
Module['FS_createPath']("/tests", "metron_bad", true, true);

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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1263}, {"filename": "/examples/rvtiny/README.md", "start": 1263, "end": 1427}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 1427, "end": 7523}, {"filename": "/examples/rvsimple/README.md", "start": 7523, "end": 7602}, {"filename": "/examples/rvsimple/metron/register.h", "start": 7602, "end": 8287}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 8287, "end": 9440}, {"filename": "/examples/rvsimple/metron/config.h", "start": 9440, "end": 10655}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 10655, "end": 11419}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 11419, "end": 13912}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 13912, "end": 15760}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 15760, "end": 16436}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 16436, "end": 21163}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 21163, "end": 22277}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 22277, "end": 23248}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 23248, "end": 24482}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 24482, "end": 26420}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 26420, "end": 26920}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 26920, "end": 29963}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 29963, "end": 35557}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 35557, "end": 36567}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 36567, "end": 37394}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 37394, "end": 40015}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 40015, "end": 41476}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 41476, "end": 47195}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 47195, "end": 48068}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 48068, "end": 50186}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 50186, "end": 50877}, {"filename": "/examples/pong/README.md", "start": 50877, "end": 50937}, {"filename": "/examples/pong/metron/pong.h", "start": 50937, "end": 54801}, {"filename": "/examples/pong/reference/README.md", "start": 54801, "end": 54861}, {"filename": "/examples/ibex/README.md", "start": 54861, "end": 54912}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 54912, "end": 69322}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 69322, "end": 81420}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 81420, "end": 83005}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 83005, "end": 99029}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 99029, "end": 101530}, {"filename": "/examples/rvtiny_sync/README.md", "start": 101530, "end": 101582}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 101582, "end": 108537}, {"filename": "/examples/tutorial/tutorial5.h", "start": 108537, "end": 108578}, {"filename": "/examples/tutorial/vga.h", "start": 108578, "end": 109725}, {"filename": "/examples/tutorial/declaration_order.h", "start": 109725, "end": 110504}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 110504, "end": 111040}, {"filename": "/examples/tutorial/tutorial3.h", "start": 111040, "end": 111081}, {"filename": "/examples/tutorial/checksum.h", "start": 111081, "end": 111804}, {"filename": "/examples/tutorial/submodules.h", "start": 111804, "end": 112921}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 112921, "end": 114339}, {"filename": "/examples/tutorial/adder.h", "start": 114339, "end": 114519}, {"filename": "/examples/tutorial/tutorial4.h", "start": 114519, "end": 114560}, {"filename": "/examples/tutorial/blockram.h", "start": 114560, "end": 115077}, {"filename": "/examples/tutorial/tutorial2.h", "start": 115077, "end": 115145}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 115145, "end": 116126}, {"filename": "/examples/tutorial/counter.h", "start": 116126, "end": 116275}, {"filename": "/examples/tutorial/nonblocking.h", "start": 116275, "end": 116397}, {"filename": "/examples/tutorial/templates.h", "start": 116397, "end": 116868}, {"filename": "/examples/uart/README.md", "start": 116868, "end": 117112}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 117112, "end": 118405}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 118405, "end": 119768}, {"filename": "/examples/uart/metron/uart_top.h", "start": 119768, "end": 120828}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 120828, "end": 122674}, {"filename": "/tests/metron_good/include_guards.h", "start": 122674, "end": 122871}, {"filename": "/tests/metron_good/oneliners.h", "start": 122871, "end": 123134}, {"filename": "/tests/metron_good/enum_simple.h", "start": 123134, "end": 124502}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 124502, "end": 124667}, {"filename": "/tests/metron_good/basic_increment.h", "start": 124667, "end": 125022}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 125022, "end": 125860}, {"filename": "/tests/metron_good/magic_comments.h", "start": 125860, "end": 126163}, {"filename": "/tests/metron_good/for_loops.h", "start": 126163, "end": 126483}, {"filename": "/tests/metron_good/defines.h", "start": 126483, "end": 126797}, {"filename": "/tests/metron_good/bit_concat.h", "start": 126797, "end": 127224}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 127224, "end": 127404}, {"filename": "/tests/metron_good/README.md", "start": 127404, "end": 127485}, {"filename": "/tests/metron_good/good_order.h", "start": 127485, "end": 127781}, {"filename": "/tests/metron_good/basic_template.h", "start": 127781, "end": 128212}, {"filename": "/tests/metron_good/bit_casts.h", "start": 128212, "end": 134185}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 134185, "end": 134492}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 134492, "end": 135151}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 135151, "end": 135506}, {"filename": "/tests/metron_good/tock_task.h", "start": 135506, "end": 135862}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 135862, "end": 136082}, {"filename": "/tests/metron_good/all_func_types.h", "start": 136082, "end": 137647}, {"filename": "/tests/metron_good/basic_submod.h", "start": 137647, "end": 137936}, {"filename": "/tests/metron_good/dontcare.h", "start": 137936, "end": 138219}, {"filename": "/tests/metron_good/basic_param.h", "start": 138219, "end": 138478}, {"filename": "/tests/metron_good/basic_output.h", "start": 138478, "end": 138739}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 138739, "end": 139493}, {"filename": "/tests/metron_good/basic_function.h", "start": 139493, "end": 139772}, {"filename": "/tests/metron_good/builtins.h", "start": 139772, "end": 140103}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 140103, "end": 140349}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 140349, "end": 140756}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 140756, "end": 140987}, {"filename": "/tests/metron_good/basic_literals.h", "start": 140987, "end": 141599}, {"filename": "/tests/metron_good/basic_switch.h", "start": 141599, "end": 142076}, {"filename": "/tests/metron_good/bit_dup.h", "start": 142076, "end": 142735}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 142735, "end": 143394}, {"filename": "/tests/metron_good/local_localparam.h", "start": 143394, "end": 143572}, {"filename": "/tests/metron_good/minimal.h", "start": 143572, "end": 143647}, {"filename": "/tests/metron_good/multi_tick.h", "start": 143647, "end": 144013}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 144013, "end": 144276}, {"filename": "/tests/metron_good/namespaces.h", "start": 144276, "end": 144626}, {"filename": "/tests/metron_good/structs.h", "start": 144626, "end": 144845}, {"filename": "/tests/metron_good/basic_task.h", "start": 144845, "end": 145179}, {"filename": "/tests/metron_good/nested_structs.h", "start": 145179, "end": 145692}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 145692, "end": 146006}, {"filename": "/tests/metron_good/private_getter.h", "start": 146006, "end": 146230}, {"filename": "/tests/metron_good/init_chain.h", "start": 146230, "end": 146938}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 146938, "end": 147314}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 147314, "end": 147725}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 147725, "end": 148279}, {"filename": "/tests/metron_good/input_signals.h", "start": 148279, "end": 148941}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 148941, "end": 149486}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 149486, "end": 149645}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 149645, "end": 149940}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 149940, "end": 150080}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 150080, "end": 150490}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 150490, "end": 151030}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 151030, "end": 151278}, {"filename": "/tests/metron_bad/README.md", "start": 151278, "end": 151475}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 151475, "end": 151786}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 151786, "end": 152293}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 152293, "end": 152565}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 152565, "end": 153160}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 153160, "end": 153420}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 153420, "end": 153663}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 153663, "end": 153959}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 153959, "end": 154405}], "remote_package_size": 154405, "package_uuid": "52bf9565-6cd2-4f28-9496-c663acfa6b5d"});

  })();
