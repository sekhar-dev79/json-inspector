#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

class JsonHighlighter;

class DiffPanel : public QWidget
{
    Q_OBJECT

public:
    explicit DiffPanel(QWidget* parent = nullptr);

    QString text() const;
    void clearText();
    void setSummary(int added, int removed, int changed);
    void setSyntaxHighlighting(bool valid);

signals:
    void compareRequested(const QString& jsonB);
    void textChanged(const QString& text);

private:
    void setupUi();

    QTextEdit* m_editor = nullptr;
    QPushButton* m_runBtn = nullptr;
    QCheckBox* m_showAllChk = nullptr;
    QLabel* m_summaryLabel = nullptr;
    JsonHighlighter* m_highlighter = nullptr;
};
