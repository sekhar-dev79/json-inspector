#pragma once

#include "constants.h"
#include <QMainWindow>
#include <QSplitter>
#include <QTimer>
#include <QTreeView>

class ToolbarWidget;
class SearchBar;
class InputPanel;
class DiffPanel;
class JsonModel;
class DiffModel;
class JsonTreeView;
class JsonSearchProxy;
class ErrorBannerWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onThemeChanged(const QString& themeName);
    void onJsonTextChanged(const QString& text);
    void onDiffJsonTextChanged(const QString& text);
    void onFormatRequested();
    void onMinifyRequested();
    void onModelPopulated(int nodeCount);
    void onSearchChanged(const QString& term, SearchScope scope);
    void onMatchCountChanged(int count);
    void onCopyPerformed(const QString& description);
    void onCompareRequested();
    void onRunDiff(const QString& jsonB);

private:
    void setupUi();
    void setupConnections();
    void setupShortcuts();
    void showJsonError(const QString& message, int offset);
    void clearJsonError();
    void enterDiffMode();
    void exitDiffMode();

    ToolbarWidget* m_toolbar = nullptr;
    SearchBar* m_searchBar = nullptr;
    InputPanel* m_inputPanel = nullptr;
    DiffPanel* m_diffPanel = nullptr;
    QSplitter* m_leftSplitter = nullptr;
    QSplitter* m_splitter = nullptr;
    JsonTreeView* m_treeView = nullptr;
    QTreeView* m_diffTreeView = nullptr;
    ErrorBannerWidget* m_errorBanner = nullptr;

    JsonModel* m_jsonModel = nullptr;
    DiffModel* m_diffModel = nullptr;
    JsonSearchProxy* m_searchProxy = nullptr;

    QTimer* m_parseDebounce = nullptr;
    QTimer* m_diffParseDebounce = nullptr;
    bool m_diffModeActive = false;
};
