#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QEvent>

#include "constants.h"

// Search panel with scope combobox and match count tracking
class SearchBar : public QWidget
{
    Q_OBJECT

public:
    explicit SearchBar(QWidget* parent = nullptr);

    void setMatchCount(int matches, int total);
    void clearSearch();
    void focusSearch();
    void updateIcons();

signals:
    void searchChanged(const QString& term, SearchScope scope);

protected:
    // Required to intercept focus events from the child line edit
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setupUi();
    void onTextChanged(const QString& text);
    void onScopeChanged(int index);
    void updateClearButton(const QString& text);
    void updateMatchLabel(int matches, int total);

    QWidget* m_wrapper = nullptr;
    QLabel* m_searchIcon = nullptr;
    QLineEdit* m_lineEdit = nullptr;
    QPushButton* m_clearBtn = nullptr;
    QComboBox* m_scopeBox = nullptr;
    QLabel* m_matchLabel = nullptr;
};
