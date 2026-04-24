#include "searchbar.h"
#include "iconhelper.h"
#include "thememanager.h"
#include "constants.h"

#include <QHBoxLayout>
#include <QSizePolicy>
#include <QStyle>
#include <QAbstractItemView>
#include <QStyledItemDelegate>

SearchBar::SearchBar(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("SearchBar");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setupUi();
}

void SearchBar::setupUi()
{
    QHBoxLayout* outer = new QHBoxLayout(this);
    outer->setContentsMargins(8, 0, 8, 0);
    outer->setSpacing(6);

    // Input wrapper
    m_wrapper = new QWidget(this);
    m_wrapper->setObjectName("SearchInputWrapper");
    m_wrapper->setProperty("focused", false);
    m_wrapper->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout* wrapLayout = new QHBoxLayout(m_wrapper);
    wrapLayout->setContentsMargins(6, 0, 4, 0);
    wrapLayout->setSpacing(4);

    m_searchIcon = new QLabel(m_wrapper);
    m_searchIcon->setObjectName("SearchIconLabel");
    m_searchIcon->setFixedSize(14, 14);
    m_searchIcon->setAlignment(Qt::AlignCenter);

    m_lineEdit = new QLineEdit(m_wrapper);
    m_lineEdit->setObjectName("SearchLineEdit");
    m_lineEdit->setPlaceholderText("Search keys and values…");
    m_lineEdit->setClearButtonEnabled(false);
    m_lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Install event filter to track focus for the wrapper QSS styling
    m_lineEdit->installEventFilter(this);

    m_clearBtn = new QPushButton("✕", m_wrapper);
    m_clearBtn->setObjectName("SearchClearButton");
    m_clearBtn->setFixedSize(16, 16);
    m_clearBtn->setVisible(false);
    m_clearBtn->setCursor(Qt::PointingHandCursor);
    m_clearBtn->setToolTip("Clear search");

    wrapLayout->addWidget(m_searchIcon);
    wrapLayout->addWidget(m_lineEdit, 1);
    wrapLayout->addWidget(m_clearBtn);

    // Scope selector
    m_scopeBox = new QComboBox(this);
    m_scopeBox->setObjectName("SearchScopeCombo");
    m_scopeBox->addItem("Keys + Values", static_cast<int>(SearchScope::KeysAndValues));
    m_scopeBox->addItem("Keys only", static_cast<int>(SearchScope::KeysOnly));
    m_scopeBox->addItem("Values only", static_cast<int>(SearchScope::ValuesOnly));
    m_scopeBox->setFixedHeight(24);
    m_scopeBox->setCursor(Qt::PointingHandCursor);

    // Force drop-down to obey QSS hover rules
    m_scopeBox->setItemDelegate(new QStyledItemDelegate(m_scopeBox));

    if (QAbstractItemView* comboView = m_scopeBox->view()) {
        comboView->window()->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
        comboView->window()->setAttribute(Qt::WA_TranslucentBackground);
        comboView->setMouseTracking(true);
        comboView->viewport()->setMouseTracking(true);
        comboView->setCursor(Qt::PointingHandCursor);
    }

    // Match count label
    m_matchLabel = new QLabel("", this);
    m_matchLabel->setObjectName("MatchCountLabel");
    m_matchLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    m_matchLabel->setFixedWidth(80);
    m_matchLabel->setProperty("hasMatches", false);
    m_matchLabel->setProperty("noMatches",  false);

    outer->addWidget(m_wrapper, 1);
    outer->addWidget(m_scopeBox);
    outer->addWidget(m_matchLabel);

    connect(m_lineEdit, &QLineEdit::textChanged, this, &SearchBar::onTextChanged);
    connect(m_clearBtn, &QPushButton::clicked, this, [this]() { m_lineEdit->clear(); });
    connect(m_scopeBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SearchBar::onScopeChanged);

    updateIcons();
}

bool SearchBar::eventFilter(QObject* obj, QEvent* event)
{
    // Dynamically update the wrapper's property when the text input gains/loses focus
    if (obj == m_lineEdit) {
        if (event->type() == QEvent::FocusIn) {
            m_wrapper->setProperty("focused", true);
            m_wrapper->style()->unpolish(m_wrapper);
            m_wrapper->style()->polish(m_wrapper);
        } else if (event->type() == QEvent::FocusOut) {
            m_wrapper->setProperty("focused", false);
            m_wrapper->style()->unpolish(m_wrapper);
            m_wrapper->style()->polish(m_wrapper);
        }
    }
    return QWidget::eventFilter(obj, event);
}

void SearchBar::onTextChanged(const QString& text)
{
    updateClearButton(text);

    if (text.isEmpty()) {
        m_matchLabel->setText("");
        m_matchLabel->setProperty("hasMatches", false);
        m_matchLabel->setProperty("noMatches",  false);
        m_matchLabel->style()->unpolish(m_matchLabel);
        m_matchLabel->style()->polish(m_matchLabel);
    }

    const SearchScope scope = static_cast<SearchScope>(m_scopeBox->currentData().toInt());
    emit searchChanged(text, scope);
}

void SearchBar::onScopeChanged(int)
{
    const SearchScope scope = static_cast<SearchScope>(m_scopeBox->currentData().toInt());
    emit searchChanged(m_lineEdit->text(), scope);
}

void SearchBar::updateClearButton(const QString& text)
{
    m_clearBtn->setVisible(!text.isEmpty());
}

void SearchBar::updateMatchLabel(int matches, int total)
{
    const bool searching = !m_lineEdit->text().isEmpty();

    if (!searching) {
        m_matchLabel->setText("");
        m_matchLabel->setProperty("hasMatches", false);
        m_matchLabel->setProperty("noMatches",  false);
    } else if (matches == 0) {
        m_matchLabel->setText("No matches");
        m_matchLabel->setProperty("hasMatches", false);
        m_matchLabel->setProperty("noMatches",  true);
    } else {
        m_matchLabel->setText(QString("%1 / %2").arg(matches).arg(total));
        m_matchLabel->setProperty("hasMatches", true);
        m_matchLabel->setProperty("noMatches",  false);
    }

    m_matchLabel->style()->unpolish(m_matchLabel);
    m_matchLabel->style()->polish(m_matchLabel);
}

void SearchBar::setMatchCount(int matches, int total)
{
    updateMatchLabel(matches, total);
}

void SearchBar::clearSearch()
{
    m_lineEdit->blockSignals(true);
    m_lineEdit->clear();
    m_lineEdit->blockSignals(false);
    updateClearButton("");
    updateMatchLabel(0, 0);
}

void SearchBar::focusSearch()
{
    m_lineEdit->setFocus();
    m_lineEdit->selectAll();
}

void SearchBar::updateIcons()
{
    const bool isDark = ThemeManager::instance().isDark();
    const QColor iconColor = isDark ? QColor("#797876") : QColor("#BAB9B4");
    m_searchIcon->setPixmap(IconHelper::pixmap("search", iconColor, 14));
}
