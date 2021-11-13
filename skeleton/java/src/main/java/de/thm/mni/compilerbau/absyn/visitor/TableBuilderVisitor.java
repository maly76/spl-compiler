package de.thm.mni.compilerbau.absyn.visitor;

import de.thm.mni.compilerbau.absyn.*;
import de.thm.mni.compilerbau.table.*;
import de.thm.mni.compilerbau.types.ArrayType;
import de.thm.mni.compilerbau.types.PrimitiveType;
import de.thm.mni.compilerbau.types.Type;
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

    public TableBuilderVisitor(SymbolTable _table){
        if (_table != null)
            table = _table;
    }

    protected void checkType(Type expected, Type actual, SplError error) throws SplError {
        if (!expected.equals(actual))   throw error;
    }

    public void visit(ArrayAccess arrayAccess) {
        arrayAccess.array.accept(this);
        arrayAccess.index.accept(this);

        if (!(arrayAccess.array.dataType instanceof ArrayType))
            throw SplError.IndexingNonArray(arrayAccess.array.position);

        if (!arrayAccess.index.dataType.equals(PrimitiveType.intType))
            throw SplError.IndexingWithNonInteger(arrayAccess.index.position);
        arrayAccess.dataType = ((ArrayType) arrayAccess.array.dataType).baseType;
    }

    public void visit(AssignStatement assignStatement) {
        assignStatement.target.accept(this);
        assignStatement.value.accept(this);
        checkType(assignStatement.target.dataType, assignStatement.value.dataType,
                SplError.AssignmentHasDifferentTypes(assignStatement.position));
    }

    public void visit(BinaryExpression binaryExpression) {
        binaryExpression.leftOperand.accept(this);
        binaryExpression.rightOperand.accept(this);

        if (!(binaryExpression.leftOperand.dataType.equals(PrimitiveType.intType) &&
                binaryExpression.rightOperand.dataType.equals(PrimitiveType.intType)))
            throw SplError.ArithmeticOperatorNonInteger(binaryExpression.position);

        if (binaryExpression.operator.isComparison() &&
                !(binaryExpression.leftOperand.dataType.equals(PrimitiveType.intType)))
            throw SplError.ComparisonNonInteger(binaryExpression.leftOperand.position);

        if (binaryExpression.operator.isComparison() &&
                !(binaryExpression.rightOperand.dataType.equals(PrimitiveType.intType)))
            throw SplError.ComparisonNonInteger(binaryExpression.rightOperand.position);

        if (binaryExpression.operator.isArithmetic() &&
                !(binaryExpression.leftOperand.dataType.equals(PrimitiveType.intType)))
            throw SplError.OperatorDifferentTypes(binaryExpression.leftOperand.position);

        if (binaryExpression.operator.isArithmetic() &&
                !(binaryExpression.rightOperand.dataType.equals(PrimitiveType.intType)))
            throw SplError.OperatorDifferentTypes(binaryExpression.rightOperand.position);

        if (binaryExpression.operator.isComparison())
            binaryExpression.dataType = PrimitiveType.boolType;
        else if (binaryExpression.operator.isArithmetic())
            binaryExpression.dataType = PrimitiveType.intType;
    }

    public void visit(CallStatement callStatement) {
        Entry entry = table.lookup(callStatement.procedureName, SplError.UndefinedProcedure(callStatement.position, callStatement.procedureName));
        // check if the given name is a procedure
        if (!(entry instanceof ProcedureEntry))
            throw SplError.CallOfNonProcedure(callStatement.position, callStatement.procedureName);
        // check if the called procedure has too many or to few arguments
        if (callStatement.argumentList.size() > ((ProcedureEntry) entry).parameterTypes.size())
            throw SplError.TooManyArguments(callStatement.position, callStatement.procedureName);
        else if (callStatement.argumentList.size() < ((ProcedureEntry) entry).parameterTypes.size())
            throw SplError.TooFewArguments(callStatement.position, callStatement.procedureName);

        callStatement.argumentList.forEach(argument -> argument.accept(this));

        for (int i = 0; i < callStatement.argumentList.size(); i++)
        {
            if (((ProcedureEntry) entry).parameterTypes.get(i).isReference &&
                    !(callStatement.argumentList.get(i) instanceof VariableExpression))
                throw SplError.ArgumentMustBeAVariable(callStatement.argumentList.get(i).position,
                        callStatement.procedureName, i);

            if (callStatement.argumentList.get(i).dataType instanceof ArrayType
                    && !((ProcedureEntry) entry).parameterTypes.get(i).isReference)
                throw SplError.MustBeAReferenceParameter(callStatement.argumentList.get(i).position,
                        callStatement.procedureName);

            checkType(((ProcedureEntry) entry).parameterTypes.get(i).type,
                    callStatement.argumentList.get(i).dataType,
                    SplError.ArgumentTypeMismatch(callStatement.argumentList.get(i).position,
                            callStatement.procedureName, i));
        }
    }

    public void visit(CompoundStatement compoundStatement) {
        compoundStatement.statements.forEach(statement -> statement.accept(this));
    }

    public void visit(IfStatement ifStatement) {
        ifStatement.condition.accept(this);
        if (!(ifStatement.condition.dataType.equals(PrimitiveType.boolType)))
            throw SplError.IfConditionMustBeBoolean(ifStatement.condition.position);
        ifStatement.thenPart.accept(this);
        ifStatement.elsePart.accept(this);
    }

    public void visit(IntLiteral intLiteral) {
        intLiteral.dataType = PrimitiveType.intType;
    }

    public void visit(NamedVariable namedVariable) {
        Entry entry = table.lookup(namedVariable.name);

        if (entry == null) // check if the variable exists
            throw SplError.UndefinedVariable(namedVariable.position, namedVariable.name);

        if (entry instanceof VariableEntry)  // check if the result is a variable
            namedVariable.dataType = ((VariableEntry) entry).type;
        else
            throw SplError.NotAVariable(namedVariable.position, namedVariable.name);
        // für Arrays auch noch was
    }

    public void visit(VariableExpression variableExpression) {
        variableExpression.variable.accept(this);
        variableExpression.dataType = variableExpression.variable.dataType;
    }

    public void visit(WhileStatement whileStatement) {
        whileStatement.condition.accept(this);

        if (!whileStatement.condition.dataType.equals(PrimitiveType.boolType))
            throw SplError.WhileConditionMustBeBoolean(whileStatement.condition.position);

        whileStatement.body.accept(this);
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