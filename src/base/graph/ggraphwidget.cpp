#ifdef QT_GUI_LIB

#include "ggraphwidget.h"

#include <QMessageBox>
#include "base/gjson.h"

// ----------------------------------------------------------------------------
// GGraphWidget
// ----------------------------------------------------------------------------
GGraphWidget::GGraphWidget(QWidget *parent) : QWidget(parent) {
  init();
  setControl();
}

GGraphWidget::~GGraphWidget() {
  actionStop_->trigger();
  clear();
}

void GGraphWidget::init() {
  resize(QSize(640, 480));

  QStringList filters; filters << "graph files(*.graph)" << "any files(*)";
  fileDialog_.setNameFilters(filters);
  fileDialog_.setDefaultSuffix("graph");
  fileDialog_.setViewMode(QFileDialog::Detail);

  (actionNewFile_ = new QAction(this))->setText("New");
  (actionOpenFile_ = new QAction(this))->setText("Open");
  (actionSaveFile_ = new QAction(this))->setText("Save");
  (actionSaveFileAs_ = new QAction(this))->setText("Save As");
  (actionStart_ = new QAction(this))->setText("Start");
  (actionStop_ = new QAction(this))->setText("Stop");
  (actionEdit_ = new QAction(this))->setText("Edit");
  (actionLink_ = new QAction(this))->setText("Link");
  (actionDelete_ = new QAction(this))->setText("Delete");
  (actionOption_ = new QAction(this))->setText("Option");

  mainLayout_ = new QVBoxLayout(this);
  mainLayout_->setMargin(0);
  mainLayout_->setSpacing(0);

  toolBar_ = new QToolBar(this);
  midSplitter_ = new QSplitter(Qt::Horizontal, this);
  midLeftSplitter_ = new QSplitter(Qt::Vertical, this);
  (factoryWidget_ = new QTreeWidget(this))->setHeaderLabel("node");
  propWidget_ = new GPropWidget(this);
  scene_ = new GGScene(this);
  graphView_ = new QGraphicsView(this);
  graphView_->setRenderHint(QPainter::Antialiasing);
  graphView_->setAcceptDrops(true);
  graphView_->setScene(scene_);
  statusBar_ = new QStatusBar(this);

  mainLayout_->addWidget(toolBar_ , 0);
  mainLayout_->addWidget(midSplitter_, 1);
  mainLayout_->addWidget(statusBar_, 0);
  this->setLayout(mainLayout_);

  midSplitter_->addWidget(midLeftSplitter_);
  midSplitter_->addWidget(graphView_);
  midSplitter_->setStretchFactor(0, 0);
  midSplitter_->setStretchFactor(1, 1);

  midLeftSplitter_->addWidget(factoryWidget_);
  midLeftSplitter_->addWidget(propWidget_);
  midLeftSplitter_->setStretchFactor(0, 0);
  midLeftSplitter_->setStretchFactor(1, 1);

  toolBar_->addAction(actionNewFile_);
  toolBar_->addAction(actionOpenFile_);
  toolBar_->addAction(actionSaveFile_);
  toolBar_->addAction(actionSaveFileAs_);
  toolBar_->addSeparator();
  toolBar_->addAction(actionStart_);
  toolBar_->addAction(actionStop_);
  toolBar_->addSeparator();
  toolBar_->addAction(actionEdit_);
  toolBar_->addAction(actionLink_);
  toolBar_->addSeparator();
  toolBar_->addAction(actionDelete_);
  toolBar_->addSeparator();
  toolBar_->addAction(actionOption_);

  QObject::connect(actionNewFile_, &QAction::triggered, this, &GGraphWidget::actionNewFileTriggered);
  QObject::connect(actionOpenFile_, &QAction::triggered, this, &GGraphWidget::actionOpenFileTriggered);
  QObject::connect(actionSaveFile_, &QAction::triggered, this, &GGraphWidget::actionSaveFileTriggered);
  QObject::connect(actionSaveFileAs_, &QAction::triggered, this, &GGraphWidget::actionSaveFileAsTriggered);
  QObject::connect(actionStart_, &QAction::triggered, this, &GGraphWidget::actionStartTriggered);
  QObject::connect(actionStop_, &QAction::triggered, this, &GGraphWidget::actionStopTriggered);
  QObject::connect(actionEdit_, &QAction::triggered, this, &GGraphWidget::actionEditTriggered);
  QObject::connect(actionLink_, &QAction::triggered, this, &GGraphWidget::actionLinkTriggered);
  QObject::connect(actionDelete_, &QAction::triggered, this, &GGraphWidget::actionDeleteTriggered);
  QObject::connect(actionOption_, &QAction::triggered, this, &GGraphWidget::actionOptionTriggered);

  QObject::connect(factoryWidget_, &QTreeWidget::clicked, this, &GGraphWidget::factoryWidgetClicked);
  QObject::connect(scene_, &GGScene::selectionChanged, this, &GGraphWidget::setControl);
}

