#include "gg_scene.h"
#include "ggraphwidget.h"

GGScene::GGScene(QObject *parent) : QGraphicsScene(parent)
{
	m_mode         = MoveItem;
  graphWidget_   = dynamic_cast<GGraphWidget*>(parent);
  Q_ASSERT(graphWidget_ != nullptr);
  signalSlotForm_ = nullptr;
  line           = nullptr;

  // ----- gilgil temp 2016.09.20 -----
  /*
	// categoryNames.push_back("VMyObject"); // gilgil temp 2012.08.02
	categoryNames.push_back("SnoopCapture");
	categoryNames.push_back("SnoopFilter");
	categoryNames.push_back("SnoopProcess");
	categoryNames.push_back("VNet");

	removePrefixNames.push_back("VMy");
	removePrefixNames.push_back("Snoop");
	removePrefixNames.push_back("V");

	removeSignalNames.push_back("destroyed(QObject*)");
	removeSignalNames.push_back("destroyed()");
	removeSignalNames.push_back("objectNameChanged(QString)");
	removeSignalNames.push_back("opened()");
	removeSignalNames.push_back("closed()");

	removeSlotNames.push_back("deleteLater()");
	removeSlotNames.push_back("_q_reregisterTimers(void*)");
	removeSlotNames.push_back("open()");
	removeSlotNames.push_back("close()");
  */
  // ----------------------------------
}

GGScene::~GGScene()
{
	while (items().count() > 0)
	{
		QGraphicsItem* item = items().first();
    if (item->type() == GGArrow::Type)
		{
      GGArrow *arrow = (GGArrow*)(item);
      arrow->startText()->removeArrow(arrow);
      arrow->endText()->removeArrow(arrow);
			delete item;
		};
    if (item->type() == GGText::Type)
		{
      GGText* text = (GGText*)item;
      text->removeArrows();
      delete text;
		}
	}
  if (signalSlotForm_ != nullptr) {
    delete signalSlotForm_;
    signalSlotForm_ = nullptr;
  }
}

void GGScene::clear()
{
  QGraphicsScene::clear();
}

// ----- gilgil temp 2016.09.20 -----
/*
void Scene::addClass(QString categoryName)
{
	if (treeWidget == NULL)
	{
		LOG_ERROR("treeWidget is null");
		return;
	}

	QTreeWidgetItem* rootItem = new QTreeWidgetItem(treeWidget);
	rootItem->setText(0, categoryName);
	VMetaClassList& list = VMetaClassMap::getList((char*)qPrintable(categoryName));
	for (VMetaClassList::iterator it = list.begin(); it != list.end(); it++)
	{
		VMetaClass* metaClass = *it;
		QTreeWidgetItem* item = new QTreeWidgetItem(rootItem);
		QString className = metaClass->className();
		item->setText(0, className);
		treeWidget->addTopLevelItem(item);
		addClass(metaClass, item);
	}
}

void Scene::addClass(VMetaClass* parentMetaClass, QTreeWidgetItem* parentItem)
{
	char* parentCategoryName = parentMetaClass->className();
	VMetaClassList& childList = VMetaClassMap::getList(parentCategoryName);
	for (VMetaClassList::iterator it = childList.begin(); it != childList.end(); it++)
	{
		VMetaClass* childMetaClass = *it;
		QTreeWidgetItem* childItem = new QTreeWidgetItem(parentItem);
		QString childClassName = childMetaClass->className();
		childItem->setText(0, childClassName);
		addClass(childMetaClass, childItem);
	}
}

void Scene::addClasses()
{
	foreach (QString categoryName, categoryNames)
	{
		addClass(categoryName);
	}
}
*/
// ----------------------------------

// ----- gilgil temp 2016.09.20 -----
/*
QString Scene::generateObjectClassName(QString className)
{
	if (className == "")
	{
		LOG_FATAL("className is blank");
		return "";
	}
	QString baseName = className;

	foreach(QString removePrefixName, removePrefixNames)
	{
		int length = removePrefixName.length();
		if (baseName.left(length) == removePrefixName)
			baseName = baseName.mid(length);
	}

	int index = 1;
	while (true)
	{
		QString res = baseName + QString::number(index);

		bool find = false;
		int _count = this->items().count();
		for (int i = 0; i < _count; i++)
		{
			QGraphicsItem* item = this->items().at(i);
			if (!IS_CLASS(item, Node*)) continue;
			Node* node = dynamic_cast<Node*>(item);
			if (res == node->object->name)
			{
				find = true;
				break;
			}
		}
		if (!find)
		{
			return res;
		}
		index++;
	}
	return ""; // unreachable code
}

Node* Scene::createNode(QString className, QString name, bool createObject)
{
	VObject* object = NULL;
	if (createObject)
	{
		object = (VObject*)VMetaClassMgr::createByClassName((char*)qPrintable(className));
		if (object == NULL) return NULL;
		if (name == "") name = generateObjectClassName(className);
    object->owner = this->graph_;
		object->name  = name;
    this->graph_->objectList.addObject(object);
	}

	Node* res = new Node(object);
	res->setPlainText(name);
	this->addItem(res);

	return res;
}
*/
// ----------------------------------

