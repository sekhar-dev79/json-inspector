#pragma once

#include "jsontreedelegate.h"
#include <QString>

class JsonSearchProxy;

// Extends JsonTreeDelegate with search match highlighting
class JsonSearchDelegate : public JsonTreeDelegate
{
    Q_OBJECT

public:
    explicit JsonSearchDelegate(JsonSearchProxy* proxy, QObject* parent = nullptr);

    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

private:
    void paintHighlight(QPainter* painter,
                        const QStyleOptionViewItem& option,
                        const QString& text,
                        const QString& term) const;

    JsonSearchProxy* m_proxy = nullptr;
};
