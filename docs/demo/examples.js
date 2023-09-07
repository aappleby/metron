
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22817}, {"filename": "/examples/ibex/README.md", "start": 22817, "end": 22868}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22868, "end": 24466}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24466, "end": 36565}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36565, "end": 50989}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 50989, "end": 67025}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67025, "end": 69539}, {"filename": "/examples/j1/metron/dpram.h", "start": 69539, "end": 69990}, {"filename": "/examples/j1/metron/j1.h", "start": 69990, "end": 74023}, {"filename": "/examples/pong/README.md", "start": 74023, "end": 74083}, {"filename": "/examples/pong/metron/pong.h", "start": 74083, "end": 77960}, {"filename": "/examples/pong/reference/README.md", "start": 77960, "end": 78020}, {"filename": "/examples/rvsimple/README.md", "start": 78020, "end": 78099}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78099, "end": 78612}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 78612, "end": 80086}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80086, "end": 82704}, {"filename": "/examples/rvsimple/metron/config.h", "start": 82704, "end": 83932}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 83932, "end": 89664}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 89664, "end": 90787}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 90787, "end": 92733}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 92733, "end": 93973}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 93973, "end": 95230}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95230, "end": 95912}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 95912, "end": 96889}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 96889, "end": 99020}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99020, "end": 99797}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 99797, "end": 100490}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100490, "end": 101319}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101319, "end": 102331}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102331, "end": 103315}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103315, "end": 104013}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104013, "end": 107069}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107069, "end": 112656}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 112656, "end": 115162}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115162, "end": 119902}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 119902, "end": 121903}, {"filename": "/examples/scratch.h", "start": 121903, "end": 122168}, {"filename": "/examples/tutorial/adder.h", "start": 122168, "end": 122348}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122348, "end": 123342}, {"filename": "/examples/tutorial/blockram.h", "start": 123342, "end": 123872}, {"filename": "/examples/tutorial/checksum.h", "start": 123872, "end": 124608}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124608, "end": 125144}, {"filename": "/examples/tutorial/counter.h", "start": 125144, "end": 125293}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125293, "end": 126072}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126072, "end": 127490}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127490, "end": 127612}, {"filename": "/examples/tutorial/submodules.h", "start": 127612, "end": 128729}, {"filename": "/examples/tutorial/templates.h", "start": 128729, "end": 129218}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129218, "end": 129286}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129286, "end": 129327}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129327, "end": 129368}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129368, "end": 129409}, {"filename": "/examples/tutorial/vga.h", "start": 129409, "end": 130569}, {"filename": "/examples/uart/README.md", "start": 130569, "end": 130813}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 130813, "end": 133406}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 133406, "end": 135980}, {"filename": "/examples/uart/metron/uart_top.h", "start": 135980, "end": 137757}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 137757, "end": 140789}, {"filename": "/tests/metron_bad/README.md", "start": 140789, "end": 140986}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 140986, "end": 141295}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 141295, "end": 141556}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 141556, "end": 141812}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 141812, "end": 142420}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 142420, "end": 142660}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 142660, "end": 143083}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 143083, "end": 143636}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 143636, "end": 143960}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 143960, "end": 144419}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 144419, "end": 144692}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 144692, "end": 144977}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 144977, "end": 145497}, {"filename": "/tests/metron_good/README.md", "start": 145497, "end": 145578}, {"filename": "/tests/metron_good/all_func_types.h", "start": 145578, "end": 147250}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 147250, "end": 147670}, {"filename": "/tests/metron_good/basic_function.h", "start": 147670, "end": 147962}, {"filename": "/tests/metron_good/basic_increment.h", "start": 147962, "end": 148330}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 148330, "end": 148638}, {"filename": "/tests/metron_good/basic_literals.h", "start": 148638, "end": 149263}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 149263, "end": 149522}, {"filename": "/tests/metron_good/basic_output.h", "start": 149522, "end": 149796}, {"filename": "/tests/metron_good/basic_param.h", "start": 149796, "end": 150068}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 150068, "end": 150312}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 150312, "end": 150505}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 150505, "end": 150781}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 150781, "end": 151014}, {"filename": "/tests/metron_good/basic_submod.h", "start": 151014, "end": 151316}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 151316, "end": 151684}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 151684, "end": 152073}, {"filename": "/tests/metron_good/basic_switch.h", "start": 152073, "end": 152563}, {"filename": "/tests/metron_good/basic_task.h", "start": 152563, "end": 152910}, {"filename": "/tests/metron_good/basic_template.h", "start": 152910, "end": 153409}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 153409, "end": 153581}, {"filename": "/tests/metron_good/bit_casts.h", "start": 153581, "end": 159567}, {"filename": "/tests/metron_good/bit_concat.h", "start": 159567, "end": 160007}, {"filename": "/tests/metron_good/bit_dup.h", "start": 160007, "end": 160679}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 160679, "end": 161530}, {"filename": "/tests/metron_good/builtins.h", "start": 161530, "end": 161874}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 161874, "end": 162194}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 162194, "end": 162761}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 162761, "end": 163601}, {"filename": "/tests/metron_good/constructor_args.h", "start": 163601, "end": 164073}, {"filename": "/tests/metron_good/defines.h", "start": 164073, "end": 164400}, {"filename": "/tests/metron_good/dontcare.h", "start": 164400, "end": 164693}, {"filename": "/tests/metron_good/enum_simple.h", "start": 164693, "end": 166135}, {"filename": "/tests/metron_good/for_loops.h", "start": 166135, "end": 166468}, {"filename": "/tests/metron_good/good_order.h", "start": 166468, "end": 166777}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 166777, "end": 167201}, {"filename": "/tests/metron_good/include_guards.h", "start": 167201, "end": 167398}, {"filename": "/tests/metron_good/init_chain.h", "start": 167398, "end": 168119}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 168119, "end": 168417}, {"filename": "/tests/metron_good/input_signals.h", "start": 168417, "end": 169092}, {"filename": "/tests/metron_good/local_localparam.h", "start": 169092, "end": 169283}, {"filename": "/tests/metron_good/magic_comments.h", "start": 169283, "end": 169599}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 169599, "end": 169926}, {"filename": "/tests/metron_good/minimal.h", "start": 169926, "end": 170014}, {"filename": "/tests/metron_good/multi_tick.h", "start": 170014, "end": 170393}, {"filename": "/tests/metron_good/namespaces.h", "start": 170393, "end": 170756}, {"filename": "/tests/metron_good/nested_structs.h", "start": 170756, "end": 171184}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 171184, "end": 171742}, {"filename": "/tests/metron_good/oneliners.h", "start": 171742, "end": 172018}, {"filename": "/tests/metron_good/plus_equals.h", "start": 172018, "end": 172442}, {"filename": "/tests/metron_good/private_getter.h", "start": 172442, "end": 172679}, {"filename": "/tests/metron_good/structs.h", "start": 172679, "end": 172911}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 172911, "end": 173460}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 173460, "end": 176017}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 176017, "end": 176784}, {"filename": "/tests/metron_good/tock_task.h", "start": 176784, "end": 177153}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 177153, "end": 177331}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 177331, "end": 178003}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 178003, "end": 178675}], "remote_package_size": 178675});

  })();
