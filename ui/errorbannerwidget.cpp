#include "errorbannerwidget.h"

#include <QHBoxLayout>
#include <QSizePolicy>

// Constructor
ErrorBannerWidget::ErrorBannerWidget(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("ErrorBanner");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    setMaximumHeight(CollapsedHeight);
    setMinimumHeight(0);

    setupUi();

    m_animation = new QPropertyAnimation(this, "bannerHeight", this);
    m_animation->setDuration(150);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);
}

// setupUi
void ErrorBannerWidget::setupUi()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 0, 8, 0);
    layout->setSpacing(8);
    layout->setSizeConstraint(QLayout::SetNoConstraint);

    // Warning icon
    m_iconLabel = new QLabel("⚠", this);
    m_iconLabel->setObjectName("ErrorBannerIcon");
    m_iconLabel->setFixedSize(16, 16);
    m_iconLabel->setAlignment(Qt::AlignCenter);

    // Main error message
    m_messageLabel = new QLabel(this);
    m_messageLabel->setObjectName("ErrorBannerText");
    m_messageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_messageLabel->setWordWrap(false);

    // Character offset label (Monospaced in QSS)
    m_offsetLabel = new QLabel(this);
    m_offsetLabel->setObjectName("ErrorBannerOffset");
    m_offsetLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_offsetLabel->hide();

    // Dismiss button (UI/UX Polish)
    m_dismissBtn = new QPushButton("✕", this);
    m_dismissBtn->setObjectName("ErrorBannerDismiss");
    m_dismissBtn->setFixedSize(18, 18);
    m_dismissBtn->setFlat(true);
    m_dismissBtn->setCursor(Qt::PointingHandCursor);
    m_dismissBtn->setToolTip("Dismiss");

    layout->addWidget(m_iconLabel);
    layout->addWidget(m_messageLabel, 1);
    layout->addWidget(m_offsetLabel);
    layout->addWidget(m_dismissBtn);

    connect(m_dismissBtn, &QPushButton::clicked, this, &ErrorBannerWidget::onDismissClicked);
}

// showError
void ErrorBannerWidget::showError(const QString& message, int charOffset)
{
    m_messageLabel->setText(message);

    if (charOffset >= 0) {
        m_offsetLabel->setText(QString("at character %1").arg(charOffset));
        m_offsetLabel->show();
    } else {
        m_offsetLabel->hide();
    }

    if (!m_visible) {
        m_visible = true;
        m_animation->stop();
        m_animation->setStartValue(maximumHeight());
        m_animation->setEndValue(ExpandedHeight);
        m_animation->start();
    }
}

// hideError
void ErrorBannerWidget::hideError()
{
    if (!m_visible) return;
    m_visible = false;

    m_animation->stop();
    m_animation->setStartValue(maximumHeight());
    m_animation->setEndValue(CollapsedHeight);
    m_animation->start();
}

// isShowingError
bool ErrorBannerWidget::isShowingError() const
{
    return m_visible;
}

// Q_PROPERTY accessors
int ErrorBannerWidget::bannerHeight() const
{
    return maximumHeight();
}

void ErrorBannerWidget::setBannerHeight(int h)
{
    setMaximumHeight(h);
    setMinimumHeight(h);
    updateGeometry();
}

// onDismissClicked
void ErrorBannerWidget::onDismissClicked()
{
    hideError();
}
