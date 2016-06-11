/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011-2015 Emanuel Eichhammer                            **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.qcustomplot.com/                          **
**             Date: 25.04.15                                             **
**          Version: 1.3.1                                                **
****************************************************************************/

#ifndef QCP_SELECTION_H
#define QCP_SELECTION_H

#include "global.h"

class QCP_LIB_DECL QCPDataRange
{
public:
  QCPDataRange();
  QCPDataRange(int begin, int end);
  
  bool operator==(const QCPDataRange& other) const { return mBegin == other.mBegin && mEnd == other.mEnd; }
  bool operator!=(const QCPDataRange& other) const { return !(*this == other); }
  
  // getters:
  int begin() const { return mBegin; }
  int end() const { return mEnd; }
  int length() const { return mEnd-mBegin; }
  
  // setters:
  void setBegin(int begin);
  void setEnd(int end);
  
  // non-property methods:
  bool isValid() const;
  bool isEmpty() const { return length() == 0; }
  QCPDataRange bounded(const QCPDataRange &otherRange) const;
  
private:
  // property members:
  int mBegin, mEnd;
  
  // non-virtual methods:

};
Q_DECLARE_TYPEINFO(QCPDataRange, Q_MOVABLE_TYPE);

class QCP_LIB_DECL QCPDataSelection
{
public:
  explicit QCPDataSelection();
  
  bool operator==(const QCPDataSelection& other) const;
  bool operator!=(const QCPDataSelection& other) const { return !(*this == other); }
  QCPDataSelection &operator+=(const QCPDataSelection& other);
  friend inline const QCPDataSelection operator+(const QCPDataSelection& a, const QCPDataSelection& b);
  
  // getters:
  int dataRangeCount() const { return mDataRanges.size(); }
  QCPDataRange dataRange(int index=0) const;
  QList<QCPDataRange> dataRanges() const { return mDataRanges; }
  
  // non-property methods:
  void addDataRange(const QCPDataRange &dataRange, bool simplify=true);
  void removeDataRange(const QCPDataRange &dataRange);
  void clear();
  bool isEmpty() const { return mDataRanges.isEmpty(); }
  void simplify();
  void enforceType(QCP::SelectionType type);
  
private:
  // property members:
  QList<QCPDataRange> mDataRanges;
  
  inline static bool lessThanDataRangeBegin(const QCPDataRange &a, const QCPDataRange &b) { return a.begin() < b.begin(); }
};
Q_DECLARE_METATYPE(QCPDataSelection)

/*!
  Joins two selections
*/
inline const QCPDataSelection operator+(const QCPDataSelection& a, const QCPDataSelection& b)
{
  QCPDataSelection result(a);
  result += b;
  return result;
}

/* qdebug output stream operator, no doc needed */
inline QDebug operator<< (QDebug d, const QCPDataRange &dataRange)
{
    d.nospace() << "[" << dataRange.begin() << ".." << dataRange.end()-1 << "]";
    return d.space();
}

/* qdebug output stream operator, no doc needed */
inline QDebug operator<< (QDebug d, const QCPDataSelection &selection)
{
    d.nospace() << "QCPDataSelection(";
    for (int i=0; i<selection.dataRangeCount(); ++i)
    {
      if (i != 0)
        d << ", ";
      d << selection.dataRange(i);
    }
    d << ")";
    return d.space();
}



#endif // QCP_SELECTION_H

