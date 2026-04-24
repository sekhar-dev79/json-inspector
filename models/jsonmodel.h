#pragma once

#include <QObject>
#include <QStandardItemModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QList>

class JsonModel : public QObject
{
    Q_OBJECT

public:
    explicit JsonModel(QObject* parent = nullptr);

    QStandardItemModel* standardModel() const;

    void populate(const QJsonDocument& doc);
    void clear();

    int nodeCount() const;

signals:
    void modelPopulated(int nodeCount);

private:
    QList<QStandardItem*> makeRow(const QString& key,
                                   const QJsonValue& value,
                                   const QString& fullPath);

    void buildChildren(QStandardItem* parentItem,
                       const QJsonValue& value,
                       const QString& keyPath);

    QStandardItemModel* m_model     = nullptr;
    int                 m_nodeCount = 0;
};
