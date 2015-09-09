#include "test-qcpbars.h"
#include <QMainWindow>

void TestQCPBars::init()
{
  mPlot = new QCustomPlot(0);
  mBars = new QCPBars(mPlot->xAxis, mPlot->yAxis);
}

void TestQCPBars::cleanup()
{
  delete mPlot;
}

void TestQCPBars::dataManipulation()
{
  QVERIFY(mBars->data()->isEmpty());
  
  QVector<double> x, y;
  x << -1 << 1 << -2 << 2;
  y <<  1 << 2 <<  0 << 3;
  
  mBars->setData(x, y);
  QCOMPARE(mBars->data()->size(), 4);
  // data should be sorted by x:
  QCOMPARE((mBars->data()->constBegin()+0)->value, 0.0);
  QCOMPARE((mBars->data()->constBegin()+1)->value, 1.0);
  QCOMPARE((mBars->data()->constBegin()+2)->value, 2.0);
  QCOMPARE((mBars->data()->constBegin()+3)->value, 3.0);
  QCOMPARE((mBars->data()->constBegin()+0)->key, -2.0);
  QCOMPARE((mBars->data()->constBegin()+1)->key, -1.0);
  QCOMPARE((mBars->data()->constBegin()+2)->key, 1.0);
  QCOMPARE((mBars->data()->constBegin()+3)->key, 2.0);
  // map key should be identical to data key:
  for (int i=0; i<4; ++i)
    QCOMPARE((mBars->data()->constBegin()+i)->key, (mBars->data()->constBegin()+i)->key);
  
  // data removal:
  mBars->data()->removeBefore(0);
  QCOMPARE(mBars->data()->size(), 2);
  QCOMPARE((mBars->data()->constBegin()+0)->key, 1.0);
  QCOMPARE((mBars->data()->constBegin()+1)->key, 2.0);
  
  mBars->setData(x, y);
  mBars->data()->removeAfter(0);
  QCOMPARE(mBars->data()->size(), 2);
  QCOMPARE((mBars->data()->constBegin()+0)->key, -2.0);
  QCOMPARE((mBars->data()->constBegin()+1)->key, -1.0);
  
  mBars->setData(x, y);
  mBars->data()->remove(-1.1, -0.9);
  QCOMPARE(mBars->data()->size(), 3);
  QCOMPARE((mBars->data()->constBegin()+0)->key, -2.0);
  QCOMPARE((mBars->data()->constBegin()+1)->key, 1.0);
  QCOMPARE((mBars->data()->constBegin()+2)->key, 2.0);
  
  mBars->setData(x, y);
  mBars->data()->remove(-2.1, -1.9);
  QCOMPARE(mBars->data()->size(), 3);
  QCOMPARE((mBars->data()->constBegin()+0)->key, -1.0);
  QCOMPARE((mBars->data()->constBegin()+1)->key, 1.0);
  QCOMPARE((mBars->data()->constBegin()+2)->key, 2.0);
  
  mBars->setData(x, y);
  mBars->data()->remove(1.9, 2.1);
  QCOMPARE(mBars->data()->size(), 3);
  QCOMPARE((mBars->data()->constBegin()+0)->key, -2.0);
  QCOMPARE((mBars->data()->constBegin()+1)->key, -1.0);
  QCOMPARE((mBars->data()->constBegin()+2)->key, 1.0);
  
  mBars->setData(x, y);
  mBars->data()->remove(-1.1, 1.1);
  QCOMPARE(mBars->data()->size(), 2);
  QCOMPARE((mBars->data()->constBegin()+0)->key, -2.0);
  QCOMPARE((mBars->data()->constBegin()+1)->key, 2.0);
  
  mBars->setData(x, y);
  mBars->data()->clear();
  QCOMPARE(mBars->data()->size(), 0);
  
  // add single data points:
  mBars->addData(5, 6);
  QCOMPARE(mBars->data()->size(), 1);
  mBars->addData(4, 5);
  QCOMPARE(mBars->data()->size(), 2);
  QCOMPARE((mBars->data()->constBegin()+0)->value, 5.0);
  QCOMPARE((mBars->data()->constBegin()+1)->value, 6.0);
  
  mBars->setData(x, y);
  mBars->addData(3, 4);
  QCOMPARE(mBars->data()->size(), 5);
  QCOMPARE((mBars->data()->constBegin()+0)->value, 0.0);
  QCOMPARE((mBars->data()->constBegin()+1)->value, 1.0);
  QCOMPARE((mBars->data()->constBegin()+2)->value, 2.0);
  QCOMPARE((mBars->data()->constBegin()+3)->value, 3.0);
  QCOMPARE((mBars->data()->constBegin()+4)->value, 4.0);
  
  mBars->setData(x, y);
  mBars->addData(QVector<double>() << 3 << 4 << 5, QVector<double>() << 4 << 5 << 6);
  QCOMPARE(mBars->data()->size(), 7);
  QCOMPARE((mBars->data()->constBegin()+0)->value, 0.0);
  QCOMPARE((mBars->data()->constBegin()+1)->value, 1.0);
  QCOMPARE((mBars->data()->constBegin()+2)->value, 2.0);
  QCOMPARE((mBars->data()->constBegin()+3)->value, 3.0);
  QCOMPARE((mBars->data()->constBegin()+4)->value, 4.0);
  QCOMPARE((mBars->data()->constBegin()+5)->value, 5.0);
  QCOMPARE((mBars->data()->constBegin()+6)->value, 6.0);
}

void TestQCPBars::dataSharing()
{
  QCPBars *bars2 = new QCPBars(mPlot->xAxis, mPlot->yAxis);
  
  QSharedPointer<QCPBarsDataContainer> data = bars2->data();
  mBars->setData(data); // now shares data with bars2
  
  QVector<double> x, y;
  x << -1 << 1 << -2 << 2;
  y <<  1 << 2 <<  0 << 3;
  bars2->setData(x, y);
  
  QCOMPARE(mBars->data()->size(), 4);
  QCOMPARE(bars2->data()->size(), 4);
  data->add(QCPBarsData(4, 5)); // modify from outside
  QCOMPARE(mBars->data()->size(), 5);
  QCOMPARE(bars2->data()->size(), 5);
  data.clear(); // clear external reference, internals should live on
  QVERIFY(!data);
  QCOMPARE(mBars->data()->size(), 5);
  QCOMPARE(bars2->data()->size(), 5);
  
  // decouple data again:
  mBars->setData(QSharedPointer<QCPBarsDataContainer>(new QCPBarsDataContainer));
  mBars->addData(1, 1);
  bars2->addData(4, 5);
  QCOMPARE(mBars->data()->size(), 1);
  QCOMPARE(bars2->data()->size(), 6);
}
