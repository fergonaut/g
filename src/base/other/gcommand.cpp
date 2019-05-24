#include "gcommand.h"
#include <QProcess>

// ----------------------------------------------------------------------------
// GCommand
// ----------------------------------------------------------------------------
GCommand::GCommand(QObject* parent) : GStateObj(parent) {
}

GCommand::~GCommand() {
  close();
}

bool GCommand::doOpen() {
  bool res = true;
  for (QString command: openCommands_) {
    if (command.isEmpty()) continue;
    int i = QProcess::execute(command);
    if (i != EXIT_SUCCESS) {
      QString msg = QString("command(%1) return %2").arg(command).arg(2);
      SET_ERR(GErr::FAIL, msg);
      res = false;
    }
  }
  return res;
}

bool GCommand::doClose() {
  bool res = true;
  for (QString command: closeCommands_) {
    if (command.isEmpty()) continue;
    int i = QProcess::execute(command);
    if (i != EXIT_SUCCESS) {
      QString msg = QString("command(%1) return %2").arg(command).arg(2);
      SET_ERR(GErr::FAIL, msg);
      res = false;
    }
  }
  return res;
}
