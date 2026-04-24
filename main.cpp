#include <QApplication>
#include <QFont>
#include <QIcon>

#include "mainwindow.h"
#include "thememanager.h"
#include "constants.h"

int main(int argc, char* argv[])
{
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough
        );
    QApplication app(argc, argv);

    app.setApplicationName(AppInfo::Name);
    app.setApplicationVersion(AppInfo::Version);
    app.setOrganizationName("JsonInspector");

    // ── Set the Global Application Icon ──
    app.setWindowIcon(QIcon(":/icons/app_icon.png"));

    // Set the global application baseline font
    QFont appFont(FontDef::Family, FontDef::SizeBase);
    app.setFont(appFont);
    app.setStyle("Fusion");

    // Initialize the ThemeManager and load the OS-aware default theme
    ThemeManager::instance().currentTheme();

    MainWindow window;
    window.show();

    return app.exec();
}
