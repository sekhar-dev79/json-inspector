#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>

class JsonHighlighter;

class InputPanel : public QWidget
{
    Q_OBJECT

public:
    explicit InputPanel(QWidget* parent = nullptr);

    QString text() const;
    void setText(const QString& text);

    // Highlighting utilities for parsing errors
    void highlightError(int charOffset);
    void highlightErrorLine(int charOffset);
    void clearHighlights();

    void setSyntaxHighlighting(bool valid);

signals:
    void jsonTextChanged(const QString& text);

private:
    void setupUi();

    QTextEdit* m_editor = nullptr;
    QLabel* m_lineLabel = nullptr;
    JsonHighlighter* m_highlighter = nullptr;
};
