#include <QtTest/QtTest>
#include "../../../qcustomplot.h"

class TestQCPBars : public QObject
{
  Q_OBJECT
private slots:
  void init();
  void cleanup();
  
  void dataSharing();
  void dataManipulation();
  
private:
  QCustomPlot *mPlot;
  QCPBars *mBars;
};





