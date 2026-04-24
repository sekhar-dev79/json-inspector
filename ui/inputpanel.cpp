#include "inputpanel.h"
#include "thememanager.h"
#include "jsonhighlighter.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextBlock>
#include <QSizePolicy>
#include <QScrollBar>

InputPanel::InputPanel(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("InputPanel");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setupUi();
}

void InputPanel::setupUi()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_editor = new QTextEdit(this);
    m_editor->setObjectName("JsonInputEditor");
    m_editor->setFrameStyle(QFrame::NoFrame);

    m_editor->document()->setDocumentMargin(6);

    m_editor->setPlaceholderText(
        "Paste JSON here…\n\n"
        "Example:\n"
        "{\n"
        "  \"name\": \"JSON Inspector\",\n"
        "  \"version\": 1\n"
        "}"
        );

    m_editor->setAcceptRichText(false);
    m_editor->setLineWrapMode(QTextEdit::NoWrap);
    m_editor->setTabStopDistance(24.0);

    m_highlighter = new JsonHighlighter(m_editor->document());

    // Footer for cursor position tracking
    QWidget* footer = new QWidget(this);
    footer->setObjectName("InputPanelFooter");
    footer->setFixedHeight(22);

    QHBoxLayout* footerLayout = new QHBoxLayout(footer);
    footerLayout->setContentsMargins(8, 0, 8, 0);

    m_lineLabel = new QLabel("Ln 1, Col 1", footer);
    m_lineLabel->setObjectName("CursorPositionLabel");

    footerLayout->addStretch();
    footerLayout->addWidget(m_lineLabel);

    layout->addWidget(m_editor, 1);
    layout->addWidget(footer);

    connect(m_editor, &QTextEdit::textChanged, this, [this]() {
        emit jsonTextChanged(m_editor->toPlainText());
    });

    connect(m_editor, &QTextEdit::cursorPositionChanged, this, [this]() {
        const QTextCursor c = m_editor->textCursor();
        const int line = c.blockNumber() + 1;
        const int col  = c.positionInBlock() + 1;
        m_lineLabel->setText(QString("Ln %1, Col %2").arg(line).arg(col));
    });
}

QString InputPanel::text() const
{
    return m_editor->toPlainText();
}

void InputPanel::setText(const QString& text)
{
    const int savedPos = m_editor->textCursor().position();

    m_editor->blockSignals(true);
    m_editor->setPlainText(text);
    m_editor->blockSignals(false);

    QTextCursor cursor = m_editor->textCursor();
    const int newLen = m_editor->document()->characterCount();
    cursor.setPosition(qMin(savedPos, newLen - 1));
    m_editor->setTextCursor(cursor);

    emit jsonTextChanged(text);
}

void InputPanel::setSyntaxHighlighting(bool valid)
{
    m_highlighter->setEnabled(valid);
}

void InputPanel::highlightError(int charOffset)
{
    if (charOffset < 0) return;

    QTextDocument* doc = m_editor->document();
    if (charOffset >= doc->characterCount()) return;

    QList<QTextEdit::ExtraSelection> selections = m_editor->extraSelections();

    QTextCursor underlineCursor(doc);
    underlineCursor.setPosition(charOffset);
    underlineCursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);

    if (!underlineCursor.hasSelection() && charOffset > 0) {
        underlineCursor.setPosition(charOffset - 1);
        underlineCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
    }

    QTextEdit::ExtraSelection underlineSel;
    underlineSel.cursor = underlineCursor;
    underlineSel.format.setUnderlineStyle(QTextCharFormat::WaveUnderline);

    const bool isDark = ThemeManager::instance().isDark();
    underlineSel.format.setUnderlineColor(isDark ? QColor("#D163A7") : QColor("#A12C7B"));

    selections.append(underlineSel);
    m_editor->setExtraSelections(selections);

    m_editor->setTextCursor(underlineCursor);
    m_editor->ensureCursorVisible();
}

void InputPanel::highlightErrorLine(int charOffset)
{
    if (charOffset < 0) return;

    QTextDocument* doc = m_editor->document();
    if (charOffset >= doc->characterCount()) return;

    QTextCursor lineCursor(doc);
    lineCursor.setPosition(charOffset);
    lineCursor.movePosition(QTextCursor::StartOfBlock);
    lineCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

    QTextEdit::ExtraSelection lineSel;
    lineSel.cursor = lineCursor;

    const bool isDark = ThemeManager::instance().isDark();
    lineSel.format.setBackground(isDark ? QColor("#4C3D46") : QColor("#F8BBD0"));
    lineSel.format.setProperty(QTextFormat::FullWidthSelection, true);

    m_editor->setExtraSelections({ lineSel });
}

void InputPanel::clearHighlights()
{
    m_editor->setExtraSelections({});
}
