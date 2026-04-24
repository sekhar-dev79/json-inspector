#pragma once

#include <QTreeView>
#include <QHeaderView>

class JsonTreeDelegate;
class JsonSearchProxy;

class JsonTreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit JsonTreeView(QWidget* parent = nullptr);
    void applyColumnSizing();
    void expandToLevel(int depth);
    void updateEmptyState();

public slots:
    void onThemeChanged();
    void copySelectedValue();
    void copySelectedPath();
    void copySelectedAsJson();

signals:
    void copyPerformed(const QString& description);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void setupView();
    void setupHeader();
    void setupEmptyState();

    JsonTreeDelegate* m_delegate = nullptr;
    QWidget* m_emptyState = nullptr;
};
