#include "jsontreedelegate.h"
#include "thememanager.h"
#include "constants.h"

#include <QModelIndex>
#include <QStyle>

JsonTreeDelegate::JsonTreeDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{}

void JsonTreeDelegate::paint(QPainter* painter,
                             const QStyleOptionViewItem& option,
                             const QModelIndex& index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const bool isDark = ThemeManager::instance().isDark();
    const int col = index.column();

    const QModelIndex keyIndex = index.sibling(index.row(), JsonColumn::Key);
    int nodeTypeInt = static_cast<int>(NodeKind::Null);
    if (keyIndex.isValid()) {
        nodeTypeInt = keyIndex.data(JsonRole::NodeType).toInt();
    }

    QColor textColor;

    if (col == JsonColumn::Key) {
        textColor = keyColor(isDark);
        const NodeKind nt = static_cast<NodeKind>(nodeTypeInt);

        // Emphasize structural nodes for hierarchy readability
        if (nt == NodeKind::Object || nt == NodeKind::Array) {
            opt.font.setWeight(QFont::DemiBold);
        }

    } else if (col == JsonColumn::Value) {
        textColor = valueColor(nodeTypeInt, isDark);

    } else if (col == JsonColumn::Type) {
        textColor = typeColor(isDark);

        // Enforce Uppercase text matching the QSS styling goals
        opt.text = opt.text.toUpper();
        opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;

        // Shrink the font size relative to current QSS definitions
        if (opt.font.pixelSize() > 0) {
            opt.font.setPixelSize(qMax(9, opt.font.pixelSize() - 2));
        } else if (opt.font.pointSize() > 0) {
            opt.font.setPointSize(qMax(8, opt.font.pointSize() - 2));
        }
    }

    bool isSelected = opt.state & QStyle::State_Selected;

    if (!isSelected) {
        opt.palette.setColor(QPalette::Text, textColor);
        opt.palette.setColor(QPalette::WindowText, textColor);
    }

    QStyledItemDelegate::paint(painter, opt, index);
}

QColor JsonTreeDelegate::valueColor(int nodeTypeInt, bool isDark)
{
    const NodeKind nt = static_cast<NodeKind>(nodeTypeInt);

    if (isDark) {
        switch (nt) {
        case NodeKind::String:  return QColor(TypeColor::Dark::String);
        case NodeKind::Number:  return QColor(TypeColor::Dark::Number);
        case NodeKind::Boolean: return QColor(TypeColor::Dark::Boolean);
        case NodeKind::Null:    return QColor(TypeColor::Dark::Null);
        case NodeKind::Object:  return QColor(TypeColor::Dark::Object);
        case NodeKind::Array:   return QColor(TypeColor::Dark::Array);
        default:                return QColor(TypeColor::Dark::Key);
        }
    } else {
        switch (nt) {
        case NodeKind::String:  return QColor(TypeColor::Light::String);
        case NodeKind::Number:  return QColor(TypeColor::Light::Number);
        case NodeKind::Boolean: return QColor(TypeColor::Light::Boolean);
        case NodeKind::Null:    return QColor(TypeColor::Light::Null);
        case NodeKind::Object:  return QColor(TypeColor::Light::Object);
        case NodeKind::Array:   return QColor(TypeColor::Light::Array);
        default:                return QColor(TypeColor::Light::Key);
        }
    }
}

QColor JsonTreeDelegate::keyColor(bool isDark)
{
    return isDark ? QColor(TypeColor::Dark::Key) : QColor(TypeColor::Light::Key);
}

QColor JsonTreeDelegate::typeColor(bool isDark)
{
    return isDark ? QColor(TypeColor::Dark::Type) : QColor(TypeColor::Light::Type);
}
