#include "jsondiff.h"

#include <QJsonObject>
#include <QJsonArray>
#include <algorithm>

// ─── compare ─────────────────────────────────────────────────────────────────
QVector<DiffEntry> JsonDiff::compare(const QJsonDocument& docA,
                                     const QJsonDocument& docB)
{
    QVector<DiffEntry> results;
    QJsonValue a, b;

    if (docA.isObject())       a = docA.object();
    else if (docA.isArray())   a = docA.array();

    if (docB.isObject())       b = docB.object();
    else if (docB.isArray())   b = docB.array();

    diffValues(a, b, "root", results);

    std::sort(results.begin(), results.end(),
              [](const DiffEntry& x, const DiffEntry& y) {
                  return x.path < y.path;
              });

    return results;
}

// ─── diffValues ──────────────────────────────────────────────────────────────
void JsonDiff::diffValues(const QJsonValue& a,
                          const QJsonValue& b,
                          const QString& path,
                          QVector<DiffEntry>& results)
{
    if (a.isObject() && b.isObject()) {
        diffObjects(a.toObject(), b.toObject(), path, results);
        return;
    }

    if (a.isArray() && b.isArray()) {
        diffArrays(a.toArray(), b.toArray(), path, results);
        return;
    }

    if (a == b) {
        DiffEntry e;
        e.path     = path;
        e.type     = DiffType::Unchanged;
        e.oldValue = a;
        e.newValue = b;
        results.append(e);
    } else {
        DiffEntry e;
        e.path     = path;
        e.type     = DiffType::Changed;
        e.oldValue = a;
        e.newValue = b;
        results.append(e);
    }
}

// ─── diffObjects ─────────────────────────────────────────────────────────────
void JsonDiff::diffObjects(const QJsonObject& a,
                           const QJsonObject& b,
                           const QString& basePath,
                           QVector<DiffEntry>& results)
{
    QStringList allKeys;
    for (auto it = a.constBegin(); it != a.constEnd(); ++it)
        allKeys.append(it.key());
    for (auto it = b.constBegin(); it != b.constEnd(); ++it) {
        if (!allKeys.contains(it.key()))
            allKeys.append(it.key());
    }
    allKeys.sort();

    for (const QString& key : allKeys) {
        const QString childPath = basePath + "." + key;
        const bool inA = a.contains(key);
        const bool inB = b.contains(key);

        if (inA && inB) {
            diffValues(a.value(key), b.value(key), childPath, results);
        } else if (inA && !inB) {
            markRemoved(a.value(key), childPath, results);
        } else {
            markAdded(b.value(key), childPath, results);
        }
    }
}

// ─── diffArrays ──────────────────────────────────────────────────────────────
void JsonDiff::diffArrays(const QJsonArray& a,
                          const QJsonArray& b,
                          const QString& basePath,
                          QVector<DiffEntry>& results)
{
    const int maxLen = qMax(a.size(), b.size());

    for (int i = 0; i < maxLen; ++i) {
        const QString childPath = QString("%1[%2]").arg(basePath).arg(i);

        if (i < a.size() && i < b.size()) {
            diffValues(a[i], b[i], childPath, results);
        } else if (i < a.size()) {
            markRemoved(a[i], childPath, results);
        } else {
            markAdded(b[i], childPath, results);
        }
    }
}

// ─── markAdded ───────────────────────────────────────────────────────────────
void JsonDiff::markAdded(const QJsonValue& val,
                         const QString& path,
                         QVector<DiffEntry>& results)
{
    if (val.isObject()) {
        const QJsonObject obj = val.toObject();
        for (auto it = obj.constBegin(); it != obj.constEnd(); ++it)
            markAdded(it.value(), path + "." + it.key(), results);
    } else if (val.isArray()) {
        const QJsonArray arr = val.toArray();
        for (int i = 0; i < arr.size(); ++i)
            markAdded(arr[i], QString("%1[%2]").arg(path).arg(i), results);
    }

    DiffEntry e;
    e.path     = path;
    e.type     = DiffType::Added;
    e.newValue = val;
    results.append(e);
}

// ─── markRemoved ─────────────────────────────────────────────────────────────
void JsonDiff::markRemoved(const QJsonValue& val,
                           const QString& path,
                           QVector<DiffEntry>& results)
{
    if (val.isObject()) {
        const QJsonObject obj = val.toObject();
        for (auto it = obj.constBegin(); it != obj.constEnd(); ++it)
            markRemoved(it.value(), path + "." + it.key(), results);
    } else if (val.isArray()) {
        const QJsonArray arr = val.toArray();
        for (int i = 0; i < arr.size(); ++i)
            markRemoved(arr[i], QString("%1[%2]").arg(path).arg(i), results);
    }

    DiffEntry e;
    e.path     = path;
    e.type     = DiffType::Removed;
    e.oldValue = val;
    results.append(e);
}
