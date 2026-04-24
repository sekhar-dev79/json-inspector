#include "mainwindow.h"
#include "toolbarwidget.h"
#include "searchbar.h"
#include "inputpanel.h"
#include "jsontreeview.h"
#include "jsonsearchproxy.h"
#include "jsonsearchdelegate.h"
#include "thememanager.h"
#include "jsonparser.h"
#include "jsonmodel.h"
#include "constants.h"
#include "errorbannerwidget.h"
#include "diffpanel.h"
#include "diffmodel.h"
#include "jsondiff.h"

#include <QVBoxLayout>
#include <QStatusBar>
#include <QShortcut>
#include <QKeySequence>
#include <QCheckBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_jsonModel(new JsonModel(this))
    , m_searchProxy(new JsonSearchProxy(this))
{
    setWindowTitle(AppInfo::Name);
    setMinimumSize(900, 620);
    resize(1100, 720);

    setupUi();
    setupConnections();

    ThemeManager::instance().applyTheme(ThemeManager::instance().currentTheme());
}

void MainWindow::setupUi()
{
    QWidget* central = new QWidget(this);
    central->setObjectName("CentralWidget");
    setCentralWidget(central);

    // Zero margins/spacing to let QSS control padding entirely
    QVBoxLayout* rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    m_toolbar     = new ToolbarWidget(this);
    m_searchBar   = new SearchBar(this);
    m_errorBanner = new ErrorBannerWidget(this);

    // Main horizontal splitter: left panel | tree area
    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->setObjectName("MainSplitter");
    m_splitter->setHandleWidth(1);

    // Left vertical splitter: InputPanel on top, DiffPanel below
    m_leftSplitter = new QSplitter(Qt::Vertical, m_splitter);
    m_leftSplitter->setObjectName("LeftSplitter");
    m_leftSplitter->setHandleWidth(1);

    m_inputPanel = new InputPanel(m_leftSplitter);
    m_diffPanel  = new DiffPanel(m_leftSplitter);
    m_diffPanel->setVisible(false);

    m_leftSplitter->setStretchFactor(0, 1);
    m_leftSplitter->setStretchFactor(1, 1);

    // Right stacked widget area (JSON tree / diff tree)
    QWidget* rightContainer = new QWidget(m_splitter);
    rightContainer->setObjectName("RightContainer");

    QVBoxLayout* rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    m_treeView = new JsonTreeView(rightContainer);
    rightLayout->addWidget(m_treeView);

    // Diff Tree View with UX polish matching the main JsonTreeView
    m_diffTreeView = new QTreeView(rightContainer);
    m_diffTreeView->setObjectName("DiffTreeView");
    m_diffTreeView->setFrameShape(QFrame::NoFrame); // Strip OS borders
    m_diffTreeView->setAlternatingRowColors(true);
    m_diffTreeView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_diffTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_diffTreeView->setUniformRowHeights(true);
    m_diffTreeView->setRootIsDecorated(false);
    m_diffTreeView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_diffTreeView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_diffTreeView->setMouseTracking(true); // Force hover state rendering
    m_diffTreeView->viewport()->setMouseTracking(true);
    m_diffTreeView->setVisible(false);
    rightLayout->addWidget(m_diffTreeView);

    m_splitter->addWidget(m_leftSplitter);
    m_splitter->addWidget(rightContainer);

    // Favor the tree view slightly more for readability
    m_splitter->setStretchFactor(0, 30);
    m_splitter->setStretchFactor(1, 70);
    m_splitter->setSizes({330, 770});

    rootLayout->addWidget(m_toolbar);
    rootLayout->addWidget(m_searchBar);
    rootLayout->addWidget(m_errorBanner);
    rootLayout->addWidget(m_splitter, 1);

    statusBar()->showMessage("Ready — paste JSON to begin");
    statusBar()->setSizeGripEnabled(false);

    // Wire models to views
    m_searchProxy->setSourceModel(m_jsonModel->standardModel());
    m_treeView->setModel(m_searchProxy);
    m_treeView->applyColumnSizing();

    auto* delegate = new JsonSearchDelegate(m_searchProxy, m_treeView);
    m_treeView->setItemDelegate(delegate);

    m_diffModel = new DiffModel(this);
    m_diffTreeView->setModel(m_diffModel->standardModel());
    m_diffTreeView->header()->setStretchLastSection(false);
    m_diffTreeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_diffTreeView->header()->setSectionResizeMode(1, QHeaderView::Interactive);
    m_diffTreeView->header()->setSectionResizeMode(2, QHeaderView::Interactive);
    m_diffTreeView->header()->setSectionResizeMode(3, QHeaderView::Fixed);
    m_diffTreeView->header()->resizeSection(1, 160);
    m_diffTreeView->header()->resizeSection(2, 160);
    m_diffTreeView->header()->resizeSection(3, 72);
}

