#pragma once

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QStandardItem>

// ─── ParseResult ──────────────────────────────────────────────────────────────
struct ParseResult {
    bool          success      = false;
    QJsonDocument document;
    QString       errorMessage;
    int           errorOffset  = -1;
};

// ─── JsonParser ───────────────────────────────────────────────────────────────
class JsonParser
{
public:
    static ParseResult parse(const QString& jsonText);
    static QString format(const QJsonDocument& doc);
    static QString minify(const QJsonDocument& doc);
    static QJsonDocument subtreeToDocument(QStandardItem* item);
    static QString describeError(const QJsonParseError& error);

private:
    static QJsonValue itemToValue(QStandardItem* item);

    JsonParser() = delete;
};
