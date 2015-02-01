#ifndef LOG_H
#define LOG_H

#include <QFile>

#define	LOGFILE "EEKeeper.log"

class Log
{
public:
    /**
     * GetInstance gets the only instance to our Log singleton
     * @return instance of our logger
     */
    static Log& GetInstance() {
        static Log log;
        return log;
    }

    bool Write(const char *pszLine, bool bForceWrite = false);

private:
    Log();
    Log(const Log &) { /* never called */ }
    Log& operator=(const Log &);
    ~Log();


private:
    QFile   m_file;
    bool    m_writable;
};

#endif // LOG_H
