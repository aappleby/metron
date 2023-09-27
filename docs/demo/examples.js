
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 0, "end": 22885}, {"filename": "/examples/ibex/README.md", "start": 22885, "end": 22936}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 22936, "end": 24581}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 24581, "end": 36772}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 36772, "end": 51270}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 51270, "end": 67353}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 67353, "end": 69944}, {"filename": "/examples/j1/metron/dpram.h", "start": 69944, "end": 70395}, {"filename": "/examples/j1/metron/j1.h", "start": 70395, "end": 74428}, {"filename": "/examples/pong/README.md", "start": 74428, "end": 74488}, {"filename": "/examples/pong/metron/pong.h", "start": 74488, "end": 78390}, {"filename": "/examples/pong/reference/README.md", "start": 78390, "end": 78450}, {"filename": "/examples/rvsimple/README.md", "start": 78450, "end": 78529}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 78529, "end": 79042}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 79042, "end": 80516}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 80516, "end": 83134}, {"filename": "/examples/rvsimple/metron/config.h", "start": 83134, "end": 84362}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 84362, "end": 90094}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 90094, "end": 91217}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 91217, "end": 93163}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 93163, "end": 94403}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 94403, "end": 95660}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 95660, "end": 96342}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 96342, "end": 97319}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 97319, "end": 99450}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 99450, "end": 100227}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 100227, "end": 100920}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 100920, "end": 101749}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 101749, "end": 102761}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 102761, "end": 103745}, {"filename": "/examples/rvsimple/metron/register.h", "start": 103745, "end": 104443}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 104443, "end": 107499}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 107499, "end": 113086}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 113086, "end": 115592}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 115592, "end": 120332}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 120332, "end": 122333}, {"filename": "/examples/scratch.h", "start": 122333, "end": 122623}, {"filename": "/examples/tutorial/adder.h", "start": 122623, "end": 122803}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 122803, "end": 123797}, {"filename": "/examples/tutorial/blockram.h", "start": 123797, "end": 124327}, {"filename": "/examples/tutorial/checksum.h", "start": 124327, "end": 125063}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 125063, "end": 125599}, {"filename": "/examples/tutorial/counter.h", "start": 125599, "end": 125748}, {"filename": "/examples/tutorial/declaration_order.h", "start": 125748, "end": 126527}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 126527, "end": 127945}, {"filename": "/examples/tutorial/nonblocking.h", "start": 127945, "end": 128067}, {"filename": "/examples/tutorial/submodules.h", "start": 128067, "end": 129184}, {"filename": "/examples/tutorial/templates.h", "start": 129184, "end": 129673}, {"filename": "/examples/tutorial/tutorial2.h", "start": 129673, "end": 129741}, {"filename": "/examples/tutorial/tutorial3.h", "start": 129741, "end": 129782}, {"filename": "/examples/tutorial/tutorial4.h", "start": 129782, "end": 129823}, {"filename": "/examples/tutorial/tutorial5.h", "start": 129823, "end": 129864}, {"filename": "/examples/tutorial/vga.h", "start": 129864, "end": 131024}, {"filename": "/examples/uart/README.md", "start": 131024, "end": 131268}, {"filename": "/examples/uart/message.txt", "start": 131268, "end": 131780}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 131780, "end": 134373}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 134373, "end": 136947}, {"filename": "/examples/uart/metron/uart_top.h", "start": 136947, "end": 138724}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 138724, "end": 141756}, {"filename": "/tests/metron_bad/README.md", "start": 141756, "end": 141953}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 141953, "end": 142262}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 142262, "end": 142523}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 142523, "end": 142779}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 142779, "end": 143387}, {"filename": "/tests/metron_bad/func_writes_sig_and_reg.h", "start": 143387, "end": 143638}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 143638, "end": 144061}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 144061, "end": 144614}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 144614, "end": 144938}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 144938, "end": 145397}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 145397, "end": 145660}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 145660, "end": 145945}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 145945, "end": 146465}, {"filename": "/tests/metron_good/README.md", "start": 146465, "end": 146546}, {"filename": "/tests/metron_good/all_func_types.h", "start": 146546, "end": 148224}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 148224, "end": 148644}, {"filename": "/tests/metron_good/basic_function.h", "start": 148644, "end": 148936}, {"filename": "/tests/metron_good/basic_increment.h", "start": 148936, "end": 149304}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 149304, "end": 149612}, {"filename": "/tests/metron_good/basic_literals.h", "start": 149612, "end": 150237}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 150237, "end": 150496}, {"filename": "/tests/metron_good/basic_output.h", "start": 150496, "end": 150770}, {"filename": "/tests/metron_good/basic_param.h", "start": 150770, "end": 151042}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 151042, "end": 151286}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 151286, "end": 151479}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 151479, "end": 151755}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 151755, "end": 151988}, {"filename": "/tests/metron_good/basic_submod.h", "start": 151988, "end": 152307}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 152307, "end": 152675}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 152675, "end": 153064}, {"filename": "/tests/metron_good/basic_switch.h", "start": 153064, "end": 153554}, {"filename": "/tests/metron_good/basic_task.h", "start": 153554, "end": 153901}, {"filename": "/tests/metron_good/basic_template.h", "start": 153901, "end": 154400}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 154400, "end": 154572}, {"filename": "/tests/metron_good/bit_casts.h", "start": 154572, "end": 160558}, {"filename": "/tests/metron_good/bit_concat.h", "start": 160558, "end": 160998}, {"filename": "/tests/metron_good/bit_dup.h", "start": 160998, "end": 161670}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 161670, "end": 162578}, {"filename": "/tests/metron_good/builtins.h", "start": 162578, "end": 162922}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 162922, "end": 163242}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 163242, "end": 163809}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 163809, "end": 164663}, {"filename": "/tests/metron_good/constructor_args.h", "start": 164663, "end": 165135}, {"filename": "/tests/metron_good/defines.h", "start": 165135, "end": 165462}, {"filename": "/tests/metron_good/dontcare.h", "start": 165462, "end": 165755}, {"filename": "/tests/metron_good/enum_simple.h", "start": 165755, "end": 167197}, {"filename": "/tests/metron_good/for_loops.h", "start": 167197, "end": 167530}, {"filename": "/tests/metron_good/good_order.h", "start": 167530, "end": 167839}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 167839, "end": 168263}, {"filename": "/tests/metron_good/include_guards.h", "start": 168263, "end": 168460}, {"filename": "/tests/metron_good/init_chain.h", "start": 168460, "end": 169181}, {"filename": "/tests/metron_good/initialize_struct_to_zero.h", "start": 169181, "end": 169479}, {"filename": "/tests/metron_good/input_signals.h", "start": 169479, "end": 170277}, {"filename": "/tests/metron_good/local_localparam.h", "start": 170277, "end": 170468}, {"filename": "/tests/metron_good/magic_comments.h", "start": 170468, "end": 170784}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 170784, "end": 171111}, {"filename": "/tests/metron_good/minimal.h", "start": 171111, "end": 171297}, {"filename": "/tests/metron_good/multi_tick.h", "start": 171297, "end": 171676}, {"filename": "/tests/metron_good/namespaces.h", "start": 171676, "end": 172039}, {"filename": "/tests/metron_good/nested_structs.h", "start": 172039, "end": 172499}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 172499, "end": 173057}, {"filename": "/tests/metron_good/oneliners.h", "start": 173057, "end": 173333}, {"filename": "/tests/metron_good/plus_equals.h", "start": 173333, "end": 173757}, {"filename": "/tests/metron_good/private_getter.h", "start": 173757, "end": 173994}, {"filename": "/tests/metron_good/structs.h", "start": 173994, "end": 174226}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 174226, "end": 174775}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 174775, "end": 177332}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 177332, "end": 178099}, {"filename": "/tests/metron_good/tock_task.h", "start": 178099, "end": 178480}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 178480, "end": 178658}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 178658, "end": 179330}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 179330, "end": 180002}], "remote_package_size": 180002});

  })();
