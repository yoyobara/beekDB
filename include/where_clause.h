#pragma once
#include <hsql/sql/Expr.h>

/*
 * handles select where clause
 */

using hsql::Expr;

Expr* expr_equals(Expr* a, Expr* b);
