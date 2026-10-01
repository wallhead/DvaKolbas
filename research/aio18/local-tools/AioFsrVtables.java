//@category Analysis
import ghidra.app.script.GhidraScript;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolIterator;
import ghidra.program.model.address.Address;
import ghidra.program.model.mem.MemoryBlock;
import java.io.PrintWriter;
import java.util.TreeSet;

public class AioFsrVtables extends GhidraScript {
    public void run() throws Exception {
        String[] args = getScriptArgs();
        TreeSet<Long> targets = new TreeSet<>();
        SymbolIterator symbols = currentProgram.getSymbolTable().getAllSymbols(true);
        try (PrintWriter out = new PrintWriter(args[0], "UTF-8")) {
            while (symbols.hasNext()) {
                Symbol symbol = symbols.next();
                String name = symbol.getName(true);
                if (!name.endsWith("::vftable") ||
                    !(name.contains("FrameGenMethod_FSR3_FFXAPI") || name.contains("UpscaleMethod_FSR3_FFXAPI") ||
                      name.contains("UpscaleMethod_FSR3_DX11") || name.contains("DXGISwapChainDX11Wrapper") ||
                      name.contains("UpscaleMethod_DX11WrapperForDX12"))) continue;
                out.println("VTABLE " + name + " RVA=0x" + Long.toHexString(symbol.getAddress().subtract(currentProgram.getImageBase())));
                for (int slot=0; slot<48; slot++) {
                    long pointer = currentProgram.getMemory().getLong(symbol.getAddress().add(slot*8));
                    Address target = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(pointer);
                    MemoryBlock block = currentProgram.getMemory().getBlock(target);
                    if (block == null || !block.isExecute()) break;
                    long rva = target.subtract(currentProgram.getImageBase());
                    out.println("slot=" + slot + " offset=0x" + Integer.toHexString(slot*8) + " RVA=0x" + Long.toHexString(rva));
                    targets.add(rva);
                }
            }
        }
        long[] helpers = {0xffe70,0x100a00,0x1009a0,0xccc80,0x6fdf0,0x72860,0x100ed0,0xf0580,0xf0890};
        for (long rva : helpers) targets.add(rva);
        try (PrintWriter out = new PrintWriter(args[1], "UTF-8")) {
            for (long rva : targets) out.println("0x" + Long.toHexString(rva));
        }
        println("FSR vtable report: " + targets.size() + " targets");
    }
}
