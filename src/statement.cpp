#include "statement.hpp"

#include <WinSock2.h>

#include <algorithm>
#include <numeric>
#include <vector>

#include <mysql.h>


#include "exception.hpp"
#include "session.hpp"
#include "traits.hpp"

namespace pro_sql {



	struct statement_t::impl
	{
		MYSQL_STMT *stmt_ = nullptr;
		bool is_prepared = false;

		session_t &session_;
		query_t query_;

		impl(session_t &session)
			: session_(session)
		{
			stmt_ = ::mysql_stmt_init(session.native_handle());
			if( stmt_ == nullptr )
				throw exception_t(session_);
		}

		~impl()
		{
			if( stmt_ != nullptr )
			{
				auto ret = ::mysql_stmt_close(stmt_);
				assert(!ret);
			}
		}
	};

	statement_t::statement_t(session_t &s)
		: impl_(std::make_unique<impl>(s))

	{}



	statement_t::~statement_t()
	{}

	MYSQL_STMT *statement_t::native_handle() const
	{
		return impl_->stmt_;
	}

	void statement_t::prepare()
	{
		assert(!impl_->is_prepared);

		if( impl_->is_prepared )
			throw std::runtime_error("statement was prepared!");

		auto sql = impl_->query_.sql();
		if( ::mysql_stmt_prepare(impl_->stmt_, sql.c_str(), sql.size()) != 0 )
			throw stmt_exception_t(*this);

		impl_->is_prepared = true;
	}

	void statement_t::execute()
	{
		assert(impl_->is_prepared);
		if( !impl_->is_prepared )
			throw std::runtime_error("statement was prepared!");

		if( ::mysql_stmt_execute(impl_->stmt_) != 0 ||
			::mysql_stmt_store_result(impl_->stmt_) != 0 )
			throw stmt_exception_t(*this);
	}


	std::uint64_t statement_t::result_count() const
	{
		return ::mysql_stmt_num_rows(impl_->stmt_);
	}

	const query_t &statement_t::query() const
	{
		return impl_->query_;
	}

	query_t &statement_t::query()
	{
		return impl_->query_;
	}

	void statement_t::set_query(query_t &&query)
	{
		impl_->query_ = std::move(query);
	}

	std::pair<statement_t::fetch_result_type, MYSQL_ROW_OFFSET> statement_t::_fetch_impl(details::use_param_t *param, std::uint32_t cnt)
	{
		auto offset = ::mysql_stmt_row_tell(impl_->stmt_);
		auto error = ::mysql_stmt_fetch(impl_->stmt_);
		if( error == MYSQL_DATA_TRUNCATED )
			return { fetch_result_type::resize, offset };
		else
			return { error == 0 ? fetch_result_type::ok : fetch_result_type::failed, nullptr };
	}

	void statement_t::_offset_impl(MYSQL_ROW_OFFSET offset)
	{
		auto ret = ::mysql_stmt_row_seek(impl_->stmt_, offset);
	}

	void statement_t::_bind_params_impl(const details::into_param_t *params, std::uint32_t cnt)
	{
		assert(::mysql_stmt_param_count(impl_->stmt_) == cnt);

		std::vector<MYSQL_BIND> bind_params(cnt);
		for( auto i = 0; i != cnt; ++i )
		{
			bind_params[i].buffer_type	= (::enum_field_types)params[i].buffer_type_;
			bind_params[i].buffer_length= params[i].buffer_length_;
			bind_params[i].buffer		= params[i].buffer_;
			bind_params[i].is_unsigned	= params[i].is_unsigned_;
		};

		if( ::mysql_stmt_bind_param(impl_->stmt_, bind_params.data()) != 0 )
			throw stmt_exception_t(*this);
	}

	void statement_t::_bind_results_impl(const details::use_param_t *params, std::uint32_t cnt)
	{
		assert(::mysql_stmt_field_count(impl_->stmt_) == cnt);

		std::vector<MYSQL_BIND> bind_params(cnt);
		for( auto i = 0; i != cnt; ++i )
		{
			bind_params[i].buffer_type	= (::enum_field_types)params[i].buffer_type_;
			bind_params[i].buffer_length= params[i].buffer_length_;
			bind_params[i].buffer		= params[i].buffer_;
			bind_params[i].is_unsigned	= params[i].is_unsigned_;
			bind_params[i].is_null		= (my_bool *)&(params[i].is_null_);
			bind_params[i].length		= const_cast<unsigned long *>(&params[i].length_);
		};

		if( ::mysql_stmt_bind_result(impl_->stmt_, bind_params.data()) != 0 )
			throw stmt_exception_t(*this);
	}

	std::uint64_t statement_t::last_insert_rowid() const
	{
		return ::mysql_stmt_insert_id(impl_->stmt_);
	}

}
