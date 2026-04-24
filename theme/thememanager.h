#pragma once

#include <QObject>
#include <QString>

/**
 * ThemeManager — Singleton
 *
 * Centralizes all theme logic. Any widget calls:
 *   ThemeManager::instance().applyTheme("dark");
 * and the entire app's stylesheet is replaced immediately.
 *
 * Emits themeChanged(name) so widgets can react (e.g., swap icons).
 */
class ThemeManager : public QObject
{
    Q_OBJECT

public:
    // Returns the single shared instance (Singleton)
    static ThemeManager& instance();

    // Apply a named theme ("light" or "dark")
    void applyTheme(const QString& themeName);

    // Toggle between light and dark
    void toggleTheme();

    // Returns the currently active theme name
    QString currentTheme() const;

    // Returns true if the current theme is dark
    bool isDark() const;

signals:
    // Emitted after a theme is applied — widgets connect here to swap icons
    void themeChanged(const QString& themeName);

private:
    // Private constructor — Singleton: no external construction allowed
    explicit ThemeManager(QObject* parent = nullptr);

    // Deleted copy/move — Singleton must not be copied
    ThemeManager(const ThemeManager&)            = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    QString m_currentTheme;
};
