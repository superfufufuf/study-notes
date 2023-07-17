#ifndef _LOG_MANAGER_H_
#define _LOG_MANAGER_H_

#include <memory>
#include <mutex>
#include <condition_variable>
#include <string>
#include <list>

enum class LogLevel
{
    ALL = 0,
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    OFF
};

enum class WriteLogMethod
{
    FILE = 0,
    CONSOLE
};

enum LogPostionShow
{
    LogPosShow_Hide = 1 << 0,
    LogPosShow_File = 1 << 1,
    LogPosShow_Func = 1 << 2,
    LogPosShow_Line = 1 << 3
};

const std::string TimeFormatFull = "%F %H:%M:%S";
const std::string TimeFormatDay = "%F";

struct OneLogInfo;
struct LogPosInfo
{
    LogPosInfo() = default;
    LogPosInfo(const std::string &_fileName, const std::string &_funcName, const int _lineIndex) : funcName(_funcName), lineIndex(_lineIndex)
    {
        auto fileNamePos = _fileName.find_last_of('/');
        if (fileNamePos == std::string::npos)
        {
            fileName = _fileName;
        }
        else
        {
            fileName = _fileName.substr(fileNamePos + 1);
        }
    }

    std::string fileName;
    std::string funcName;
    int lineIndex;
};

class LogManager
{
public:
    static LogManager &GetInstance();
    ~LogManager() = default;

    void SetProName(const std::string &_name);
    void SetLogDir(const std::string &_dir);
    void SetWriteLogMethod(const WriteLogMethod _method);
    void SetWriteLogLevel(const LogLevel _level);

    // use strftime() get time string
    void SetTimeFormat(const std::string &_format);
    void SetLogPostionShow(const int _logPosShow);
    void WriteLog(const LogPosInfo &_logPos, const std::string &_log, const LogLevel _level);

    void Start();
    void Stop();

private:
    std::string GetCurrentTime(const std::string &_format = TimeFormatFull) const;
    std::string GetLogPos(const LogPosInfo &_logPos) const;
    void Run();

    bool m_isStop{true};
    std::string m_proName;
    std::string m_logDir;
    WriteLogMethod m_writeMethod{WriteLogMethod::FILE};
    std::list<OneLogInfo> m_logList;
    LogLevel m_writeLevel{LogLevel::ALL};
    std::string m_timeFormat = TimeFormatFull;
    int m_logPosShow = LogPosShow_Hide;

    std::mutex m_logMtx;
    std::mutex m_logWriteMtx;
    std::condition_variable m_threadCondition;

private:
    LogManager() = default;
    LogManager(const LogManager &) = delete;
    LogManager &operator=(const LogManager &) = delete;

    static std::unique_ptr<LogManager> m_instance;
};

#define _LOG(_logStr, _logLevel) LogManager::GetInstance().WriteLog(LogPosInfo(__FILE__, __func__, __LINE__), _logStr, _logLevel);

#endif