void MainWindow::setupConnections()
{
    connect(m_toolbar, &ToolbarWidget::themeToggleRequested,
            &ThemeManager::instance(), &ThemeManager::toggleTheme);

    connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &MainWindow::onThemeChanged);

    m_parseDebounce = new QTimer(this);
    m_parseDebounce->setSingleShot(true);
    m_parseDebounce->setInterval(300);

    m_diffParseDebounce = new QTimer(this);
    m_diffParseDebounce->setSingleShot(true);
    m_diffParseDebounce->setInterval(300);

    connect(m_diffPanel, &DiffPanel::textChanged,
            this, [this](const QString&) { m_diffParseDebounce->start(); });

    connect(m_diffParseDebounce, &QTimer::timeout,
            this, [this]() { onDiffJsonTextChanged(m_diffPanel->text()); });

    connect(m_inputPanel, &InputPanel::jsonTextChanged,
            this, [this](const QString&) { m_parseDebounce->start(); });

    connect(m_parseDebounce, &QTimer::timeout,
            this, [this]() { onJsonTextChanged(m_inputPanel->text()); });

    connect(m_toolbar, &ToolbarWidget::formatRequested,
            this, &MainWindow::onFormatRequested);

    connect(m_toolbar, &ToolbarWidget::minifyRequested,
            this, &MainWindow::onMinifyRequested);

    connect(m_jsonModel, &JsonModel::modelPopulated,
            this, &MainWindow::onModelPopulated);

    connect(m_searchBar, &SearchBar::searchChanged,
            this, &MainWindow::onSearchChanged);

    connect(m_searchProxy, &JsonSearchProxy::matchCountChanged,
            this, &MainWindow::onMatchCountChanged);

    connect(m_treeView, &JsonTreeView::copyPerformed,
            this, &MainWindow::onCopyPerformed);

    connect(m_toolbar, &ToolbarWidget::compareRequested,
            this, &MainWindow::onCompareRequested);

    connect(m_diffPanel, &DiffPanel::compareRequested,
            this, &MainWindow::onRunDiff);

    connect(m_diffModel, &DiffModel::populated,
            this, [this](int totalDiffs) {
                m_diffPanel->setSummary(
                    m_diffModel->addedCount(),
                    m_diffModel->removedCount(),
                    m_diffModel->changedCount()
                    );
                statusBar()->showMessage(
                    totalDiffs == 0
                        ? "✓  JSONs are identical"
                        : QString("Diff: +%1 added  -%2 removed  ~%3 changed")
                              .arg(m_diffModel->addedCount())
                              .arg(m_diffModel->removedCount())
                              .arg(m_diffModel->changedCount())
                    );
            });

    setupShortcuts();
}

void MainWindow::setupShortcuts()
{
    auto makeShortcut = [this](QKeySequence seq) -> QShortcut* {
        auto* sc = new QShortcut(seq, this);
        sc->setContext(Qt::WindowShortcut);
        return sc;
    };

    connect(makeShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F)),
            &QShortcut::activated, this, &MainWindow::onFormatRequested);

    connect(makeShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_M)),
            &QShortcut::activated, this, &MainWindow::onMinifyRequested);

    connect(makeShortcut(QKeySequence(Qt::CTRL | Qt::Key_F)),
            &QShortcut::activated, this, [this]() { m_searchBar->focusSearch(); });

    connect(makeShortcut(QKeySequence(Qt::CTRL | Qt::Key_E)),
            &QShortcut::activated, m_treeView, &QTreeView::expandAll);

    connect(makeShortcut(QKeySequence(Qt::CTRL | Qt::Key_W)),
            &QShortcut::activated, this, [this]() {
                m_treeView->collapseAll();
                m_treeView->expandToDepth(0);
            });
}

void MainWindow::onThemeChanged(const QString& themeName)
{
    Q_UNUSED(themeName)
    m_toolbar->updateIcons();
    m_searchBar->updateIcons();
    m_treeView->viewport()->update();
    statusBar()->showMessage(
        QString("Theme: %1").arg(ThemeManager::instance().currentTheme()), 3000
        );
}

void MainWindow::onJsonTextChanged(const QString& text)
{
    if (text.trimmed().isEmpty()) {
        m_jsonModel->clear();
        m_treeView->updateEmptyState();
        m_searchBar->clearSearch();
        clearJsonError();
        m_toolbar->setValidationState("empty");
        m_inputPanel->setSyntaxHighlighting(false);
        statusBar()->showMessage("Ready — paste JSON to begin");
        return;
    }

    const ParseResult result = JsonParser::parse(text);

    if (result.success) {
        clearJsonError();
        m_toolbar->setValidationState("valid");
        m_jsonModel->populate(result.document);
        m_inputPanel->setSyntaxHighlighting(true);
    } else {
        m_jsonModel->clear();
        m_toolbar->setValidationState("invalid");
        m_inputPanel->setSyntaxHighlighting(false);
        showJsonError(result.errorMessage, result.errorOffset);
        statusBar()->showMessage(QString("✗  %1").arg(result.errorMessage));
    }
}

void MainWindow::onDiffJsonTextChanged(const QString& text)
{
    if (text.trimmed().isEmpty()) {
        m_diffPanel->setSyntaxHighlighting(false);
        return;
    }

    const ParseResult result = JsonParser::parse(text);
    m_diffPanel->setSyntaxHighlighting(result.success);
}

