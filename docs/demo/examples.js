
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22885}, {"filename": "/examples/ibex/README.md", "start": 22885, "end": 22936}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22936, "end": 24581}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24581, "end": 36772}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36772, "end": 51270}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 51270, "end": 67353}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67353, "end": 69944}, {"filename": "/examples/j1/metron/dpram.h", "start": 69944, "end": 70395}, {"filename": "/examples/j1/metron/j1.h", "start": 70395, "end": 74428}, {"filename": "/examples/pong/README.md", "start": 74428, "end": 74488}, {"filename": "/examples/pong/metron/pong.h", "start": 74488, "end": 78390}, {"filename": "/examples/pong/reference/README.md", "start": 78390, "end": 78450}, {"filename": "/examples/rvsimple/README.md", "start": 78450, "end": 78529}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78529, "end": 79042}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 79042, "end": 80516}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80516, "end": 83134}, {"filename": "/examples/rvsimple/metron/config.h", "start": 83134, "end": 84362}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 84362, "end": 90094}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 90094, "end": 91217}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 91217, "end": 93163}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 93163, "end": 94403}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 94403, "end": 95660}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95660, "end": 96342}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 96342, "end": 97319}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 97319, "end": 99450}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99450, "end": 100227}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 100227, "end": 100920}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100920, "end": 101749}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101749, "end": 102761}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102761, "end": 103745}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103745, "end": 104443}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104443, "end": 107499}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107499, "end": 113086}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 113086, "end": 115592}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115592, "end": 120332}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 120332, "end": 122333}, {"filename": "/examples/scratch.h", "start": 122333, "end": 122614}, {"filename": "/examples/tutorial/adder.h", "start": 122614, "end": 122794}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122794, "end": 123788}, {"filename": "/examples/tutorial/blockram.h", "start": 123788, "end": 124318}, {"filename": "/examples/tutorial/checksum.h", "start": 124318, "end": 125054}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 125054, "end": 125590}, {"filename": "/examples/tutorial/counter.h", "start": 125590, "end": 125739}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125739, "end": 126518}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126518, "end": 127936}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127936, "end": 128058}, {"filename": "/examples/tutorial/submodules.h", "start": 128058, "end": 129175}, {"filename": "/examples/tutorial/templates.h", "start": 129175, "end": 129664}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129664, "end": 129732}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129732, "end": 129773}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129773, "end": 129814}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129814, "end": 129855}, {"filename": "/examples/tutorial/vga.h", "start": 129855, "end": 131015}, {"filename": "/examples/uart/README.md", "start": 131015, "end": 131259}, {"filename": "/examples/uart/message.txt", "start": 131259, "end": 131771}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 131771, "end": 134364}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 134364, "end": 136938}, {"filename": "/examples/uart/metron/uart_top.h", "start": 136938, "end": 138715}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 138715, "end": 141747}, {"filename": "/tests/metron_bad/README.md", "start": 141747, "end": 141944}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141944, "end": 142253}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 142253, "end": 142514}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 142514, "end": 142770}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142770, "end": 143378}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 143378, "end": 143629}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 143629, "end": 144052}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 144052, "end": 144605}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 144605, "end": 144929}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144929, "end": 145388}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 145388, "end": 145651}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 145651, "end": 145936}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 145936, "end": 146456}, {"filename": "/tests/metron_bad/wrong_submod_call_order.h", "start": 146456, "end": 146800}, {"filename": "/tests/metron_good/README.md", "start": 146800, "end": 146881}, {"filename": "/tests/metron_good/all_func_types.h", "start": 146881, "end": 148559}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 148559, "end": 148979}, {"filename": "/tests/metron_good/basic_function.h", "start": 148979, "end": 149271}, {"filename": "/tests/metron_good/basic_increment.h", "start": 149271, "end": 149639}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 149639, "end": 149947}, {"filename": "/tests/metron_good/basic_literals.h", "start": 149947, "end": 150572}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 150572, "end": 150831}, {"filename": "/tests/metron_good/basic_output.h", "start": 150831, "end": 151105}, {"filename": "/tests/metron_good/basic_param.h", "start": 151105, "end": 151377}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 151377, "end": 151621}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 151621, "end": 151814}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 151814, "end": 152090}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 152090, "end": 152323}, {"filename": "/tests/metron_good/basic_submod.h", "start": 152323, "end": 152642}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 152642, "end": 153010}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 153010, "end": 153399}, {"filename": "/tests/metron_good/basic_switch.h", "start": 153399, "end": 153889}, {"filename": "/tests/metron_good/basic_task.h", "start": 153889, "end": 154236}, {"filename": "/tests/metron_good/basic_template.h", "start": 154236, "end": 154735}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 154735, "end": 154907}, {"filename": "/tests/metron_good/bit_casts.h", "start": 154907, "end": 160893}, {"filename": "/tests/metron_good/bit_concat.h", "start": 160893, "end": 161333}, {"filename": "/tests/metron_good/bit_dup.h", "start": 161333, "end": 162005}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 162005, "end": 162913}, {"filename": "/tests/metron_good/builtins.h", "start": 162913, "end": 163257}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 163257, "end": 163577}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 163577, "end": 164144}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 164144, "end": 164998}, {"filename": "/tests/metron_good/constructor_args.h", "start": 164998, "end": 165470}, {"filename": "/tests/metron_good/defines.h", "start": 165470, "end": 165797}, {"filename": "/tests/metron_good/dontcare.h", "start": 165797, "end": 166090}, {"filename": "/tests/metron_good/enum_simple.h", "start": 166090, "end": 167532}, {"filename": "/tests/metron_good/for_loops.h", "start": 167532, "end": 167865}, {"filename": "/tests/metron_good/good_order.h", "start": 167865, "end": 168174}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 168174, "end": 168598}, {"filename": "/tests/metron_good/include_guards.h", "start": 168598, "end": 168795}, {"filename": "/tests/metron_good/init_chain.h", "start": 168795, "end": 169516}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 169516, "end": 169814}, {"filename": "/tests/metron_good/input_signals.h", "start": 169814, "end": 170612}, {"filename": "/tests/metron_good/local_localparam.h", "start": 170612, "end": 170803}, {"filename": "/tests/metron_good/magic_comments.h", "start": 170803, "end": 171119}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 171119, "end": 171446}, {"filename": "/tests/metron_good/minimal.h", "start": 171446, "end": 171632}, {"filename": "/tests/metron_good/multi_tick.h", "start": 171632, "end": 172011}, {"filename": "/tests/metron_good/namespaces.h", "start": 172011, "end": 172374}, {"filename": "/tests/metron_good/nested_structs.h", "start": 172374, "end": 172834}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 172834, "end": 173392}, {"filename": "/tests/metron_good/oneliners.h", "start": 173392, "end": 173668}, {"filename": "/tests/metron_good/plus_equals.h", "start": 173668, "end": 174092}, {"filename": "/tests/metron_good/private_getter.h", "start": 174092, "end": 174329}, {"filename": "/tests/metron_good/structs.h", "start": 174329, "end": 174561}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 174561, "end": 175110}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 175110, "end": 177667}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 177667, "end": 178434}, {"filename": "/tests/metron_good/tock_task.h", "start": 178434, "end": 178815}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 178815, "end": 178993}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 178993, "end": 179665}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 179665, "end": 180337}], "remote_package_size": 180337});

  })();
