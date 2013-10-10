//////////////////////////////////////////////////////////////////////////////
#include "binders.hpp"


namespace pro_sql {


int into_binder_t::bind(statement_t &st, std::uint32_t pos)
{
	do_bind(st, pos);
	return pos + 1;
}

void into_binder_t::update(statement_t &st)
{
	do_update(st);
}

std::uint32_t use_binder_t::bind(statement_t &st, std::uint32_t pos)
{
	do_bind(st, pos);
	return pos + 1;
}


} 