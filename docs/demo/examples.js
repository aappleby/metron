
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
Module['FS_createPath']("/examples", "gb_spu", true, true);
Module['FS_createPath']("/examples/gb_spu", "metron", true, true);
Module['FS_createPath']("/examples", "ibex", true, true);
Module['FS_createPath']("/examples/ibex", "metron", true, true);
Module['FS_createPath']("/examples/ibex", "metron_ref", true, true);
Module['FS_createPath']("/examples", "j1", true, true);
Module['FS_createPath']("/examples/j1", "metron", true, true);
Module['FS_createPath']("/examples/j1", "metron_sv", true, true);
Module['FS_createPath']("/examples", "pong", true, true);
Module['FS_createPath']("/examples/pong", "metron", true, true);
Module['FS_createPath']("/examples/pong", "reference", true, true);
Module['FS_createPath']("/examples", "rvsimple", true, true);
Module['FS_createPath']("/examples/rvsimple", "metron", true, true);
Module['FS_createPath']("/examples/rvsimple", "reference_sv", true, true);
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
    loadPackage({"files": [{"filename": "/examples/gb_spu/gb_spu_main.cpp", "start": 0, "end": 4066}, {"filename": "/examples/gb_spu/metron/MetroBoySPU2.h", "start": 4066, "end": 26876}, {"filename": "/examples/ibex/README.md", "start": 26876, "end": 26927}, {"filename": "/examples/ibex/main.cpp", "start": 26927, "end": 27068}, {"filename": "/examples/ibex/metron/ibex_alu.h", "start": 27068, "end": 28653}, {"filename": "/examples/ibex/metron/ibex_compressed_decoder.h", "start": 28653, "end": 40751}, {"filename": "/examples/ibex/metron/ibex_multdiv_slow.h", "start": 40751, "end": 55161}, {"filename": "/examples/ibex/metron/ibex_pkg.h", "start": 55161, "end": 71185}, {"filename": "/examples/ibex/metron/prim_arbiter_fixed.h", "start": 71185, "end": 73686}, {"filename": "/examples/ibex/metron_ref/ibex_alu.sv", "start": 73686, "end": 126004}, {"filename": "/examples/ibex/metron_ref/ibex_compressed_decoder.sv", "start": 126004, "end": 136846}, {"filename": "/examples/ibex/metron_ref/ibex_multdiv_slow.sv", "start": 136846, "end": 149956}, {"filename": "/examples/ibex/metron_ref/ibex_pkg.sv", "start": 149956, "end": 166195}, {"filename": "/examples/ibex/metron_ref/prim_arbiter_fixed.sv", "start": 166195, "end": 172306}, {"filename": "/examples/j1/main.cpp", "start": 172306, "end": 172425}, {"filename": "/examples/j1/metron/dpram.h", "start": 172425, "end": 172863}, {"filename": "/examples/j1/metron/j1.h", "start": 172863, "end": 176888}, {"filename": "/examples/j1/metron_sv/j1.sv", "start": 176888, "end": 176916}, {"filename": "/examples/pong/README.md", "start": 176916, "end": 176976}, {"filename": "/examples/pong/main.cpp", "start": 176976, "end": 178922}, {"filename": "/examples/pong/main_vl.cpp", "start": 178922, "end": 179073}, {"filename": "/examples/pong/metron/pong.h", "start": 179073, "end": 182937}, {"filename": "/examples/pong/reference/README.md", "start": 182937, "end": 182997}, {"filename": "/examples/pong/reference/hvsync_generator.sv", "start": 182997, "end": 183936}, {"filename": "/examples/pong/reference/pong.sv", "start": 183936, "end": 187550}, {"filename": "/examples/rvsimple/README.md", "start": 187550, "end": 187629}, {"filename": "/examples/rvsimple/main.cpp", "start": 187629, "end": 190444}, {"filename": "/examples/rvsimple/main_ref_vl.cpp", "start": 190444, "end": 193346}, {"filename": "/examples/rvsimple/main_vl.cpp", "start": 193346, "end": 196248}, {"filename": "/examples/rvsimple/metron/adder.h", "start": 196248, "end": 196748}, {"filename": "/examples/rvsimple/metron/alu.h", "start": 196748, "end": 198209}, {"filename": "/examples/rvsimple/metron/alu_control.h", "start": 198209, "end": 200830}, {"filename": "/examples/rvsimple/metron/config.h", "start": 200830, "end": 202045}, {"filename": "/examples/rvsimple/metron/constants.h", "start": 202045, "end": 207764}, {"filename": "/examples/rvsimple/metron/control_transfer.h", "start": 207764, "end": 208878}, {"filename": "/examples/rvsimple/metron/data_memory_interface.h", "start": 208878, "end": 210816}, {"filename": "/examples/rvsimple/metron/example_data_memory.h", "start": 210816, "end": 212056}, {"filename": "/examples/rvsimple/metron/example_data_memory_bus.h", "start": 212056, "end": 213209}, {"filename": "/examples/rvsimple/metron/example_text_memory.h", "start": 213209, "end": 213891}, {"filename": "/examples/rvsimple/metron/example_text_memory_bus.h", "start": 213891, "end": 214764}, {"filename": "/examples/rvsimple/metron/immediate_generator.h", "start": 214764, "end": 216882}, {"filename": "/examples/rvsimple/metron/instruction_decoder.h", "start": 216882, "end": 217646}, {"filename": "/examples/rvsimple/metron/multiplexer2.h", "start": 217646, "end": 218337}, {"filename": "/examples/rvsimple/metron/multiplexer4.h", "start": 218337, "end": 219164}, {"filename": "/examples/rvsimple/metron/multiplexer8.h", "start": 219164, "end": 220174}, {"filename": "/examples/rvsimple/metron/regfile.h", "start": 220174, "end": 221145}, {"filename": "/examples/rvsimple/metron/register.h", "start": 221145, "end": 221830}, {"filename": "/examples/rvsimple/metron/riscv_core.h", "start": 221830, "end": 224873}, {"filename": "/examples/rvsimple/metron/singlecycle_control.h", "start": 224873, "end": 230467}, {"filename": "/examples/rvsimple/metron/singlecycle_ctlpath.h", "start": 230467, "end": 232960}, {"filename": "/examples/rvsimple/metron/singlecycle_datapath.h", "start": 232960, "end": 237687}, {"filename": "/examples/rvsimple/metron/toplevel.h", "start": 237687, "end": 239535}, {"filename": "/examples/rvsimple/reference_sv/adder.sv", "start": 239535, "end": 239984}, {"filename": "/examples/rvsimple/reference_sv/alu.sv", "start": 239984, "end": 243105}, {"filename": "/examples/rvsimple/reference_sv/alu_control.sv", "start": 243105, "end": 246558}, {"filename": "/examples/rvsimple/reference_sv/config.sv", "start": 246558, "end": 247965}, {"filename": "/examples/rvsimple/reference_sv/constants.sv", "start": 247965, "end": 252702}, {"filename": "/examples/rvsimple/reference_sv/control_transfer.sv", "start": 252702, "end": 253587}, {"filename": "/examples/rvsimple/reference_sv/data_memory_interface.sv", "start": 253587, "end": 255498}, {"filename": "/examples/rvsimple/reference_sv/example_data_memory.sv", "start": 255498, "end": 256413}, {"filename": "/examples/rvsimple/reference_sv/example_data_memory_bus.sv", "start": 256413, "end": 257427}, {"filename": "/examples/rvsimple/reference_sv/example_memory_bus.sv", "start": 257427, "end": 258735}, {"filename": "/examples/rvsimple/reference_sv/example_text_memory.sv", "start": 258735, "end": 259261}, {"filename": "/examples/rvsimple/reference_sv/example_text_memory_bus.sv", "start": 259261, "end": 259946}, {"filename": "/examples/rvsimple/reference_sv/immediate_generator.sv", "start": 259946, "end": 261886}, {"filename": "/examples/rvsimple/reference_sv/instruction_decoder.sv", "start": 261886, "end": 262599}, {"filename": "/examples/rvsimple/reference_sv/multiplexer.sv", "start": 262599, "end": 263377}, {"filename": "/examples/rvsimple/reference_sv/multiplexer2.sv", "start": 263377, "end": 263965}, {"filename": "/examples/rvsimple/reference_sv/multiplexer4.sv", "start": 263965, "end": 264619}, {"filename": "/examples/rvsimple/reference_sv/multiplexer8.sv", "start": 264619, "end": 265405}, {"filename": "/examples/rvsimple/reference_sv/regfile.sv", "start": 265405, "end": 266314}, {"filename": "/examples/rvsimple/reference_sv/register.sv", "start": 266314, "end": 266947}, {"filename": "/examples/rvsimple/reference_sv/riscv_core.sv", "start": 266947, "end": 270321}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_control.sv", "start": 270321, "end": 275053}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_ctlpath.sv", "start": 275053, "end": 276843}, {"filename": "/examples/rvsimple/reference_sv/singlecycle_datapath.sv", "start": 276843, "end": 281072}, {"filename": "/examples/rvsimple/reference_sv/toplevel.sv", "start": 281072, "end": 282703}, {"filename": "/examples/scratch.h", "start": 282703, "end": 283463}, {"filename": "/examples/scratch.sv", "start": 283463, "end": 284850}, {"filename": "/examples/tutorial/adder.h", "start": 284850, "end": 285030}, {"filename": "/examples/tutorial/bit_twiddling.h", "start": 285030, "end": 286011}, {"filename": "/examples/tutorial/blockram.h", "start": 286011, "end": 286528}, {"filename": "/examples/tutorial/checksum.h", "start": 286528, "end": 287251}, {"filename": "/examples/tutorial/clocked_adder.h", "start": 287251, "end": 287787}, {"filename": "/examples/tutorial/counter.h", "start": 287787, "end": 287936}, {"filename": "/examples/tutorial/declaration_order.h", "start": 287936, "end": 288715}, {"filename": "/examples/tutorial/functions_and_tasks.h", "start": 288715, "end": 290133}, {"filename": "/examples/tutorial/nonblocking.h", "start": 290133, "end": 290255}, {"filename": "/examples/tutorial/submodules.h", "start": 290255, "end": 291372}, {"filename": "/examples/tutorial/templates.h", "start": 291372, "end": 291843}, {"filename": "/examples/tutorial/tutorial2.h", "start": 291843, "end": 291911}, {"filename": "/examples/tutorial/tutorial3.h", "start": 291911, "end": 291952}, {"filename": "/examples/tutorial/tutorial4.h", "start": 291952, "end": 291993}, {"filename": "/examples/tutorial/tutorial5.h", "start": 291993, "end": 292034}, {"filename": "/examples/tutorial/vga.h", "start": 292034, "end": 293181}, {"filename": "/examples/uart/README.md", "start": 293181, "end": 293425}, {"filename": "/examples/uart/main.cpp", "start": 293425, "end": 294757}, {"filename": "/examples/uart/main_vl.cpp", "start": 294757, "end": 297277}, {"filename": "/examples/uart/metron/uart_hello.h", "start": 297277, "end": 299857}, {"filename": "/examples/uart/metron/uart_rx.h", "start": 299857, "end": 302418}, {"filename": "/examples/uart/metron/uart_top.h", "start": 302418, "end": 304182}, {"filename": "/examples/uart/metron/uart_tx.h", "start": 304182, "end": 307201}, {"filename": "/examples/uart/uart_test_ice40.sv", "start": 307201, "end": 309376}, {"filename": "/examples/uart/uart_test_iv.sv", "start": 309376, "end": 310973}, {"filename": "/tests/metron_bad/README.md", "start": 310973, "end": 311170}, {"filename": "/tests/metron_bad/basic_reg_rwr.h", "start": 311170, "end": 311466}, {"filename": "/tests/metron_bad/basic_sig_wrw.h", "start": 311466, "end": 311714}, {"filename": "/tests/metron_bad/bowtied_signals.h", "start": 311714, "end": 311957}, {"filename": "/tests/metron_bad/case_without_break.h", "start": 311957, "end": 312552}, {"filename": "/tests/metron_bad/if_with_no_compound.h", "start": 312552, "end": 312962}, {"filename": "/tests/metron_bad/mid_block_break.h", "start": 312962, "end": 313502}, {"filename": "/tests/metron_bad/mid_block_return.h", "start": 313502, "end": 313813}, {"filename": "/tests/metron_bad/multiple_submod_function_bindings.h", "start": 313813, "end": 314259}, {"filename": "/tests/metron_bad/multiple_tock_returns.h", "start": 314259, "end": 314519}, {"filename": "/tests/metron_bad/tick_with_return_value.h", "start": 314519, "end": 314791}, {"filename": "/tests/metron_bad/too_many_breaks.h", "start": 314791, "end": 315298}, {"filename": "/tests/metron_good/README.md", "start": 315298, "end": 315379}, {"filename": "/tests/metron_good/all_func_types.h", "start": 315379, "end": 316944}, {"filename": "/tests/metron_good/basic_constructor.h", "start": 316944, "end": 317351}, {"filename": "/tests/metron_good/basic_function.h", "start": 317351, "end": 317630}, {"filename": "/tests/metron_good/basic_increment.h", "start": 317630, "end": 317985}, {"filename": "/tests/metron_good/basic_inputs.h", "start": 317985, "end": 318280}, {"filename": "/tests/metron_good/basic_literals.h", "start": 318280, "end": 318892}, {"filename": "/tests/metron_good/basic_localparam.h", "start": 318892, "end": 319138}, {"filename": "/tests/metron_good/basic_output.h", "start": 319138, "end": 319399}, {"filename": "/tests/metron_good/basic_param.h", "start": 319399, "end": 319658}, {"filename": "/tests/metron_good/basic_public_reg.h", "start": 319658, "end": 319889}, {"filename": "/tests/metron_good/basic_public_sig.h", "start": 319889, "end": 320069}, {"filename": "/tests/metron_good/basic_reg_rww.h", "start": 320069, "end": 320332}, {"filename": "/tests/metron_good/basic_sig_wwr.h", "start": 320332, "end": 320552}, {"filename": "/tests/metron_good/basic_submod.h", "start": 320552, "end": 320841}, {"filename": "/tests/metron_good/basic_submod_param.h", "start": 320841, "end": 321196}, {"filename": "/tests/metron_good/basic_submod_public_reg.h", "start": 321196, "end": 321572}, {"filename": "/tests/metron_good/basic_switch.h", "start": 321572, "end": 322049}, {"filename": "/tests/metron_good/basic_task.h", "start": 322049, "end": 322383}, {"filename": "/tests/metron_good/basic_template.h", "start": 322383, "end": 322869}, {"filename": "/tests/metron_good/basic_tock_with_return.h", "start": 322869, "end": 323028}, {"filename": "/tests/metron_good/bit_casts.h", "start": 323028, "end": 329001}, {"filename": "/tests/metron_good/bit_concat.h", "start": 329001, "end": 329428}, {"filename": "/tests/metron_good/bit_dup.h", "start": 329428, "end": 330087}, {"filename": "/tests/metron_good/both_tock_and_tick_use_tasks_and_funcs.h", "start": 330087, "end": 330925}, {"filename": "/tests/metron_good/builtins.h", "start": 330925, "end": 331256}, {"filename": "/tests/metron_good/call_tick_from_tock.h", "start": 331256, "end": 331563}, {"filename": "/tests/metron_good/case_with_fallthrough.h", "start": 331563, "end": 332117}, {"filename": "/tests/metron_good/constructor_arg_passing.h", "start": 332117, "end": 333010}, {"filename": "/tests/metron_good/constructor_args.h", "start": 333010, "end": 333518}, {"filename": "/tests/metron_good/defines.h", "start": 333518, "end": 333832}, {"filename": "/tests/metron_good/dontcare.h", "start": 333832, "end": 334115}, {"filename": "/tests/metron_good/enum_simple.h", "start": 334115, "end": 335483}, {"filename": "/tests/metron_good/for_loops.h", "start": 335483, "end": 335803}, {"filename": "/tests/metron_good/good_order.h", "start": 335803, "end": 336099}, {"filename": "/tests/metron_good/if_with_compound.h", "start": 336099, "end": 336510}, {"filename": "/tests/metron_good/include_guards.h", "start": 336510, "end": 336707}, {"filename": "/tests/metron_good/init_chain.h", "start": 336707, "end": 337415}, {"filename": "/tests/metron_good/input_signals.h", "start": 337415, "end": 338077}, {"filename": "/tests/metron_good/local_localparam.h", "start": 338077, "end": 338255}, {"filename": "/tests/metron_good/magic_comments.h", "start": 338255, "end": 338558}, {"filename": "/tests/metron_good/matching_port_and_arg_names.h", "start": 338558, "end": 338872}, {"filename": "/tests/metron_good/minimal.h", "start": 338872, "end": 338947}, {"filename": "/tests/metron_good/multi_tick.h", "start": 338947, "end": 339313}, {"filename": "/tests/metron_good/namespaces.h", "start": 339313, "end": 339663}, {"filename": "/tests/metron_good/nested_structs.h", "start": 339663, "end": 340176}, {"filename": "/tests/metron_good/nested_submod_calls.h", "start": 340176, "end": 340721}, {"filename": "/tests/metron_good/oneliners.h", "start": 340721, "end": 340984}, {"filename": "/tests/metron_good/plus_equals.h", "start": 340984, "end": 341334}, {"filename": "/tests/metron_good/private_getter.h", "start": 341334, "end": 341558}, {"filename": "/tests/metron_good/structs.h", "start": 341558, "end": 341777}, {"filename": "/tests/metron_good/structs_as_args.h", "start": 341777, "end": 342313}, {"filename": "/tests/metron_good/structs_as_ports.h", "start": 342313, "end": 344674}, {"filename": "/tests/metron_good/submod_bindings.h", "start": 344674, "end": 345428}, {"filename": "/tests/metron_good/tock_task.h", "start": 345428, "end": 345784}, {"filename": "/tests/metron_good/trivial_adder.h", "start": 345784, "end": 345949}, {"filename": "/tests/metron_good/utf8-mod.bom.h", "start": 345949, "end": 346608}, {"filename": "/tests/metron_good/utf8-mod.h", "start": 346608, "end": 347267}], "remote_package_size": 347267});

  })();
