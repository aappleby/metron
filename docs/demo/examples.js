
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22817}, {"filename": "/examples/ibex/README.md", "start": 22817, "end": 22868}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22868, "end": 24466}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24466, "end": 36565}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36565, "end": 50989}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50989, "end": 67025}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67025, "end": 69539}, {"filename": "/examples/j1/metron/dpram.h", "start": 69539, "end": 69990}, {"filename": "/examples/j1/metron/j1.h", "start": 69990, "end": 74023}, {"filename": "/examples/pong/README.md", "start": 74023, "end": 74083}, {"filename": "/examples/pong/metron/pong.h", "start": 74083, "end": 77960}, {"filename": "/examples/pong/reference/README.md", "start": 77960, "end": 78020}, {"filename": "/examples/rvsimple/README.md", "start": 78020, "end": 78099}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78099, "end": 78612}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78612, "end": 80086}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80086, "end": 82704}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82704, "end": 83932}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83932, "end": 89664}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89664, "end": 90787}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90787, "end": 92733}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92733, "end": 93973}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93973, "end": 95230}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95230, "end": 95912}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95912, "end": 96889}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96889, "end": 99020}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99020, "end": 99797}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99797, "end": 100490}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100490, "end": 101319}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101319, "end": 102331}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102331, "end": 103315}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103315, "end": 104013}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104013, "end": 107069}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107069, "end": 112656}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112656, "end": 115162}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115162, "end": 119902}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119902, "end": 121903}, {"filename": "/examples/scratch.h", "start": 121903, "end": 123782}, {"filename": "/examples/tutorial/adder.h", "start": 123782, "end": 123962}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 123962, "end": 124956}, {"filename": "/examples/tutorial/blockram.h", "start": 124956, "end": 125486}, {"filename": "/examples/tutorial/checksum.h", "start": 125486, "end": 126222}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 126222, "end": 126758}, {"filename": "/examples/tutorial/counter.h", "start": 126758, "end": 126907}, {"filename": "/examples/tutorial/declaration_order.h", "start": 126907, "end": 127686}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 127686, "end": 129104}, {"filename": "/examples/tutorial/nonblocking.h", "start": 129104, "end": 129226}, {"filename": "/examples/tutorial/submodules.h", "start": 129226, "end": 130343}, {"filename": "/examples/tutorial/templates.h", "start": 130343, "end": 130832}, {"filename": "/examples/tutorial/tutorial2.h", "start": 130832, "end": 130900}, {"filename": "/examples/tutorial/tutorial3.h", "start": 130900, "end": 130941}, {"filename": "/examples/tutorial/tutorial4.h", "start": 130941, "end": 130982}, {"filename": "/examples/tutorial/tutorial5.h", "start": 130982, "end": 131023}, {"filename": "/examples/tutorial/vga.h", "start": 131023, "end": 132183}, {"filename": "/examples/uart/README.md", "start": 132183, "end": 132427}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 132427, "end": 135020}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 135020, "end": 137594}, {"filename": "/examples/uart/metron/uart_top.h", "start": 137594, "end": 139371}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 139371, "end": 142403}, {"filename": "/tests/metron_bad/README.md", "start": 142403, "end": 142600}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 142600, "end": 142909}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 142909, "end": 143170}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 143170, "end": 143426}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 143426, "end": 144034}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 144034, "end": 144274}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 144274, "end": 144697}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 144697, "end": 145250}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 145250, "end": 145574}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 145574, "end": 146033}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 146033, "end": 146306}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 146306, "end": 146591}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 146591, "end": 147111}, {"filename": "/tests/metron_good/README.md", "start": 147111, "end": 147192}, {"filename": "/tests/metron_good/all_func_types.h", "start": 147192, "end": 148864}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 148864, "end": 149284}, {"filename": "/tests/metron_good/basic_function.h", "start": 149284, "end": 149576}, {"filename": "/tests/metron_good/basic_increment.h", "start": 149576, "end": 149944}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 149944, "end": 150252}, {"filename": "/tests/metron_good/basic_literals.h", "start": 150252, "end": 150877}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 150877, "end": 151136}, {"filename": "/tests/metron_good/basic_output.h", "start": 151136, "end": 151410}, {"filename": "/tests/metron_good/basic_param.h", "start": 151410, "end": 151682}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 151682, "end": 151926}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 151926, "end": 152119}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 152119, "end": 152395}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 152395, "end": 152628}, {"filename": "/tests/metron_good/basic_submod.h", "start": 152628, "end": 152930}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 152930, "end": 153298}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 153298, "end": 153687}, {"filename": "/tests/metron_good/basic_switch.h", "start": 153687, "end": 154177}, {"filename": "/tests/metron_good/basic_task.h", "start": 154177, "end": 154524}, {"filename": "/tests/metron_good/basic_template.h", "start": 154524, "end": 155023}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 155023, "end": 155195}, {"filename": "/tests/metron_good/bit_casts.h", "start": 155195, "end": 161181}, {"filename": "/tests/metron_good/bit_concat.h", "start": 161181, "end": 161621}, {"filename": "/tests/metron_good/bit_dup.h", "start": 161621, "end": 162293}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 162293, "end": 163144}, {"filename": "/tests/metron_good/builtins.h", "start": 163144, "end": 163488}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 163488, "end": 163808}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 163808, "end": 164375}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 164375, "end": 165215}, {"filename": "/tests/metron_good/constructor_args.h", "start": 165215, "end": 165687}, {"filename": "/tests/metron_good/defines.h", "start": 165687, "end": 166014}, {"filename": "/tests/metron_good/dontcare.h", "start": 166014, "end": 166307}, {"filename": "/tests/metron_good/enum_simple.h", "start": 166307, "end": 167749}, {"filename": "/tests/metron_good/for_loops.h", "start": 167749, "end": 168082}, {"filename": "/tests/metron_good/good_order.h", "start": 168082, "end": 168391}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 168391, "end": 168815}, {"filename": "/tests/metron_good/include_guards.h", "start": 168815, "end": 169012}, {"filename": "/tests/metron_good/init_chain.h", "start": 169012, "end": 169733}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 169733, "end": 170033}, {"filename": "/tests/metron_good/input_signals.h", "start": 170033, "end": 170708}, {"filename": "/tests/metron_good/local_localparam.h", "start": 170708, "end": 170899}, {"filename": "/tests/metron_good/magic_comments.h", "start": 170899, "end": 171215}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 171215, "end": 171542}, {"filename": "/tests/metron_good/minimal.h", "start": 171542, "end": 171630}, {"filename": "/tests/metron_good/multi_tick.h", "start": 171630, "end": 172009}, {"filename": "/tests/metron_good/namespaces.h", "start": 172009, "end": 172372}, {"filename": "/tests/metron_good/nested_structs.h", "start": 172372, "end": 172800}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 172800, "end": 173358}, {"filename": "/tests/metron_good/oneliners.h", "start": 173358, "end": 173634}, {"filename": "/tests/metron_good/plus_equals.h", "start": 173634, "end": 174058}, {"filename": "/tests/metron_good/private_getter.h", "start": 174058, "end": 174295}, {"filename": "/tests/metron_good/structs.h", "start": 174295, "end": 174527}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 174527, "end": 175076}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 175076, "end": 177633}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 177633, "end": 178400}, {"filename": "/tests/metron_good/tock_task.h", "start": 178400, "end": 178769}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 178769, "end": 178947}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 178947, "end": 179619}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 179619, "end": 180291}], "remote_package_size": 180291});

  })();
