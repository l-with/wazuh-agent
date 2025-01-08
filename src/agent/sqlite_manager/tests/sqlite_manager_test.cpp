#include <gtest/gtest.h>

#include <sqlite_manager.hpp>

#include <memory>
#include <string>
#include <vector>

using namespace sqlite_manager;

class SQLiteManagerTest : public ::testing::Test
{
protected:
    const std::string m_dbName = "testdb.db";
    const std::string m_tableName = "TestTable";

    std::unique_ptr<SQLiteManager> m_db;

    void SetUp() override
    {
        m_db = std::make_unique<SQLiteManager>(m_dbName);
    }

    void TearDown() override {}

    void AddTestData()
    {
        EXPECT_NO_THROW(m_db->Remove(m_tableName));
        m_db->Insert(
            m_tableName,
            {ColumnValue("Name", ColumnType::TEXT, "ItemName"), ColumnValue("Status", ColumnType::TEXT, "ItemStatus")});
        m_db->Insert(m_tableName,
                     {ColumnValue("Name", ColumnType::TEXT, "MyTestName"),
                      ColumnValue("Status", ColumnType::TEXT, "MyTestValue")});
        m_db->Insert(m_tableName,
                     {ColumnValue("Name", ColumnType::TEXT, "ItemName2"),
                      ColumnValue("Status", ColumnType::TEXT, "ItemStatus2")});
        m_db->Insert(m_tableName,
                     {ColumnValue("Name", ColumnType::TEXT, "ItemName3"),
                      ColumnValue("Status", ColumnType::TEXT, "ItemStatus3"),
                      ColumnValue("Module", ColumnType::TEXT, "ItemModule3")});
        m_db->Insert(m_tableName,
                     {ColumnValue("Name", ColumnType::TEXT, "ItemName4"),
                      ColumnValue("Status", ColumnType::TEXT, "ItemStatus4"),
                      ColumnValue("Module", ColumnType::TEXT, "ItemModule4"),
                      ColumnValue("Orden", ColumnType::INTEGER, "19"),
                      ColumnValue("Amount", ColumnType::REAL, "2.8")});
        m_db->Insert(m_tableName,
                     {ColumnValue("Name", ColumnType::TEXT, "ItemName5"),
                      ColumnValue("Status", ColumnType::TEXT, "ItemStatus5"),
                      ColumnValue("Module", ColumnType::TEXT, "ItemModule5"),
                      ColumnValue("Orden", ColumnType::INTEGER, "21"),
                      ColumnValue("Amount", ColumnType::REAL, "3.5")});
    }
};

TEST_F(SQLiteManagerTest, CreateTableTest)
{
    ColumnKey col1 {"Id", ColumnType::INTEGER, true, true, true};
    ColumnKey col2 {"Name", ColumnType::TEXT, true, false, false};
    ColumnKey col3 {"Status", ColumnType::TEXT, true, false};
    ColumnKey col4 {"Module", ColumnType::TEXT, false, false};
    ColumnKey col5 {"Orden", ColumnType::INTEGER, false, false, false};
    ColumnKey col6 {"Amount", ColumnType::REAL, false, false, false};
    EXPECT_NO_THROW(m_db->CreateTable(m_tableName, {col1, col2, col3, col4, col5, col6}));
    EXPECT_TRUE(m_db->TableExists(m_tableName));

    ColumnKey col21 {"Id", ColumnType::INTEGER, true, false, true};
    ColumnKey col212 {"Id2", ColumnType::INTEGER, true, false, true};
    ColumnKey col22 {"Name", ColumnType::TEXT, true, false, true};
    ColumnKey col23 {"Status", ColumnType::TEXT, true, false};
    ColumnKey col24 {"Module", ColumnType::TEXT, false, false};
    ColumnKey col25 {"Orden", ColumnType::INTEGER, false, false, false};
    ColumnKey col26 {"Amount", ColumnType::REAL, false, false, false};
    EXPECT_NO_THROW(m_db->CreateTable("TableTest2", {col21, col212, col22, col23, col24, col25, col26}));
    EXPECT_TRUE(m_db->TableExists("TableTest2"));
}

