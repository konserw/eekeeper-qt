#include "ui/EEKeeperWindow.h"
#include "EEKeeper.h"


#include <cstdio>
#include <cstdlib>

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QPixmap>
#include <QTextStream>
#include <QSplashScreen>
#include <QtGlobal>

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // use static variable to ensure they're always open
    static QFile output("EEKeeper.log");
    static QTextStream stream(stderr);
    static int count = 0;

    QString txt = QDateTime::currentDateTime().toString("[dd/MM/yyyy hh:mm:ss] ");

    switch (type) {
    case QtDebugMsg:
        txt = txt + msg;
        break;
    case QtWarningMsg:
        txt = txt + EEKeeper::tr("WARNING: ") + msg;
        break;
    case QtCriticalMsg:
        txt = txt + EEKeeper::tr("CRITICAL: ") + msg;
        break;
    case QtFatalMsg:
        stream.flush();
        output.close();
        abort();
    }

    // add context specific information to the output
    // this look to be a little too much clutter for now
    //txt = txt + " :: " + context.function + " (" + context.file + " : " + QString::number(context.line) + ")";

    // open our logfile and set our stream
    if (false && !output.isOpen()) {
        if (output.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Truncate) ) {
            stream.setDevice(&output);
        }
    }

#ifdef QT_DEBUG
    txt = txt + "\n [ " + context.function + " (" + context.file + " : " + QString::number(context.line) + ") ]";
#endif

    stream << txt << endl;

    // flush somewhat regularly
    // TODO: is this needed?
    if (++count > 100) {
        stream.flush();
        count = 0;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qInstallMessageHandler(messageHandler);

    // load up our main window
    EEKeeperWindow w;
    w.show();

    // attempt to resolve the installation directory
    //if (_strInstallPath.isEmpty() && !EEKeeper::FindInstallPath(_strInstallPath) ) {
    //    QMessageBox::critical(
    //                &w,
    //                w.tr("Unable to find installation directory"),
    //                w.tr("EE Keeper was unable to locate your BG:EE installation. You will need to set the path yourself.") );
    //}

    int ret = a.exec();
    return ret;
}
