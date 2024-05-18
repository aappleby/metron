
        print_b(f"Checking that all converted files can be {'synthesized' if options.synth else 'parsed'} by Yosys (after sv2v)")
        if shutil.which("yosys") and shutil.which("sv2v"):
            if options.synth:
                errors += check_commands_good(
                    [
                        f"yosys -q -p 'read_verilog {includes} -sv {filename}; dump; synth_ice40 -json /dev/null'"
                        for filename in metron_v
                    ]
                )
            else:
                errors += check_commands_good(
                    [
                        f"yosys -q -p 'read_verilog {includes} -sv {filename};'"
                        for filename in metron_v
                    ]
                )
            print()
        else:
            print_r("Yosys and/or SV2V not found")
            errors += 1
