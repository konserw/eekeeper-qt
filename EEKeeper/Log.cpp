#include "Log.h"

#define LOGGING true

Log::Log() : m_file(QString(LOGFILE))
{
    // open the log file
    m_writable = m_file.open(QIODevice::Append);
}

Log::~Log()
{
    // ensure that our log file gets closed
    if (m_writable)
        m_file.close();
}

bool Log::Write(const char *pszLine, bool bForceWrite)
{
    // if not writable or we aren't logging
    if (!m_writable || !(LOGGING || bForceWrite))
        return false;

    return (m_file.write(pszLine) == (qint64)qstrlen(pszLine) );
}