void GGraphWidget::setGraph(GGraph* graph) {
  if (graph == graph_) return;
  graph_ = graph;
  update();
  setControl();
}

void GGraphWidget::update() {
  factoryWidget_->clear();
  Q_ASSERT(graph_ != nullptr);
  GGraph::Factory* factory = graph_->factory();
  Q_ASSERT(factory!= nullptr);
  foreach(GGraph::Factory::Item* item, factory->items_) {
    updateFactory(item, nullptr);
  }
  factoryWidget_->expandAll();
}

void GGraphWidget::clear() {
  fileName_ = "";
  graph_->clear();
  scene_->clear();
}

void GGraphWidget::loadGraph(QJsonObject jo) {
  graph_->propLoad(jo);

  QJsonArray nodeJa = jo["nodes"].toArray();
  foreach (QJsonValue jv, nodeJa) {
    QJsonObject nodeJo = jv.toObject();
    QString objectName = nodeJo["objectName"].toString();
    if (objectName == "") {
      qWarning() << "objectName is empty" << nodeJo;
      continue;
    }
    GGraph::Node* node = graph_->nodes_.findNode(objectName);
    if (node == nullptr) {
      qWarning() << "node is null" << nodeJo;
      continue;
    }
    qreal x = nodeJo["_x"].toVariant().toFloat();
    qreal y = nodeJo["_y"].toVariant().toFloat();
    QPointF pos(x, y);
    scene_->createText(node, pos);
  }

  foreach (GGraph::Connection* connection, graph_->connections_) {
    QString startNodeName = connection->sender_->objectName();
    QString endNodeName = connection->receiver_->objectName();
    scene_->createArrow(startNodeName, endNodeName, connection);
  }
}

void GGraphWidget::saveGraph(QJsonObject& jo) {
  graph_->propSave(jo);

  QJsonArray nodeJa = jo["nodes"].toArray();
  for (int i = 0; i < nodeJa.count(); i++) {
    QJsonObject nodeJo = nodeJa.at(i).toObject();
    QString objectName = nodeJo["objectName"].toString();
    if (objectName == "") {
      qWarning() << "objectName is empty" << nodeJo;
      continue;
    }
    GGText* text = scene_->findTextByObjectName(objectName);
    if (text == nullptr) {
      qWarning() << "text is null" << nodeJo;
      continue;
    }
    nodeJo["_x"] = text->pos().x();
    nodeJo["_y"] = text->pos().y();
    nodeJa.removeAt(i);
    nodeJa.insert(i, nodeJo);
  }
  jo["nodes"] = nodeJa;
}

void GGraphWidget::updateFactory(GGraph::Factory::Item* item, QTreeWidgetItem* parent) {
  QTreeWidgetItem* newWidgetItem;
  if (parent != nullptr)
    newWidgetItem = new QTreeWidgetItem(parent);
  else
    newWidgetItem = new QTreeWidgetItem(factoryWidget_);

  newWidgetItem->setText(0, item->name_);
  QVariant v = QVariant::fromValue((void*)item);
  newWidgetItem->setData(0, Qt::UserRole, v);

  GGraph::Factory::ItemCategory* category = dynamic_cast<GGraph::Factory::ItemCategory*>(item);
  if (category != nullptr) {
    foreach (GGraph::Factory::Item* child, category->items_) {
      updateFactory(child, newWidgetItem);
    }
    return;
  }
}

