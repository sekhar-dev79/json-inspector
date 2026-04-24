#pragma once

#include <QStyledItemDelegate>
#include <QPainter>

// Custom item delegate handling colors, bold states, and typography in the tree
class JsonTreeDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit JsonTreeDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

private:
    static QColor valueColor(int nodeTypeInt, bool isDark);
    static QColor keyColor(bool isDark);
    static QColor typeColor(bool isDark);
};
