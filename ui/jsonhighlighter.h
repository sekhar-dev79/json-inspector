#pragma once

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>

class JsonHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit JsonHighlighter(QTextDocument* parent = nullptr);

    void setEnabled(bool enabled);

public slots:
    void updateTheme();

protected:
    void highlightBlock(const QString& text) override;

private:
    void setupRules();

    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> m_rules;
    bool m_enabled = false;
};
