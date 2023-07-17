#include <thread>
#include <iostream>
#include <fstream>
#include "LogManager.h"

std::unique_ptr<LogManager> LogManager::m_instance;

using namespace std;

struct OneLogInfo
{
    std::string info;
    LogLevel level;
    std::string time;

    std::string GetFinalLogStr()
    {
        std::string levelStr;
        switch (level)
        {
        case LogLevel::ALL:
            levelStr = "ALL";
            break;
        case LogLevel::TRACE:
            levelStr = "TRACE";
            break;
        case LogLevel::DEBUG:
            levelStr = "DEBUG";
            break;
        case LogLevel::INFO:
            levelStr = "INFO";
            break;
        case LogLevel::WARN:
            levelStr = "WARN";
            break;
        case LogLevel::ERROR:
            levelStr = "ERROR";
            break;
        case LogLevel::FATAL:
            levelStr = "FATAL";
            break;
        case LogLevel::OFF:
            levelStr = "OFF";
            break;
        default:
            break;
        }
        return "[" + levelStr + "]" + time + " " + info;
    }
};

LogManager &LogManager::GetInstance()
{
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]()
                   {
        m_instance.reset(new LogManager);
        if (m_instance->m_isStop == true)
        {
            m_instance->Start();
        } });

    return *m_instance;
}

void LogManager::SetProName(const string &_name)
{
    m_proName = _name;
}

void LogManager::SetLogDir(const string &_dir)
{
    m_logDir = _dir;
}

void LogManager::SetWriteLogMethod(const WriteLogMethod _method)
{
    m_writeMethod = _method;
}

void LogManager::SetWriteLogLevel(const LogLevel _level)
{
    m_writeLevel = _level;
}

void LogManager::SetTimeFormat(const std::string &_format)
{
    m_timeFormat = _format;
}

void LogManager::SetLogPostionShow(const int _logPosShow)
{
    m_logPosShow = _logPosShow;
}

void LogManager::WriteLog(const LogPosInfo &_logPos, const string &_log, const LogLevel _level)
{
    OneLogInfo logInfo;
    if (_level < m_writeLevel)
    {
        return;
    }
    logInfo.info = GetLogPos(_logPos) + _log;
    logInfo.level = _level;
    logInfo.time = GetCurrentTime(m_timeFormat);

    {
        unique_lock<mutex> locker(m_logMtx);
        m_logList.push_back(logInfo);
    }
    unique_lock<mutex> locker(m_logWriteMtx);
    m_threadCondition.notify_all();
}

string LogManager::GetCurrentTime(const string &_format) const
{
    string timeStr;
    auto now = std::chrono::system_clock::now();
    auto timet = std::chrono::system_clock::to_time_t(now);
    auto ltime = std::localtime(&timet);
    char ctime[100] = {0};
    strftime(ctime, sizeof(ctime), _format.c_str(), ltime);
    timeStr = string(ctime);
    return timeStr;
}

string LogManager::GetLogPos(const LogPosInfo &_logPos) const
{
    string logPos;
    if (m_logPosShow & LogPosShow_File)
    {
        logPos += "file[" + _logPos.fileName + "] ";
    }
    if (m_logPosShow & LogPosShow_Func)
    {
        logPos += "func[" + _logPos.funcName + "] ";
    }
    if (m_logPosShow & LogPosShow_Line)
    {
        logPos += "line[" + to_string(_logPos.lineIndex) + "] ";
    }
    return logPos;
}

void LogManager::Start()
{
    m_isStop = false;
    thread logThread = thread(&LogManager::Run, this);
    logThread.detach();
}

void LogManager::Stop()
{
    m_isStop = true;
}

void LogManager::Run()
{
    while (!m_isStop)
    {
        unique_lock<mutex> locker(m_logWriteMtx);
        m_threadCondition.wait(locker);
        locker.unlock();
        switch (m_writeMethod)
        {
        case WriteLogMethod::FILE:
        {
            string fileName;
            fileName = m_logDir + m_proName + "_" + GetCurrentTime(TimeFormatDay) + ".txt";
            fstream logFile;
            logFile.open(fileName, std::ios_base::app);
            if (!logFile.is_open())
            {
                cerr << "failed to open " << fileName << endl;
            }
            else
            {
                unique_lock<mutex> locker(m_logMtx);
                for (auto log : m_logList)
                {
                    logFile << log.GetFinalLogStr() << endl;
                }
                m_logList.clear();
            }
            // logFile.sync();
            logFile.close();
        }
        break;
        case WriteLogMethod::CONSOLE:
        {
            unique_lock<mutex> locker(m_logMtx);
            for (auto log : m_logList)
            {
                cout << log.GetFinalLogStr() << endl;
            }
            m_logList.clear();
        }
        break;
        default:
            break;
        }
    }
}
