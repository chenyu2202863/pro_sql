#ifndef __PRO_SQL_QUERY_HPP
#define __PRO_SQL_QUERY_HPP

#include <sstream>
#include <tuple>

#include "config.hpp"
#include "into.hpp"
#include "use.hpp"

namespace pro_sql {

	class session_t;
	class statement_t;


	class query_t
	{
		std::ostringstream sql_;

	public:
		query_t() = default;
		explicit query_t(const std::string &sql);
		query_t(query_t &&);
		query_t &operator=(query_t &&);
		~query_t();


		query_t(const query_t &) = delete;
		query_t& operator=(const query_t &) = delete;


		std::string sql() const;
		void sql(const std::string &);

		void clear();
		bool empty() const;

		template < typename T >
		query_t &operator<<(const T &t)
		{
			sql_ << t;
			return *this;
		}
	};


	class prepare_query_t
		: public query_t
	{
		statement_t *statement_ = nullptr;

	public:
		prepare_query_t(statement_t &);
		prepare_query_t(prepare_query_t &&);
		~prepare_query_t();

		prepare_query_t(const prepare_query_t &) = delete;
		prepare_query_t& operator=(const prepare_query_t &) = delete;

		template < typename ...Args >
		prepare_query_t &operator,(const into_params_t<Args...> &param)
		{
			statement_->bind_params(param);
			return *this;
		}
	};


	class once_query_t
		: public query_t
	{
		session_t *session_ = nullptr;

	public:
		once_query_t(session_t &);
		once_query_t(once_query_t &&);
		~once_query_t();

		once_query_t(const once_query_t &) = delete;
		once_query_t &operator=(const once_query_t&) = delete;
	};


} 

#endif