#pragma once

#include "diffentry.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QVector>
#include <QString>

/**
 * JsonDiff — recursive JSON comparison engine
 */
class JsonDiff
{
public:
    static QVector<DiffEntry> compare(const QJsonDocument& docA,
                                      const QJsonDocument& docB);

private:
    static void diffObjects(const QJsonObject& a,
                            const QJsonObject& b,
                            const QString& basePath,
                            QVector<DiffEntry>& results);

    static void diffArrays(const QJsonArray& a,
                           const QJsonArray& b,
                           const QString& basePath,
                           QVector<DiffEntry>& results);

    static void diffValues(const QJsonValue& a,
                           const QJsonValue& b,
                           const QString& path,
                           QVector<DiffEntry>& results);

    static void markAdded(const QJsonValue& val,
                          const QString& path,
                          QVector<DiffEntry>& results);

    static void markRemoved(const QJsonValue& val,
                            const QString& path,
                            QVector<DiffEntry>& results);

    JsonDiff() = delete;
};
