// Apply reviewed source/report address names to a Ghidra reference project.
// @category Rec2

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.SourceType;

import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;

/** Input TSV: hexadecimal address, C identifier. Never splits existing functions. */
public class ImportRec2FunctionNames extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length != 1) {
            throw new IllegalArgumentException("Expected a function-name TSV path");
        }
        int named = 0;
        int created = 0;
        int skipped = 0;
        for (String line : Files.readAllLines(Paths.get(args[0]), StandardCharsets.UTF_8)) {
            monitor.checkCancelled();
            if (line.isBlank() || line.startsWith("#")) {
                continue;
            }
            String[] fields = line.split("\t", -1);
            if (fields.length != 2 || !fields[1].matches("[A-Za-z_][A-Za-z_0-9]*")) {
                throw new IllegalArgumentException("Invalid TSV record: " + line);
            }
            Address address = toAddr(fields[0]);
            MemoryBlock block = currentProgram.getMemory().getBlock(address);
            if (block == null || !block.isExecute()) {
                println("SKIP non-code address: " + line);
                skipped++;
                continue;
            }
            Function function = getFunctionAt(address);
            if (function == null) {
                Function containing = getFunctionContaining(address);
                if (containing != null) {
                    println("SKIP existing body " + containing.getEntryPoint() + ": " + line);
                    skipped++;
                    continue;
                }
                if (!disassemble(address)) {
                    println("SKIP disassembly failed: " + line);
                    skipped++;
                    continue;
                }
                function = createFunction(address, fields[1]);
                if (function == null) {
                    println("SKIP function creation failed: " + line);
                    skipped++;
                    continue;
                }
                created++;
            }
            function.setName(fields[1], SourceType.USER_DEFINED);
            named++;
        }
        println("Function names applied=" + named + ", newly discovered=" + created + ", skipped=" + skipped);
    }
}
