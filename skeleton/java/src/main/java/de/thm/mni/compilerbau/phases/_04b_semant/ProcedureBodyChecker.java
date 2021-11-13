package de.thm.mni.compilerbau.phases._04b_semant;

import de.thm.mni.compilerbau.absyn.*;
import de.thm.mni.compilerbau.absyn.visitor.TableBuilderVisitor;
import de.thm.mni.compilerbau.table.ProcedureEntry;
import de.thm.mni.compilerbau.table.SymbolTable;
import de.thm.mni.compilerbau.types.Type;

/**
 * This class is used to check if the currently compiled SPL program is semantically valid.
 * Every statement and expression has to be checked, to ensure that every type is correct.
 * <p>
 * Calculated {@link Type}s can be stored in and read from the dataType field of the {@link Expression},
 * {@link TypeExpression} or {@link Variable} classes.
 */
public class ProcedureBodyChecker {
    public void procedureCheck(Program program, SymbolTable globalTable) {
        //TODO (assignment 4b): Check all procedure bodies for semantic errors
        for (GlobalDeclaration declartaion : program.declarations)
        {
            if (!(declartaion instanceof ProcedureDeclaration))
                continue;

            SymbolTable localTable = ((ProcedureEntry) globalTable.lookup(declartaion.name)).localTable;
            TableBuilderVisitor visitor = new TableBuilderVisitor(localTable);
            for (Statement statement : ((ProcedureDeclaration) declartaion).body)
                statement.accept(visitor);
        }
    }
}
