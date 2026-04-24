#include "jsonsearchdelegate.h"
#include "jsonsearchproxy.h"
#include "thememanager.h"
#include "constants.h"

#include <QPainter>
#include <QStyle>
#include <QFontMetrics>

JsonSearchDelegate::JsonSearchDelegate(JsonSearchProxy* proxy, QObject* parent)
    : JsonTreeDelegate(parent)
    , m_proxy(proxy)
{}

void JsonSearchDelegate::paint(QPainter* painter,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index) const
{
    // Let the parent class draw the base text and background
    JsonTreeDelegate::paint(painter, option, index);

    if (!m_proxy || m_proxy->searchTerm().isEmpty()) return;

    // Only highlight Key and Value columns
    const int col = index.column();
    if (col != JsonColumn::Key && col != JsonColumn::Value) return;

    // Check if this row contains a match
    const QModelIndex sourceIndex = m_proxy->mapToSource(index);
    const QModelIndex sourceCol0  = sourceIndex.sibling(sourceIndex.row(), JsonColumn::Key);
    if (!m_proxy->isMatch(sourceCol0)) return;

    const QString text = index.data(Qt::DisplayRole).toString();
    if (text.isEmpty()) return;

    // Paint the highlights over the matching substrings
    paintHighlight(painter, option, text, m_proxy->searchTerm());
}

void JsonSearchDelegate::paintHighlight(QPainter* painter,
                                        const QStyleOptionViewItem& option,
                                        const QString& text,
                                        const QString& term) const
{
    if (term.isEmpty()) return;

    const bool isDark = ThemeManager::instance().isDark();

    const QColor matchBg = isDark ? QColor(SearchColor::Dark::MatchBg)
                                  : QColor(SearchColor::Light::MatchBg);
    const QColor matchFg = isDark ? QColor(SearchColor::Dark::MatchFg)
                                  : QColor(SearchColor::Light::MatchFg);

    // Get the exact text rectangle Qt uses internally for this cell
    QStyleOptionViewItem opt = option;
    const QRect textRect = option.widget
                               ? option.widget->style()->subElementRect(
                                     QStyle::SE_ItemViewItemText, &opt, option.widget)
                               : option.rect.adjusted(4, 0, -4, 0);

    const QFontMetrics fm(option.font);
    int searchFrom = 0;

    while (true) {
        const int pos = text.indexOf(term, searchFrom, Qt::CaseInsensitive);
        if (pos < 0) break;

        const int xBefore = fm.horizontalAdvance(text.left(pos));
        const int xMatch  = fm.horizontalAdvance(text.mid(pos, term.length()));

        // Background highlight rect
        QRect highlightRect(
            textRect.left() + xBefore,
            textRect.top() + 1,
            xMatch,
            textRect.height() - 2
            );

        highlightRect = highlightRect.intersected(textRect);

        if (highlightRect.isValid()) {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);

            // Draw the background pill
            painter->setPen(Qt::NoPen);
            painter->setBrush(matchBg);
            painter->drawRoundedRect(highlightRect, 2, 2);

            // Draw the matched text on top to ensure crisp readability
            QRect textDrawRect(
                textRect.left() + xBefore,
                textRect.top(),
                xMatch,
                textRect.height()
                );

            painter->setPen(matchFg);
            painter->setFont(option.font);
            painter->drawText(textDrawRect,
                              Qt::AlignVCenter | Qt::AlignLeft,
                              text.mid(pos, term.length()));

            painter->restore();
        }

        searchFrom = pos + term.length();
    }
}
