/*
 * table.h -- symbol table
 */


#ifndef _TABLE_H_
#define _TABLE_H_

#include <types/types.h>
#include <table/identifier.h>

/**
 * Used to differentiate between entry kinds.
 */
typedef enum {
    ENTRY_KIND_TYPE,
    ENTRY_KIND_VAR,
    ENTRY_KIND_PROC,
} entry_kind;

/**
 * Represents a table entry for a declaration in SPL.
 *
 * There are three kinds of entries, that can be stored in a table.
 *
 * 1. Type Entries, that represent a type-declaration and store a type
 *
 * 2. Variable Entries, that represent a variable-declaration.
 * There are no separate Entries for parameters and local Variables, as they behave identically.
 * A Variable Entry stores a type and whether the variable is a reference.
 *
 * 3. Procedure Entries, that represent a procedure-declaration.
 * A Procedure Entry stores a list of types for the procedures parameters and the procedures local table.
 *
 */
typedef struct {
    entry_kind kind;
    Identifier *name;
    union {
        struct {
            Type *type;
        } typeEntry;
        struct {
            Type *type;
            bool isRef;
            int offset;        /* filled in by variable allocator */
        } varEntry;
        struct {
            ParamTypes *paramTypes;
            struct table *localTable;
            int argumentArea;        /* filled in by variable allocator */
            int localvarArea;        /* filled in by variable allocator */
            int outgoingArea;        /* filled in by variable allocator */
        } procEntry;
    } u;
} Entry;

typedef struct bintree {
    Identifier *sym;
    unsigned key;
    Entry *entry;
    struct bintree *left;
    struct bintree *right;
} Bintree;

/**
 * Represents a symbol table for a definition scope in SPL.
 * Every table maps identifiers to the corresponding symbols.
 */
typedef struct table {
    Bintree *bintree;
    struct table *upperLevel;
} SymbolTable;

/**
 * Creates a new Type Entry representing a type-declaration in SPL.
 * @param name The name of this type entry
 * @param type The "meaning" of the type-declaration.
 * It is determined by the type of the type expression on the right hand side of the declaration.
 * @return The reference to a newly created Entry in memory.
 */
Entry *newTypeEntry(Identifier *name, Type *type);
/**
 * Creates a new Variable Entry representing a variable-declaration in SPL.
 * @param name The name of this variable entry
 * @param type The semantic type of the variable. It is determined through the type of the declaration.
 * @param isRef Whether or not the variable is declared as a reference.
 * @return The reference to a newly created Entry in memory.
 */
Entry *newVarEntry(Identifier *name, Type *type, bool isRef);
/**
 * Creates a new Procedure Entry representing a procedure-declaration in SPL.
 * @param name The name of this procedure entry
 * @param paramTypes A list of types representing the procedures parameters.
 * @param localTable The local table for the declared procedure.
 * @return The reference to a newly created Entry in memory.
 */
Entry *newProcEntry(Identifier *name, ParamTypes *paramTypes, SymbolTable *localTable);
/**
 * This function is required to enter additional information about the predefined procedures in phase 5.
 * For every procedures declared by the user, this information can be calculated during semantic analysis.
 * Since there is no analysis for predefined procedures, this information has to be entered manually.
 * @param paramTypes A list of types for the procedures parameters.
 * @param argumentAreaSize The size in bytes required on the stack to hold all of this procedures parameters.
 * @return The reference to a newly created Entry for a predefined procedure.
 */
Entry *newPredefinedProcEntry(Identifier *name, ParamTypes *paramTypes, int argumentAreaSize);

/**
 * Constructs a local table representing a local definition scope.
 * @param upperLevel The symbol table for the surrounding scope.
 * @return The reference to a newly constructed table in memory.
 */
SymbolTable *newTable(SymbolTable *upperLevel);
/**
 * Creates a new table and enters entries for all predefined types and procedures.
 * @return The reference to the symbol table representing the global definition scope.
 */
SymbolTable *initializeGlobalTable(void);

/**
 * Inserts a new symbol into the table.
 * Returns NULL if a symbol with this name already exists in this scope.
 * @param table The table, where the symbol should be added.
 * @param sym The symbol to be added.
 * @param entry The entry for the symbol.
 * @return The given entry or NULL if a symbol with the same name already exists in this scope.
 */
Entry *enter(SymbolTable *table, Entry *entry);
/**
 * Looks for the symbol defined with the given name.
 * Recursively looks in outer scopes if the name is not defined in this scope.
 * @param table The table representing the current scope.
 * @param name The symbol to look for.
 * @return NULL if the symbol wasn't found, the corresponding entry otherwise.
 */
Entry *lookup(SymbolTable *table, Identifier *name);

void showEntry(Entry *entry);
/**
 * Converts a table into a human readable format and prints it.
 * @param table The table to print.
 */
void showTable(SymbolTable *table);


/**
 * Returns an empty list of parameter types.
 */
ParamTypes *emptyParamTypes(void);
/**
 * Prepends a parameter to a parameter type list.
 * @param type The type of the parameter.
 * @param isRef Whether the parameter is a reference parameter.
 * @param next The tail of the parameter list.
 * @return A reference to the parameter list with the new parameter prepended.
 */
ParamTypes *newParamTypes(Type *type, bool isRef, ParamTypes *next);

/**
 * Prints a list of parameter types in a human readable format.
 * @param paramTypes The parameter type list to print.
 */
void showParamTypes(ParamTypes *paramTypes);


/**
 * This function is required to prepends a parameter to a parameter type list of a predefined procedure in phase 5.
 * For every procedure declared by the user, the extra information given to this function can be calculated during
 * semantic analysis. Since there is no analysis for predefined procedures, this information has to be entered manually.
 * @param type The type of the parameter.
 * @param isRef Whether the parameter is a reference parameter.
 * @param offset The parameters offset in the stack frame.
 * @param next The tail of the parameter list.
 * @return A reference to the parameter list of a predefined procedure with the new parameter prepended
 */
ParamTypes *newPredefinedParamTypes(Type *type, bool isRef, int offset, ParamTypes *next);

#endif /* _TABLE_H_ */
