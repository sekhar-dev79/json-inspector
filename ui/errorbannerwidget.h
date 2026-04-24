#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPropertyAnimation>

class ErrorBannerWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int bannerHeight READ bannerHeight WRITE setBannerHeight)

public:
    explicit ErrorBannerWidget(QWidget* parent = nullptr);

    void showError(const QString& message, int charOffset = -1);
    void hideError();
    bool isShowingError() const;

    int  bannerHeight() const;
    void setBannerHeight(int h);

private slots:
    void onDismissClicked();

private:
    void setupUi();

    QLabel* m_iconLabel    = nullptr;
    QLabel* m_messageLabel = nullptr;
    QLabel* m_offsetLabel  = nullptr;
    QPushButton* m_dismissBtn   = nullptr;
    QPropertyAnimation* m_animation    = nullptr;

    bool m_visible = false;

    static constexpr int CollapsedHeight = 0;
    static constexpr int ExpandedHeight  = 24;
};
