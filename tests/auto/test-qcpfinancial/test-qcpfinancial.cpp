#include "test-qcpfinancial.h"
#include <QMainWindow>

void TestQCPFinancial::init()
{
  mPlot = new QCustomPlot(0);
  mFinancial = new QCPFinancial(mPlot->xAxis, mPlot->yAxis);
}

void TestQCPFinancial::cleanup()
{
  delete mPlot;
}

void TestQCPFinancial::dataManipulation()
{
  QVERIFY(mFinancial->data()->isEmpty());
  
  QVector<double> key, open, high, low, close;
  key << -1 << 1 << -2 << 2;
  open << 11 << 13 << 15 << 17;
  high << 20 << 22 << 24 << 26;
  low << 10 << 12 << 14 << 16;
  close << 12 << 14 << 16 << 18;
  
  mFinancial->setData(key, open, high, low, close);
  QCOMPARE(mFinancial->data()->size(), 4);
  // data should be sorted by key:
  QCOMPARE((mFinancial->data()->constBegin()+0)->key, -2.0);
  QCOMPARE((mFinancial->data()->constBegin()+1)->key, -1.0);
  QCOMPARE((mFinancial->data()->constBegin()+2)->key, 1.0);
  QCOMPARE((mFinancial->data()->constBegin()+3)->key, 2.0);
  QCOMPARE((mFinancial->data()->constBegin()+0)->open, 15.0);
  QCOMPARE((mFinancial->data()->constBegin()+1)->open, 11.0);
  QCOMPARE((mFinancial->data()->constBegin()+2)->open, 13.0);
  QCOMPARE((mFinancial->data()->constBegin()+3)->open, 17.0);
  QCOMPARE((mFinancial->data()->constBegin()+0)->high, 24.0);
  QCOMPARE((mFinancial->data()->constBegin()+1)->high, 20.0);
  QCOMPARE((mFinancial->data()->constBegin()+2)->high, 22.0);
  QCOMPARE((mFinancial->data()->constBegin()+3)->high, 26.0);
  QCOMPARE((mFinancial->data()->constBegin()+0)->low, 14.0);
  QCOMPARE((mFinancial->data()->constBegin()+1)->low, 10.0);
  QCOMPARE((mFinancial->data()->constBegin()+2)->low, 12.0);
  QCOMPARE((mFinancial->data()->constBegin()+3)->low, 16.0);
  QCOMPARE((mFinancial->data()->constBegin()+0)->close, 16.0);
  QCOMPARE((mFinancial->data()->constBegin()+1)->close, 12.0);
  QCOMPARE((mFinancial->data()->constBegin()+2)->close, 14.0);
  QCOMPARE((mFinancial->data()->constBegin()+3)->close, 18.0);

  // map key should be identical to data key:
  for (int i=0; i<4; ++i)
    QCOMPARE((mFinancial->data()->constBegin()+i)->key, (mFinancial->data()->constBegin()+i)->key);
  
  // data removal:
  mFinancial->data()->removeBefore(0);
  QCOMPARE(mFinancial->data()->size(), 2);
  QCOMPARE((mFinancial->data()->constBegin()+0)->key, 1.0);
  QCOMPARE((mFinancial->data()->constBegin()+1)->key, 2.0);
  
  mFinancial->setData(key, open, high, low, close);
  mFinancial->data()->removeAfter(0);
  QCOMPARE(mFinancial->data()->size(), 2);
  QCOMPARE((mFinancial->data()->constBegin()+0)->key, -2.0);
  QCOMPARE((mFinancial->data()->constBegin()+1)->key, -1.0);
  
  mFinancial->setData(key, open, high, low, close);
  mFinancial->data()->remove(-1.1, -0.9);
  QCOMPARE(mFinancial->data()->size(), 3);
  QCOMPARE((mFinancial->data()->constBegin()+0)->key, -2.0);
  QCOMPARE((mFinancial->data()->constBegin()+1)->key, 1.0);
  QCOMPARE((mFinancial->data()->constBegin()+2)->key, 2.0);
  
  mFinancial->setData(key, open, high, low, close);
  mFinancial->data()->remove(-2.1, -1.9);
  QCOMPARE(mFinancial->data()->size(), 3);
  QCOMPARE((mFinancial->data()->constBegin()+0)->key, -1.0);
  QCOMPARE((mFinancial->data()->constBegin()+1)->key, 1.0);
  QCOMPARE((mFinancial->data()->constBegin()+2)->key, 2.0);
  
  mFinancial->setData(key, open, high, low, close);
  mFinancial->data()->remove(1.9, 2.1);
  QCOMPARE(mFinancial->data()->size(), 3);
  QCOMPARE((mFinancial->data()->constBegin()+0)->key, -2.0);
  QCOMPARE((mFinancial->data()->constBegin()+1)->key, -1.0);
  QCOMPARE((mFinancial->data()->constBegin()+2)->key, 1.0);
  
  mFinancial->setData(key, open, high, low, close);
  mFinancial->data()->remove(-1.1, 1.1);
  QCOMPARE(mFinancial->data()->size(), 2);
  QCOMPARE((mFinancial->data()->constBegin()+0)->key, -2.0);
  QCOMPARE((mFinancial->data()->constBegin()+1)->key, 2.0);
  
  mFinancial->setData(key, open, high, low, close);
  mFinancial->data()->clear();
  QCOMPARE(mFinancial->data()->size(), 0);
  
  // add single data points:
  mFinancial->addData(5, 19, 28, 18, 20);
  QCOMPARE(mFinancial->data()->size(), 1);
  mFinancial->addData(4, 21, 30, 20, 22);
  QCOMPARE(mFinancial->data()->size(), 2);
  QCOMPARE((mFinancial->data()->constBegin()+0)->open, 21.0);
  QCOMPARE((mFinancial->data()->constBegin()+1)->open, 19.0);
  
  mFinancial->setData(key, open, high, low, close);
  mFinancial->addData(3, 19, 28, 18, 20);
  QCOMPARE(mFinancial->data()->size(), 5);
  QCOMPARE((mFinancial->data()->constBegin()+0)->open, 15.0);
  QCOMPARE((mFinancial->data()->constBegin()+1)->open, 11.0);
  QCOMPARE((mFinancial->data()->constBegin()+2)->open, 13.0);
  QCOMPARE((mFinancial->data()->constBegin()+3)->open, 17.0);
  QCOMPARE((mFinancial->data()->constBegin()+4)->open, 19.0);
  
  mFinancial->setData(key, open, high, low, close);
  mFinancial->addData(QVector<double>() << 3 << 4 << 5,
                      QVector<double>() << 19 << 21 << 23,
                      QVector<double>() << 28 << 30 << 32,
                      QVector<double>() << 18 << 20 << 22,
                      QVector<double>() << 20 << 22 << 24);
  QCOMPARE(mFinancial->data()->size(), 7);
  QCOMPARE((mFinancial->data()->constBegin()+0)->open, 15.0);
  QCOMPARE((mFinancial->data()->constBegin()+1)->open, 11.0);
  QCOMPARE((mFinancial->data()->constBegin()+2)->open, 13.0);
  QCOMPARE((mFinancial->data()->constBegin()+3)->open, 17.0);
  QCOMPARE((mFinancial->data()->constBegin()+4)->open, 19.0);
  QCOMPARE((mFinancial->data()->constBegin()+5)->open, 21.0);
  QCOMPARE((mFinancial->data()->constBegin()+6)->open, 23.0);
}

