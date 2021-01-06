package de.thm.mni.compilerbau.absyn.visitor;

import com.sun.jdi.connect.Connector;
import de.thm.mni.compilerbau.absyn.*;
import de.thm.mni.compilerbau.table.*;
import de.thm.mni.compilerbau.types.ArrayType;
import de.thm.mni.compilerbau.types.PrimitiveType;
import de.thm.mni.compilerbau.types.Type;
import de.thm.mni.compilerbau.utils.SplError;

import java.lang.reflect.Parameter;

public class CheckerVisitor extends DoNothingVisitor {
    private SymbolTable table;

    public void setLocalTable(SymbolTable globaltable) {
        table = globaltable;
    }

    protected void checkType(Type expected, Type actual, SplError error) throws SplError {
        // This method may be used to check types. It must be implemented before it can be used.
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

    
    public void visit(ArrayTypeExpression arrayTypeExpression) {
    }

    
    public void visit(AssignStatement assignStatement) {

        assignStatement.target.accept(this);
        assignStatement.value.accept(this);
        checkType(assignStatement.target.dataType, assignStatement.value.dataType,
                SplError.AssignmentHasDifferentTypes(assignStatement.position));

            //throw SplError.NotAVariable(assignStatement.position, assignStatement.);
        // check if the types are equal
        //if (assignStatement.target.dataType == assignStatement.value.dataType)

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

            checkType(((ProcedureEntry) entry).parameterTypes.get(i).type,
                    callStatement.argumentList.get(i).dataType,
                    SplError.ArgumentTypeMismatch(callStatement.argumentList.get(i).position,
                                                    callStatement.procedureName, i));
        }
    }

    
    public void visit(CompoundStatement compoundStatement) {
        compoundStatement.statements.forEach(statement -> statement.accept(this));
    }

    
    public void visit(EmptyStatement emptyStatement) {
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

    
    public void visit(NamedTypeExpression namedTypeExpression) {
    }

    
    public void visit(NamedVariable namedVariable) {
        Entry entry = table.lookup(namedVariable.name);
        if (entry == null) // check if the variable exists
            throw SplError.UndefinedVariable(namedVariable.position, namedVariable.name);
        if (entry instanceof VariableEntry)
            namedVariable.dataType = ((VariableEntry) entry).type;
        // für Arrays auch noch was
    }

    
    public void visit(ParameterDeclaration parameterDeclaration) {
    }

    
    public void visit(ProcedureDeclaration procedureDeclaration) {
    }

    
    public void visit(Program program) {
    }

    
    public void visit(TypeDeclaration typeDeclaration) {
    }

    
    public void visit(VariableDeclaration variableDeclaration) {
    }

    
    public void visit(VariableExpression variableExpression) {
        variableExpression.variable.accept(this);
        variableExpression.dataType = variableExpression.variable.dataType;
    }

    
    public void visit(WhileStatement whileStatement) {
    }
}
