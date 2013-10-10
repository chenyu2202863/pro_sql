#ifndef __PRO_SQL_TYPE_TRAITS_HPP
#define __PRO_SQL_TYPE_TRAITS_HPP

#include <type_traits>
#include <ctime>


namespace pro_sql {

	enum enum_field_types
	{
		MYSQL_TYPE_DECIMAL, MYSQL_TYPE_TINY,
		MYSQL_TYPE_SHORT, MYSQL_TYPE_LONG,
		MYSQL_TYPE_FLOAT, MYSQL_TYPE_DOUBLE,
		MYSQL_TYPE_NULL, MYSQL_TYPE_TIMESTAMP,
		MYSQL_TYPE_LONGLONG, MYSQL_TYPE_INT24,
		MYSQL_TYPE_DATE, MYSQL_TYPE_TIME,
		MYSQL_TYPE_DATETIME, MYSQL_TYPE_YEAR,
		MYSQL_TYPE_NEWDATE, MYSQL_TYPE_VARCHAR,
		MYSQL_TYPE_BIT,
		MYSQL_TYPE_TIMESTAMP2,
		MYSQL_TYPE_DATETIME2,
		MYSQL_TYPE_TIME2,
		MYSQL_TYPE_NEWDECIMAL = 246,
		MYSQL_TYPE_ENUM = 247,
		MYSQL_TYPE_SET = 248,
		MYSQL_TYPE_TINY_BLOB = 249,
		MYSQL_TYPE_MEDIUM_BLOB = 250,
		MYSQL_TYPE_LONG_BLOB = 251,
		MYSQL_TYPE_BLOB = 252,
		MYSQL_TYPE_VAR_STRING = 253,
		MYSQL_TYPE_STRING = 254,
		MYSQL_TYPE_GEOMETRY = 255

	};

	template < typename T >
	struct cpp_type_to_sql_type_t;

	template < >
	struct cpp_type_to_sql_type_t<bool>
	{
		static const enum_field_types value = MYSQL_TYPE_TINY;
	};

	template < >
	struct cpp_type_to_sql_type_t<std::int8_t>
	{
		static const enum_field_types value = MYSQL_TYPE_TINY;
	};

	template < >
	struct cpp_type_to_sql_type_t<std::uint8_t>
	{
		static const enum_field_types value = MYSQL_TYPE_TINY;
	};

	template < >
	struct cpp_type_to_sql_type_t<std::int16_t>
	{
		static const enum_field_types value = MYSQL_TYPE_SHORT;
	};

	template < >
	struct cpp_type_to_sql_type_t<std::uint16_t>
	{
		static const enum_field_types value = MYSQL_TYPE_SHORT;
	};
	
	template < >
	struct cpp_type_to_sql_type_t<std::int32_t>
	{
		static const enum_field_types value = MYSQL_TYPE_LONG;
	}; 

	template < >
	struct cpp_type_to_sql_type_t<std::uint32_t>
	{
		static const enum_field_types value = MYSQL_TYPE_LONG;
	};


	template < >
	struct cpp_type_to_sql_type_t<std::uint64_t>
	{
		static const enum_field_types value = MYSQL_TYPE_LONGLONG;
	};
	
	template < >
	struct cpp_type_to_sql_type_t<float>
	{
		static const enum_field_types value = MYSQL_TYPE_FLOAT;
	}; 

	template < >
	struct cpp_type_to_sql_type_t<double>
	{
		static const enum_field_types value = MYSQL_TYPE_DOUBLE;
	};


	template < >
	struct cpp_type_to_sql_type_t<std::time_t>
	{
		static const enum_field_types value = MYSQL_TYPE_TIMESTAMP;
	};
	
	template < >
	struct cpp_type_to_sql_type_t<std::string>
	{
		static const enum_field_types value = MYSQL_TYPE_VAR_STRING;
	};

	template < >
	struct cpp_type_to_sql_type_t<char *>
	{
		static const enum_field_types value = MYSQL_TYPE_VARCHAR;
	};


	// ------------------
	template < typename T, typename EnableT = void >
	struct buffer_size_t;

	template < typename T >
	struct buffer_size_t<T, typename std::enable_if<std::is_pod<T>::value>::type>
	{
		static std::uint32_t size(const T *)
		{
			return sizeof(T);
		}
	};

	template < >
	struct buffer_size_t<std::string>
	{
		static std::uint32_t size(const std::string *val)
		{
			if( val->empty() )
				const_cast<std::string *>(val)->resize(15);

			return val->size();
		}
	};


	// ------------------

	template < typename T, typename EnableT = void >
	struct buffer_address_t;

	template < typename T >
	struct buffer_address_t<T, typename std::enable_if<std::is_pod<T>::value>::type>
	{
		static void *address(const T *val)
		{
			return (void *) val;
		}
	};

	template < >
	struct buffer_address_t<std::string>
	{
		static void *address(const std::string *val)
		{
			return (void *) val->data();
		}
	};
}

#endif