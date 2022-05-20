
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
      var PACKAGE_NAME = 'docs/app/examples.data';
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
          }          Module['removeRunDependency']('datafile_docs/app/examples.data');

      };
      Module['addRunDependency']('datafile_docs/app/examples.data');

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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 220}, {"filename": "/examples/rvtiny/README.md", "start": 220, "end": 384}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 384, "end": 6474}, {"filename": "/examples/rvsimple/README.md", "start": 6474, "end": 6553}, {"filename": "/examples/rvsimple/metron/register.h", "start": 6553, "end": 7238}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7238, "end": 8345}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8345, "end": 9560}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 9560, "end": 10324}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10324, "end": 12817}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 12817, "end": 14665}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 14665, "end": 15341}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15341, "end": 20068}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20068, "end": 21182}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21182, "end": 22153}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22153, "end": 23387}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23387, "end": 25325}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25325, "end": 25825}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 25825, "end": 28868}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 28868, "end": 34462}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 34462, "end": 35472}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 35472, "end": 36299}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36299, "end": 38920}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 38920, "end": 40381}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40381, "end": 46100}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46100, "end": 46973}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 46973, "end": 49091}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49091, "end": 49782}, {"filename": "/examples/pong/README.md", "start": 49782, "end": 49842}, {"filename": "/examples/pong/metron/pong.h", "start": 49842, "end": 53706}, {"filename": "/examples/pong/reference/README.md", "start": 53706, "end": 53766}, {"filename": "/examples/ibex/README.md", "start": 53766, "end": 53817}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 53817, "end": 68227}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68227, "end": 80325}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80325, "end": 81910}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 81910, "end": 97934}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 97934, "end": 100435}, {"filename": "/examples/rvtiny_sync/README.md", "start": 100435, "end": 100487}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 100487, "end": 107442}, {"filename": "/examples/uart/README.md", "start": 107442, "end": 107686}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 107686, "end": 109004}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109004, "end": 110387}, {"filename": "/examples/uart/metron/uart_top.h", "start": 110387, "end": 111413}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 111413, "end": 113332}, {"filename": "/tests/metron_good/include_guards.h", "start": 113332, "end": 113529}, {"filename": "/tests/metron_good/oneliners.h", "start": 113529, "end": 113792}, {"filename": "/tests/metron_good/enum_simple.h", "start": 113792, "end": 115160}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115160, "end": 115325}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115325, "end": 115680}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 115680, "end": 116349}, {"filename": "/tests/metron_good/magic_comments.h", "start": 116349, "end": 116652}, {"filename": "/tests/metron_good/defines.h", "start": 116652, "end": 116966}, {"filename": "/tests/metron_good/bit_concat.h", "start": 116966, "end": 117393}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 117393, "end": 117573}, {"filename": "/tests/metron_good/README.md", "start": 117573, "end": 117654}, {"filename": "/tests/metron_good/good_order.h", "start": 117654, "end": 117950}, {"filename": "/tests/metron_good/basic_template.h", "start": 117950, "end": 118381}, {"filename": "/tests/metron_good/bit_casts.h", "start": 118381, "end": 124354}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 124354, "end": 124661}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 124661, "end": 125317}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 125317, "end": 125672}, {"filename": "/tests/metron_good/tock_task.h", "start": 125672, "end": 126028}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 126028, "end": 126248}, {"filename": "/tests/metron_good/all_func_types.h", "start": 126248, "end": 127044}, {"filename": "/tests/metron_good/basic_submod.h", "start": 127044, "end": 127333}, {"filename": "/tests/metron_good/dontcare.h", "start": 127333, "end": 127581}, {"filename": "/tests/metron_good/basic_param.h", "start": 127581, "end": 127840}, {"filename": "/tests/metron_good/basic_output.h", "start": 127840, "end": 128106}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 128106, "end": 128885}, {"filename": "/tests/metron_good/basic_function.h", "start": 128885, "end": 129164}, {"filename": "/tests/metron_good/builtins.h", "start": 129164, "end": 129495}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 129495, "end": 129741}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 129741, "end": 130145}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 130145, "end": 130376}, {"filename": "/tests/metron_good/basic_literals.h", "start": 130376, "end": 130988}, {"filename": "/tests/metron_good/basic_switch.h", "start": 130988, "end": 131465}, {"filename": "/tests/metron_good/bit_dup.h", "start": 131465, "end": 132124}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 132124, "end": 132780}, {"filename": "/tests/metron_good/local_localparam.h", "start": 132780, "end": 132958}, {"filename": "/tests/metron_good/minimal.h", "start": 132958, "end": 133033}, {"filename": "/tests/metron_good/multi_tick.h", "start": 133033, "end": 133399}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 133399, "end": 133662}, {"filename": "/tests/metron_good/namespaces.h", "start": 133662, "end": 133930}, {"filename": "/tests/metron_good/structs.h", "start": 133930, "end": 134149}, {"filename": "/tests/metron_good/basic_task.h", "start": 134149, "end": 134483}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 134483, "end": 134803}, {"filename": "/tests/metron_good/private_getter.h", "start": 134803, "end": 135001}, {"filename": "/tests/metron_good/init_chain.h", "start": 135001, "end": 135709}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 135709, "end": 136090}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 136090, "end": 136501}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 136501, "end": 137055}, {"filename": "/tests/metron_good/input_signals.h", "start": 137055, "end": 137717}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 137717, "end": 138282}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 138282, "end": 138412}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 138412, "end": 138707}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 138707, "end": 138847}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 138847, "end": 139181}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 139181, "end": 139721}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 139721, "end": 139969}, {"filename": "/tests/metron_bad/README.md", "start": 139969, "end": 140166}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 140166, "end": 140477}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 140477, "end": 140984}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 140984, "end": 141256}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 141256, "end": 141851}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 141851, "end": 142111}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 142111, "end": 142354}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 142354, "end": 142650}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 142650, "end": 143096}], "remote_package_size": 143096, "package_uuid": "1f74610c-8530-4e7e-a7b4-4e2225dcf8b5"});

  })();
