// SPDX-FileCopyrightText: 2008-2025 Thorsten Roth
// SPDX-License-Identifier: GPL-3.0-or-later

#include "./game.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <ctime>
#include <random>  // std::shuffle

Game::Game(const QString &sRessource, QObject *pParent) {
  Q_UNUSED(pParent)
  std::srand(static_cast<unsigned int>(time(nullptr)));  // Seed num generator
  this->loadWordlist(sRessource);

  m_nQuantity = static_cast<quint16>(m_sListWords.size());
  if (0 == m_nQuantity) {
    qCritical() << "Word list is empty!";
    QMessageBox::critical(nullptr, tr("Error"), tr("Word list is empty!"));
    exit(-2);
  }
  // qDebug() << "Num of Words:" << m_nQuantity;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void Game::loadWordlist(const QString &sRessource) {
  QFile dataFile(sRessource);
  if (!dataFile.open(QIODevice::ReadOnly)) {
    qCritical() << "Word list" << dataFile.fileName() << "could not be opened!";
    QMessageBox::critical(nullptr, tr("Error"),
                          tr("Word list could not be opened!"));
    exit(-1);
  }

  QTextStream textStream(&dataFile);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  // Since Qt 6 UTF-8 is used by default
  textStream.setCodec("UTF-8");
#endif
  while (true) {
    QString sLine = textStream.readLine();
    if (sLine.isNull()) {
      break;
    }
    m_sListWords.append(sLine.trimmed());
  }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void Game::newGame() {
  m_nPlayedWords = 0;
  m_nSumCorrect = 0;
  m_nCorrectInRow = 0;

  // Shuffle questions
  std::shuffle(m_sListWords.begin(), m_sListWords.end(),
               std::mt19937(std::random_device()()));
  this->nextWord();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void Game::nextWord() {
  static QStringList sListTmp;

  if (m_nPlayedWords >= m_nQuantity) {
    emit updateWord("", Game::PLAYEDALL, m_nCorrectInRow, m_nSumCorrect,
                    m_nPlayedWords, m_nQuantity);
    return;
  }

  sListTmp.clear();
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
  sListTmp << m_sListWords[m_nPlayedWords].split(';', Qt::SkipEmptyParts);
#else
  sListTmp << m_sListWords[m_nPlayedWords].split(';', QString::SkipEmptyParts);
#endif
  if (!sListTmp.isEmpty()) {
    QString sWord = sListTmp[0].trimmed();
    sWord = sWord.toUpper();
    m_baWord.clear();
    m_baWord = sWord.toLatin1();
    m_baShownWord.clear();
    sWord = QString('-').repeated(sWord.length());
    m_baShownWord = sWord.toLatin1();
    m_sAnswer = QLatin1String("");
  }
  if (2 == sListTmp.size()) {
    m_sAnswer = sListTmp[1].trimmed();
  }

  emit updateWord(m_baShownWord, Game::NEWRORD, m_nCorrectInRow, m_nSumCorrect,
                  m_nPlayedWords, m_nQuantity);
  m_nCntFalse = 0;
  m_nPlayedWords++;

  // qDebug() << "Next word:" << m_baWord;
  // qDebug() << "Answer   :" << m_sAnswer;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void Game::checkLetter(const QByteArray &baLetter) {
  // qDebug() << "Check letter:" << baLetter;

  if (m_baWord.contains(baLetter)) {
    for (int i = 0; i < m_baWord.length(); i++) {
      if (baLetter.at(0) == m_baWord.at(i)) {
        m_baShownWord[i] = m_baWord.at(i);
      }
    }
    emit updateWord(m_baShownWord, m_nCntFalse, m_nCorrectInRow, m_nSumCorrect,
                    m_nPlayedWords, m_nQuantity);

    if (m_baShownWord == m_baWord) {
      m_nSumCorrect++;
      m_nCorrectInRow++;
      emit showAnswer(m_sAnswer);
      this->nextWord();
    }
  } else {
    m_nCntFalse++;
    emit updateWord(m_baShownWord, m_nCntFalse, m_nCorrectInRow, m_nSumCorrect,
                    m_nPlayedWords, m_nQuantity);
    if (Game::MAXTRIES == m_nCntFalse) {
      m_nCorrectInRow = 0;
      emit showAnswer(m_sAnswer, QString::fromLatin1(m_baWord));
      this->nextWord();
    }
  }
}
