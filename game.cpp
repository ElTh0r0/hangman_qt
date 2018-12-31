/**
 * \file game.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2008-2018 Thorsten Roth <elthoro@gmx.de>
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
 * along with Hangman.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "./game.h"

#include <time.h>

#include <QDebug>
#include <QFile>
#include <QMessageBox>

#include <algorithm>  // std::random_shuffle

Game::Game(const QString &sRessource, QObject *pParent) {
  Q_UNUSED(pParent);
  std::srand(time(0));  // Seed random number generator
  this->loadWordlist(sRessource);

  m_nQuantity = m_sListWords.size();
  if (m_nQuantity <= 0) {
    qCritical() << "Word list is empty!";
    QMessageBox::critical(NULL, tr("Error"), tr("Word list is empty!"));
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
    QMessageBox::critical(NULL, tr("Error"),
                          tr("Word list could not be opened!"));
    exit(-1);
  }

  QTextStream textStream(&dataFile);
  textStream.setCodec("UTF-8");
  while (true) {
    QString sLine = textStream.readLine();
    if (sLine.isNull()) {
      break;
    } else {
      m_sListWords.append(sLine.trimmed());
    }
  }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void Game::newGame() {
  m_nPlayedWords = 0;
  m_nSumCorrect = 0;
  m_nCorrectInRow = 0;

  // Shuffel questions
  std::random_shuffle(m_sListWords.begin(), m_sListWords.end());
  this->nextWord();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void Game::nextWord() {
  static QStringList sListTmp;

  if (m_nPlayedWords >= m_nQuantity) {
    emit updateWord("", 112, m_nCorrectInRow, m_nSumCorrect,
                    m_nPlayedWords, m_nQuantity);
    return;
  }

  sListTmp.clear();
  sListTmp << m_sListWords[m_nPlayedWords].split(';', QString::SkipEmptyParts);
  if (sListTmp.size() > 0) {
    QString sWord = sListTmp[0].trimmed();
    sWord = sWord.toUpper();
    m_baWord.clear();
    m_baWord = sWord.toLatin1();
    m_baShownWord.clear();
    sWord = QString('-').repeated(sWord.length());
    m_baShownWord = sWord.toLatin1();
    m_sAnswer = QString("");
  }
  if (2 == sListTmp.size()) {
    m_sAnswer = sListTmp[1].trimmed();
  }

  emit updateWord(m_baShownWord, 111, m_nCorrectInRow,
                  m_nSumCorrect, m_nPlayedWords, m_nQuantity);
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
    emit updateWord(m_baShownWord, m_nCntFalse, m_nCorrectInRow,
                    m_nSumCorrect, m_nPlayedWords, m_nQuantity);

    if (m_baShownWord == m_baWord) {
      m_nSumCorrect++;
      m_nCorrectInRow++;
      emit showAnswer(m_sAnswer);
      this->nextWord();
    }
  } else {
    m_nCntFalse++;
    emit updateWord(m_baShownWord, m_nCntFalse, m_nCorrectInRow,
                    m_nSumCorrect, m_nPlayedWords, m_nQuantity);
    if (7 == m_nCntFalse) {
      m_nCorrectInRow = 0;
      emit showAnswer(m_sAnswer, QString(m_baWord));
      this->nextWord();
    }
  }
}
