#include "test-qcustomplot/test-qcustomplot.h"
#include "test-qcpgraph/test-qcpgraph.h"
#include "test-qcpcurve/test-qcpcurve.h"
#include "test-qcpbars/test-qcpbars.h"
#include "test-qcpfinancial/test-qcpfinancial.h"
#include "test-colormap/test-colormap.h"
#include "test-qcplayout/test-qcplayout.h"
#include "test-qcplegend/test-qcplegend.h"
#include "test-qcpaxisrect/test-qcpaxisrect.h"
#include "test-datacontainer/test-datacontainer.h"

#define QCPTEST(t) t t##instance; QTest::qExec(&t##instance)

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  
  QCPTEST(TestQCustomPlot);
  QCPTEST(TestQCPGraph);
  QCPTEST(TestQCPCurve);
  QCPTEST(TestQCPBars);
  QCPTEST(TestQCPFinancial);
  QCPTEST(TestColorMap);
  QCPTEST(TestQCPLayout);
  QCPTEST(TestQCPLegend);
  QCPTEST(TestQCPAxisRect);
  QCPTEST(TestDatacontainer);
  
  return 0;
}
