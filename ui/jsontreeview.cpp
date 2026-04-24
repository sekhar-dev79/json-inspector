#include "jsontreeview.h"
#include "jsontreedelegate.h"
#include "thememanager.h"
#include "constants.h"
#include "jsonparser.h"

#include <QHeaderView>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QApplication>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QLabel>
#include <QVBoxLayout>

JsonTreeView::JsonTreeView(QWidget* parent)
    : QTreeView(parent)
{
    setObjectName("JsonTreeView");

    // Enable hover states for QSS pointer triggers
    this->setMouseTracking(true);
    this->viewport()->setMouseTracking(true);

    setupView();
    setupHeader();
    setupEmptyState();

    connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &JsonTreeView::onThemeChanged);
}

void JsonTreeView::setupView()
{
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setAlternatingRowColors(true);
    setAnimated(true);
    setUniformRowHeights(true);
    setWordWrap(false);
    setExpandsOnDoubleClick(true);
    setRootIsDecorated(true);
    setIndentation(16); // Compact nested nodes
    setFocusPolicy(Qt::StrongFocus);

    m_delegate = new JsonTreeDelegate(this);
    setItemDelegate(m_delegate);
}

void JsonTreeView::setupHeader()
{
    QHeaderView* hdr = header();
    hdr->setVisible(true);
    hdr->setStretchLastSection(false);
    hdr->setSectionResizeMode(QHeaderView::Interactive);
    hdr->setMinimumSectionSize(50);
    hdr->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

void JsonTreeView::applyColumnSizing()
{
    QHeaderView* hdr = header();
    if (!model() || model()->columnCount() < JsonColumn::Count) return;

    hdr->setSectionResizeMode(JsonColumn::Key, QHeaderView::Interactive);
    hdr->setSectionResizeMode(JsonColumn::Value, QHeaderView::Stretch);
    hdr->setSectionResizeMode(JsonColumn::Type, QHeaderView::Fixed);
    hdr->resizeSection(JsonColumn::Key, 200);
    hdr->resizeSection(JsonColumn::Type, 68);
}

void JsonTreeView::expandToLevel(int depth)
{
    if (depth < 0) { expandAll(); return; }
    collapseAll();
    expandToDepth(depth);
}

void JsonTreeView::onThemeChanged()
{
    viewport()->update();
}

void JsonTreeView::copySelectedValue()
{
    const QModelIndex idx = currentIndex();
    if (!idx.isValid()) return;

    QModelIndex sourceIdx = idx;
    if (auto* proxy = qobject_cast<QSortFilterProxyModel*>(model())) {
        sourceIdx = proxy->mapToSource(idx);
    }

    const QModelIndex col0 = sourceIdx.sibling(sourceIdx.row(), JsonColumn::Key);
    const QString value = col0.data(JsonRole::RawValue).toString();

    if (!value.isEmpty()) {
        QApplication::clipboard()->setText(value);
        emit copyPerformed(QString("Value copied: \"%1\"")
                               .arg(value.length() > 40 ? value.left(40) + "…" : value));
    }
}

void JsonTreeView::copySelectedPath()
{
    const QModelIndex idx = currentIndex();
    if (!idx.isValid()) return;

    QModelIndex sourceIdx = idx;
    if (auto* proxy = qobject_cast<QSortFilterProxyModel*>(model())) {
        sourceIdx = proxy->mapToSource(idx);
    }

    const QModelIndex col0 = sourceIdx.sibling(sourceIdx.row(), JsonColumn::Key);
    const QString path = col0.data(JsonRole::FullPath).toString();

    if (!path.isEmpty()) {
        QApplication::clipboard()->setText(path);
        emit copyPerformed(QString("Path copied: %1").arg(path));
    }
}

void JsonTreeView::copySelectedAsJson()
{
    const QModelIndex idx = currentIndex();
    if (!idx.isValid()) return;

    QModelIndex sourceIdx = idx;
    if (auto* proxy = qobject_cast<QSortFilterProxyModel*>(model())) {
        sourceIdx = proxy->mapToSource(idx);
    }

    const QModelIndex col0src = sourceIdx.sibling(sourceIdx.row(), JsonColumn::Key);
    auto* stdModel = qobject_cast<QStandardItemModel*>(
        qobject_cast<QSortFilterProxyModel*>(model())
            ? qobject_cast<QSortFilterProxyModel*>(model())->sourceModel()
            : model()
        );

    if (!stdModel) return;

    QStandardItem* item = stdModel->itemFromIndex(col0src);
    if (!item) return;

    const QJsonDocument doc = JsonParser::subtreeToDocument(item);
    const QString json = JsonParser::format(doc);

    QApplication::clipboard()->setText(json);
    emit copyPerformed("Subtree copied as JSON");
}

void JsonTreeView::keyPressEvent(QKeyEvent* event)
{
    const Qt::KeyboardModifiers mods = event->modifiers();
    const int key = event->key();

    if (mods == Qt::ControlModifier && key == Qt::Key_C) {
        copySelectedValue();
        return;
    }
    if (mods == (Qt::ControlModifier | Qt::ShiftModifier) && key == Qt::Key_C) {
        copySelectedPath();
        return;
    }
    if (mods == (Qt::ControlModifier | Qt::AltModifier) && key == Qt::Key_C) {
        copySelectedAsJson();
        return;
    }
    if (mods == Qt::ControlModifier && key == Qt::Key_E) {
        expandAll();
        return;
    }
    if (mods == Qt::ControlModifier && key == Qt::Key_W) {
        collapseAll();
        expandToDepth(0);
        return;
    }

    QTreeView::keyPressEvent(event);
}

void JsonTreeView::setupEmptyState()
{
    m_emptyState = new QWidget(viewport());
    m_emptyState->setObjectName("EmptyStateWidget");

    // Force transparency so QSS background works cleanly
    m_emptyState->setAttribute(Qt::WA_TranslucentBackground);
    m_emptyState->setAttribute(Qt::WA_TransparentForMouseEvents);

    QVBoxLayout* layout = new QVBoxLayout(m_emptyState);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(4);

    QLabel* iconLabel = new QLabel("{ }", m_emptyState);
    iconLabel->setObjectName("EmptyStateIcon");
    iconLabel->setAlignment(Qt::AlignCenter);

    QLabel* titleLabel = new QLabel("No JSON loaded", m_emptyState);
    titleLabel->setObjectName("EmptyStateTitle");
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel* subLabel = new QLabel("Paste JSON in the editor on the left", m_emptyState);
    subLabel->setObjectName("EmptyStateSubtitle");
    subLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(iconLabel);
    layout->addSpacing(8);
    layout->addWidget(titleLabel);
    layout->addWidget(subLabel);

    m_emptyState->setGeometry(viewport()->rect());
    m_emptyState->setVisible(true);
}

void JsonTreeView::contextMenuEvent(QContextMenuEvent* event)
{
    const QModelIndex idx = indexAt(event->pos());
    if (!idx.isValid()) return;

    QModelIndex sourceIdx = idx;
    if (auto* proxy = qobject_cast<QSortFilterProxyModel*>(model())) {
        sourceIdx = proxy->mapToSource(idx);
    }

    const QModelIndex col0 = sourceIdx.sibling(sourceIdx.row(), JsonColumn::Key);
    if (!col0.isValid()) return;

    const QString keyText = col0.data(Qt::DisplayRole).toString();

    QMenu menu(this);

    // Strip OS borders for clean, rounded QSS menu corners
    menu.setAttribute(Qt::WA_TranslucentBackground);
    menu.setWindowFlags(menu.windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

    QAction* copyValueAction = menu.addAction("Copy Value\tCtrl+C");
    QAction* copyPathAction = menu.addAction("Copy Full Path\tCtrl+Shift+C");
    QAction* copyJsonAction = menu.addAction("Copy as JSON\tCtrl+Alt+C");
    QAction* copyKeyAction = menu.addAction("Copy Key");
    menu.addSeparator();
    QAction* expandAction = menu.addAction("Expand All Children");
    QAction* collapseAction = menu.addAction("Collapse All Children");

    const QAction* chosen = menu.exec(event->globalPos());

    if (chosen == copyValueAction) { copySelectedValue(); }
    else if (chosen == copyPathAction) { copySelectedPath(); }
    else if (chosen == copyJsonAction) { copySelectedAsJson(); }
    else if (chosen == copyKeyAction) {
        QApplication::clipboard()->setText(keyText);
        emit copyPerformed(QString("Key copied: %1").arg(keyText));
    }
    else if (chosen == expandAction) { expandRecursively(idx); }
    else if (chosen == collapseAction) { collapse(idx); }
}

void JsonTreeView::resizeEvent(QResizeEvent* event)
{
    QTreeView::resizeEvent(event);
    if (m_emptyState) {
        m_emptyState->setGeometry(viewport()->rect());
    }
}

void JsonTreeView::updateEmptyState()
{
    if (!m_emptyState) return;
    const bool isEmpty = (model() == nullptr || model()->rowCount() == 0);
    m_emptyState->setVisible(isEmpty);
}
