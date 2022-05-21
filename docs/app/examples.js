
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 859}, {"filename": "/examples/rvtiny/README.md", "start": 859, "end": 1023}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 1023, "end": 7113}, {"filename": "/examples/rvsimple/README.md", "start": 7113, "end": 7192}, {"filename": "/examples/rvsimple/metron/register.h", "start": 7192, "end": 7877}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7877, "end": 8984}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8984, "end": 10199}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 10199, "end": 10963}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10963, "end": 13456}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 13456, "end": 15304}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 15304, "end": 15980}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15980, "end": 20707}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20707, "end": 21821}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21821, "end": 22792}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22792, "end": 24026}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 24026, "end": 25964}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25964, "end": 26464}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 26464, "end": 29507}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 29507, "end": 35101}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 35101, "end": 36111}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 36111, "end": 36938}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36938, "end": 39559}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 39559, "end": 41020}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 41020, "end": 46739}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46739, "end": 47612}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 47612, "end": 49730}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49730, "end": 50421}, {"filename": "/examples/pong/README.md", "start": 50421, "end": 50481}, {"filename": "/examples/pong/metron/pong.h", "start": 50481, "end": 54345}, {"filename": "/examples/pong/reference/README.md", "start": 54345, "end": 54405}, {"filename": "/examples/ibex/README.md", "start": 54405, "end": 54456}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 54456, "end": 68866}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68866, "end": 80964}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80964, "end": 82549}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 82549, "end": 98573}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 98573, "end": 101074}, {"filename": "/examples/rvtiny_sync/README.md", "start": 101074, "end": 101126}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 101126, "end": 108081}, {"filename": "/examples/uart/README.md", "start": 108081, "end": 108325}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 108325, "end": 109643}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109643, "end": 111026}, {"filename": "/examples/uart/metron/uart_top.h", "start": 111026, "end": 112052}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 112052, "end": 113971}, {"filename": "/tests/metron_good/include_guards.h", "start": 113971, "end": 114168}, {"filename": "/tests/metron_good/oneliners.h", "start": 114168, "end": 114431}, {"filename": "/tests/metron_good/enum_simple.h", "start": 114431, "end": 115799}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115799, "end": 115964}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115964, "end": 116319}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 116319, "end": 117157}, {"filename": "/tests/metron_good/magic_comments.h", "start": 117157, "end": 117460}, {"filename": "/tests/metron_good/defines.h", "start": 117460, "end": 117774}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117774, "end": 118201}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 118201, "end": 118381}, {"filename": "/tests/metron_good/README.md", "start": 118381, "end": 118462}, {"filename": "/tests/metron_good/good_order.h", "start": 118462, "end": 118758}, {"filename": "/tests/metron_good/basic_template.h", "start": 118758, "end": 119189}, {"filename": "/tests/metron_good/bit_casts.h", "start": 119189, "end": 125162}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 125162, "end": 125469}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 125469, "end": 126128}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 126128, "end": 126483}, {"filename": "/tests/metron_good/tock_task.h", "start": 126483, "end": 126839}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 126839, "end": 127059}, {"filename": "/tests/metron_good/all_func_types.h", "start": 127059, "end": 127855}, {"filename": "/tests/metron_good/basic_submod.h", "start": 127855, "end": 128144}, {"filename": "/tests/metron_good/dontcare.h", "start": 128144, "end": 128427}, {"filename": "/tests/metron_good/basic_param.h", "start": 128427, "end": 128686}, {"filename": "/tests/metron_good/basic_output.h", "start": 128686, "end": 128947}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 128947, "end": 129701}, {"filename": "/tests/metron_good/basic_function.h", "start": 129701, "end": 129980}, {"filename": "/tests/metron_good/builtins.h", "start": 129980, "end": 130311}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 130311, "end": 130557}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 130557, "end": 130964}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 130964, "end": 131195}, {"filename": "/tests/metron_good/basic_literals.h", "start": 131195, "end": 131807}, {"filename": "/tests/metron_good/basic_switch.h", "start": 131807, "end": 132284}, {"filename": "/tests/metron_good/bit_dup.h", "start": 132284, "end": 132943}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 132943, "end": 133602}, {"filename": "/tests/metron_good/local_localparam.h", "start": 133602, "end": 133780}, {"filename": "/tests/metron_good/minimal.h", "start": 133780, "end": 133855}, {"filename": "/tests/metron_good/multi_tick.h", "start": 133855, "end": 134221}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 134221, "end": 134484}, {"filename": "/tests/metron_good/namespaces.h", "start": 134484, "end": 134834}, {"filename": "/tests/metron_good/structs.h", "start": 134834, "end": 135053}, {"filename": "/tests/metron_good/basic_task.h", "start": 135053, "end": 135387}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 135387, "end": 135701}, {"filename": "/tests/metron_good/private_getter.h", "start": 135701, "end": 135925}, {"filename": "/tests/metron_good/init_chain.h", "start": 135925, "end": 136633}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 136633, "end": 137009}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 137009, "end": 137420}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 137420, "end": 137974}, {"filename": "/tests/metron_good/input_signals.h", "start": 137974, "end": 138636}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 138636, "end": 139181}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 139181, "end": 139340}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 139340, "end": 139635}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 139635, "end": 139775}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 139775, "end": 140186}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 140186, "end": 140726}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 140726, "end": 140974}, {"filename": "/tests/metron_bad/README.md", "start": 140974, "end": 141171}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 141171, "end": 141482}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 141482, "end": 141989}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 141989, "end": 142261}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142261, "end": 142856}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 142856, "end": 143116}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 143116, "end": 143359}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 143359, "end": 143655}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 143655, "end": 144101}], "remote_package_size": 144101, "package_uuid": "bbea8a3d-2388-4bfc-9221-714a7067e70d"});

  })();
