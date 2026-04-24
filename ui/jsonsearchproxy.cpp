#include "jsonsearchproxy.h"
#include "constants.h"
#include <QStandardItemModel>

JsonSearchProxy::JsonSearchProxy(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);

    // Automatically keep parent rows visible if any children match
    setRecursiveFilteringEnabled(true);
}

void JsonSearchProxy::setSearchTerm(const QString& term)
{
    m_term = term;
    m_matchingIndices.clear();
    m_matchCount = 0;

    invalidateFilter();
    rebuildMatchSet();

    emit matchCountChanged(m_matchCount);
}

void JsonSearchProxy::setSearchScope(SearchScope scope)
{
    m_scope = scope;
    setSearchTerm(m_term);
}

QString JsonSearchProxy::searchTerm() const
{
    return m_term;
}

bool JsonSearchProxy::isMatch(const QModelIndex& sourceIndex) const
{
    const QModelIndex col0 = sourceIndex.sibling(sourceIndex.row(), JsonColumn::Key);
    return m_matchingIndices.contains(col0);
}

int JsonSearchProxy::matchCount() const
{
    return m_matchCount;
}

bool JsonSearchProxy::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    if (m_term.isEmpty()) return true;
    return rowMatchesTerm(sourceRow, sourceParent);
}

bool JsonSearchProxy::rowMatchesTerm(int sourceRow, const QModelIndex& sourceParent) const
{
    const QAbstractItemModel* src = sourceModel();
    if (!src) return false;

    const bool checkKeys   = (m_scope == SearchScope::KeysAndValues || m_scope == SearchScope::KeysOnly);
    const bool checkValues = (m_scope == SearchScope::KeysAndValues || m_scope == SearchScope::ValuesOnly);

    if (checkKeys) {
        const QModelIndex keyIdx = src->index(sourceRow, JsonColumn::Key, sourceParent);
        if (keyIdx.data(Qt::DisplayRole).toString().contains(m_term, Qt::CaseInsensitive)) {
            return true;
        }
    }

    if (checkValues) {
        const QModelIndex valIdx = src->index(sourceRow, JsonColumn::Value, sourceParent);
        if (valIdx.data(Qt::DisplayRole).toString().contains(m_term, Qt::CaseInsensitive)) {
            return true;
        }
    }

    return false;
}

void JsonSearchProxy::rebuildMatchSet()
{
    m_matchingIndices.clear();
    m_matchCount = 0;

    if (m_term.isEmpty() || !sourceModel()) return;

    // Recursively walk the proxy model to count visible matches
    std::function<void(const QModelIndex&)> walk = [&](const QModelIndex& proxyParent) {
        const int rows = rowCount(proxyParent);
        for (int r = 0; r < rows; ++r) {
            const QModelIndex proxyIdx  = index(r, 0, proxyParent);
            const QModelIndex sourceIdx = mapToSource(proxyIdx);

            if (rowMatchesTerm(sourceIdx.row(), sourceIdx.parent())) {
                m_matchingIndices.insert(sourceIdx);
                ++m_matchCount;
            }

            if (hasChildren(proxyIdx)) {
                walk(proxyIdx);
            }
        }
    };

    walk(QModelIndex());
}
