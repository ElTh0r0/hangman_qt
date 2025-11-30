// SPDX-FileCopyrightText: 2008-2025 Thorsten Roth
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QTextStream>
#include <QTime>

#include "./hangman.h"

static QFile logfile;
static QTextStream out(&logfile);

void setupLogger(const QString &sDebugFilePath, const QString &sAppName,
                 const QString &sVersion);

void LoggingHandler(QtMsgType type, const QMessageLogContext &context,
                    const QString &sMsg);

auto main(int argc, char *argv[]) -> int {
  QApplication app(argc, argv);
  app.setApplicationName(QStringLiteral(APP_NAME));
  app.setApplicationVersion(QStringLiteral(APP_VERSION));
  app.setApplicationDisplayName(QStringLiteral(APP_NAME));
#if !defined(Q_OS_WIN) && !defined(Q_OS_MACOS)
  app.setWindowIcon(QIcon::fromTheme(QStringLiteral("hangman"),
                                     QIcon(QStringLiteral(":/icon.png"))));
  app.setDesktopFileName(QStringLiteral("com.github.elth0r0.hangman"));
#endif

  QCommandLineParser cmdparser;
  cmdparser.setApplicationDescription(QStringLiteral(APP_DESC));
  cmdparser.addHelpOption();
  cmdparser.addVersionOption();
  cmdparser.process(app);

  // Create log file
  QStringList sListPaths =
      QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);
  if (sListPaths.isEmpty()) {
    qCritical() << "Error while getting data standard path.";
    sListPaths << app.applicationDirPath();
  }
  const QDir userDataDir(sListPaths[0].toLower());

  // Create folder including possible parent directories (mkPATH)
  if (!userDataDir.exists()) {
    userDataDir.mkpath(userDataDir.absolutePath());
  }

  const QString sDebugFile(QStringLiteral("debug.log"));
  setupLogger(userDataDir.absolutePath() + "/" + sDebugFile,
              app.applicationName(), app.applicationVersion());

  app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
  Hangman myApp;
  myApp.show();
  int nRet = app.exec();

  logfile.close();
  return nRet;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void setupLogger(const QString &sDebugFilePath, const QString &sAppName,
                 const QString &sVersion) {
  // Remove old debug file
  if (QFile(sDebugFilePath).exists()) {
    QFile(sDebugFilePath).remove();
  }

  // Create new file
  logfile.setFileName(sDebugFilePath);
  if (!logfile.open(QIODevice::WriteOnly)) {
    qWarning() << "Couldn't create logging file: " << sDebugFilePath;
  } else {
    qInstallMessageHandler(LoggingHandler);
  }

  qDebug() << sAppName << sVersion;
  qDebug() << "Compiled with Qt" << QT_VERSION_STR;
  qDebug() << "Qt runtime" << qVersion();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void LoggingHandler(QtMsgType type, const QMessageLogContext &context,
                    const QString &sMsg) {
  QString sContext = sMsg + " (" + QString::fromLatin1(context.file) + ":" +
                     QString::number(context.line) + ", " +
                     QString::fromLatin1(context.function) + ")";
  QString sTime(QTime::currentTime().toString());

  switch (type) {
    case QtDebugMsg:
      out << sTime << " Debug: " << sMsg << "\n";
      out.flush();
      break;
    case QtWarningMsg:
      out << sTime << " Warning: " << sContext << "\n";
      out.flush();
      break;
    case QtCriticalMsg:
      out << sTime << " Critical: " << sContext << "\n";
      out.flush();
      break;
    case QtFatalMsg:
      out << sTime << " Fatal: " << sContext << "\n";
      out.flush();
      logfile.close();
      abort();
    default:
      out << sTime << " OTHER INFO: " << sContext << "\n";
      out.flush();
      break;
  }
}
