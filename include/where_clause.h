#pragma once
#include <hsql/sql/Expr.h>

/*
 * handles select where clause
 */

using hsql::Expr;

Expr* expr_equals(Expr* a, Expr* b);
Expr* expr_plus(Expr* a, Expr* b);
Expr* expr_minus(Expr* a, Expr* b);
Expr* expr_mul(Expr* a, Expr* b);
