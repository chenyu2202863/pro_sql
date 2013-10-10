#ifndef __PRO_SQL_STATEMENT_HPP
#define __PRO_SQL_STATEMENT_HPP

#include "config.hpp"
#include "query.hpp"

typedef struct st_mysql_stmt MYSQL_STMT;
typedef struct st_mysql_rows MYSQL_ROWS;
typedef MYSQL_ROWS *MYSQL_ROW_OFFSET;

namespace pro_sql {

	class session_t;

	class statement_t
	{
		struct impl;
		std::unique_ptr<impl> impl_;

	public:
		explicit statement_t(session_t &);
		~statement_t();

		statement_t(const statement_t &) = delete;
		statement_t& operator=(const statement_t &) = delete;

	public:
		MYSQL_STMT *native_handle() const;

		void execute();
		void prepare();

		template < typename ...Args >
		bool fetch(use_params_t<Args...> &use_param);

		std::uint64_t result_count() const;

		template < typename T >
		prepare_query_t operator<<(const T &t);

		const query_t &query() const;
		query_t &query();
		void set_query(query_t &&);

		std::uint64_t last_insert_rowid() const;

		template < typename ...Args >
		void bind_params(const into_params_t<Args...> &into_param);

		template < typename ...Args >
		void bind_results(const use_params_t<Args...> &use_param);

	private:
		enum class fetch_result_type
		{
			ok, failed, resize
		};
		std::pair<fetch_result_type, MYSQL_ROW_OFFSET> _fetch_impl(details::use_param_t *, std::uint32_t);
		void _offset_impl(MYSQL_ROW_OFFSET);

		void _bind_params_impl(const details::into_param_t *, std::uint32_t);
		void _bind_results_impl(const details::use_param_t *, std::uint32_t);
	};

	template < typename ...Args >
	bool statement_t::fetch(use_params_t<Args...> &use_param)
	{
		auto &buffers = use_param.buffers();
		auto ret = _fetch_impl(buffers.data(), buffers.size());
		if( ret.first == fetch_result_type::resize )
		{
			use_param.resize();

			bind_results(use_param);
			_offset_impl(ret.second);

			return _fetch_impl(buffers.data(), buffers.size()).first == fetch_result_type::ok;
		}

		bool suc = ret.first == fetch_result_type::ok;
		if( suc )
		{
			use_param.finaly();
		}

		return suc;
	}

	template < typename T >
	prepare_query_t statement_t::operator<<(const T &t)
	{
		prepare_query_t pq(*this);
		pq << t;
		return pq;
	}

	template < typename ...Args >
	void statement_t::bind_params(const into_params_t<Args...> &into_param)
	{
		const auto &buffers = into_param.buffers();
		_bind_params_impl(buffers.data(), buffers.size());
	}

	template < typename ...Args >
	void statement_t::bind_results(const use_params_t<Args...> &use_param)
	{
		const auto &buffers = use_param.buffers();
		_bind_results_impl(buffers.data(), buffers.size());
	}
}


#endif