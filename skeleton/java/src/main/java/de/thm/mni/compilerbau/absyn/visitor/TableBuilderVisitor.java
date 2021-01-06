package de.thm.mni.compilerbau.absyn.visitor;

import de.thm.mni.compilerbau.absyn.*;
import de.thm.mni.compilerbau.table.*;
import de.thm.mni.compilerbau.types.ArrayType;
import de.thm.mni.compilerbau.utils.SplError;

import java.util.ArrayList;

/**
 * This implementation of a {@link Visitor} implements every visit-method but does nothing in every case.
 * <p>
 * It can be used to hide unneeded implementations in a visitor.
 * If for example a visitor would only need a visit-method for the {@link Program}-node, the java code would not compile
 * until every other method is implemented. When the {@link DoNothingVisitor} is extended in this case, all other methods
 * are implemented in this superclass and thus no compile-errors are produced.
 */
public class TableBuilderVisitor extends DoNothingVisitor{
    private SymbolTable table;
    public SymbolTable getTable() {
        return table;
    }
    public void setTable(SymbolTable globalTable){
        table = globalTable;
    }

    public void visit(ArrayAccess arrayAccess) {
    }

    public void visit(ArrayTypeExpression arrayTypeExpression) {
        arrayTypeExpression.baseType.accept(this);
        arrayTypeExpression.dataType = new ArrayType(arrayTypeExpression.baseType.dataType, arrayTypeExpression.arraySize);
    }

    public void visit(ProcedureDeclaration procedureDeclaration) {
        if (table.lookup(procedureDeclaration.name) != null)    // check if the procedure already exists
            throw SplError.RedeclarationAsProcedure(procedureDeclaration.position, procedureDeclaration.name);

        ArrayList<ParameterType> parametertypes = new ArrayList<>();
        SymbolTable localtable = new SymbolTable(table);

         //  put the parameters in the localtable
        for (ParameterDeclaration parameter : procedureDeclaration.parameters)
        {
            // check if the parameter deklaration exists as a variable
            if (localtable.lookup(parameter.name) != null)
                throw SplError.RedeclarationAsParameter(parameter.position, parameter.name);

            parameter.typeExpression.accept(this);
            parametertypes.add(new ParameterType(parameter.typeExpression.dataType, parameter.isReference));
            localtable.enter(new VariableEntry(parameter.name, parameter.typeExpression.dataType, parameter.isReference));
        }

         //  put the variables in the localtable
        for (VariableDeclaration variable : procedureDeclaration.variables)
        {
            // check if the variable exists in the function scope and the global scope
            if (localtable.lookup(variable.name) != null && !(localtable.lookup(variable.name) instanceof TypeEntry))
                throw SplError.RedeclarationAsVariable(variable.position, variable.name);

            variable.typeExpression.accept(this);
            localtable.enter(new VariableEntry(variable.name, variable.typeExpression.dataType, false));
        }
        table.enter(new ProcedureEntry(procedureDeclaration.name, localtable, parametertypes),
                SplError.RedeclarationAsProcedure(procedureDeclaration.position, procedureDeclaration.name));
    }


    public void visit(Program program) {
        for (GlobalDeclaration deklaration : program.declarations)
            deklaration.accept(this);
        Entry main = table.lookup(new Identifier("main"));

        if (main == null)
            throw  SplError.MainIsMissing();

        if (!(main instanceof ProcedureEntry))
            throw SplError.MainIsNotAProcedure();

        if (!(((ProcedureEntry) main).parameterTypes.isEmpty()))
            throw SplError.MainMustNotHaveParameters();

    }

    public void visit(TypeDeclaration typeDeclaration) {
        // check if the type deklaration exists
        typeDeclaration.typeExpression.accept(this);
        table.enter(new TypeEntry(typeDeclaration.name, typeDeclaration.typeExpression.dataType),
                SplError.RedeclarationAsType(typeDeclaration.position, typeDeclaration.name));
    }

    public void visit(NamedTypeExpression namedTypeExpression) {
        Entry entry = table.lookup(namedTypeExpression.name,
                SplError.UndefinedType(namedTypeExpression.position, namedTypeExpression.name));
        if (entry instanceof TypeEntry)
            namedTypeExpression.dataType = ((TypeEntry) entry).type;
        else
            throw SplError.NotAType(namedTypeExpression.position, namedTypeExpression.name);
    }
}