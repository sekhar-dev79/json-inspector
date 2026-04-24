#include "thememanager.h"
#include "../utils/constants.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QGuiApplication>
#include <QStyleHints>

// ─── Singleton accessor ───────────────────────────────────────────────────────
ThemeManager& ThemeManager::instance()
{
    static ThemeManager s_instance;
    return s_instance;
}

// ─── Private Constructor ──────────────────────────────────────────────────────
ThemeManager::ThemeManager(QObject* parent)
    : QObject(parent)
    , m_currentTheme(ThemeName::Light)
{
    // Auto-detect OS color scheme (Qt 6.5+)
    auto scheme = QGuiApplication::styleHints()->colorScheme();
    if (scheme == Qt::ColorScheme::Dark) {
        m_currentTheme = ThemeName::Dark;
    }
}

// ─── applyTheme ───────────────────────────────────────────────────────────────
void ThemeManager::applyTheme(const QString& themeName)
{
    QString path = QString(":/themes/%1.qss").arg(themeName);

    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "[ThemeManager] Critical: Could not open theme file:" << path;
        return;
    }

    QTextStream stream(&file);
    QString styleSheet = stream.readAll();
    file.close();

    // Apply globally to every widget in the application
    qApp->setStyleSheet(styleSheet);
    m_currentTheme = themeName;

    emit themeChanged(themeName);
}

// ─── toggleTheme ─────────────────────────────────────────────────────────────
void ThemeManager::toggleTheme()
{
    if (isDark()) {
        applyTheme(ThemeName::Light);
    } else {
        applyTheme(ThemeName::Dark);
    }
}

// ─── currentTheme ─────────────────────────────────────────────────────────────
QString ThemeManager::currentTheme() const
{
    return m_currentTheme;
}

// ─── isDark ──────────────────────────────────────────────────────────────────
bool ThemeManager::isDark() const
{
    return m_currentTheme == ThemeName::Dark;
}
