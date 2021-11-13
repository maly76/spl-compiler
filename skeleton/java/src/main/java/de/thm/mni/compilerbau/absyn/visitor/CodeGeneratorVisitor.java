package de.thm.mni.compilerbau.absyn.visitor;

import de.thm.mni.compilerbau.absyn.*;
import de.thm.mni.compilerbau.phases._06_codegen.*;
import de.thm.mni.compilerbau.table.*;
import de.thm.mni.compilerbau.types.ArrayType;
import de.thm.mni.compilerbau.utils.SplError;

import java.util.EmptyStackException;

public class CodeGeneratorVisitor extends DoNothingVisitor {
    enum Instruction{
        add,
        sub,
        mul,
        div,
        rem,
        ldw,
        stw,
        j,
        jr,
        jal,
        beq,
        bne,
        ble,
        blt,
        bge,
        bgeu,
        bgt;
    }
    CodePrinter output;
    SymbolTable table;
    ProcedureDeclaration procedureDeklaration;
    final Register[] registers;
    final Register nullReg = new Register(0);
    final Register fp = new Register(25);
    final Register sp = new Register(29);
    final Register ra = new Register(31);
    private static int labelNumber = 0;
    int rsp = 0;
    String labelForJumbing;

    public CodeGeneratorVisitor(CodePrinter printer, SymbolTable _table, ProcedureDeclaration procedure) {
        registers = new Register[16];
        for (int i = 0; i < registers.length; i++)
            registers[i] = new Register(i+8);

        if (printer != null)    output = printer;
        if (_table != null)     table = _table;
        if (procedure != null)  procedureDeklaration = procedure;
    }

    Register getNextFreeRegister(){
        if (rsp >= 16)
            throw SplError.RegisterOverflow();

        Register reg = registers[rsp++];
        if (reg.isFreeUse())
            return reg;
        else
            throw SplError.RegisterOverflow();
    }

    Register getLastRegister(){
        if (rsp > 0)
            return registers[--rsp];
        else
            throw new EmptyStackException();
    }

    String getLabelName(){
        return "L" + labelNumber++;
    }

    String getLastLabel(){
        return "L" + labelNumber;
    }
    
    public void visit(ArrayAccess arrayAccess) {
        arrayAccess.array.accept(this);
        arrayAccess.index.accept(this);
        output.emitInstruction(Instruction.add.name(), getNextFreeRegister(), nullReg,
                (arrayAccess.array.dataType.byteSize / 4), "arraysize");
        Register secondOperand = getLastRegister();
        Register firstOperand = getLastRegister();
        output.emitInstruction(Instruction.bgeu.name(), firstOperand, secondOperand, "_indexError"
                , "error if index is negative or greather then the arraysize");

        output.emitInstruction(Instruction.mul.name(), getNextFreeRegister(), getLastRegister(),
                arrayAccess.dataType.byteSize);
        output.emitInstruction(Instruction.add.name(), getLastRegister(), getNextFreeRegister(), getNextFreeRegister());
        getLastRegister();
    }
    
    public void visit(AssignStatement assignStatement) {
        assignStatement.target.accept(this);
        assignStatement.value.accept(this);
        output.emitInstruction(Instruction.stw.name(), getLastRegister(), getLastRegister(), 0);
    }
    
    public void visit(BinaryExpression binaryExpression) {
        binaryExpression.leftOperand.accept(this);
        binaryExpression.rightOperand.accept(this);
        Register secondOperand = getLastRegister();
        Register firstOperand = getLastRegister();
        switch (binaryExpression.operator.name())
        {
            case "ADD":     output.emitInstruction(Instruction.add.name(), getNextFreeRegister(), firstOperand, secondOperand);  break;
            case "SUB":     output.emitInstruction(Instruction.sub.name(), getNextFreeRegister(), firstOperand, secondOperand);  break;
            case "MUL":     output.emitInstruction(Instruction.mul.name(), getNextFreeRegister(), firstOperand, secondOperand);  break;
            case "DIV":     output.emitInstruction(Instruction.div.name(), getNextFreeRegister(), firstOperand, secondOperand);  break;
            case "EQU":     output.emitInstruction(Instruction.bne.name(), firstOperand, secondOperand, labelForJumbing);    break;
            case "NEQ":     output.emitInstruction(Instruction.beq.name(), firstOperand, secondOperand, labelForJumbing);    break;
            case "LST":     output.emitInstruction(Instruction.bge.name(), firstOperand, secondOperand, labelForJumbing);    break;
            case "LSE":     output.emitInstruction(Instruction.bgt.name(), firstOperand, secondOperand, labelForJumbing);    break;
            case "GRT":     output.emitInstruction(Instruction.ble.name(), firstOperand, secondOperand, labelForJumbing);    break;
            case "GRE":     output.emitInstruction(Instruction.blt.name(), firstOperand, secondOperand, labelForJumbing);    break;
            default:    break;
        }
    }

