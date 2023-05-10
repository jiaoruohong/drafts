// class BoostLogger,
// wrapper for boost::log,
// has two sink: one for file, and another for console,
// both have the same filter for severity level 'info',
// both have the same format for output,
// when logger write text, should use BOOST_LOG_SEV(logger, severity_level) << text,
// and the log info will be output to file and console.

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

// severity level
enum severity_level
{
    trace,
    debug,
    info,
    warning,
    error,
    fatal
};

// operator << for severity level
std::ostream& operator<< (std::ostream& strm, severity_level level)
{
    static const char* strings[] =
    {
        "trace",
        "debug",
        "info",
        "warning",
        "error",
        "fatal"
    };

    if (static_cast< std::size_t >(level) < sizeof(strings) / sizeof(*strings))
        strm << strings[level];
    else
        strm << static_cast< int >(level);

    return strm;
}

// class BoostLogger
class BoostLogger
{
public:
    // constructor
    BoostLogger();
    // destructor
    ~BoostLogger();

    // init logger
    void init();

    // get logger
    boost::log::sources::severity_logger<severity_level>& getLogger();

private:

    // init sink for file
    void initFileSink();

    // init sink for console
    void initConsoleSink();

    // init filter
    void initFilter();

    // init format
    void initFormat();

private:

    // logger
    boost::log::sources::severity_logger<severity_level> m_logger;

    // file sink
    boost::shared_ptr< boost::log::sinks::synchronous_sink< boost::log::sinks::text_file_backend > > m_fileSink;

    // console sink
    boost::shared_ptr< boost::log::sinks::synchronous_sink< boost::log::sinks::text_ostream_backend > > m_consoleSink;

    // filter
    boost::shared_ptr< boost::log::filter > m_filter;

    // format
    boost::shared_ptr< boost::log::formatter > m_format;
};

boost::log::sources::severity_logger<severity_level>& BoostLogger::getLogger(){
    return m_logger;
}

// init sink for file
    void BoostLogger::initFileSink(){
        // create backend
        boost::shared_ptr< boost::log::sinks::text_file_backend > backend = boost::make_shared< boost::log::sinks::text_file_backend >(
            boost::log::keywords::file_name = "log_%Y-%m-%d_%H-%M-%S.%N.log", // file name
            boost::log::keywords::rotation_size = 10 * 1024 * 1024, // max file size
            boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0), // rotation at midnight
            boost::log::keywords::auto_flush = true // flush after each log
        );

        // create sink frontend
        m_fileSink = boost::make_shared< boost::log::sinks::synchronous_sink< boost::log::sinks::text_file_backend > >(backend);

        // set filter
        m_fileSink->set_filter(m_filter);

        // set format
        m_fileSink->set_formatter(m_format);

        // add sink to core
        boost::log::core::get()->add_sink(m_fileSink);
    }

    // init sink for console
    void BoostLogger::initConsoleSink(){
        // create backend
        boost::shared_ptr< boost::log::sinks::text_ostream_backend > backend = boost::make_shared< boost::log::sinks::text_ostream_backend >();

        // add stream
        backend->add_stream(boost::shared_ptr< std::ostream >(&std::clog, boost::null_deleter()));

        // create sink frontend
        m_consoleSink = boost::make_shared< boost::log::sinks::synchronous_sink< boost::log::sinks::text_ostream_backend > >(backend);

        // set filter
        m_consoleSink->set_filter(m_filter);

        // set format
        m_consoleSink->set_formatter(m_format);

        // add sink to core
        boost::log::core::get()->add_sink(m_consoleSink);
    }

    // init filter
    void BoostLogger::initFilter(){
        // create filter
        m_filter = boost::log::filter(boost::log::trivial::severity >= boost::log::trivial::info);
    }

    // init format
    void BoostLogger::initFormat(){
        // create format
        m_format = boost::log::expressions::format("[%1%] [%2%] [%3%] [%4%] [%5%] %6%")
            % boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
            % boost::log::expressions::attr< boost::log::attributes::current_thread_id::value_type >("ThreadID")
            % boost::log::expressions::attr< boost::log::trivial::severity_level >("Severity")
            % boost::log::expressions::attr< boost::log::attributes::current_process_id::value_type >("ProcessID")
            % boost::log::expressions::attr< boost::log::attributes::current_process_name::value_type >("ProcessName")
            % boost::log::expressions::smessage;
        
        // add format to core
        boost::log::core::get()->add_global_attribute("TimeStamp", boost::log::attributes::local_clock());
    }