GGraph::Node* GGraphWidget::createInstance(QString className) {
  GGraph::Node* node = graph()->createInstance(className);
  if (node == nullptr) return nullptr;

  QString objectName = node->metaObject()->className();
  foreach (QString removePrefixName, removePrefixNames_) {
    if (objectName.startsWith(removePrefixName))
      objectName = objectName.mid(removePrefixName.length());
  }
  if (objectName == "") {
    qWarning() << QString("objectName is empty for (%1)").arg(className);
    delete node;
    return nullptr;
  }
  if (toLowerFirstCharacter_) {
    char first = objectName.at(0).toLatin1();
    if (first >= 'A' && first <= 'Z') {
      first = first + 'a' - 'A';
      objectName = QString(first) + objectName.mid(1);
    }
  }

  int suffix = 1;
  while (true) {
    QString _objectName = objectName + QString::number(suffix);
    bool isExist = false;
    foreach(GGraph::Node* node, graph()->nodes_) {
      if (node->objectName() == _objectName) {
        isExist = true;
        break;
      }
    }
    if (!isExist) break;
    suffix++;
  }
  objectName = objectName + QString::number(suffix);
  node->setObjectName(objectName);

  return node;
}

GGraph::Node* GGraphWidget::createNodeIfItemNodeSelected() {
  QList<QTreeWidgetItem*> widgetItems = factoryWidget_->selectedItems();
  if (widgetItems.count() == 0)
    return nullptr;
  QTreeWidgetItem* widgetItem = widgetItems.at(0);
  QVariant variant = widgetItem->data(0, Qt::UserRole);
  void* p = qvariant_cast<void*>(variant);
  GGraph::Factory::Item* item = dynamic_cast<GGraph::Factory::Item*>((GGraph::Factory::Item*)p);
  GGraph::Factory::ItemNode* itemNode = dynamic_cast<GGraph::Factory::ItemNode*>(item);
  if (itemNode == nullptr)
    return nullptr;
  QString className = itemNode->mobj_->className();
  GGraph::Node* node = createInstance(className);
  if (node == nullptr) {
    QString msg = QString("createInstance failed for (%1)").arg(className);
    QMessageBox::warning(nullptr, "Error", msg);
    return nullptr;
  }
  node->setParent(graph_);
  graph_->nodes_.push_back(node);
  return node;
}

void GGraphWidget::propLoad(QJsonObject jo) {
  jo["rect"] >> GJson::rect(this);

  QJsonObject splitter = jo["splitter"].toObject();
  splitter["mid"] >> GJson::splitterSizes(midSplitter_);
  splitter["left"] >> GJson::splitterSizes(midLeftSplitter_);
  splitter["prop"] >> GJson::headerSizes(propWidget_);

  toLowerFirstCharacter_ = jo["toLowerFirstCharacter"].toBool();
  removePrefixNames_ = jo["removePrefixNames"].toString().split(",");
  ignoreSignalNames_ = jo["ignoreSignalNames"].toString().split(",");
  ignoreSlotNames_ = jo["ignoreSlotNames"].toString().split(",");

  loadGraph(jo["graph"].toObject());
}

void GGraphWidget::propSave(QJsonObject& jo) {
  jo["rect"] << GJson::rect(this);

  QJsonObject splitter;
  splitter["mid"] << GJson::splitterSizes(midSplitter_);
  splitter["left"] << GJson::splitterSizes(midLeftSplitter_);
  splitter["prop"] << GJson::headerSizes(propWidget_);
  jo["splitter"] = splitter;

  jo["toLowerFirstCharacter"] = toLowerFirstCharacter_;
  jo["removePrefixNames"] = removePrefixNames_.join(",");
  jo["ignoreSignalNames"] = ignoreSignalNames_.join(",");
  jo["ignoreSlotNames"] = ignoreSlotNames_.join(",");

  QJsonObject graphJo;
  saveGraph(graphJo);
  jo["graph"] = graphJo;
}

