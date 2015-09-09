#include "test-qcpcurve.h"
#include <QMainWindow>

void TestQCPCurve::init()
{
  mPlot = new QCustomPlot(0);
  mCurve = new QCPCurve(mPlot->xAxis, mPlot->yAxis);
}

void TestQCPCurve::cleanup()
{
  delete mPlot;
}

void TestQCPCurve::dataManipulation()
{
  QVERIFY(mCurve->data()->isEmpty());
  
  QVector<double> t, x, y;
  t << -1 << 1 << -2 << 2;
  x <<  1 << 2 << 3 << 4;
  y << 10 << 9 << 12 << 15;
  
  mCurve->setData(t, x, y);
  QCOMPARE(mCurve->data()->size(), 4);
  // data should be sorted by t:
  QCOMPARE((mCurve->data()->constBegin()+0)->t, -2.0);
  QCOMPARE((mCurve->data()->begin()+1)->t, -1.0);
  QCOMPARE((mCurve->data()->constBegin()+2)->t, 1.0);
  QCOMPARE((mCurve->data()->constBegin()+3)->t, 2.0);
  QCOMPARE((mCurve->data()->constBegin()+0)->value, 12.0);
  QCOMPARE((mCurve->data()->constBegin()+1)->value, 10.0);
  QCOMPARE((mCurve->data()->constBegin()+2)->value, 9.0);
  QCOMPARE((mCurve->data()->constBegin()+3)->value, 15.0);
  QCOMPARE((mCurve->data()->constBegin()+0)->key, 3.0);
  QCOMPARE((mCurve->data()->constBegin()+1)->key, 1.0);
  QCOMPARE((mCurve->data()->constBegin()+2)->key, 2.0);
  QCOMPARE((mCurve->data()->constBegin()+3)->key, 4.0);
  
  // data removal:
  mCurve->data()->removeBefore(0);
  QCOMPARE(mCurve->data()->size(), 2);
  QCOMPARE((mCurve->data()->constBegin()+0)->t, 1.0);
  QCOMPARE((mCurve->data()->constBegin()+1)->t, 2.0);
  
  mCurve->setData(t, x, y);
  mCurve->data()->removeAfter(0);
  QCOMPARE(mCurve->data()->size(), 2);
  QCOMPARE((mCurve->data()->constBegin()+0)->t, -2.0);
  QCOMPARE((mCurve->data()->constBegin()+1)->t, -1.0);
  
  mCurve->setData(t, x, y);
  mCurve->data()->remove(-1.1, -0.9);
  QCOMPARE(mCurve->data()->size(), 3);
  QCOMPARE((mCurve->data()->constBegin()+0)->t, -2.0);
  QCOMPARE((mCurve->data()->constBegin()+1)->t, 1.0);
  QCOMPARE((mCurve->data()->constBegin()+2)->t, 2.0);
  
  mCurve->setData(t, x, y);
  mCurve->data()->remove(-2.1, -1.9);
  QCOMPARE(mCurve->data()->size(), 3);
  QCOMPARE((mCurve->data()->constBegin()+0)->t, -1.0);
  QCOMPARE((mCurve->data()->constBegin()+1)->t, 1.0);
  QCOMPARE((mCurve->data()->constBegin()+2)->t, 2.0);
  
  mCurve->setData(t, x, y);
  mCurve->data()->remove(1.9, 2.1);
  QCOMPARE(mCurve->data()->size(), 3);
  QCOMPARE((mCurve->data()->constBegin()+0)->t, -2.0);
  QCOMPARE((mCurve->data()->constBegin()+1)->t, -1.0);
  QCOMPARE((mCurve->data()->constBegin()+2)->t, 1.0);
  
  mCurve->setData(t, x, y);
  mCurve->data()->remove(-1.1, 1.1);
  QCOMPARE(mCurve->data()->size(), 2);
  QCOMPARE((mCurve->data()->constBegin()+0)->t, -2.0);
  QCOMPARE((mCurve->data()->constBegin()+1)->t, 2.0);
  
  mCurve->setData(t, x, y);
  mCurve->data()->clear();
  QCOMPARE(mCurve->data()->size(), 0);
  
  // add single data points:
  mCurve->addData(5, 12, 6);
  QCOMPARE(mCurve->data()->size(), 1);
  mCurve->addData(4, 14, 5);
  QCOMPARE(mCurve->data()->size(), 2);
  QCOMPARE((mCurve->data()->constBegin()+0)->value, 5.0);
  QCOMPARE((mCurve->data()->constBegin()+1)->value, 6.0);
  
  mCurve->setData(t, x, y);
  mCurve->addData(3, 12, 4);
  QCOMPARE(mCurve->data()->size(), 5);
  QCOMPARE((mCurve->data()->constBegin()+0)->value, 12.0);
  QCOMPARE((mCurve->data()->constBegin()+1)->value, 10.0);
  QCOMPARE((mCurve->data()->constBegin()+2)->value, 9.0);
  QCOMPARE((mCurve->data()->constBegin()+3)->value, 15.0);
  QCOMPARE((mCurve->data()->constBegin()+4)->value, 4.0);
  
  mCurve->setData(t, x, y);
  mCurve->addData(QVector<double>() << 3 << 4 << 5, QVector<double>() << 17 << 16 << 15, QVector<double>() << 4 << 5 << 6);
  QCOMPARE(mCurve->data()->size(), 7);
  QCOMPARE((mCurve->data()->constBegin()+0)->value, 12.0);
  QCOMPARE((mCurve->data()->constBegin()+1)->value, 10.0);
  QCOMPARE((mCurve->data()->constBegin()+2)->value, 9.0);
  QCOMPARE((mCurve->data()->constBegin()+3)->value, 15.0);
  QCOMPARE((mCurve->data()->constBegin()+4)->value, 4.0);
  QCOMPARE((mCurve->data()->constBegin()+5)->value, 5.0);
  QCOMPARE((mCurve->data()->constBegin()+6)->value, 6.0);
}

void TestQCPCurve::dataSharing()
{
  QCPCurve *curve2 = new QCPCurve(mPlot->xAxis, mPlot->yAxis);
  
  QSharedPointer<QCPCurveDataContainer> data = curve2->data();
  mCurve->setData(data); // now shares data with curve2
  
  QVector<double> t, x, y;
  t << -1 << 1 << -2 << 2;
  x <<  1 << 2 <<  0 << 3;
  y <<  10 << 9 <<  12 << 15;
  curve2->setData(t, x, y);
  
  QCOMPARE(mCurve->data()->size(), 4);
  QCOMPARE(curve2->data()->size(), 4);
  data->add(QCPCurveData(4, 9, 5)); // modify from outside
  QCOMPARE(mCurve->data()->size(), 5);
  QCOMPARE(curve2->data()->size(), 5);
  data.clear(); // clear external reference, internals should live on
  QVERIFY(!data);
  QCOMPARE(mCurve->data()->size(), 5);
  QCOMPARE(curve2->data()->size(), 5);
  
  // decouple data again:
  mCurve->setData(QSharedPointer<QCPCurveDataContainer>(new QCPCurveDataContainer));
  mCurve->addData(1, 1);
  curve2->addData(4, 5);
  QCOMPARE(mCurve->data()->size(), 1);
  QCOMPARE(curve2->data()->size(), 6);
}

