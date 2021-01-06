package de.thm.mni.compilerbau.phases._04b_semant;

import de.thm.mni.compilerbau.absyn.*;
import de.thm.mni.compilerbau.absyn.visitor.CheckerVisitor;
import de.thm.mni.compilerbau.absyn.visitor.DoNothingVisitor;
import de.thm.mni.compilerbau.absyn.visitor.TableBuilderVisitor;
import de.thm.mni.compilerbau.phases._04a_tablebuild.TableBuilder;
import de.thm.mni.compilerbau.table.ProcedureEntry;
import de.thm.mni.compilerbau.table.SymbolTable;
import de.thm.mni.compilerbau.table.VariableEntry;
import de.thm.mni.compilerbau.types.ArrayType;
import de.thm.mni.compilerbau.types.PrimitiveType;
import de.thm.mni.compilerbau.types.Type;
import de.thm.mni.compilerbau.utils.List;
import de.thm.mni.compilerbau.utils.NotImplemented;
import de.thm.mni.compilerbau.utils.SplError;

/**
 * This class is used to check if the currently compiled SPL program is semantically valid.
 * Every statement and expression has to be checked, to ensure that every type is correct.
 * <p>
 * Calculated {@link Type}s can be stored in and read from the dataType field of the {@link Expression},
 * {@link TypeExpression} or {@link Variable} classes.
 */
public class ProcedureBodyChecker {
    CheckerVisitor visitor;
    public void procedureCheck(Program program, SymbolTable globalTable) {
        //TODO (assignment 4b): Check all procedure bodies for semantic errors
        visitor = new CheckerVisitor();
        for (GlobalDeclaration deklartaion : program.declarations)
        {
            if (!(deklartaion instanceof ProcedureDeclaration))
                continue;
            SymbolTable localTable = ((ProcedureEntry) globalTable.lookup(deklartaion.name)).localTable;
            visitor.setLocalTable(localTable);
            for (Statement statement : ((ProcedureDeclaration) deklartaion).body)
            {
                statement.accept(visitor);
            }
        }
        //throw new NotImplemented();
    }


    protected void checkType(Type expected, Type actual, SplError error) throws SplError {
        // This method may be used to check types. It must be implemented before it can be used.
        // TODO: The implementation should compare the types and throw the given error if the types are not equal.

        //throw new NotImplemented();
    }
}
