#include "SqliteWrap.h"
#include "RssManagerException.h"

SqliteWrap::~SqliteWrap(){}
SqliteWrap::SqliteWrap(const wstring& dbname)
{
	sqlite3* connPtr;
	int res = sqlite3_open16(dbname.c_str(), &connPtr);
	this->conn.reset(connPtr, sqlite3_close);
	if (res != SQLITE_OK)
	{
		throw  RssBdException(sqlite3_errstr(res));
	}
}
void SqliteWrap::GetQuery(const wstring& query)
{
	sqlite3_stmt *order;
	int res = sqlite3_prepare16(this->conn.get(), query.c_str(), query.size()*sizeof(wchar_t), &order, 0);
	if (res != SQLITE_OK)
	{
		sqlite3_finalize(order);
		throw  RssBdException(sqlite3_errstr(res));
	}
	
	res = sqlite3_step(order);
	if (res != SQLITE_DONE)
	{
		sqlite3_finalize(order);
		throw  RssBdException(sqlite3_errstr(res));
	}
	sqlite3_finalize(order);
}
void SqliteWrap::Reset()
{
	this->conn.reset();
}
vector<vector<wstring>> SqliteWrap::GetQueryResultWString(const wstring& query)
{
	sqlite3_stmt *order;
	int res = sqlite3_prepare16(this->conn.get(), query.c_str(), query.size()*sizeof(wchar_t), &order, 0);
	if (res != SQLITE_OK)
	{
		sqlite3_finalize(order);
		throw  RssBdException(sqlite3_errstr(res));
	}
	vector<vector<wstring>> result;

	while (sqlite3_step(order) == SQLITE_ROW)
	{
		vector<wstring>colums;
		int count = sqlite3_column_count(order);
		for (int i = 0; i < count; ++i)
		{
			wstring row = (wchar_t*)sqlite3_column_text16(order, i);
			colums.push_back(row);
		}
		result.push_back(colums);
	}
	if (res)
	{
		sqlite3_finalize(order);
		throw  RssBdException(sqlite3_errstr(res));
	}
	
	sqlite3_finalize(order);
	return result;
}