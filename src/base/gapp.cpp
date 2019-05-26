#include "gapp.h"
#include <GGraphWidget>

// ----------------------------------------------------------------------------
// GApp
// ----------------------------------------------------------------------------
#ifdef QT_GUI_LIB
GApp::GApp(int &argc, char **argv) : QApplication (argc, argv) {
#else
GApp::GApp(int &argc, char **argv) : QCoreApplication (argc, argv) {
#endif // QT_GUI_LIB
  init();
}

void GApp::init() {
  qSetMessagePattern("%{time yyyyMMdd h:mm:ss.zzz} %{threadid} [%{file}:%{line}] %{message}");
}

#ifdef QT_GUI_LIB
bool GApp::execObj(GObj* obj) {
  GPropWidget propWidget(obj);

  QJsonObject jo = GJson::loadFromFile();
  jo["dept"] >> *obj;
  jo["propWidget"] >> propWidget;

  propWidget.update();
  propWidget.show();
  int res = QApplication::exec();

  jo["dept"] << *obj;
  jo["propWidget"] << propWidget;

  GJson::saveToFile(jo);
  return res;
}

bool GApp::execFactory(GPluginFactory* pluginFactory) {
  GGraph graph;
  if (pluginFactory == nullptr) {
    pluginFactory = &GPluginFactory::instance();
    QString folder = QCoreApplication::applicationDirPath() + "/plugin";
    pluginFactory->load(folder);
  }
  graph.setFactory(pluginFactory);

  GGraphWidget graphWidget;
  graphWidget.setGraph(&graph);

  QJsonObject jo = GJson::loadFromFile();
  jo["graphWidget"] >> graphWidget;

  graphWidget.update();
  graphWidget.show();
  int res = QApplication::exec();

  jo["graphWidget"] << graphWidget;

  GJson::saveToFile(jo);
  return res;
}
#endif // QT_GUI_LIB
