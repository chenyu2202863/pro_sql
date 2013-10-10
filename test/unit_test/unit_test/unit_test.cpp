// unit_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <pro_sql\session.hpp>
#include <pro_sql\statement.hpp>

void test_session()
{
	pro_sql::session_t session("localhost", 3306, "test", "root", "2202863");
	assert(session.is_open());

	session.open("localhost", 3306, "test", "root", "2202863");
	assert(session.is_open());
}

void test_once_query()
{
	pro_sql::session_t session("localhost", 3306, "test", "root", "2202863");
	assert(session.is_open());

	session << "insert into person(name, age, number) values('chenyu', 27, 10.98);";
	auto id = session.last_insert_rowid();
}

void test_stmt()
{
	pro_sql::session_t session("localhost", 3306, "test", "root", "2202863");
	assert(session.is_open());

	{
		pro_sql::statement_t st(session);
		st << "delete from person where id = ?";
		st.prepare();

		auto params = pro_sql::into(21);
		st.bind_params(params);
		//st.execute();
	}

	{
		pro_sql::statement_t st(session);
		st << "delete from person where number = ? and age = ?";
		st.prepare();

		auto params = pro_sql::into(10.98, 27);
		st.bind_params(params);
		//st.execute();
	}

	{
		pro_sql::statement_t st(session);
		st << "select * from person";
		st.prepare();

		st.execute();

		std::uint32_t id = 0;
		std::string name;
		std::uint32_t age = 0;
		double number = 0.0;

		auto params = pro_sql::use(id, name, age, number);
		st.bind_results(params);
		auto cnt = st.result_count();

		while( st.fetch(params) )
		{
			std::cout << id << " : "
				<< name << " "
				<< age << " "
				<< number << std::endl;
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		test_session();
		test_once_query();
		test_stmt();
	}
	catch( const std::exception &e )
	{
		std::cerr << e.what() << std::endl;
	}

	system("pause");
	return 0;
}

