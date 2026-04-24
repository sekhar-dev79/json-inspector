#include "jsonmodel.h"
#include "constants.h"

#include <QStandardItem>
#include <QJsonObject>
#include <QJsonArray>

// ─── Constructor ──────────────────────────────────────────────────────────────
JsonModel::JsonModel(QObject* parent)
    : QObject(parent)
{
    m_model = new QStandardItemModel(this);
    m_model->setHorizontalHeaderLabels({"KEY", "VALUE", "TYPE"});
}

// ─── standardModel ────────────────────────────────────────────────────────────
QStandardItemModel* JsonModel::standardModel() const
{
    return m_model;
}

// ─── nodeCount ────────────────────────────────────────────────────────────────
int JsonModel::nodeCount() const
{
    return m_nodeCount;
}

// ─── clear ────────────────────────────────────────────────────────────────────
void JsonModel::clear()
{
    m_model->clear();
    m_model->setHorizontalHeaderLabels({"KEY", "VALUE", "TYPE"});
    m_nodeCount = 0;
}

// ─── populate ────────────────────────────────────────────────────────────────
void JsonModel::populate(const QJsonDocument& doc)
{
    clear();
    if (doc.isNull()) return;

    m_model->blockSignals(true);

    QStandardItem* root = m_model->invisibleRootItem();

    if (doc.isObject()) {
        const QJsonObject obj = doc.object();
        for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
            const QString path = "root." + it.key();
            auto row = makeRow(it.key(), it.value(), path);
            buildChildren(row[0], it.value(), path);
            root->appendRow(row);
            ++m_nodeCount;
        }
    } else if (doc.isArray()) {
        const QJsonArray arr = doc.array();
        for (int i = 0; i < arr.size(); ++i) {
            const QString key  = QString("[%1]").arg(i);
            const QString path = QString("root[%1]").arg(i);
            auto row = makeRow(key, arr[i], path);
            buildChildren(row[0], arr[i], path);
            root->appendRow(row);
            ++m_nodeCount;
        }
    }

    m_model->blockSignals(false);
    emit m_model->layoutChanged();
    emit modelPopulated(m_nodeCount);
}

// ─── buildChildren ────────────────────────────────────────────────────────────
void JsonModel::buildChildren(QStandardItem* parentItem,
                              const QJsonValue& value,
                              const QString& keyPath)
{
    if (value.isObject()) {
        const QJsonObject obj = value.toObject();
        for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
            const QString childPath = keyPath + "." + it.key();
            auto row = makeRow(it.key(), it.value(), childPath);
            buildChildren(row[0], it.value(), childPath);
            parentItem->appendRow(row);
            ++m_nodeCount;
        }
    } else if (value.isArray()) {
        const QJsonArray arr = value.toArray();
        for (int i = 0; i < arr.size(); ++i) {
            const QString childPath = QString("%1[%2]").arg(keyPath).arg(i);
            const QString key       = QString("[%1]").arg(i);
            auto row = makeRow(key, arr[i], childPath);
            buildChildren(row[0], arr[i], childPath);
            parentItem->appendRow(row);
            ++m_nodeCount;
        }
    }
}

// ─── makeRow ─────────────────────────────────────────────────────────────────
QList<QStandardItem*> JsonModel::makeRow(const QString& key,
                                          const QJsonValue& value,
                                          const QString& fullPath)
{
    QStandardItem* keyItem  = new QStandardItem(key);
    QStandardItem* valItem  = new QStandardItem();
    QStandardItem* typeItem = new QStandardItem();

    keyItem->setEditable(false);
    valItem->setEditable(false);
    typeItem->setEditable(false);

    NodeKind nodeKind = NodeKind::Null;
    QString  rawValue;
    QString  typeText;

    if (value.isObject()) {
        nodeKind = NodeKind::Object;
        rawValue = "{}";
        typeText = "object";
        const int c = value.toObject().count();
        valItem->setText(QString("{ %1 %2 }").arg(c).arg(c == 1 ? "key" : "keys"));

    } else if (value.isArray()) {
        nodeKind = NodeKind::Array;
        rawValue = "[]";
        typeText = "array";
        const int c = value.toArray().count();
        valItem->setText(QString("[ %1 %2 ]").arg(c).arg(c == 1 ? "item" : "items"));

    } else if (value.isString()) {
        nodeKind = NodeKind::String;
        rawValue = value.toString();
        typeText = "string";
        valItem->setText(QString("\"%1\"").arg(rawValue));

    } else if (value.isDouble()) {
        nodeKind = NodeKind::Number;
        const double    d = value.toDouble();
        const qlonglong i = static_cast<qlonglong>(d);
        rawValue = (static_cast<double>(i) == d)
                       ? QString::number(i)
                       : QString::number(d, 'g', 15);
        typeText = "number";
        valItem->setText(rawValue);

    } else if (value.isBool()) {
        nodeKind = NodeKind::Boolean;
        rawValue = value.toBool() ? "true" : "false";
        typeText = "bool";
        valItem->setText(rawValue);

    } else {
        nodeKind = NodeKind::Null;
        rawValue = "null";
        typeText = "null";
        valItem->setText("null");
    }

    typeItem->setText(typeText);

    // Store roles on column-0 item only
    keyItem->setData(static_cast<int>(nodeKind), JsonRole::NodeType);
    keyItem->setData(rawValue,                   JsonRole::RawValue);
    keyItem->setData(fullPath,                   JsonRole::FullPath);

    return { keyItem, valItem, typeItem };
}
