#include "test-datacontainer.h"
#include <QMainWindow>

void TestDatacontainer::init()
{
  mData = new QCPDataContainer<QCPGraphData>;
}

void TestDatacontainer::cleanup()
{
  delete mData;
}

void TestDatacontainer::sizeAndClear()
{
  const int n = 100;
  QVector<QCPGraphData> sortedData;
  for (int i=0; i<n; ++i)
    sortedData << QCPGraphData(i-n/2, 0);
  
  QVERIFY(mData->isEmpty());
  QCOMPARE(mData->size(), 0);
  mData->set(sortedData, true);
  QCOMPARE(mData->size(), sortedData.size());
  QVERIFY(!mData->isEmpty());
  mData->clear();
  QVERIFY(mData->isEmpty());
  QCOMPARE(mData->size(), 0);
}

void TestDatacontainer::setSorted()
{
  const int n = 100;
  // set sorted data with sorted flag:
  QVector<QCPGraphData> sortedData;
  for (int i=0; i<n; ++i)
    sortedData << QCPGraphData(i-n/2, 0);
  mData->set(sortedData, true);
  QVERIFY(isSameData(sortedData, mData));
  QVERIFY(isSorted());
  // set sorted data without sorted flag:
  mData->set(sortedData, false);
  QVERIFY(isSameData(sortedData, mData));
  QVERIFY(isSorted());
}

void TestDatacontainer::setUnsorted()
{
  BadRandom r(42, -10, 15);
  // set unsorted data without sorted flag:
  QVector<QCPGraphData> unsortedData;
  for (int i=0; i<100; ++i)
    unsortedData << QCPGraphData(r.get(), 0);
  mData->set(unsortedData, false);
  QVERIFY(isSameData(unsortedData, mData));
  QVERIFY(isSorted());
}

void TestDatacontainer::addSorted()
{
  QList<int> nCases = QList<int>() << 5 << 50 << 100 << 1000 << 2500 << 10000;
  foreach (int n, nCases)
  {
    mData->clear();
    // add sorted data with sorted flag:
    QVector<QCPGraphData> sortedData;
    for (int i=0; i<n; ++i)
      sortedData << QCPGraphData(i-n/2, 0);
    mData->add(sortedData, true);
    QVERIFY(isSameData(sortedData, mData));
    QVERIFY(isSorted());
    // add more sorted data with sorted flag, with pre-overlap to old data:
    QVector<QCPGraphData> sortedData2;
    for (int i=0; i<n; ++i)
      sortedData2 << QCPGraphData(i-0.5, 0);
    mData->add(sortedData2, true);
    QVERIFY(isSameData(QVector<QCPGraphData>() << sortedData << sortedData2, mData));
    QVERIFY(isSorted());
    // add more sorted data without sorted flag, with post-overlap to old data:
    QVector<QCPGraphData> sortedData3;
    for (int i=0; i<n; ++i)
      sortedData3 << QCPGraphData(i-n+0.1, 0);
    mData->add(sortedData3, false);
    QVERIFY(isSameData(QVector<QCPGraphData>() << sortedData << sortedData2 << sortedData3, mData));
    QVERIFY(isSorted());
    // add more sorted data with sorted flag, which appends to old data:
    QVector<QCPGraphData> sortedData4;
    for (int i=0; i<n; ++i)
      sortedData4 << QCPGraphData(i+n*5, 0);
    mData->add(sortedData4, true);
    QVERIFY(isSameData(QVector<QCPGraphData>() << sortedData << sortedData2 << sortedData3 << sortedData4, mData));
    QVERIFY(isSorted());
    // add more sorted data with sorted flag, which prepends to old data:
    QVector<QCPGraphData> sortedData5;
    for (int i=0; i<n; ++i)
      sortedData5 << QCPGraphData(i-n*5, 0);
    mData->add(sortedData5, true);
    QVERIFY(isSameData(QVector<QCPGraphData>() << sortedData << sortedData2 << sortedData3 << sortedData4 << sortedData5, mData));
    QVERIFY(isSorted());
    // add more sorted data without sorted flag, which appends to old data:
    QVector<QCPGraphData> sortedData6;
    for (int i=0; i<n; ++i)
      sortedData6 << QCPGraphData(i+n*10, 0);
    mData->add(sortedData6, false);
    QVERIFY(isSameData(QVector<QCPGraphData>() << sortedData << sortedData2 << sortedData3 << sortedData4 << sortedData5 << sortedData6, mData));
    QVERIFY(isSorted());
    // add more sorted data without sorted flag, which prepends to old data:
    QVector<QCPGraphData> sortedData7;
    for (int i=0; i<n; ++i)
      sortedData7 << QCPGraphData(i-n*10, 0);
    mData->add(sortedData7, false);
    QVERIFY(isSameData(QVector<QCPGraphData>() << sortedData << sortedData2 << sortedData3 << sortedData4 << sortedData5 << sortedData6 << sortedData7, mData));
    QVERIFY(isSorted());
  }
}

