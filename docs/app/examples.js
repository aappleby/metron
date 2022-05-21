
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 529}, {"filename": "/examples/rvtiny/README.md", "start": 529, "end": 693}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 693, "end": 6783}, {"filename": "/examples/rvsimple/README.md", "start": 6783, "end": 6862}, {"filename": "/examples/rvsimple/metron/register.h", "start": 6862, "end": 7547}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 7547, "end": 8654}, {"filename": "/examples/rvsimple/metron/config.h", "start": 8654, "end": 9869}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 9869, "end": 10633}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 10633, "end": 13126}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 13126, "end": 14974}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 14974, "end": 15650}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 15650, "end": 20377}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 20377, "end": 21491}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 21491, "end": 22462}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 22462, "end": 23696}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23696, "end": 25634}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 25634, "end": 26134}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 26134, "end": 29177}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 29177, "end": 34771}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 34771, "end": 35781}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 35781, "end": 36608}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 36608, "end": 39229}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 39229, "end": 40690}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 40690, "end": 46409}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 46409, "end": 47282}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 47282, "end": 49400}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 49400, "end": 50091}, {"filename": "/examples/pong/README.md", "start": 50091, "end": 50151}, {"filename": "/examples/pong/metron/pong.h", "start": 50151, "end": 54015}, {"filename": "/examples/pong/reference/README.md", "start": 54015, "end": 54075}, {"filename": "/examples/ibex/README.md", "start": 54075, "end": 54126}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 54126, "end": 68536}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 68536, "end": 80634}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 80634, "end": 82219}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 82219, "end": 98243}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 98243, "end": 100744}, {"filename": "/examples/rvtiny_sync/README.md", "start": 100744, "end": 100796}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 100796, "end": 107751}, {"filename": "/examples/uart/README.md", "start": 107751, "end": 107995}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 107995, "end": 109313}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 109313, "end": 110696}, {"filename": "/examples/uart/metron/uart_top.h", "start": 110696, "end": 111722}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 111722, "end": 113641}, {"filename": "/tests/metron_good/include_guards.h", "start": 113641, "end": 113838}, {"filename": "/tests/metron_good/oneliners.h", "start": 113838, "end": 114101}, {"filename": "/tests/metron_good/enum_simple.h", "start": 114101, "end": 115469}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 115469, "end": 115634}, {"filename": "/tests/metron_good/basic_increment.h", "start": 115634, "end": 115989}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 115989, "end": 116827}, {"filename": "/tests/metron_good/magic_comments.h", "start": 116827, "end": 117130}, {"filename": "/tests/metron_good/defines.h", "start": 117130, "end": 117444}, {"filename": "/tests/metron_good/bit_concat.h", "start": 117444, "end": 117871}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 117871, "end": 118051}, {"filename": "/tests/metron_good/README.md", "start": 118051, "end": 118132}, {"filename": "/tests/metron_good/good_order.h", "start": 118132, "end": 118428}, {"filename": "/tests/metron_good/basic_template.h", "start": 118428, "end": 118859}, {"filename": "/tests/metron_good/bit_casts.h", "start": 118859, "end": 124832}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 124832, "end": 125139}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 125139, "end": 125798}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 125798, "end": 126153}, {"filename": "/tests/metron_good/tock_task.h", "start": 126153, "end": 126509}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 126509, "end": 126729}, {"filename": "/tests/metron_good/all_func_types.h", "start": 126729, "end": 127910}, {"filename": "/tests/metron_good/basic_submod.h", "start": 127910, "end": 128199}, {"filename": "/tests/metron_good/dontcare.h", "start": 128199, "end": 128482}, {"filename": "/tests/metron_good/basic_param.h", "start": 128482, "end": 128741}, {"filename": "/tests/metron_good/basic_output.h", "start": 128741, "end": 129002}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 129002, "end": 129756}, {"filename": "/tests/metron_good/basic_function.h", "start": 129756, "end": 130035}, {"filename": "/tests/metron_good/builtins.h", "start": 130035, "end": 130366}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 130366, "end": 130612}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 130612, "end": 131019}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 131019, "end": 131250}, {"filename": "/tests/metron_good/basic_literals.h", "start": 131250, "end": 131862}, {"filename": "/tests/metron_good/basic_switch.h", "start": 131862, "end": 132339}, {"filename": "/tests/metron_good/bit_dup.h", "start": 132339, "end": 132998}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 132998, "end": 133657}, {"filename": "/tests/metron_good/local_localparam.h", "start": 133657, "end": 133835}, {"filename": "/tests/metron_good/minimal.h", "start": 133835, "end": 133910}, {"filename": "/tests/metron_good/multi_tick.h", "start": 133910, "end": 134276}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 134276, "end": 134539}, {"filename": "/tests/metron_good/namespaces.h", "start": 134539, "end": 134889}, {"filename": "/tests/metron_good/structs.h", "start": 134889, "end": 135108}, {"filename": "/tests/metron_good/basic_task.h", "start": 135108, "end": 135442}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 135442, "end": 135756}, {"filename": "/tests/metron_good/private_getter.h", "start": 135756, "end": 135980}, {"filename": "/tests/metron_good/init_chain.h", "start": 135980, "end": 136688}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 136688, "end": 137064}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 137064, "end": 137475}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 137475, "end": 138029}, {"filename": "/tests/metron_good/input_signals.h", "start": 138029, "end": 138691}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 138691, "end": 139236}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 139236, "end": 139395}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 139395, "end": 139690}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 139690, "end": 139830}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 139830, "end": 140241}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 140241, "end": 140781}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 140781, "end": 141029}, {"filename": "/tests/metron_bad/README.md", "start": 141029, "end": 141226}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 141226, "end": 141537}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 141537, "end": 142044}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 142044, "end": 142316}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142316, "end": 142911}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 142911, "end": 143171}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 143171, "end": 143414}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 143414, "end": 143710}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 143710, "end": 144156}], "remote_package_size": 144156, "package_uuid": "e40fa5c7-d3ea-4ed9-9ff5-8c8b1cd18448"});

  })();
