#pragma once

#include <QString>
#include <QJsonValue>

// ─── DiffType ─────────────────────────────────────────────────────────────────
enum class DiffType : int {
    Unchanged = 0,   // key and value identical in both
    Added     = 1,   // key present in B only
    Removed   = 2,   // key present in A only
    Changed   = 3    // key present in both, values differ
};

// ─── DiffEntry ────────────────────────────────────────────────────────────────
/**
 * Represents one unit of difference between JSON A and JSON B.
 */
struct DiffEntry {
    QString    path;
    DiffType   type = DiffType::Unchanged;
    QJsonValue oldValue;   // A side
    QJsonValue newValue;   // B side

    QString typeLabel() const {
        switch (type) {
        case DiffType::Added:     return "added";
        case DiffType::Removed:   return "removed";
        case DiffType::Changed:   return "changed";
        case DiffType::Unchanged: return "same";
        }
        return "same";
    }

    bool isDifferent() const {
        return type != DiffType::Unchanged;
    }
};