TEST_F(SQLiteManagerTest, InsertTest)
{
    ColumnValue col1 {"Name", ColumnType::TEXT, "ItemName1"};
    ColumnValue col2 {"Status", ColumnType::TEXT, "ItemStatus1"};

    EXPECT_NO_THROW(m_db->Insert(m_tableName, {col1, col2}));
    EXPECT_NO_THROW(m_db->Insert(
        m_tableName,
        {ColumnValue("Name", ColumnType::TEXT, "ItemName2"), ColumnValue("Status", ColumnType::TEXT, "ItemStatus2")}));
    EXPECT_NO_THROW(m_db->Insert(m_tableName,
                                 {ColumnValue("Name", ColumnType::TEXT, "ItemName3"),
                                  ColumnValue("Status", ColumnType::TEXT, "ItemStatus3"),
                                  ColumnValue("Module", ColumnType::TEXT, "ItemModule3")}));

    EXPECT_NO_THROW(m_db->Insert(m_tableName,
                                 {ColumnValue("Name", ColumnType::TEXT, "ItemName4"),
                                  ColumnValue("Status", ColumnType::TEXT, "ItemStatus4"),
                                  ColumnValue("Module", ColumnType::TEXT, "ItemModule4"),
                                  ColumnValue("Orden", ColumnType::INTEGER, "16")}));

    EXPECT_NO_THROW(m_db->Insert(m_tableName,
                                 {ColumnValue("Name", ColumnType::TEXT, "ItemName4"),
                                  ColumnValue("Status", ColumnType::TEXT, "ItemStatus4"),
                                  ColumnValue("Module", ColumnType::TEXT, "ItemModule4"),
                                  ColumnValue("Orden", ColumnType::INTEGER, "16"),
                                  ColumnValue("Amount", ColumnType::REAL, "4.5")}));
}

TEST_F(SQLiteManagerTest, GetCountTest)
{
    EXPECT_NO_THROW(m_db->Remove(m_tableName));
    int count = m_db->GetCount(m_tableName);
    EXPECT_EQ(count, 0);

    ColumnValue col1 {"Name", ColumnType::TEXT, "ItemName1"};
    ColumnValue col2 {"Status", ColumnType::TEXT, "ItemStatus1"};
    EXPECT_NO_THROW(m_db->Insert(m_tableName, {col1, col2}));

    count = m_db->GetCount(m_tableName);
    EXPECT_EQ(count, 1);

    EXPECT_NO_THROW(m_db->Insert(m_tableName, {col1, col2}));

    count = m_db->GetCount(m_tableName);
    EXPECT_EQ(count, 2);
}

static void DumpResults(std::vector<Row>& ret)
{
    std::cout << "---------- " << ret.size() << " rows returned. ----------" << '\n';
    for (const auto& row : ret)
    {
        for (const auto& field : row)
        {
            std::cout << "[" << field.Name << ": " << field.Value << "]";
        }
        std::cout << '\n';
    }
}

