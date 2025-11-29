// SPDX-FileCopyrightText: 2008-2025 Thorsten Roth
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef GAME_H_
#define GAME_H_

#include <QObject>
#include <QString>

class Game : public QObject {
  Q_OBJECT

 public:
  explicit Game(const QString &sRessource, QObject *pParent = nullptr);
  static const uint MAXTRIES = 7;
  static const uint NEWRORD = 111;
  static const uint PLAYEDALL = 112;

 public slots:
  void newGame();
  void checkLetter(const QByteArray &baLetter);

 signals:
  void updateWord(const QByteArray &baWord, const quint8 nWrong,
                  const quint16 nCorrectInRow, const quint16 nSumCorrect,
                  const quint16 nPlayedWords, const quint16 nQuantity);
  void showAnswer(const QString &sAnswer,
                  const QString &sWord = QLatin1String(""));

 private slots:
  void nextWord();

 private:
  void loadWordlist(const QString &sRessource);

  QStringList m_sListWords;
  QByteArray m_baWord;
  QByteArray m_baShownWord;
  QString m_sAnswer;
  quint8 m_nCntFalse{};
  quint16 m_nQuantity;
  quint16 m_nPlayedWords{};
  quint16 m_nSumCorrect{};
  quint16 m_nCorrectInRow{};
};

#endif  // GAME_H_
