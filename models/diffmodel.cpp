#include "diffmodel.h"
#include "constants.h"
#include "thememanager.h"

#include <QStandardItem>
#include <QBrush>
#include <QJsonObject>
#include <QJsonArray>

namespace DiffRole {
constexpr int Type = Qt::UserRole + 10;
}

namespace {
QColor getRowColor(DiffType type, bool isDark) {
    switch (type) {
    case DiffType::Added:   return isDark ? QColor("#1D2819") : QColor("#EAF2E3");
    case DiffType::Removed: return isDark ? QColor("#2D1A25") : QColor("#F4E3EC");
    case DiffType::Changed: return isDark ? QColor("#332A1B") : QColor("#FDF3E1");
    default:                return QColor(Qt::transparent);
    }
}

QColor getStatusColor(DiffType type, bool isDark) {
    switch (type) {
    case DiffType::Added:   return QColor(isDark ? "#6DAA45" : "#437A22");
    case DiffType::Removed: return QColor(isDark ? "#D163A7" : "#A12C7B");
    case DiffType::Changed: return QColor(isDark ? "#FDAB43" : "#964219");
    default:                return QColor(isDark ? TypeColor::Dark::Null : TypeColor::Light::Null);
    }
}
}

DiffModel::DiffModel(QObject* parent)
    : QObject(parent)
{
    m_model = new QStandardItemModel(this);
    m_model->setHorizontalHeaderLabels({"Path", "Value A", "Value B", "Status"});

    // Ensure baked item colors update on theme toggle
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &DiffModel::refreshTheme);
}

QStandardItemModel* DiffModel::standardModel() const { return m_model; }

void DiffModel::clear()
{
    m_model->clear();
    m_model->setHorizontalHeaderLabels({"Path", "Value A", "Value B", "Status"});
    m_changedCount = m_addedCount = m_removedCount = 0;
}

void DiffModel::populate(const QVector<DiffEntry>& entries, bool showUnchanged)
{
    clear();
    m_model->blockSignals(true);

    for (const DiffEntry& e : entries) {
        if (!showUnchanged && e.type == DiffType::Unchanged) continue;

        m_model->invisibleRootItem()->appendRow(makeRow(e));

        switch (e.type) {
        case DiffType::Added:   ++m_addedCount;   break;
        case DiffType::Removed: ++m_removedCount; break;
        case DiffType::Changed: ++m_changedCount; break;
        default: break;
        }
    }

    m_model->blockSignals(false);
    emit m_model->layoutChanged();

    const int totalDiffs = m_addedCount + m_removedCount + m_changedCount;
    emit populated(totalDiffs);
}

QList<QStandardItem*> DiffModel::makeRow(const DiffEntry& entry) const
{
    const bool isDark = ThemeManager::instance().isDark();

    QColor rowColor = getRowColor(entry.type, isDark);
    QColor statusColor = getStatusColor(entry.type, isDark);

    auto fmt = [](const QJsonValue& v) -> QString {
        if (v.isUndefined() || v.isNull()) return "—";
        if (v.isString())  return QString("\"%1\"").arg(v.toString());
        if (v.isBool())    return v.toBool() ? "true" : "false";
        if (v.isDouble()) {
            const double d = v.toDouble();
            const qlonglong i = static_cast<qlonglong>(d);
            return (static_cast<double>(i) == d) ? QString::number(i) : QString::number(d, 'g', 10);
        }
        if (v.isObject()) return QString("{ %1 keys }").arg(v.toObject().count());
        if (v.isArray())  return QString("[ %1 items ]").arg(v.toArray().count());
        return "null";
    };

    auto* pathItem = new QStandardItem(entry.path);
    auto* oldItem = new QStandardItem(fmt(entry.oldValue));
    auto* newItem = new QStandardItem(fmt(entry.newValue));
    auto* statusItem = new QStandardItem(entry.typeLabel());

    pathItem->setData(static_cast<int>(entry.type), DiffRole::Type);

    if (rowColor.isValid() && rowColor != QColor(Qt::transparent)) {
        for (auto* item : {pathItem, oldItem, newItem, statusItem}) {
            item->setBackground(QBrush(rowColor));
        }
    }

    statusItem->setForeground(QBrush(statusColor));

    for (auto* item : {pathItem, oldItem, newItem, statusItem}) {
        item->setEditable(false);
    }

    return {pathItem, oldItem, newItem, statusItem};
}

void DiffModel::refreshTheme()
{
    if (!m_model) return;

    const bool isDark = ThemeManager::instance().isDark();

    for (int row = 0; row < m_model->rowCount(); ++row) {
        auto* pathItem = m_model->item(row, 0);
        if (!pathItem) continue;

        DiffType type = static_cast<DiffType>(pathItem->data(DiffRole::Type).toInt());

        QColor rowColor = getRowColor(type, isDark);
        QColor statusColor = getStatusColor(type, isDark);

        for (int col = 0; col < m_model->columnCount(); ++col) {
            auto* item = m_model->item(row, col);
            if (!item) continue;

            if (rowColor.isValid() && rowColor != QColor(Qt::transparent)) {
                item->setBackground(QBrush(rowColor));
            } else {
                item->setBackground(QBrush(Qt::transparent));
            }

            if (col == 3) {
                item->setForeground(QBrush(statusColor));
            }
        }
    }
}

int DiffModel::changedCount() const { return m_changedCount; }
int DiffModel::addedCount() const { return m_addedCount; }
int DiffModel::removedCount() const { return m_removedCount; }
