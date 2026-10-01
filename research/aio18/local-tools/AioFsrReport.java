// Offline structural/decompiler report; never executes the imported binary.
//@category Analysis
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import java.nio.file.Files;
import java.nio.file.Path;
import java.io.PrintWriter;
import java.util.LinkedHashSet;
import java.util.Set;

public class AioFsrReport extends GhidraScript {
    public void run() throws Exception {
        String[] args = getScriptArgs();
        Set<Function> targets = new LinkedHashSet<>();
        for (String line : Files.readAllLines(Path.of(args[1]))) {
            if (line.isBlank()) continue;
            Address address = currentProgram.getImageBase().add(Long.decode(line.trim()));
            Function f = currentProgram.getFunctionManager().getFunctionAt(address);
            if (f == null) f = currentProgram.getFunctionManager().getFunctionContaining(address);
            if (f == null) {
                disassemble(address);
                f = createFunction(address, null);
            }
            if (f != null) targets.add(f);
        }
        DecompInterface decompiler = new DecompInterface();
        decompiler.openProgram(currentProgram);
        try (PrintWriter out = new PrintWriter(args[0], "UTF-8")) {
            out.println("PROGRAM " + currentProgram.getName());
            out.println("IMAGE_BASE " + currentProgram.getImageBase());
            out.println("TARGETS " + targets.size());
            for (Function f : targets) {
                if (monitor.isCancelled()) break;
                out.println("\n=== FUNCTION " + f.getName() + " VA=" + f.getEntryPoint() +
                    " RVA=0x" + Long.toHexString(f.getEntryPoint().subtract(currentProgram.getImageBase())) + " ===");
                out.println("CALLERS");
                for (Function caller : f.getCallingFunctions(monitor)) out.println(caller.getEntryPoint() + " " + caller.getName());
                out.println("CALLEES");
                for (Function callee : f.getCalledFunctions(monitor)) out.println(callee.getEntryPoint() + " " + callee.getName());
                DecompileResults result = decompiler.decompileFunction(f, 20, monitor);
                if (result.decompileCompleted()) out.println(result.getDecompiledFunction().getC());
                else out.println("DECOMPILE_UNAVAILABLE " + result.getErrorMessage());
                out.flush();
            }
        } finally {
            decompiler.dispose();
        }
        println("FSR report written: " + args[0]);
    }
}
