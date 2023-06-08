#pragma once
#include "exceptions.h"
#include <table/table.h>
#include <hsql/sql/Expr.h>

/*
 * handles select where clause
 */

using hsql::Expr;

void convert_to_values(Expr* a, Expr* b, const Record& record_ref);
Expr* expr_equals(Expr* a, Expr* b);
Expr* expr_plus(Expr* a, Expr* b);
Expr* expr_minus(Expr* a, Expr* b);

Expr* perform_operation(Expr* ex1, Expr* ex2, hsql::OperatorType optype);

Expr* eval(Expr* expression);
