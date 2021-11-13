package de.thm.mni.compilerbau.absyn.visitor;

import de.thm.mni.compilerbau.absyn.*;
import de.thm.mni.compilerbau.phases._05_varalloc.VarAllocator;
import de.thm.mni.compilerbau.table.*;


public class VarAllocVisitor extends DoNothingVisitor {
    private ProcedureEntry procEntry;
    private SymbolTable table;
    public boolean callStmFound = false;
    private int offset = 0;

    public VarAllocVisitor(SymbolTable _table, ProcedureEntry _procEntry){
        if (_table != null)
            this.table = _table;
        if (_procEntry != null)
            this.procEntry = _procEntry;
    }
    
    public void visit(ProcedureDeclaration procedureDeclaration) {
        var paramterIterrator = procedureDeclaration.parameters.iterator();
        var paramterTypeItterator = procEntry.parameterTypes.iterator();
        while(paramterIterrator.hasNext()){
            var parameter = paramterIterrator.next();
            var parameterType = paramterTypeItterator.next();
            var entry = procEntry.localTable.lookup(parameter.name);

            if (parameterType.isReference)
                procEntry.argumentAreaSize += VarAllocator.REFERENCE_BYTESIZE;
            else
                procEntry.argumentAreaSize += parameter.typeExpression.dataType.byteSize;

            if (entry instanceof VariableEntry){
                ((VariableEntry) entry).offset = offset;
                parameterType.offset = offset;
                if (parameterType.isReference)
                    offset += VarAllocator.REFERENCE_BYTESIZE;
                else
                    offset += ((VariableEntry) entry).type.byteSize;
            }
        }
        offset = 0;
        for (VariableDeclaration variable : procedureDeclaration.variables)
        {
            var entry = procEntry.localTable.lookup(variable.name);
            procEntry.localVarAreaSize += variable.typeExpression.dataType.byteSize;

            if (entry instanceof VariableEntry){
                offset -= ((VariableEntry) entry).type.byteSize;
                ((VariableEntry) entry).offset = offset;
            }
        }
    }

    /**
     * 1-(2+3)*4
     * add $8, $0, 1
     * add $9, $0, 2
     * add $10, $0, 3
     * add $9, $9, $10
     * add $10, 0, 4
     * mul $9, $9, $10
     * sub $8, $8, $9
     */

    /**
     * 2*3 + 4*5
     * add $8, $0, 2
     * add $9, $0, 3
     * mul $8, $8, $9
     * add $9, $0, 4
     * add $10, $0, 5
     * mul $9, $9, $10
     * add $8, $8, $9
     */
    
    public void visit(CallStatement callStatement) {
        callStmFound = true;
        Entry entry = table.lookup(callStatement.procedureName);
        if (entry instanceof ProcedureEntry) {
            if (((ProcedureEntry) entry).argumentAreaSize > procEntry.outgoingAreaSize)
                procEntry.outgoingAreaSize = ((ProcedureEntry) entry).argumentAreaSize;
        }
    }
    
    public void visit(CompoundStatement compoundStatement) {
        compoundStatement.statements.forEach(stm -> stm.accept(this));
    }
    
    public void visit(IfStatement ifStatement) {
        ifStatement.thenPart.accept(this);
        ifStatement.elsePart.accept(this);
    }

    public void visit(WhileStatement whileStatement) {
        whileStatement.body.accept(this);
    }
}