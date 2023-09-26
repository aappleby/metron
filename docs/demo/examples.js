
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22885}, {"filename": "/examples/ibex/README.md", "start": 22885, "end": 22936}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22936, "end": 24581}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24581, "end": 36772}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36772, "end": 51270}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 51270, "end": 67353}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67353, "end": 69944}, {"filename": "/examples/j1/metron/dpram.h", "start": 69944, "end": 70395}, {"filename": "/examples/j1/metron/j1.h", "start": 70395, "end": 74428}, {"filename": "/examples/pong/README.md", "start": 74428, "end": 74488}, {"filename": "/examples/pong/metron/pong.h", "start": 74488, "end": 78390}, {"filename": "/examples/pong/reference/README.md", "start": 78390, "end": 78450}, {"filename": "/examples/rvsimple/README.md", "start": 78450, "end": 78529}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78529, "end": 79042}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 79042, "end": 80516}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80516, "end": 83134}, {"filename": "/examples/rvsimple/metron/config.h", "start": 83134, "end": 84362}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 84362, "end": 90094}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 90094, "end": 91217}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 91217, "end": 93163}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 93163, "end": 94403}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 94403, "end": 95660}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95660, "end": 96342}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 96342, "end": 97319}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 97319, "end": 99450}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99450, "end": 100227}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 100227, "end": 100920}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100920, "end": 101749}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101749, "end": 102761}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102761, "end": 103745}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103745, "end": 104443}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104443, "end": 107499}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107499, "end": 113086}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 113086, "end": 115592}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115592, "end": 120332}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 120332, "end": 122333}, {"filename": "/examples/scratch.h", "start": 122333, "end": 122494}, {"filename": "/examples/tutorial/adder.h", "start": 122494, "end": 122674}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122674, "end": 123668}, {"filename": "/examples/tutorial/blockram.h", "start": 123668, "end": 124198}, {"filename": "/examples/tutorial/checksum.h", "start": 124198, "end": 124934}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 124934, "end": 125470}, {"filename": "/examples/tutorial/counter.h", "start": 125470, "end": 125619}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125619, "end": 126398}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126398, "end": 127816}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127816, "end": 127938}, {"filename": "/examples/tutorial/submodules.h", "start": 127938, "end": 129055}, {"filename": "/examples/tutorial/templates.h", "start": 129055, "end": 129544}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129544, "end": 129612}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129612, "end": 129653}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129653, "end": 129694}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129694, "end": 129735}, {"filename": "/examples/tutorial/vga.h", "start": 129735, "end": 130895}, {"filename": "/examples/uart/README.md", "start": 130895, "end": 131139}, {"filename": "/examples/uart/message.txt", "start": 131139, "end": 131651}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 131651, "end": 134244}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 134244, "end": 136818}, {"filename": "/examples/uart/metron/uart_top.h", "start": 136818, "end": 138595}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 138595, "end": 141627}, {"filename": "/tests/metron_bad/README.md", "start": 141627, "end": 141824}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141824, "end": 142133}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 142133, "end": 142394}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 142394, "end": 142650}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142650, "end": 143258}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 143258, "end": 143509}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 143509, "end": 143932}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 143932, "end": 144485}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 144485, "end": 144809}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144809, "end": 145268}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 145268, "end": 145531}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 145531, "end": 145816}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 145816, "end": 146336}, {"filename": "/tests/metron_good/README.md", "start": 146336, "end": 146417}, {"filename": "/tests/metron_good/all_func_types.h", "start": 146417, "end": 148095}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 148095, "end": 148515}, {"filename": "/tests/metron_good/basic_function.h", "start": 148515, "end": 148807}, {"filename": "/tests/metron_good/basic_increment.h", "start": 148807, "end": 149175}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 149175, "end": 149483}, {"filename": "/tests/metron_good/basic_literals.h", "start": 149483, "end": 150108}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 150108, "end": 150367}, {"filename": "/tests/metron_good/basic_output.h", "start": 150367, "end": 150641}, {"filename": "/tests/metron_good/basic_param.h", "start": 150641, "end": 150913}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 150913, "end": 151157}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 151157, "end": 151350}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 151350, "end": 151626}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 151626, "end": 151859}, {"filename": "/tests/metron_good/basic_submod.h", "start": 151859, "end": 152178}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 152178, "end": 152546}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 152546, "end": 152935}, {"filename": "/tests/metron_good/basic_switch.h", "start": 152935, "end": 153425}, {"filename": "/tests/metron_good/basic_task.h", "start": 153425, "end": 153772}, {"filename": "/tests/metron_good/basic_template.h", "start": 153772, "end": 154271}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 154271, "end": 154443}, {"filename": "/tests/metron_good/bit_casts.h", "start": 154443, "end": 160429}, {"filename": "/tests/metron_good/bit_concat.h", "start": 160429, "end": 160869}, {"filename": "/tests/metron_good/bit_dup.h", "start": 160869, "end": 161541}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 161541, "end": 162449}, {"filename": "/tests/metron_good/builtins.h", "start": 162449, "end": 162793}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 162793, "end": 163113}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 163113, "end": 163680}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 163680, "end": 164534}, {"filename": "/tests/metron_good/constructor_args.h", "start": 164534, "end": 165006}, {"filename": "/tests/metron_good/defines.h", "start": 165006, "end": 165333}, {"filename": "/tests/metron_good/dontcare.h", "start": 165333, "end": 165626}, {"filename": "/tests/metron_good/enum_simple.h", "start": 165626, "end": 167068}, {"filename": "/tests/metron_good/for_loops.h", "start": 167068, "end": 167401}, {"filename": "/tests/metron_good/good_order.h", "start": 167401, "end": 167710}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 167710, "end": 168134}, {"filename": "/tests/metron_good/include_guards.h", "start": 168134, "end": 168331}, {"filename": "/tests/metron_good/init_chain.h", "start": 168331, "end": 169052}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 169052, "end": 169350}, {"filename": "/tests/metron_good/input_signals.h", "start": 169350, "end": 170148}, {"filename": "/tests/metron_good/local_localparam.h", "start": 170148, "end": 170339}, {"filename": "/tests/metron_good/magic_comments.h", "start": 170339, "end": 170655}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 170655, "end": 170982}, {"filename": "/tests/metron_good/minimal.h", "start": 170982, "end": 171168}, {"filename": "/tests/metron_good/multi_tick.h", "start": 171168, "end": 171547}, {"filename": "/tests/metron_good/namespaces.h", "start": 171547, "end": 171910}, {"filename": "/tests/metron_good/nested_structs.h", "start": 171910, "end": 172370}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 172370, "end": 172928}, {"filename": "/tests/metron_good/oneliners.h", "start": 172928, "end": 173204}, {"filename": "/tests/metron_good/plus_equals.h", "start": 173204, "end": 173628}, {"filename": "/tests/metron_good/private_getter.h", "start": 173628, "end": 173865}, {"filename": "/tests/metron_good/structs.h", "start": 173865, "end": 174097}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 174097, "end": 174646}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 174646, "end": 177203}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 177203, "end": 177970}, {"filename": "/tests/metron_good/tock_task.h", "start": 177970, "end": 178351}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 178351, "end": 178529}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 178529, "end": 179201}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 179201, "end": 179873}], "remote_package_size": 179873});

  })();