void TestQCPFinancial::dataSharing()
{
  QCPFinancial *financial2 = new QCPFinancial(mPlot->xAxis, mPlot->yAxis);
  
  QSharedPointer<QCPFinancialDataContainer> data = financial2->data();
  mFinancial->setData(data); // now shares data with financial2
  
  QVector<double> key, open, high, low, close;
  key << -1 << 1 << -2 << 2;
  open << 11 << 13 << 15 << 17;
  high << 20 << 22 << 24 << 26;
  low << 10 << 12 << 14 << 16;
  close << 12 << 14 << 16 << 18;
  financial2->setData(key, open, high, low, close);
  
  QCOMPARE(mFinancial->data()->size(), 4);
  QCOMPARE(financial2->data()->size(), 4);
  data->add(QCPFinancialData(4, 19, 28, 18, 20)); // modify from outside
  QCOMPARE(mFinancial->data()->size(), 5);
  QCOMPARE(financial2->data()->size(), 5);
  data.clear(); // clear external reference, internals should live on
  QVERIFY(!data);
  QCOMPARE(mFinancial->data()->size(), 5);
  QCOMPARE(financial2->data()->size(), 5);
  
  // decouple data again:
  mFinancial->setData(QSharedPointer<QCPFinancialDataContainer>(new QCPFinancialDataContainer));
  mFinancial->addData(1, 1, 6, 0, 3);
  financial2->addData(4, 1, 6, 0, 3);
  QCOMPARE(mFinancial->data()->size(), 1);
  QCOMPARE(financial2->data()->size(), 6);
}