TEST_F(SQLiteManagerTest, SelectTest)
{
    AddTestData();

    std::vector<ColumnName> cols;

    // all fields, no selection criteria
    std::vector<Row> ret = m_db->Select(m_tableName, cols);

    DumpResults(ret);
    EXPECT_NE(ret.size(), 0);

    // all fields with default selection criteria
    ret = m_db->Select(
        m_tableName,
        cols,
        {ColumnValue("Name", ColumnType::TEXT, "MyTestName"), ColumnValue("Status", ColumnType::TEXT, "MyTestValue")});

    DumpResults(ret);
    EXPECT_NE(ret.size(), 0);

    // all fields with 'OR' selection criteria
    ret = m_db->Select(
        m_tableName,
        cols,
        {ColumnValue("Name", ColumnType::TEXT, "MyTestName"), ColumnValue("Module", ColumnType::TEXT, "ItemModule5")},
        LogicalOperator::OR);

    DumpResults(ret);
    EXPECT_EQ(ret.size(), 2);

    // only Name field no selection criteria
    cols.clear();
    cols.emplace_back("Name", ColumnType::TEXT);
    ret.clear();
    ret = m_db->Select(m_tableName, cols);

    DumpResults(ret);
    EXPECT_NE(ret.size(), 0);

    // only Name field with default selection criteria
    cols.clear();
    cols.emplace_back("Name", ColumnType::TEXT);
    ret.clear();
    ret = m_db->Select(
        m_tableName,
        cols,
        {ColumnValue("Name", ColumnType::TEXT, "MyTestName"), ColumnValue("Status", ColumnType::TEXT, "MyTestValue")});

    DumpResults(ret);
    EXPECT_NE(ret.size(), 0);

    // only Name field with single selection criteria
    cols.clear();
    cols.emplace_back("Name", ColumnType::TEXT);
    ret.clear();
    ret = m_db->Select(m_tableName, cols, {ColumnValue("Amount", ColumnType::REAL, "3.5")});

    DumpResults(ret);
    EXPECT_EQ(ret.size(), 1);

    // only Name and Amount fields with single selection criteria
    cols.clear();
    cols.emplace_back("Name", ColumnType::TEXT);
    cols.emplace_back("Amount", ColumnType::REAL);
    ret.clear();
    ret = m_db->Select(m_tableName, cols, {ColumnValue("Amount", ColumnType::REAL, "2.8")});

    DumpResults(ret);
    EXPECT_EQ(ret.size(), 1);
}

TEST_F(SQLiteManagerTest, RemoveTest)
{
    AddTestData();

    int initialCount = m_db->GetCount(m_tableName);

    // Remove a single record
    EXPECT_NO_THROW(m_db->Remove(
        m_tableName,
        {ColumnValue("Name", ColumnType::TEXT, "MyTestName"), ColumnValue("Status", ColumnType::TEXT, "MyTestValue")}));
    int count = m_db->GetCount(m_tableName);
    EXPECT_EQ(count, initialCount - 1);

    // Remove all remaining records
    EXPECT_NO_THROW(m_db->Remove(m_tableName));
    count = m_db->GetCount(m_tableName);
    EXPECT_EQ(count, 0);
}

TEST_F(SQLiteManagerTest, UpdateTest)
{
    AddTestData();
    EXPECT_NO_THROW(m_db->Update(m_tableName,
                                 {ColumnValue("Name", ColumnType::TEXT, "Updated name"),
                                  ColumnValue("Status", ColumnType::TEXT, "Updated status")},
                                 {ColumnValue("Name", ColumnType::TEXT, "MyTestName")}));

    auto ret = m_db->Select(m_tableName, {}, {ColumnValue("Name", ColumnType::TEXT, "Updated name")});
    DumpResults(ret);
    EXPECT_EQ(ret.size(), 1);

    EXPECT_NO_THROW(m_db->Update(m_tableName,
                                 {ColumnValue("Name", ColumnType::TEXT, "Updated name2"),
                                  ColumnValue("Status", ColumnType::TEXT, "Updated status2")},
                                 {ColumnValue("Name", ColumnType::TEXT, "Updated name"),
                                  ColumnValue("Status", ColumnType::TEXT, "Updated status")}));

    ret = m_db->Select(m_tableName, {}, {ColumnValue("Name", ColumnType::TEXT, "Updated name2")});
    DumpResults(ret);
    EXPECT_EQ(ret.size(), 1);

    EXPECT_NO_THROW(m_db->Update(m_tableName,
                                 {ColumnValue("Amount", ColumnType::REAL, "2.0")},
                                 {ColumnValue("Name", ColumnType::TEXT, "Updated name2"),
                                  ColumnValue("Status", ColumnType::TEXT, "ItemStatus3"),
                                  ColumnValue("Status", ColumnType::TEXT, "Updated status3")},
                                 LogicalOperator::OR));

    ret = m_db->Select(m_tableName, {}, {});
    DumpResults(ret);

    EXPECT_NO_THROW(m_db->Update(m_tableName,
                                 {ColumnValue("Amount", ColumnType::REAL, "2.0")},
                                 {ColumnValue("Status", ColumnType::TEXT, "ItemStatus3")},
                                 LogicalOperator::OR));

    ret = m_db->Select(m_tableName, {}, {});
    DumpResults(ret);

    EXPECT_NO_THROW(m_db->Update(m_tableName, {ColumnValue("Amount", ColumnType::REAL, "2.0")}, {}));

    ret = m_db->Select(m_tableName, {}, {});
    DumpResults(ret);
}

