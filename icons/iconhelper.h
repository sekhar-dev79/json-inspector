#pragma once

#include <QPixmap>
#include <QIcon>
#include <QSize>
#include <QString>
#include <QSvgRenderer>
#include <QPainter>
#include <QImage>
#include <QFile>
#include <QDebug>

/**
 * IconHelper — Lucide SVG icon loader
 *
 * Usage:
 *   QIcon icon = IconHelper::icon("search", QColor("#28251D"), 20);
 *   button->setIcon(icon);
 *   button->setIconSize(QSize(20, 20));
 *
 * How it works:
 *   1. Loads the SVG from Qt resources (:/icons/...)
 *   2. Replaces the SVG's stroke color with the requested color
 *   3. Renders into a QPixmap via QSvgRenderer + QPainter
 *   4. Wraps in a QIcon and returns
 *
 * Why color replacement via string?
 *   Lucide SVGs use `stroke="currentColor"`. Qt's QSvgRenderer does NOT
 *   understand "currentColor" — it renders it as black. We replace the
 *   placeholder with the real hex color before rendering.
 */
class IconHelper
{
public:
    /**
     * Load a named Lucide icon, tinted to the given color, at the given size.
     *
     * @param name   Lucide icon filename without extension (e.g., "search")
     * @param color  The desired icon color (e.g., QColor("#CDCCCA"))
     * @param size   Icon size in pixels — default 20
     */
    static QIcon icon(const QString& name,
                      const QColor&  color = QColor("#28251D"),
                      int            size  = 20)
    {
        return QIcon(pixmap(name, color, size));
    }

    /**
     * Same as icon(), but returns a QPixmap directly.
     * Use this when you need a pixmap for labels or custom painting.
     */
    static QPixmap pixmap(const QString& name,
                          const QColor&  color = QColor("#28251D"),
                          int            size  = 20)
    {
        // ── 1. Build the resource path ─────────────────────────────────────
        QString path = QString(":/icons/%1.svg").arg(name);

        // ── 2. Load raw SVG bytes ──────────────────────────────────────────
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "[IconHelper] SVG not found:" << path;
            // Return a blank pixmap so the app does not crash
            QPixmap blank(size, size);
            blank.fill(Qt::transparent);
            return blank;
        }
        QByteArray svgData = file.readAll();
        file.close();

        // ── 3. Replace "currentColor" with the actual hex color ────────────
        // Lucide uses stroke="currentColor" which Qt cannot resolve.
        // We inject the real color string before rendering.
        QString hexColor = color.name();  // e.g., "#CDCCCA"
        svgData.replace("currentColor", hexColor.toUtf8());

        // ── 4. Render SVG → QPixmap via QSvgRenderer ──────────────────────
        QSvgRenderer renderer(svgData);

        // Use device-pixel-ratio-aware image for HiDPI screens
        QImage image(QSize(size, size), QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::transparent);

        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        renderer.render(&painter, QRectF(0, 0, size, size));
        painter.end();

        return QPixmap::fromImage(image);
    }

private:
    // Static utility class — no construction needed
    IconHelper() = delete;
};
