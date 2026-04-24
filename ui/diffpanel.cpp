#include "diffpanel.h"
#include "constants.h"
#include "jsonhighlighter.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QStyle>

DiffPanel::DiffPanel(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("DiffPanel");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setupUi();
}

void DiffPanel::setupUi()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Header bar setup
    QWidget* header = new QWidget(this);
    header->setObjectName("DiffPanelHeader");
    header->setFixedHeight(32);

    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(8, 0, 8, 0);
    headerLayout->setSpacing(6);

    QLabel* label = new QLabel("JSON B  —  paste to compare", header);
    label->setObjectName("DiffPanelLabel");

    m_summaryLabel = new QLabel("", header);
    m_summaryLabel->setObjectName("DiffSummaryLabel");

    m_showAllChk = new QCheckBox("Show unchanged", header);
    m_showAllChk->setObjectName("DiffShowAllCheckBox");
    m_showAllChk->setChecked(false);
    m_showAllChk->setCursor(Qt::PointingHandCursor);

    m_runBtn = new QPushButton("Run Diff", header);
    m_runBtn->setObjectName("DiffRunButton");
    m_runBtn->setFixedHeight(24);
    m_runBtn->setCursor(Qt::PointingHandCursor);

    headerLayout->addWidget(label);
    headerLayout->addStretch();
    headerLayout->addWidget(m_summaryLabel);
    headerLayout->addSpacing(8);
    headerLayout->addWidget(m_showAllChk);
    headerLayout->addSpacing(6);
    headerLayout->addWidget(m_runBtn);

    // Editor setup
    m_editor = new QTextEdit(this);
    m_editor->setObjectName("DiffInputEditor");
    m_editor->setFrameStyle(QFrame::NoFrame);
    m_editor->setPlaceholderText("Paste second JSON here to compare with JSON A above…");
    m_editor->setAcceptRichText(false);
    m_editor->setLineWrapMode(QTextEdit::NoWrap);
    m_editor->setTabStopDistance(24.0); // Compact indentation
    m_editor->document()->setDocumentMargin(6); // Add breathing room for text

    m_highlighter = new JsonHighlighter(m_editor->document());

    layout->addWidget(header);
    layout->addWidget(m_editor, 1);

    connect(m_runBtn, &QPushButton::clicked, this, [this]() {
        emit compareRequested(m_editor->toPlainText().trimmed());
    });

    connect(m_editor, &QTextEdit::textChanged, this, [this]() {
        emit textChanged(m_editor->toPlainText());
    });
}

QString DiffPanel::text() const
{
    return m_editor->toPlainText();
}

void DiffPanel::clearText()
{
    m_editor->clear();
    m_summaryLabel->setText("");
    setSyntaxHighlighting(false);
}

void DiffPanel::setSummary(int added, int removed, int changed)
{
    if (added == 0 && removed == 0 && changed == 0) {
        m_summaryLabel->setText("✓  JSONs are identical");
        m_summaryLabel->setProperty("summaryState", "identical");
    } else {
        m_summaryLabel->setText(QString("+%1  -%2  ~%3").arg(added).arg(removed).arg(changed));
        m_summaryLabel->setProperty("summaryState", "diff");
    }

    // Force QSS re-evaluation
    m_summaryLabel->style()->unpolish(m_summaryLabel);
    m_summaryLabel->style()->polish(m_summaryLabel);
}

void DiffPanel::setSyntaxHighlighting(bool valid)
{
    m_highlighter->setEnabled(valid);
}