void MainWindow::onModelPopulated(int nodeCount)
{
    m_treeView->applyColumnSizing();
    m_treeView->updateEmptyState();

    QTimer::singleShot(0, m_treeView, [this]() {
        m_treeView->expandToDepth(0);
    });

    statusBar()->showMessage(QString("✓  Valid JSON  |  %1 nodes").arg(nodeCount));
}

void MainWindow::onSearchChanged(const QString& term, SearchScope scope)
{
    m_searchProxy->setSearchScope(scope);
    m_searchProxy->setSearchTerm(term);

    if (!term.isEmpty()) {
        QTimer::singleShot(0, m_treeView, [this]() { m_treeView->expandAll(); });
    } else {
        QTimer::singleShot(0, m_treeView, [this]() {
            m_treeView->collapseAll();
            m_treeView->expandToDepth(0);
        });
    }
}

void MainWindow::onMatchCountChanged(int count)
{
    const int total = m_jsonModel->nodeCount();
    m_searchBar->setMatchCount(count, total);

    if (!m_searchProxy->searchTerm().isEmpty()) {
        statusBar()->showMessage(
            count > 0
                ? QString("🔍  %1 match%2 found").arg(count).arg(count == 1 ? "" : "es")
                : QString("No matches for \"%1\"").arg(m_searchProxy->searchTerm())
            );
    }
}

void MainWindow::onFormatRequested()
{
    const ParseResult result = JsonParser::parse(m_inputPanel->text());
    if (result.success) {
        m_inputPanel->setText(JsonParser::format(result.document));
        m_toolbar->flashButtonLabel(m_toolbar->m_formatBtn, "✓ Formatted");
        statusBar()->showMessage("JSON formatted", 3000);
    } else {
        statusBar()->showMessage(QString("Cannot format — %1").arg(result.errorMessage));
    }
}

void MainWindow::onMinifyRequested()
{
    const ParseResult result = JsonParser::parse(m_inputPanel->text());
    if (result.success) {
        m_inputPanel->setText(JsonParser::minify(result.document));
        m_toolbar->flashButtonLabel(m_toolbar->m_minifyBtn, "✓ Minified");
        statusBar()->showMessage("JSON minified", 3000);
    } else {
        statusBar()->showMessage(QString("Cannot minify — %1").arg(result.errorMessage));
    }
}

void MainWindow::onCopyPerformed(const QString& description)
{
    statusBar()->showMessage(QString("✓  %1").arg(description), 2500);
}

void MainWindow::showJsonError(const QString& message, int offset)
{
    m_errorBanner->showError(message, offset);
    if (offset >= 0) {
        m_inputPanel->highlightErrorLine(offset);
        m_inputPanel->highlightError(offset);
    }
}

void MainWindow::clearJsonError()
{
    m_errorBanner->hideError();
    m_inputPanel->clearHighlights();
}

void MainWindow::onCompareRequested()
{
    if (m_diffModeActive) {
        exitDiffMode();
    } else {
        enterDiffMode();
    }
}

void MainWindow::enterDiffMode()
{
    m_diffModeActive = true;

    m_diffPanel->setVisible(true);
    m_diffPanel->show();

    QTimer::singleShot(0, this, [this]() {
        const int total = m_leftSplitter->height();
        m_leftSplitter->setSizes({ total / 2, total / 2 });
    });

    m_treeView->setVisible(false);
    m_diffTreeView->setVisible(true);

    m_toolbar->setCompareButtonActive(true);

    statusBar()->showMessage("Compare mode — paste JSON B below and click Run Diff");
}

void MainWindow::exitDiffMode()
{
    m_diffModeActive = false;

    m_diffPanel->setVisible(false);
    m_diffPanel->clearText();
    m_diffModel->clear();

    QTimer::singleShot(0, this, [this]() {
        const int total = m_leftSplitter->height();
        m_leftSplitter->setSizes({ total, 0 });
    });

    m_diffTreeView->setVisible(false);
    m_treeView->setVisible(true);

    m_toolbar->setCompareButtonActive(false);

    statusBar()->showMessage("Compare mode closed", 3000);
}

void MainWindow::onRunDiff(const QString& jsonB)
{
    if (jsonB.trimmed().isEmpty()) {
        statusBar()->showMessage("Paste JSON B first, then click Run Diff");
        return;
    }

    const ParseResult resultA = JsonParser::parse(m_inputPanel->text());
    if (!resultA.success) {
        statusBar()->showMessage(QString("JSON A is invalid — %1").arg(resultA.errorMessage));
        return;
    }

    const ParseResult resultB = JsonParser::parse(jsonB);
    if (!resultB.success) {
        statusBar()->showMessage(QString("JSON B is invalid — %1").arg(resultB.errorMessage));
        return;
    }

    const QVector<DiffEntry> entries = JsonDiff::compare(resultA.document, resultB.document);

    bool showAll = false;
    if (auto* checkBox = m_diffPanel->findChild<QCheckBox*>("DiffShowAllCheckBox")) {
        showAll = checkBox->isChecked();
    }

    m_diffModel->populate(entries, showAll);
    m_diffTreeView->expandAll();
}
