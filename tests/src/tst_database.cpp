#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>

#include <iostream>
#include "DataBase.h"
#include "tst_database.h"

TstDataBase::TstDataBase()
{
}


void TstDataBase::Query()
{
    DataBase db;

    bool ret = db.Open("test.sqlite");
    std::int32_t expected[2][2] = {
        { 1, 2 },
        { 5, 4 }
    };

    if (ret)
    {
        db.Query("DROP TABLE a;");
        db.Query("CREATE TABLE a (a INTEGER, b INTEGER);");
        db.Query("INSERT INTO a VALUES(1, 2);");
        db.Query("INSERT INTO a VALUES(5, 4);");

        std::vector<std::vector<Value> > result = db.Query("SELECT a, b FROM a;");

        size_t expected_val = 2;
        QCOMPARE(result.size(), expected_val);

        int line = 0U;
        for(std::vector<std::vector<Value> >::iterator it = result.begin(); it < result.end(); ++it)
        {
            std::vector<Value> row = *it;
            if (row.size() == 2)
            {
                std::cout << "Values: ( ";
                for (int col = 0; col < 2; col++)
                {
                    Value val = row.at(col);
                    if (val.IsValid())
                    {
                        if (val.GetType() == Value::INTEGER)
                        {
                            QCOMPARE(val.GetInteger(), expected[line][col]);
                            std::cout << val.GetInteger() << " ";
                        }
                        else
                        {
                            QFAIL("Not an integer");
                        }
                    }
                    else
                    {
                        QFAIL("Invalid value");
                    }
                }
                std::cout << ")" << std::endl;
            }
            else
            {
                QFAIL("Bad number of colums");
            }
            line++;
        }
        db.Close();
    }
    else
    {
        QFAIL("Cannot open the database");
    }


}
