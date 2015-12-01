#include "gpropitemlineeditqvariant.h"

#ifdef QT_GUI_LIB

// ----------------------------------------------------------------------------
// GPropItemLineEditQVariant
// ----------------------------------------------------------------------------
GPropItemLineEditQVariant::GPropItemLineEditQVariant(QTreeWidgetItem* parent, QObject* object, QMetaProperty mpro)
  : GPropItemLineEdit(parent, object, mpro) {
  QObject::connect(lineEdit_, SIGNAL(editingFinished()), this, SLOT(myEditingFinished()));
}

void GPropItemLineEditQVariant::update() {
  QVariant value = object_->property(mpro_.name());
  lineEdit_->setText(value.toString());
}

void GPropItemLineEditQVariant::myEditingFinished() {
  bool res = object_->setProperty(mpro_.name(), QVariant::fromValue<QString>(lineEdit_->text()));
  if (!res) {
    qWarning() << QString("object->setProperty(%1, %2) return false").arg(mpro_.name(), lineEdit_->text());
  }
  update();
}

#endif // QT_GUI_LIB
