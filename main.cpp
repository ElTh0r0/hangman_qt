/**
 * \file main.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2008-present Thorsten Roth
 *
 * This file is part of Hangman.
 *
 * Hangman is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hangman is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hangman.  If not, see <https://www.gnu.org/licenses/>.
 */

/** \mainpage
 * \section Introduction
 * Simple hangman game framework written in C++ and Qt.
 */

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFile>
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
                                     QIcon(QStringLiteral(":/hangman.png"))));
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
  app.setDesktopFileName(QStringLiteral("com.github.elth0r0.hangman"));
#endif
#endif

  QCommandLineParser cmdparser;
  cmdparser.setApplicationDescription(QStringLiteral(APP_DESC));
  cmdparser.addHelpOption();
  cmdparser.addVersionOption();
  cmdparser.process(app);

  // Create log file
  /*
  QStringList sListPaths = QStandardPaths::standardLocations(
                             QStandardPaths::DataLocation);
  if (sListPaths.isEmpty()) {
    qCritical() << "Error while getting data standard path.";
    sListPaths << app.applicationDirPath();
  }
  const QDir userDataDir(sListPaths[0].toLower());

  // Create folder including possible parent directories (mkPATH)
  if (!userDataDir.exists()) {
    userDataDir.mkpath(userDataDir.absolutePath());
  }

  const QString sDebugFile("debug.log");
  setupLogger(userDataDir.absolutePath() + "/" + sDebugFile,
              app.applicationName(), app.applicationVersion());
  */

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
