#include "jsonparser.h"
#include "constants.h"

#include <QJsonParseError>

// ─── parse ────────────────────────────────────────────────────────────────────
ParseResult JsonParser::parse(const QString& jsonText)
{
    ParseResult result;

    if (jsonText.trimmed().isEmpty()) {
        result.errorMessage = "Input is empty";
        return result;
    }

    QJsonParseError error;
    result.document = QJsonDocument::fromJson(jsonText.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        result.success      = false;
        result.errorMessage = describeError(error);
        result.errorOffset  = error.offset;
    } else {
        result.success = true;
    }

    return result;
}

// ─── format ──────────────────────────────────────────────────────────────────
QString JsonParser::format(const QJsonDocument& doc)
{
    return QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
}

// ─── minify ──────────────────────────────────────────────────────────────────
QString JsonParser::minify(const QJsonDocument& doc)
{
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}

// ─── subtreeToDocument ───────────────────────────────────────────────────────
QJsonDocument JsonParser::subtreeToDocument(QStandardItem* item)
{
    if (!item) return QJsonDocument();

    const QJsonValue val = itemToValue(item);

    if (val.isObject()) {
        return QJsonDocument(val.toObject());
    } else if (val.isArray()) {
        return QJsonDocument(val.toArray());
    } else {
        QJsonObject wrapper;
        wrapper.insert(item->text(), val);
        return QJsonDocument(wrapper);
    }
}

// ─── itemToValue (private, recursive) ────────────────────────────────────────
QJsonValue JsonParser::itemToValue(QStandardItem* item)
{
    if (!item) return QJsonValue(QJsonValue::Null);

    const NodeKind nt = static_cast<NodeKind>(
        item->data(JsonRole::NodeType).toInt()
        );

    switch (nt) {

    case NodeKind::Object: {
        QJsonObject obj;
        for (int r = 0; r < item->rowCount(); ++r) {
            QStandardItem* child = item->child(r, JsonColumn::Key);
            if (child) {
                obj.insert(child->text(), itemToValue(child));
            }
        }
        return QJsonValue(obj);
    }

    case NodeKind::Array: {
        QJsonArray arr;
        for (int r = 0; r < item->rowCount(); ++r) {
            QStandardItem* child = item->child(r, JsonColumn::Key);
            if (child) {
                arr.append(itemToValue(child));
            }
        }
        return QJsonValue(arr);
    }

    case NodeKind::String:
        return QJsonValue(item->data(JsonRole::RawValue).toString());

    case NodeKind::Number: {
        bool ok = false;
        const QString raw = item->data(JsonRole::RawValue).toString();
        const qlonglong i = raw.toLongLong(&ok);
        if (ok) return QJsonValue(static_cast<double>(i));
        return QJsonValue(raw.toDouble());
    }

    case NodeKind::Boolean:
        return QJsonValue(item->data(JsonRole::RawValue).toString() == "true");

    case NodeKind::Null:
    default:
        return QJsonValue(QJsonValue::Null);
    }
}

// ─── describeError ───────────────────────────────────────────────────────────
QString JsonParser::describeError(const QJsonParseError& error)
{
    // FIX: Removed the "(at character %2)" injection.
    // The ErrorBannerWidget handles displaying the character offset natively
    // in the UI, so we only need to pass the clean error message here.
    return error.errorString();
}
