#pragma once

#include <QSortFilterProxyModel>
#include <QString>
#include <QSet>
#include <QModelIndex>

#include "constants.h"

// Filters rows based on a search term and tracks matches for delegate highlighting
class JsonSearchProxy : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit JsonSearchProxy(QObject* parent = nullptr);

    void setSearchTerm(const QString& term);
    void setSearchScope(SearchScope scope);

    QString searchTerm() const;
    bool isMatch(const QModelIndex& sourceIndex) const;
    int matchCount() const;

signals:
    void matchCountChanged(int count);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    bool rowMatchesTerm(int sourceRow, const QModelIndex& sourceParent) const;
    void rebuildMatchSet();

    QString m_term;
    SearchScope m_scope = SearchScope::KeysAndValues;

    mutable QSet<QModelIndex> m_matchingIndices;
    mutable int m_matchCount = 0;
};
