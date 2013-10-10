#include "query.hpp"

#include <WinSock2.h>
#include <algorithm>
#include <stdexcept>

#include <mysql.h>

#include "exception.hpp"
#include "statement.hpp"
#include "session.hpp"

namespace pro_sql {



	query_t::query_t(const std::string &sql)
	{
		sql_ << sql;
	}

	query_t::query_t(query_t &&rhs)
		: sql_(std::move(rhs.sql_))
	{

	}

	query_t &query_t::operator=(query_t &&rhs)
	{
		if( this != &rhs )
		{
			sql_	= std::move(rhs.sql_);
		}

		return *this;
	}

	query_t::~query_t()
	{
		clear();
	}

	std::string query_t::sql() const
	{
		return sql_.str();
	}

	void query_t::sql(const std::string &text)
	{
		sql_.str(text);
		sql_.seekp(0, std::ios_base::end).clear();
	}

	void query_t::clear()
	{
		sql_.str("");
	}

	bool query_t::empty() const
	{
		return sql_.str().empty();
	}
	

	prepare_query_t::prepare_query_t(statement_t &st)
		: statement_(&st)
	{}
	
	prepare_query_t::prepare_query_t(prepare_query_t &&rhs)
		: query_t(std::move(rhs))
		, statement_(std::move(rhs.statement_))
	{
		rhs.statement_ = nullptr;
	}
	
	prepare_query_t::~prepare_query_t()
	{
		if( statement_ )
		{
			statement_->set_query(std::move(*this));
		}
	}

	once_query_t::once_query_t(once_query_t &&rhs)
		: query_t(std::move(rhs))
		, session_(std::move(rhs.session_))
	{
		rhs.session_ = nullptr;
	}

	once_query_t::once_query_t(session_t &session)
		: session_(&session)
	{}

	once_query_t::~once_query_t()
	{
		if( session_ )
		{
			if( !session_->is_open() )
				throw std::runtime_error("session not open");

			const std::string &sql_text = sql();
			auto ret = ::mysql_real_query(session_->native_handle(), sql_text.c_str(), sql_text.size());
			if( ret != 0 )
				throw exception_t(*session_);
		}
	}

}