    public void visit(CallStatement callStatement) {
        Entry entry = null;
        if (table.getUpperLevel().isPresent())
            entry = table.getUpperLevel().get().lookup(callStatement.procedureName);
        if (entry instanceof ProcedureEntry){
            var parTypeIterator = ((ProcedureEntry) entry).parameterTypes.iterator();
            for (Expression arg : callStatement.argumentList) {
                var parType = parTypeIterator.next();
                if (parType.isReference){
                    ((VariableExpression) arg).variable.accept(this);
                } else {
                    arg.accept(this);
                }
                output.emitInstruction(Instruction.stw.name(), getLastRegister(), sp, parType.offset);
            }
        }
        output.emitInstruction(Instruction.jal.name(), callStatement.procedureName.toString());
    }

    public void visit(CompoundStatement compoundStatement) {
        compoundStatement.statements.forEach(stm -> stm.accept(this));
    }

    public void visit(IfStatement ifStatement) {
        String label1 = "L"+labelNumber++;
        labelForJumbing = label1;
        ifStatement.condition.accept(this);
        ifStatement.thenPart.accept(this);
        if (!(ifStatement.elsePart instanceof EmptyStatement)) {
            String label2 = "L"+labelNumber++;
            output.emitInstruction(Instruction.j.name(), label2);
            output.emitLabel(label1);
            ifStatement.elsePart.accept(this);
            output.emitLabel(label2);
        }
        else
            output.emitLabel(label1);
    }

    public void visit(WhileStatement whileStatement) {
        String label1 = "L"+labelNumber++;
        String label2 = "L"+labelNumber++;
        output.emitLabel(label1);
        labelForJumbing = label2;
        whileStatement.condition.accept(this);
        whileStatement.body.accept(this);
        output.emitInstruction(Instruction.j.name(), label1, "Wiederholen..");
        output.emitLabel(label2);
    }
    
    public void visit(IntLiteral intLiteral) {
        output.emitInstruction(Instruction.add.name(), getNextFreeRegister(),  nullReg, intLiteral.value);
    }

    public void visit(NamedVariable namedVariable) {
        Entry target = table.lookup(namedVariable.name);
        if (target instanceof VariableEntry){
            output.emitInstruction(Instruction.add.name(), getNextFreeRegister(), fp, ((VariableEntry) target).offset);
            if (((VariableEntry) target).isReference)
                output.emitInstruction(Instruction.ldw.name(), getLastRegister(), getNextFreeRegister(),0);
        }
    }
    
    public void visit(ProcedureDeclaration procedureDeclaration) {
        output.emit("");
        output.emitExport(procedureDeclaration.name.toString());
        output.emitLabel(procedureDeclaration.name.toString());
        var localTable = table.getUpperLevel();
        Entry entry = null;
        if (localTable.isPresent())
            entry = localTable.get().lookup(procedureDeklaration.name);
        if (entry instanceof ProcedureEntry) {
            int framesize = ((ProcedureEntry) entry).localVarAreaSize + 4;
            if (((ProcedureEntry) entry).outgoingAreaSize > -1)
                framesize += ((ProcedureEntry) entry).outgoingAreaSize + 4;
            int fpoffset = ((ProcedureEntry) entry).outgoingAreaSize > -1 ? ((ProcedureEntry) entry).outgoingAreaSize + 4 : 0;
            int fpAdress = ((ProcedureEntry) entry).localVarAreaSize + 8;
            output.emitInstruction(Instruction.sub.name(), sp, sp, framesize, "SP = SP - Framesize("+procedureDeclaration.name+")");
            output.emitInstruction(Instruction.stw.name(), fp, sp, fpoffset, "FP Alt speichern");
            output.emitInstruction(Instruction.add.name(), fp, sp, framesize, "FP New = SP New + Framesize("+procedureDeclaration.name+")");
            if (((ProcedureEntry) entry).outgoingAreaSize > -1)
                output.emitInstruction(Instruction.stw.name(), ra, fp, -fpAdress, "Return Address specihern");
            //procedureDeclaration.body.forEach(stm -> stm.accept(this));
            for (Statement stm : procedureDeclaration.body){
                stm.accept(this);
            }
            if (((ProcedureEntry) entry).outgoingAreaSize > -1)
                output.emitInstruction(Instruction.ldw.name(), ra, fp, -fpAdress, "Return restaurieren");
            output.emitInstruction(Instruction.ldw.name(), fp, sp, fpoffset, "FP Alt restaurieren");
            output.emitInstruction(Instruction.add.name(), sp, sp, framesize, "SP = SP + Framesize("+procedureDeclaration.name+")");
            output.emitInstruction(Instruction.jr.name(), ra, "Rücksprung zum Caller");
        }
    }

    public void visit(VariableExpression variableExpression) {
        variableExpression.variable.accept(this);
        output.emitInstruction(Instruction.ldw.name(), getLastRegister(), getNextFreeRegister(), 0);
    }
}
