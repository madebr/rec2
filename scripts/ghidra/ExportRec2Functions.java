// Export analyzed original functions without changing the reference program.
// @category Rec2

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;

import java.io.BufferedWriter;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.LinkedHashSet;
import java.util.Set;

/**
 * Arguments: output directory, optionally a text file of hexadecimal entry points.
 * Always exports function metadata. Decompiles only explicitly requested entries;
 * decompiler output is investigation material, never verified replacement code.
 */
public class ExportRec2Functions extends GhidraScript {
    private static String quote(String value) {
        StringBuilder out = new StringBuilder("\"");
        for (int i = 0; i < value.length(); i++) {
            char c = value.charAt(i);
            if (c == '"' || c == '\\') {
                out.append('\\').append(c);
            } else if (c < 32) {
                out.append(String.format("\\u%04x", (int)c));
            } else {
                out.append(c);
            }
        }
        return out.append('"').toString();
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1 || args.length > 2) {
            throw new IllegalArgumentException("Expected output directory and optional entry-point file");
        }
        Path output = Paths.get(args[0]).toAbsolutePath();
        Files.createDirectories(output);
        int count = 0;
        try (BufferedWriter writer = Files.newBufferedWriter(output.resolve("functions.jsonl"), StandardCharsets.UTF_8)) {
            FunctionIterator functions = currentProgram.getFunctionManager().getFunctions(true);
            while (functions.hasNext()) {
                monitor.checkCancelled();
                Function f = functions.next();
                if (f.isExternal()) {
                    continue;
                }
                writer.write("{\"address\":" + quote("0x" + f.getEntryPoint())
                    + ",\"name\":" + quote(f.getName())
                    + ",\"body_bytes\":" + f.getBody().getNumAddresses()
                    + ",\"thunk\":" + f.isThunk()
                    + ",\"signature\":" + quote(f.getSignature().toString()) + "}");
                writer.newLine();
                count++;
            }
        }
        println("Exported " + count + " analyzed functions to " + output);
        if (args.length == 1) {
            return;
        }

        Set<Address> requested = new LinkedHashSet<>();
        for (String line : Files.readAllLines(Paths.get(args[1]), StandardCharsets.UTF_8)) {
            String value = line.trim();
            if (!value.isEmpty() && !value.startsWith("#")) {
                requested.add(toAddr(value));
            }
        }
        DecompInterface decompiler = new DecompInterface();
        try {
            if (!decompiler.openProgram(currentProgram)) {
                throw new IllegalStateException(decompiler.getLastMessage());
            }
            for (Address address : requested) {
                monitor.checkCancelled();
                Function f = currentProgram.getFunctionManager().getFunctionAt(address);
                if (f == null) {
                    throw new IllegalArgumentException("No analyzed function starts at " + address);
                }
                DecompileResults result = decompiler.decompileFunction(f, 60, monitor);
                if (!result.decompileCompleted()) {
                    throw new IllegalStateException("Decompilation failed at " + address + ": " + result.getErrorMessage());
                }
                Files.writeString(output.resolve(address + ".c"),
                    "/* Ghidra analysis of original function " + address + "; not verified C. */\n"
                    + result.getDecompiledFunction().getC(), StandardCharsets.UTF_8);
            }
        } finally {
            decompiler.dispose();
        }
    }
}
