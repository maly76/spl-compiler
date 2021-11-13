package de.thm.mni.compilerbau.phases._06_codegen;

import de.thm.mni.compilerbau.absyn.*;
import de.thm.mni.compilerbau.absyn.visitor.CodeGeneratorVisitor;
import de.thm.mni.compilerbau.table.ProcedureEntry;
import de.thm.mni.compilerbau.table.SymbolTable;

import java.io.PrintWriter;

public class CodeGenerator {
    private final CodePrinter output;

    public CodeGenerator(PrintWriter output) {
        this.output = new CodePrinter(output);
    }

    private void assemblerProlog() {
        output.emitImport("printi");
        output.emitImport("printc");
        output.emitImport("readi");
        output.emitImport("readc");
        output.emitImport("exit");
        output.emitImport("time");
        output.emitImport("clearAll");
        output.emitImport("setPixel");
        output.emitImport("drawLine");
        output.emitImport("drawCircle");
        output.emitImport("_indexError");
        output.emit("");
        output.emit("\t.code");
        output.emit("\t.align\t4");
    }

    public void generateCode(Program program, SymbolTable table) {

        assemblerProlog();
        for (GlobalDeclaration dec : program.declarations)
        {
            var entry = table.lookup(dec.name);
            if (entry instanceof ProcedureEntry){
                CodeGeneratorVisitor visitor = new CodeGeneratorVisitor(output, ((ProcedureEntry) entry).localTable,
                                                                     ((ProcedureDeclaration)dec));
                dec.accept(visitor);
            }
        }
    }
}