GGText* GGScene::createText(GGraph::Node* node, QPointF pos) {
  GGText* text = new GGText(node);
  text->setPlainText(node->objectName());
  addItem(text);
  text->setPos(pos);
  return text;
}

GGArrow* GGScene::createArrow(GGText* startText, GGText* endText, GGraph::Connection* connection) {
  GGArrow* arrow = new GGArrow(startText, endText, connection);
  arrow->setColor(Qt::black);
  startText->addArrow(arrow);
  endText->addArrow(arrow);
  addItem(arrow);
  arrow->updatePosition();
  return arrow;
}

GGArrow* GGScene::createArrow(QString startNodeName, QString endNodeName, GGraph::Connection* connection) {
  GGText* startText = findTextByObjectName(startNodeName);
  if (startText == nullptr) return nullptr;
  GGText* endText   = findTextByObjectName(endNodeName);
  if (endText == nullptr) return nullptr;
  return createArrow(startText, endText, connection);
}

GGText* GGScene::findTextByObjectName(QString objectName) {
	int _count = this->items().count();
	for (int i = 0; i < _count; i++)
	{
		QGraphicsItem* item = this->items().at(i);
    GGText* res = dynamic_cast<GGText*>(item);
    if (res == nullptr) continue;
    if (res->node_->objectName() == objectName)
			return res;
	}
  qWarning() << QString("can not find for '%1'").arg(objectName);
  return nullptr;
}

// ----- gilgil temp 2016.09.20 -----
/*
bool Scene::newFile(QString& errStr)
{
	clear();
	errStr = "";
	return true;
}

bool Scene::loadFromFile(QString fileName, QString& errStr)
{
	clear();
  bool res = this->graph_->loadFromFile(fileName, "graph");
	if (!res)
	{
		errStr = qformat("can not open file(%s)", qPrintable(fileName));
		LOG_ERROR("%s", qPrintable(errStr));
		return false;
	}

	//
	// coord
	//
	{
		VXmlDoc doc(fileName);
		doc.loadFromFile(fileName);
		VXml xml = doc.root().gotoChild("coord");
		int _count = xml.childs().count();
		for (int i = 0 ; i < _count; i++)
		{
			VXml     childXml = xml.childs().at(i);
			QString  name = childXml.getStr("name");
			if (name == "")
			{
				errStr = "name is null";
				LOG_ERROR("%s", qPrintable(errStr));
				return false;
			}

      VObject* object   = this->graph_->objectList.findByName(name);
			if (object == NULL)
			{
				errStr = qformat("can not find object ('%s')", qPrintable(name));
				LOG_ERROR("%s", qPrintable(errStr));
				return false;
			}

			Node*    node = this->createNode(object->className(), object->name, false);
			node->object = object;
			QPointF  f;
			f.setX(childXml.getDouble("x", f.x()));
			f.setY(childXml.getDouble("y", f.y()));
			node->setPos(f);
		}
	}

	//
	// arrow
	//
	{
    int _count = this->graph_->connectList.count();
		for (int i = 0; i < _count; i++)
		{
      VGraphConnect connect = (VGraphConnect&)this->graph_->connectList.at(i);
      GGArrow* arrow = createArrow(connect.sender, connect.signal, connect.receiver, connect.slot);
			if (arrow == NULL) return false;
			addItem(arrow);
			arrow->updatePosition();
		}
	}

	return true;
}

bool Scene::saveToFile(QString fileName, QString& errStr)
{
  bool res = this->graph_->saveToFile(fileName, "graph");
	if (!res)
	{
    errStr = this->graph_->error.msg;
		return false;
	}

	//
	// coord
	//
	VXmlDoc doc(fileName);
	doc.loadFromFile(fileName);
	VXml xml = doc.root().gotoChild("coord");
	xml.clearChild();
  int _count = this->graph_->objectList.count();
	for (int i = 0; i < _count; i++)
	{
    VObject* object = this->graph_->objectList.at(i);
		Node*    node   = findNodeByName(object->name);
		if (node == NULL) continue;

		VXml     childXml = xml.addChild("object");
		childXml.setStr("name", object->name);
		childXml.setDouble("x", node->pos().x());
		childXml.setDouble("y", node->pos().y());
	}
	doc.saveToFile(fileName);

	return true;
}
*/
// ----------------------------------

void GGScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	// LOG_DEBUG("mousePressEvent"); // gilgil temp 2012.06.30
	switch (m_mode)	{
    case InsertItem: {
        GGraph::Node* newNode = graphWidget_->createNodeIfItemNodeSelected();
        if (newNode != nullptr) {
          createText(newNode, event->scenePos());
        }
        setMode(MoveItem);
      }
			break;
		case MoveItem:
			break;
		case InsertLine:
			line = new QGraphicsLineItem(QLineF(event->scenePos(), event->scenePos()));
			line->setPen(QPen(Qt::black, 2));
			addItem(line);
			break;
	}

	QGraphicsScene::mousePressEvent(event);
}

void GGScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  // qDebug() << "mouseMoveEvent"; // gilgil temp 2012.06.30
	switch (m_mode)
	{
    case InsertItem:
      break;
		case MoveItem:
			break;
		case InsertLine:
      if (line != NULL) {
				QLineF newLine(line->line().p1(), event->scenePos());
				line->setLine(newLine);
				return;
			}
			break;
	}

	QGraphicsScene::mouseMoveEvent(event);
}

void GGScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	switch (m_mode)	{
    case InsertItem:
      break;
		case MoveItem:
			break;
		case InsertLine:
      if (line != nullptr)
			{
					QList<QGraphicsItem *> startItems = items(line->line().p1());
					if (startItems.count() && startItems.first() == line)
							startItems.removeFirst();
					QList<QGraphicsItem *> endItems = items(line->line().p2());
					if (endItems.count() && endItems.first() == line)
							endItems.removeFirst();

					if (startItems.count() > 0 && endItems.count() > 0 &&
							startItems.first() != endItems.first())
					{
              GGText *startText = (GGText*)startItems.first();
              GGText *endText = (GGText*)endItems.first();
              GGraph* graph = graphWidget_->graph();
              Q_ASSERT(graph != nullptr);

              QStringList _signalList = startText->node_->signalList();
              foreach(QString name, graphWidget_->ignoreSignalNames_) {
								_signalList.removeAll(name);
							}

              QStringList _slotList = endText->node_->slotList();
              foreach (QString name, graphWidget_->ignoreSlotNames_) {
								_slotList.removeAll(name);
							}

              if (signalSlotForm_ == nullptr)
                signalSlotForm_ = new GGSignalSlotForm((QWidget*)this->parent());
              signalSlotForm_->lwSignalList_->clear();
              signalSlotForm_->lwSignalList_->addItems(_signalList);
              signalSlotForm_->lwSlotList_->clear();
              signalSlotForm_->lwSlotList_->addItems(_slotList);
              signalSlotForm_->exec();
              if (signalSlotForm_->result() == QDialog::Accepted)
							{
                QString signal = signalSlotForm_->lwSignalList_->selectedItems().first()->text();
                QString slot   = signalSlotForm_->lwSlotList_->selectedItems().first()->text();

                bool res = GObj::connect(
                  startText->node_, qPrintable(signal),
                  endText->node_, qPrintable(slot),
                  Qt::DirectConnection);
                if (!res) {
                  QString msg = QString("Can not connect %1:%2 > %3:%4").arg(
                    startText->node_->objectName(), signal,
                    endText->node_->objectName(), slot);
                  QMessageBox::warning(nullptr, "Error", msg);
                  return;
                }
                GGraph::Connection* connection = new GGraph::Connection;
                connection->sender_   = startText->node_;
                connection->signal_   = signal;
                connection->receiver_ = endText->node_;
                connection->slot_     = slot;
                graphWidget_->graph()->connections_.push_back(connection);

                GGArrow *arrow = createArrow(startText, endText, connection);
							}
					}
					removeItem(line);
					delete line;
					line = NULL;
			}
			break;
	}

	QGraphicsScene::mouseReleaseEvent(event);
}

// ----- gilgil temp 2016.09.20 -----
/*
void Scene::load(VXml xml)
{
	categoryNames     = xml.getStr("categoryNames",     categoryNames.join("/")).split("/");
	removePrefixNames = xml.getStr("removePrefixNames", removePrefixNames.join("/")).split("/");
	removeSignalNames = xml.getStr("removeSignalNames", removeSignalNames.join("/")).split("/");
	removeSlotNames   = xml.getStr("removeSlotNames",   removeSlotNames.join("/")).split("/");
}

void Scene::save(VXml xml)
{
	xml.setStr("categoryNames",     categoryNames.join("/"));
	xml.setStr("removePrefixNames", removePrefixNames.join("/"));
	xml.setStr("removeSignalNames", removeSignalNames.join("/"));
	xml.setStr("removeSlotNames",   removeSlotNames.join("/"));
}
*/
// ----------------------------------