void TestDatacontainer::addUnsorted()
{
  QList<int> nCases = QList<int>() << 5 << 50 << 100 << 1000 << 2500 << 10000;
  foreach (int n, nCases)
  {
    mData->clear();
    BadRandom r(66, -10, 15);
    // add unsorted data:
    QVector<QCPGraphData> unsortedData;
    for (int i=0; i<n; ++i)
      unsortedData << QCPGraphData(r.get(), 0);
    mData->add(unsortedData, false);
    QVERIFY(isSameData(unsortedData, mData));
    QVERIFY(isSorted());
    // add unsorted data, with pre-overlap to old data:
    QVector<QCPGraphData> unsortedData2;
    for (int i=0; i<n; ++i)
      unsortedData2 << QCPGraphData(r.get()+10, 0);
    mData->add(unsortedData2, false);
    QVERIFY(isSameData(QVector<QCPGraphData>() << unsortedData << unsortedData2, mData));
    QVERIFY(isSorted());
    // add unsorted data, with post-overlap to old data:
    QVector<QCPGraphData> unsortedData3;
    for (int i=0; i<n; ++i)
      unsortedData3 << QCPGraphData(r.get()-15, 0);
    mData->add(unsortedData3, false);
    QVERIFY(isSameData(QVector<QCPGraphData>() << unsortedData << unsortedData2 << unsortedData3, mData));
    QVERIFY(isSorted());
    // add unsorted data which appends to old data:
    QVector<QCPGraphData> unsortedData4;
    for (int i=0; i<n; ++i)
      unsortedData4 << QCPGraphData(r.get()+100, 0);
    mData->add(unsortedData4, false);
    QVERIFY(isSameData(QVector<QCPGraphData>() << unsortedData << unsortedData2 << unsortedData3 << unsortedData4, mData));
    QVERIFY(isSorted());
    // add unsorted data which prepends to old data:
    QVector<QCPGraphData> unsortedData5;
    for (int i=0; i<n; ++i)
      unsortedData5 << QCPGraphData(r.get()-100, 0);
    mData->add(unsortedData5, false);
    QVERIFY(isSameData(QVector<QCPGraphData>() << unsortedData << unsortedData2 << unsortedData3 << unsortedData4 << unsortedData5, mData));
    QVERIFY(isSorted());
  }
}

void TestDatacontainer::remove()
{
  QList<int> nCases = QList<int>() << 5 << 50 << 100 << 1000 << 5000 << 20000;
  foreach (int n, nCases)
  {
    QVector<QCPGraphData> sortedData;
    for (int i=0; i<n; ++i)
      sortedData << QCPGraphData(i-n/2, 0);
    mData->set(sortedData, true);
    while (sortedData.size() > 4)
    {
      const double removeBegin = sortedData.at(sortedData.size()/4).key + 0.1;
      const double removeEnd = -sortedData.at(sortedData.size()/4).key - 0.1;
      int p = 0;
      while (p < sortedData.size() && sortedData.at(p).key <= removeBegin) ++p;
      int pEnd = p;
      while (pEnd < sortedData.size() && sortedData.at(pEnd).key <= removeEnd) ++pEnd;
      if (p < sortedData.size())
        sortedData.remove(p, pEnd-p);
      
      mData->remove(removeBegin, removeEnd);
      QVERIFY(isSameData(sortedData, mData));
      QVERIFY(isSorted());
    }
  }
}

void TestDatacontainer::removeBefore()
{
  QList<int> nCases = QList<int>() << 5 << 50 << 100 << 1000 << 5000 << 20000;
  foreach (int n, nCases)
  {
    QVector<QCPGraphData> sortedData;
    for (int i=0; i<n; ++i)
      sortedData << QCPGraphData(i-n/2, 0);
    mData->set(sortedData, true);
    
    while (sortedData.size() > 4)
    {
      const double beforeKey = sortedData.at(sortedData.size()/4).key+0.1;
      int p = 0;
      while (p < sortedData.size() && sortedData.at(p).key <= beforeKey) ++p;
      sortedData.remove(0, p);
      
      mData->removeBefore(beforeKey);
      QVERIFY(isSameData(sortedData, mData));
      QVERIFY(isSorted());
    }
  }
}

