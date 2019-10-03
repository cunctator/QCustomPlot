INCLUDEPATH *= $${PWD}/

LIBS += -L$$PWD
CONFIG(debug, debug|release) {
  LIBS += -lqcustomplotd
} else {
  LIBS += -lqcustomplot
}
