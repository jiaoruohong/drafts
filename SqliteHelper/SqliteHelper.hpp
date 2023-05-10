// class SqliteHelper,
// has a member of sqlite3* db, which is a pointer to sqlite3 database,
// has a static method for getting instance of SqliteHelper,
// has a method for executing sql statement.
// this class can multi-thread safe.
// user pass the path to db file to SqliteHelper::setDbPath() before using SqliteHelper.
// the class also provide method for connecting & disconnecting to db.
class SqliteHelper{
public:
    // get instance of SqliteHelper
    static SqliteHelper& getInstance(){
        static SqliteHelper instance;
        return instance;
    }

    // set db path
    void setDbPath(const std::string& dbPath){
        m_dbPath = dbPath;
    }

    // connect to db
    void connect(){
        if(m_dbPath.empty()){
            throw std::runtime_error("db path is empty");
        }

        // open db
        int rc = sqlite3_open(m_dbPath.c_str(), &m_db);
        if(rc != SQLITE_OK){
            throw std::runtime_error("can't open db");
        }
    }

    // disconnect from db
    void disconnect(){
        // close db
        int rc = sqlite3_close(m_db);
        if(rc != SQLITE_OK){
            throw std::runtime_error("can't close db");
        }
    }

    // execute sql statement
    void execute(const std::string& sql){
        // execute sql
        char* errMsg = nullptr;
        int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
        if(rc != SQLITE_OK){
            std::string msg = "can't execute sql: " + std::string(errMsg);
            sqlite3_free(errMsg);
            throw std::runtime_error(msg);
        }
    }

private:
    // constructor
    SqliteHelper() = default;

    // destructor
    ~SqliteHelper() = default;

    // copy constructor
    SqliteHelper(const SqliteHelper&) = delete;

    // move constructor
    SqliteHelper(SqliteHelper&&) = delete;

    // copy assignment operator
    SqliteHelper& operator=(const SqliteHelper&) = delete;

    // move assignment operator
    SqliteHelper& operator=(SqliteHelper&&) = delete;

private:

    // db path
    std::string m_dbPath;

    // db
    sqlite3* m_db = nullptr;
};