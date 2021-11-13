package de.thm.mni.compilerbau.phases._06_codegen;

import java.io.PrintWriter;

public class CodePrinter {
    private final PrintWriter outputFile;

    CodePrinter(PrintWriter outputFile) {
        this.outputFile = outputFile;
    }

    public void emitInstruction(String opcode, Register r1, Register r2, Register r3) {
        outputFile.printf("\t%s\t%s,%s,%s\n", opcode, r1, r2, r3);
    }

    public void emitInstruction(String opcode, Register r1, Register r2, int value) {
        outputFile.printf("\t%s\t%s,%s,%d\n", opcode, r1, r2, value);
    }

    public void emitInstruction(String opcode, Register r1, Register r2, String label) {
        outputFile.printf("\t%s\t%s,%s,%s\n", opcode, r1, r2, label);
    }

    public void emitInstruction(String opcode, Register r1) {
        outputFile.printf("\t%s\t%s\n", opcode, r1);
    }

    public void emitInstruction(String opcode, String label) {
        outputFile.printf("\t%s\t%s\n", opcode, label);
    }

    public void emitInstruction(String opcode, Register r1, Register r2, Register r3, String comment) {
        outputFile.printf("\t%s\t%s,%s,%s\t\t; %s\n", opcode, r1, r2, r3, comment);
    }

    public void emitInstruction(String opcode, Register r1, Register r2, int value, String comment) {
        outputFile.printf("\t%s\t%s,%s,%d\t\t; %s\n", opcode, r1, r2, value, comment);
    }

    public void emitInstruction(String opcode, Register r1, Register r2, String label, String comment) {
        outputFile.printf("\t%s\t%s,%s,%s\t\t; %s\n", opcode, r1, r2, label, comment);
    }

    public void emitInstruction(String opcode, Register r1, String comment) {
        outputFile.printf("\t%s\t%s\t\t\t; %s\n", opcode, r1, comment);
    }

    public void emitInstruction(String opcode, String label, String comment) {
        outputFile.printf("\t%s\t%s\t\t; %s\n", opcode, label, comment);
    }

    public void emitLabel(String label) {
        outputFile.printf("%s:\n", label);
    }

    public void emitImport(String label) {
        outputFile.printf("\t.import\t%s\n", label);
    }

    public void emitExport(String label) {
        outputFile.printf("\t.export\t%s\n", label);
    }

    public void emit(String str) {
        outputFile.println(str);
    }
}

