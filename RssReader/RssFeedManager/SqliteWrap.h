#include "stdafx.h"
using namespace std;

class SqliteWrap
{
public:
	SqliteWrap(const wstring& dbname);
	~SqliteWrap();
	void GetQuery(const wstring& query);
	vector<vector<wstring>> GetQueryResultWString(const wstring& query);
	void Reset();
private:
	shared_ptr<sqlite3> conn;
};