TEST_F(SQLiteManagerTest, TransactionTest)
{
    {
        auto transaction = m_db->BeginTransaction();

        m_db->Insert(m_tableName,
                     {ColumnValue("Name", ColumnType::TEXT, "TransactionName"),
                      ColumnValue("Status", ColumnType::TEXT, "TransactionStatus")});

        m_db->Insert(m_tableName,
                     {ColumnValue("Name", ColumnType::TEXT, "TransactionName2"),
                      ColumnValue("Status", ColumnType::TEXT, "TransactionStatus2")});
        EXPECT_NO_THROW(m_db->RollbackTransaction(transaction));
    }

    // since we rolled back the transaction we should find nothing
    auto ret = m_db->Select(m_tableName, {}, {ColumnValue("Status", ColumnType::TEXT, "TransactionStatus2")});

    EXPECT_EQ(ret.size(), 0);

    {
        auto transaction = m_db->BeginTransaction();

        m_db->Insert(m_tableName,
                     {ColumnValue("Name", ColumnType::TEXT, "TransactionName"),
                      ColumnValue("Status", ColumnType::TEXT, "TransactionStatus")});

        m_db->Insert(m_tableName,
                     {ColumnValue("Name", ColumnType::TEXT, "TransactionName2"),
                      ColumnValue("Status", ColumnType::TEXT, "TransactionStatus2")});
    }

    // since transaction obejct ran out of scope without being committed we should find nothing
    ret = m_db->Select(m_tableName, {}, {ColumnValue("Status", ColumnType::TEXT, "TransactionStatus2")});

    EXPECT_EQ(ret.size(), 0);

    {
        auto transaction = m_db->BeginTransaction();

        m_db->Insert(m_tableName,
                     {ColumnValue("Name", ColumnType::TEXT, "TransactionName"),
                      ColumnValue("Status", ColumnType::TEXT, "TransactionStatus")});

        m_db->Insert(m_tableName,
                     {ColumnValue("Name", ColumnType::TEXT, "TransactionName2"),
                      ColumnValue("Status", ColumnType::TEXT, "TransactionStatus2")});
        EXPECT_NO_THROW(m_db->CommitTransaction(transaction));
    }

    // since we commited the transaction we should find something
    ret = m_db->Select(m_tableName, {}, {ColumnValue("Status", ColumnType::TEXT, "TransactionStatus2")});

    EXPECT_EQ(ret.size(), 1);
}

TEST_F(SQLiteManagerTest, DropTableTest)
{
    ColumnKey col1 {"Id", ColumnType::INTEGER, true, true, true};
    ColumnKey col2 {"Name", ColumnType::TEXT, true, false};
    ColumnKey col3 {"Status", ColumnType::TEXT, true, false};
    ColumnKey col4 {"Module", ColumnType::TEXT, false, false};
    ColumnKey col5 {"Orden", ColumnType::INTEGER, false, false, false};

    EXPECT_NO_THROW(m_db->CreateTable("DropMe", {col1, col2, col3, col4, col5}));
    EXPECT_TRUE(m_db->TableExists("DropMe"));
    EXPECT_NO_THROW(m_db->DropTable("DropMe"));
    EXPECT_FALSE(m_db->TableExists("DropMe"));

    EXPECT_ANY_THROW(auto ret = m_db->Select("DropMe", {}, {}));
}
