#pragma once

#include <QObject>
#include <QStandardItemModel>
#include <QVector>

#include "diffentry.h"

// Builds a QStandardItemModel to display JSON differences
class DiffModel : public QObject
{
    Q_OBJECT

public:
    explicit DiffModel(QObject* parent = nullptr);

    QStandardItemModel* standardModel() const;

    void populate(const QVector<DiffEntry>& entries, bool showUnchanged = false);
    void clear();

    int changedCount() const;
    int addedCount() const;
    int removedCount() const;

public slots:
    // Repaints cached row colors when theme changes
    void refreshTheme();

signals:
    void populated(int totalDiffs);

private:
    QList<QStandardItem*> makeRow(const DiffEntry& entry) const;

    QStandardItemModel* m_model = nullptr;
    int m_changedCount = 0;
    int m_addedCount = 0;
    int m_removedCount = 0;
};
