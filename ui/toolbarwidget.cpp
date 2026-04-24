#include "toolbarwidget.h"
#include "thememanager.h"
#include "iconhelper.h"
#include "constants.h"

#include <QHBoxLayout>
#include <QSizePolicy>
#include <QStyle>

ToolbarWidget::ToolbarWidget(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("ToolbarWidget");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setupUi();
}

void ToolbarWidget::setupUi()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 0, 8, 0);
    layout->setSpacing(6);

    m_titleLabel = new QLabel(AppInfo::Name, this);
    m_titleLabel->setObjectName("AppTitleLabel");

    // Separator helper lambda
    auto makeSep = [this]() -> QWidget* {
        QWidget* sep = new QWidget(this);
        sep->setObjectName("ToolbarSeparator");
        sep->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        return sep;
    };

    // Action buttons
    m_formatBtn  = new QPushButton("Format",  this);
    m_minifyBtn  = new QPushButton("Minify",  this);
    m_compareBtn = new QPushButton("Compare", this);

    m_formatBtn->setObjectName("FormatButton");
    m_minifyBtn->setObjectName("MinifyButton");
    m_compareBtn->setObjectName("CompareButton");

    m_formatBtn->setToolTip("Format JSON  (Ctrl+Shift+F)");
    m_minifyBtn->setToolTip("Minify JSON  (Ctrl+Shift+M)");
    m_compareBtn->setToolTip("Compare two JSONs");

    for (auto* btn : {m_formatBtn, m_minifyBtn, m_compareBtn}) {
        btn->setIconSize(QSize(16, 16));
        btn->setCursor(Qt::PointingHandCursor);
    }

    // Validation indicator pill
    m_validationLabel = new QLabel("—", this);
    m_validationLabel->setObjectName("ValidationIndicator");
    m_validationLabel->setProperty("state", "empty");
    m_validationLabel->setAlignment(Qt::AlignCenter);

    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Theme toggle
    m_themeToggleBtn = new QPushButton(this);
    m_themeToggleBtn->setObjectName("ThemeToggleButton");
    m_themeToggleBtn->setToolTip("Toggle light / dark theme");
    m_themeToggleBtn->setFlat(true);
    m_themeToggleBtn->setIconSize(QSize(16, 16));
    m_themeToggleBtn->setCursor(Qt::PointingHandCursor);

    layout->addWidget(m_titleLabel);
    layout->addSpacing(8);
    layout->addWidget(makeSep());
    layout->addWidget(m_formatBtn);
    layout->addWidget(m_minifyBtn);
    layout->addWidget(makeSep());
    layout->addWidget(m_compareBtn);
    layout->addSpacing(8);
    layout->addWidget(m_validationLabel);
    layout->addWidget(spacer);
    layout->addWidget(m_themeToggleBtn);

    connect(m_formatBtn,      &QPushButton::clicked, this, &ToolbarWidget::formatRequested);
    connect(m_minifyBtn,      &QPushButton::clicked, this, &ToolbarWidget::minifyRequested);
    connect(m_compareBtn,     &QPushButton::clicked, this, &ToolbarWidget::compareRequested);
    connect(m_themeToggleBtn, &QPushButton::clicked, this, &ToolbarWidget::themeToggleRequested);

    updateIcons();
}

void ToolbarWidget::updateIcons()
{
    const bool isDark = ThemeManager::instance().isDark();
    const QColor iconColor = isDark ? QColor("#CDCCCA") : QColor("#28251D");

    m_formatBtn->setIcon(IconHelper::icon("code-2", iconColor, 16));
    m_minifyBtn->setIcon(IconHelper::icon("minimize-2", iconColor, 16));

    if (!m_compareBtn->property("active").toBool()) {
        m_compareBtn->setIcon(IconHelper::icon("git-compare", iconColor, 16));
    }

    const QString toggleIcon = isDark ? "sun" : "moon";
    m_themeToggleBtn->setIcon(IconHelper::icon(toggleIcon, iconColor, 16));
}

void ToolbarWidget::setCompareButtonActive(bool active)
{
    if (!m_compareBtn) return;

    if (active) {
        m_compareBtn->setText("✕  Close Diff");
        m_compareBtn->setIcon(QIcon());
        m_compareBtn->setProperty("active", true);
    } else {
        m_compareBtn->setText("Compare");
        m_compareBtn->setProperty("active", false);

        const bool isDark = ThemeManager::instance().isDark();
        const QColor iconColor = isDark ? QColor("#CDCCCA") : QColor("#28251D");
        m_compareBtn->setIcon(IconHelper::icon("git-compare", iconColor, 16));
    }

    m_compareBtn->style()->unpolish(m_compareBtn);
    m_compareBtn->style()->polish(m_compareBtn);
    m_compareBtn->update();
}

void ToolbarWidget::setValidationState(const QString& state)
{
    if (state == "valid") {
        m_validationLabel->setText("✓ Valid");
    } else if (state == "invalid") {
        m_validationLabel->setText("✗ Error");
    } else {
        m_validationLabel->setText("—");
    }

    m_validationLabel->setProperty("state", state);
    m_validationLabel->style()->unpolish(m_validationLabel);
    m_validationLabel->style()->polish(m_validationLabel);
}

void ToolbarWidget::flashButtonLabel(QPushButton* btn, const QString& tempText, int ms)
{
    if (!btn) return;
    const QString original = btn->text();
    btn->setText(tempText);

    if (ms > 0 && ms < 9999) {
        QTimer::singleShot(ms, btn, [btn, original]() {
            if (btn) btn->setText(original);
        });
    }
}
