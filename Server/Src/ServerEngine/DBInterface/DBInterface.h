#pragma once
#include <string>

class CDBStoredProcedure;

struct IDBInterface
{
	virtual bool Execute(std::string sql) = 0;
	virtual bool Query(std::string sql) = 0;
	virtual bool Execute(CDBStoredProcedure* pSpc) = 0;
	virtual bool Query(CDBStoredProcedure* pSpc) = 0;
};

