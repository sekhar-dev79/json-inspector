#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTimer>

class ToolbarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ToolbarWidget(QWidget* parent = nullptr);

    void updateIcons();
    void setCompareButtonActive(bool active);
    void setValidationState(const QString& state);
    void flashButtonLabel(QPushButton* btn, const QString& tempText, int ms = 1800);

    QPushButton* m_formatBtn = nullptr;
    QPushButton* m_minifyBtn = nullptr;
    QPushButton* m_compareBtn = nullptr;

signals:
    void formatRequested();
    void minifyRequested();
    void compareRequested();
    void themeToggleRequested();

private:
    void setupUi();

    QLabel* m_titleLabel = nullptr;
    QLabel* m_validationLabel = nullptr;
    QPushButton* m_themeToggleBtn = nullptr;
};
