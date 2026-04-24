#include "jsonhighlighter.h"
#include "constants.h"
#include "thememanager.h"

JsonHighlighter::JsonHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    setupRules();

    connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &JsonHighlighter::updateTheme);
}

void JsonHighlighter::setEnabled(bool enabled)
{
    if (m_enabled == enabled) return;
    m_enabled = enabled;
    rehighlight();
}

void JsonHighlighter::updateTheme()
{
    setupRules();
    if (m_enabled) {
        rehighlight();
    }
}

void JsonHighlighter::setupRules()
{
    m_rules.clear();
    const bool isDark = ThemeManager::instance().isDark();

    auto color = [&](const char* darkHex, const char* lightHex) {
        return QColor(isDark ? darkHex : lightHex);
    };

    HighlightingRule rule;

    // Strings
    rule.format.setForeground(color(TypeColor::Dark::String, TypeColor::Light::String));
    rule.format.setFontWeight(QFont::Normal);
    rule.pattern = QRegularExpression(R"(\"([^\"\\]|\\.)*\")");
    m_rules.append(rule);

    // Keys (Uses positive lookahead to exclude the colon from formatting)
    rule.format.setForeground(color(TypeColor::Dark::Key, TypeColor::Light::Key));
    rule.format.setFontWeight(QFont::DemiBold);
    rule.pattern = QRegularExpression(R"(\"([^\"\\]|\\.)*\"(?=\s*:))");
    m_rules.append(rule);

    // Numbers
    rule.format.setForeground(color(TypeColor::Dark::Number, TypeColor::Light::Number));
    rule.format.setFontWeight(QFont::Normal);
    rule.pattern = QRegularExpression(R"(-?(?:0|[1-9]\d*)(?:\.\d+)?(?:[eE][+-]?\d+)?)");
    m_rules.append(rule);

    // Booleans
    rule.format.setForeground(color(TypeColor::Dark::Boolean, TypeColor::Light::Boolean));
    rule.pattern = QRegularExpression(R"(\b(?:true|false)\b)");
    m_rules.append(rule);

    // Null
    rule.format.setForeground(color(TypeColor::Dark::Null, TypeColor::Light::Null));
    rule.pattern = QRegularExpression(R"(\bnull\b)");
    m_rules.append(rule);
}

void JsonHighlighter::highlightBlock(const QString& text)
{
    if (!m_enabled) return;

    for (const HighlightingRule& rule : std::as_const(m_rules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