void GGraphWidget::setControl() {
  QString title = "SnoopSpy";
  if (fileName_ != "")
  {
    QFileInfo fi(fileName_);
    title = fi.completeBaseName();
  }
  setWindowTitle(title);

  GGScene::Mode mode = scene_->mode();

  actionEdit_->setEnabled(mode != GGScene::MoveItem);
  actionLink_->setEnabled(mode != GGScene::InsertLine);

  bool active = false;
  if (graph_ != nullptr)
    active = graph_->active();

  actionNewFile_->setEnabled(!active);
  actionOpenFile_->setEnabled(!active);
  actionSaveFile_->setEnabled(!active && fileName_ != "");
  actionSaveFileAs_->setEnabled(!active);
  actionStart_->setEnabled(!active);
  actionStop_->setEnabled(active);

  bool selected = scene_->selectedItems().count() > 0;
  actionDelete_->setEnabled(selected);
  GObj* selectedObj = nullptr;
  if (selected)
  {
    QGraphicsItem* item = scene_->selectedItems().first();
    GGText* text = dynamic_cast<GGText*>(item);
    if (text != nullptr)
      selectedObj = dynamic_cast<GObj*>(text->node_);
  }
  propWidget_->setObject(selectedObj);
  actionOption_->setEnabled(selectedObj != nullptr);
}

void GGraphWidget::actionNewFileTriggered(bool) {
  clear();
  setControl();
}

void GGraphWidget::actionOpenFileTriggered(bool) {
  fileDialog_.setAcceptMode(QFileDialog::AcceptOpen);
  fileDialog_.setFileMode(QFileDialog::ExistingFile);
  if (fileDialog_.exec() == QDialog::Accepted) {
    fileName_ = fileDialog_.selectedFiles().first();
    QJsonObject jo = GJson::loadFromFile(fileName_);
    loadGraph(jo);
    setControl();
  }
}

void GGraphWidget::actionSaveFileTriggered(bool) {
  QJsonObject jo;
  saveGraph(jo);
  GJson::saveToFile(jo, fileName_);
  setControl();
}

void GGraphWidget::actionSaveFileAsTriggered(bool) {
  fileDialog_.setAcceptMode(QFileDialog::AcceptSave);
  fileDialog_.setFileMode(QFileDialog::AnyFile);
  if (fileDialog_.exec() == QDialog::Accepted)
  {
    fileName_ = fileDialog_.selectedFiles().first();
    actionSaveFileTriggered(false);
  }
}

void GGraphWidget::actionStartTriggered(bool) {
  bool res = graph_->open();
  if (!res) {
    QString msg = graph_->err->msg();
    QMessageBox::warning(nullptr, "Error", msg);
  }
  setControl();
}

void GGraphWidget::actionStopTriggered(bool) {
  graph_->close();
  setControl();
}

void GGraphWidget::actionEditTriggered(bool) {
  scene_->setMode(GGScene::MoveItem);
  setControl();
}

void GGraphWidget::actionLinkTriggered(bool) {
  scene_->setMode(GGScene::InsertLine);
  setControl();
}

void GGraphWidget::actionDeleteTriggered(bool) {
  if (scene_->selectedItems().count() == 0)
    return;
  QGraphicsItem* item = scene_->selectedItems().first();

  GGText* text = dynamic_cast<GGText*>(item);
  if (text != nullptr) {
    GGraph::Node* node = text->node_;

    foreach (QGraphicsItem* item, scene_->items()) {
      GGArrow* arrow = dynamic_cast<GGArrow*>(item);
      if (arrow == nullptr) continue;
      if (arrow->startText() == text || arrow->endText() == text) {
        GGraph::Connection* connection = arrow->connection_;
        GObj::disconnect(
          connection->sender_, qPrintable(connection->signal_),
          connection->receiver_, qPrintable(connection->slot_));
        graph_->connections_.removeOne(connection);
        delete connection;
        delete arrow;
      }
    }
    graph_->nodes_.removeOne(node);
    delete node;
    delete text;
  }

  GGArrow* arrow = dynamic_cast<GGArrow*>(item);
  if (arrow != nullptr) {
    GGraph::Connection* connection = arrow->connection_;
    GObj::disconnect(
      connection->sender_, qPrintable(connection->signal_),
      connection->receiver_, qPrintable(connection->slot_));
    graph_->connections_.removeOne(connection);
    delete arrow;
  }

  setControl();
}

void GGraphWidget::actionOptionTriggered(bool) {
  qDebug() << ""; // gilgil temp 2016.09.18
}

void GGraphWidget::factoryWidgetClicked(const QModelIndex&) {
  if (factoryWidget_->selectedItems().isEmpty()) return;
   scene_->setMode(GGScene::InsertItem);
}

#endif // QT_GUI_LIB
