#include "where_clause.h"
#include "exceptions.h"
#include "table/types.h"
#include <cstring>
#include <hsql/sql/Expr.h>

using namespace hsql;

void convert_to_values(Expr *a, Expr *b, const Record& record_ref)
{
	for (Expr* e : {a, b})
	{
		if (e->isType(kExprColumnRef))
		{
			switch (record_ref.of_table->get_column(e->name).get_type()) {
				case INTEGER:
					e->type = kExprLiteralInt;
					e->ival = record_ref.get<IntegerValue>(e->name).int_val;

				case REAL:
					e->type = kExprLiteralFloat;
					e->fval = record_ref.get<RealValue>(e->name).real_val;
case VARCHAR_50:
					e->type = kExprLiteralString;
					e->name = record_ref.get<VarChar50Value>(e->name).str_val.data(); // TODO
			}
		}
	}

}

Expr* expr_equals(Expr *a, Expr *b)
{
	if (a->isType(kExprLiteralInt) && b->isType(kExprLiteralInt))
		return Expr::makeLiteral(a->ival == b->ival);

	else if (a->isType(kExprLiteralFloat) && b->isType(kExprLiteralFloat))
		return Expr::makeLiteral(a->fval == b->fval);

	else if (a->isType(kExprLiteralString) && b->isType(kExprLiteralString))
		return Expr::makeLiteral((bool)strcmp(a->name, b->name));
    
	else
		throw not_implemented("checking equality here not implemented");
}

Expr* expr_plus(Expr* a, Expr* b)
{
    if (a->isType(kExprLiteralInt) && b->isType(hsql::kExprLiteralInt))
        return Expr::makeLiteral(a->ival + b->ival);

    else if (a->isType(kExprLiteralFloat) && b->isType(kExprLiteralFloat))
        return Expr::makeLiteral(a->fval + b->fval);

    else
        throw not_implemented("such addition is not implemented.");
}

Expr* expr_minus(Expr* a, Expr* b)
{
    if (a->isType(kExprLiteralInt) && b->isType(hsql::kExprLiteralInt))
        return Expr::makeLiteral(a->ival - b->ival);

    else if (a->isType(kExprLiteralFloat) && b->isType(kExprLiteralFloat))
        return Expr::makeLiteral(a->fval - b->fval);

    else
        throw not_implemented("such subtraction is not implemented.");
}

/*
 * perform the operation on two exprs of the same literal type.
 */
Expr* perform_operation(Expr* ex1, Expr* ex2, hsql::OperatorType optype) {
    switch (optype) {
        case hsql::kOpPlus:
            return expr_plus(ex1, ex2);

        case hsql::kOpMinus:
            return expr_minus(ex1, ex2);

        case hsql::kOpEquals: 
            return expr_equals(ex1, ex2);

        default:
            throw not_implemented("such operation is not implemented yet..");
    }
}

Expr* eval(Expr* expression, const Record& record_ref)
{
    switch (expression->type) {
        case hsql::kExprOperator: {
            Expr* ex1 = eval(expression->expr, record_ref);
            Expr* ex2 = eval(expression->expr2, record_ref);

            convert_to_values(ex1, ex2, record_ref);

            return perform_operation(ex1, ex2, expression->opType);
        }
            
        case hsql::kExprLiteralFloat:
        case hsql::kExprLiteralString:
        case hsql::kExprLiteralInt:
        case hsql::kExprColumnRef:
            return expression;

        default:
            throw not_implemented("such expression type is not implemented");
    }
}
