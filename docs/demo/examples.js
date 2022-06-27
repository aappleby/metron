
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
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples", "tutorial", true, true);
Module['FS_createPath']("/examples", "gb_spu", true, true);
Module['FS_createPath']("/examples/gb_spu", "metron", true, true);
Module['FS_createPath']("/examples", "rvtiny_sync", true, true);
Module['FS_createPath']("/examples/rvtiny_sync", "metron", true, true);
Module['FS_createPath']("/examples", "rvtiny", true, true);
Module['FS_createPath']("/examples/rvtiny", "metron", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples", "uart", true, true);
Module['FS_createPath']("/examples/uart", "metron", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples/j1", "metron", true, true);
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
    loadPackage({"files": [{"filename": "/examples/scratch.h", "start": 0, "end": 1263}, {"filename": "/examples/rvsimple/README.md", "start": 1263, "end": 1342}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 1342, "end": 3460}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 3460, "end": 9054}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 9054, "end": 14773}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 14773, "end": 15783}, {"filename": "/examples/rvsimple/metron/config.h", "start": 15783, "end": 16998}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 16998, "end": 17969}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 17969, "end": 18733}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 18733, "end": 21776}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 21776, "end": 22603}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 22603, "end": 23103}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 23103, "end": 25041}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 25041, "end": 26194}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 26194, "end": 28815}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 28815, "end": 29506}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 29506, "end": 30967}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 30967, "end": 31643}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 31643, "end": 34136}, {"filename": "/examples/rvsimple/metron/register.h", "start": 34136, "end": 34821}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 34821, "end": 36669}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 36669, "end": 37783}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 37783, "end": 39017}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 39017, "end": 39890}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 39890, "end": 44617}, {"filename": "/examples/ibex/README.md", "start": 44617, "end": 44668}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 44668, "end": 59078}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 59078, "end": 61579}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 61579, "end": 77603}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 77603, "end": 79188}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 79188, "end": 91286}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 91286, "end": 92704}, {"filename": "/examples/tutorial/counter.h", "start": 92704, "end": 92853}, {"filename": "/examples/tutorial/tutorial2.h", "start": 92853, "end": 92921}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 92921, "end": 93457}, {"filename": "/examples/tutorial/tutorial5.h", "start": 93457, "end": 93498}, {"filename": "/examples/tutorial/declaration_order.h", "start": 93498, "end": 94277}, {"filename": "/examples/tutorial/nonblocking.h", "start": 94277, "end": 94399}, {"filename": "/examples/tutorial/tutorial4.h", "start": 94399, "end": 94440}, {"filename": "/examples/tutorial/adder.h", "start": 94440, "end": 94620}, {"filename": "/examples/tutorial/vga.h", "start": 94620, "end": 95767}, {"filename": "/examples/tutorial/tutorial3.h", "start": 95767, "end": 95808}, {"filename": "/examples/tutorial/checksum.h", "start": 95808, "end": 96531}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 96531, "end": 97512}, {"filename": "/examples/tutorial/templates.h", "start": 97512, "end": 97983}, {"filename": "/examples/tutorial/submodules.h", "start": 97983, "end": 99100}, {"filename": "/examples/tutorial/blockram.h", "start": 99100, "end": 99617}, {"filename": "/examples/gb_spu/regs.h", "start": 99617, "end": 119982}, {"filename": "/examples/gb_spu/Sch_ApuControl.h", "start": 119982, "end": 160131}, {"filename": "/examples/gb_spu/gates.h", "start": 160131, "end": 163622}, {"filename": "/examples/gb_spu/metron/gb_spu.h", "start": 163622, "end": 184413}, {"filename": "/examples/rvtiny_sync/README.md", "start": 184413, "end": 184465}, {"filename": "/examples/rvtiny_sync/metron/toplevel.h", "start": 184465, "end": 191420}, {"filename": "/examples/rvtiny/README.md", "start": 191420, "end": 191584}, {"filename": "/examples/rvtiny/metron/toplevel.h", "start": 191584, "end": 197680}, {"filename": "/examples/pong/README.md", "start": 197680, "end": 197740}, {"filename": "/examples/pong/reference/README.md", "start": 197740, "end": 197800}, {"filename": "/examples/pong/metron/pong.h", "start": 197800, "end": 201664}, {"filename": "/examples/uart/README.md", "start": 201664, "end": 201908}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 201908, "end": 204927}, {"filename": "/examples/uart/metron/uart_top.h", "start": 204927, "end": 206691}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 206691, "end": 209271}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 209271, "end": 211832}, {"filename": "/examples/j1/metron/j1.h", "start": 211832, "end": 215857}, {"filename": "/examples/j1/metron/dpram.h", "start": 215857, "end": 216295}, {"filename": "/tests/metron_good/bit_casts.h", "start": 216295, "end": 222268}, {"filename": "/tests/metron_good/magic_comments.h", "start": 222268, "end": 222571}, {"filename": "/tests/metron_good/tock_task.h", "start": 222571, "end": 222927}, {"filename": "/tests/metron_good/structs.h", "start": 222927, "end": 223146}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 223146, "end": 223377}, {"filename": "/tests/metron_good/basic_increment.h", "start": 223377, "end": 223732}, {"filename": "/tests/metron_good/bit_concat.h", "start": 223732, "end": 224159}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 224159, "end": 224704}, {"filename": "/tests/metron_good/bit_dup.h", "start": 224704, "end": 225363}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 225363, "end": 226201}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 226201, "end": 226608}, {"filename": "/tests/metron_good/basic_param.h", "start": 226608, "end": 226867}, {"filename": "/tests/metron_good/init_chain.h", "start": 226867, "end": 227575}, {"filename": "/tests/metron_good/basic_function.h", "start": 227575, "end": 227854}, {"filename": "/tests/metron_good/all_func_types.h", "start": 227854, "end": 229419}, {"filename": "/tests/metron_good/plus_equals.h", "start": 229419, "end": 229769}, {"filename": "/tests/metron_good/basic_switch.h", "start": 229769, "end": 230246}, {"filename": "/tests/metron_good/dontcare.h", "start": 230246, "end": 230529}, {"filename": "/tests/metron_good/basic_literals.h", "start": 230529, "end": 231141}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 231141, "end": 231300}, {"filename": "/tests/metron_good/README.md", "start": 231300, "end": 231381}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 231381, "end": 231792}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 231792, "end": 231957}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 231957, "end": 232616}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 232616, "end": 232911}, {"filename": "/tests/metron_good/builtins.h", "start": 232911, "end": 233242}, {"filename": "/tests/metron_good/basic_template.h", "start": 233242, "end": 233673}, {"filename": "/tests/metron_good/defines.h", "start": 233673, "end": 233987}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 233987, "end": 234646}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 234646, "end": 235400}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 235400, "end": 235663}, {"filename": "/tests/metron_good/namespaces.h", "start": 235663, "end": 236013}, {"filename": "/tests/metron_good/include_guards.h", "start": 236013, "end": 236210}, {"filename": "/tests/metron_good/basic_task.h", "start": 236210, "end": 236544}, {"filename": "/tests/metron_good/enum_simple.h", "start": 236544, "end": 237912}, {"filename": "/tests/metron_good/oneliners.h", "start": 237912, "end": 238175}, {"filename": "/tests/metron_good/for_loops.h", "start": 238175, "end": 238495}, {"filename": "/tests/metron_good/private_getter.h", "start": 238495, "end": 238719}, {"filename": "/tests/metron_good/multi_tick.h", "start": 238719, "end": 239085}, {"filename": "/tests/metron_good/good_order.h", "start": 239085, "end": 239381}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 239381, "end": 239688}, {"filename": "/tests/metron_good/basic_output.h", "start": 239688, "end": 239949}, {"filename": "/tests/metron_good/basic_submod.h", "start": 239949, "end": 240238}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 240238, "end": 240792}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 240792, "end": 240972}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 240972, "end": 241348}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 241348, "end": 241662}, {"filename": "/tests/metron_good/input_signals.h", "start": 241662, "end": 242324}, {"filename": "/tests/metron_good/minimal.h", "start": 242324, "end": 242399}, {"filename": "/tests/metron_good/nested_structs.h", "start": 242399, "end": 242912}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 242912, "end": 243267}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 243267, "end": 243513}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 243513, "end": 243733}, {"filename": "/tests/metron_good/local_localparam.h", "start": 243733, "end": 243911}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 243911, "end": 244222}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 244222, "end": 244762}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 244762, "end": 245005}, {"filename": "/tests/metron_bad/README.md", "start": 245005, "end": 245202}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 245202, "end": 245797}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 245797, "end": 246243}, {"filename": "/tests/metron_bad/constructor_has_params.h", "start": 246243, "end": 246383}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 246383, "end": 246643}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 246643, "end": 246891}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 246891, "end": 247187}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 247187, "end": 247694}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 247694, "end": 248104}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 248104, "end": 248376}], "remote_package_size": 248376});

  })();
