#include "session.hpp"
#include "exception.hpp"

#include <WinSock2.h>
#include <mysql.h>

#pragma comment(lib, "Ws2_32")
#pragma comment(lib, "Advapi32")  
#pragma comment(lib, "User32")


namespace pro_sql {


	namespace details 
	{
		std::string last_error_msg(MYSQL *impl)
		{
			assert(impl);

			if( impl )
			{
				return ::mysql_error(impl);
			}

			return "";
		}

		
	}

	struct session_t::impl
	{
		MYSQL *conn_ = nullptr;
	};

session_t::session_t()
	: impl_(std::make_unique<impl>())
{
}

session_t::session_t(const std::string &host, 
	std::uint16_t port, 
	const std::string &db_name,
	const std::string &user_name, 
	const std::string &password, 
	std::uint32_t client_flag /* = 0 */)
	: impl_(std::make_unique<impl>())
{
	open(host, port, db_name, user_name, password, client_flag);
}

session_t::~session_t()
{
	try
	{
		close();
	}
	catch (...)
	{
	}
}

void session_t::open(const std::string &host, 
	std::uint16_t port, 
	const std::string &db_name,
	const std::string &user_name, 
	const std::string &password, 
	std::uint32_t client_flag /* = 0 */)
{
	assert(!host.empty());
	assert(!user_name.empty());
	assert(!password.empty());

	close();

	impl_->conn_ = ::mysql_init(nullptr);
	if( ::mysql_real_connect(impl_->conn_,
		host.c_str(),
		user_name.c_str(),
		password.c_str(),
		db_name.c_str(),
		port,
		nullptr,
		CLIENT_FOUND_ROWS | CLIENT_MULTI_RESULTS | CLIENT_MULTI_STATEMENTS) == nullptr )
	{
		throw exception_t(*this);
	}
}

void session_t::close()
{
	if ( is_open() )
	{
		::mysql_close(impl_->conn_);
		impl_->conn_ = nullptr;
	}
}

bool session_t::is_open() const
{
	return impl_->conn_ != nullptr;
}

MYSQL *session_t::native_handle() const
{
	assert(is_open());
	return impl_->conn_;
}


std::uint64_t session_t::last_insert_rowid() const
{
	return is_open() ? ::mysql_insert_id(impl_->conn_) : 0;
}


}