void TestDatacontainer::removeAfter()
{
  QList<int> nCases = QList<int>() << 5 << 50 << 100 << 1000 << 5000 << 20000;
  foreach (int n, nCases)
  {
    QVector<QCPGraphData> sortedData;
    for (int i=0; i<n; ++i)
      sortedData << QCPGraphData(i-n/2, 0);
    mData->set(sortedData, true);
    
    while (sortedData.size() > 4)
    {
      const double afterKey = sortedData.at(sortedData.size()/4*3).key-0.1;
      int p = sortedData.size()-1;
      while (p >= 0 && sortedData.at(p).key >= afterKey) --p;
      sortedData.remove(p+1, sortedData.size()-(p+1));
      
      mData->removeAfter(afterKey);
      QVERIFY(isSameData(sortedData, mData));
      QVERIFY(isSorted());
    }
  }
}

bool TestDatacontainer::isSorted()
{
  if (mData->isEmpty())
    return true;
  QCPDataContainer<QCPGraphData>::const_iterator it = mData->constBegin()+1;
  QCPDataContainer<QCPGraphData>::const_iterator itEnd = mData->constEnd();
  while (it != itEnd)
  {
    if (it->key < (it-1)->key)
      return false;
    ++it;
  }
  return true;
}

bool TestDatacontainer::isSameData(QVector<QCPGraphData> data, const QCPGraphDataContainer *container)
{
  if (data.size() < 6000) // data is small, use slower variant that can detect missing as well as excess data in container
  {
    QCPGraphDataContainer::const_iterator it = container->constBegin();
    QCPGraphDataContainer::const_iterator itEnd = container->constEnd();
    bool success = true;
    while (it != itEnd)
    {
      bool found = false;
      for (int i=0; i<data.size(); ++i)
      {
        if (qFuzzyCompare(data.at(i).key, it->key) && qFuzzyCompare(data.at(i).value, it->value))
        {
          data.remove(i);
          found = true;
          break;
        }
      }
      if (!found)
      {
        qDebug() << "container data not found in vector (data " << it->key << it->value << ")";
        success = false;
      }
      ++it;
    }
    for (int i=0; i<data.size(); ++i) // data vector should be empty now
    {
      qDebug() << "vector data not found in container (data " << data.at(i).key << data.at(i).value << ")";
      success = false;
    }
    return success;
  } else // data is large, use faster variant that can only detect if data is missing in container
  {
    bool success = true;
    for (int i=0; i<data.size(); ++i)
    {
      QCPGraphDataContainer::const_iterator it = container->findBegin(data.at(i).key);
      QCPGraphDataContainer::const_iterator itEnd = container->findEnd(data.at(i).key);
      bool found = false;
      while (it != itEnd)
      {
        if (qFuzzyCompare(data.at(i).key, it->key) && qFuzzyCompare(data.at(i).value, it->value))
        {
          found = true;
          break;
        }
        ++it;
      }
      if (!found)
      {
        qDebug() << "vector data not found in container (data " << data.at(i).key << data.at(i).value << ")";
        success = false;
      }
    }
    return success;
  }
}

void TestDatacontainer::dumpContainer(const QCPGraphDataContainer *container, const QString &csvFilename)
{
  QFile file(csvFilename);
  if (file.open(QFile::ReadWrite))
  {
    QTextStream stream(&file);
    QCPGraphDataContainer::const_iterator it = container->constBegin();
    QCPGraphDataContainer::const_iterator itEnd = container->constEnd();
    while (it != itEnd)
    {
      stream << it->key << "\t" << it->value << "\n";
      ++it;
    }
    file.close();
  } else
    qDebug() << Q_FUNC_INFO << "couldn't write to file" << csvFilename;
}

BadRandom::BadRandom(quint32 seed, double min, double max) :
  mState(seed),
  mMin(min),
  mMax(max)
{
}

double BadRandom::get()
{
  mState = (quint32)(1103515245u*mState + 12345u); // GCC Linear congruential generator
  return mMin+(mMax-mMin)*mState/(double)0xffffffff;
}
