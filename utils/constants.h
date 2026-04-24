#pragma once

#include <QColor>
#include <Qt>

// ─── Application Identity ─────────────────────────────────────────────────────
namespace AppInfo {
constexpr const char* Name    = "JSON Inspector";
constexpr const char* Version = "1.0.0";
}

// ─── Layout Spacing (4px grid) ────────────────────────────────────────────────
namespace Spacing {
constexpr int Xs =  4;
constexpr int Sm =  6;
constexpr int Md = 10;
constexpr int Lg = 14;
constexpr int Xl = 20;
}

// ─── Icon Sizes ───────────────────────────────────────────────────────────────
namespace IconSize {
constexpr int Small  = 16;
constexpr int Medium = 20;
constexpr int Large  = 24;
}

// ─── Font ─────────────────────────────────────────────────────────────────────
namespace FontDef {
constexpr const char* Family   = "Segoe UI";
constexpr int         SizeSm   = 11;
constexpr int         SizeBase = 13;
constexpr int         SizeMd   = 14;
}

// ─── Theme Names ──────────────────────────────────────────────────────────────
namespace ThemeName {
constexpr const char* Light = "light";
constexpr const char* Dark  = "dark";
}

// ─── JSON Node Types ──────────────────────────────────────────────────────────
enum class NodeKind : int {
    Object  = 0,
    Array   = 1,
    String  = 2,
    Number  = 3,
    Boolean = 4,
    Null    = 5
};

// ─── JSON Tree Column Indices ─────────────────────────────────────────────────
namespace JsonColumn {
constexpr int Key   = 0;
constexpr int Value = 1;
constexpr int Type  = 2;
constexpr int Count = 3;
}

// ─── JSON Model — Custom Item Data Roles ──────────────────────────────────────
namespace JsonRole {
constexpr int NodeType = Qt::UserRole + 1;
constexpr int RawValue = Qt::UserRole + 2;
constexpr int FullPath = Qt::UserRole + 3;
}

// ─── JSON Type Colors (Amber JSON Branding) ───────────────────────────────────
namespace TypeColor {
namespace Light {
constexpr const char* Key     = "#28251D";
constexpr const char* String  = "#D97706"; // Brand Amber
constexpr const char* Number  = "#006494";
constexpr const char* Boolean = "#A12C7B";
constexpr const char* Null    = "#BAB9B4";
constexpr const char* Object  = "#7A7974";
constexpr const char* Array   = "#7A7974";
constexpr const char* Type    = "#BAB9B4";
}
namespace Dark {
constexpr const char* Key     = "#CDCCCA";
constexpr const char* String  = "#F5A623"; // Brand Amber
constexpr const char* Number  = "#4EC9B0";
constexpr const char* Boolean = "#D16969";
constexpr const char* Null    = "#5A5957";
constexpr const char* Object  = "#797876";
constexpr const char* Array   = "#797876";
constexpr const char* Type    = "#5A5957";
}
}

// ─── Search Scope ─────────────────────────────────────────────────────────────
enum class SearchScope {
    KeysAndValues = 0,
    KeysOnly      = 1,
    ValuesOnly    = 2
};

// ─── Search Highlight Colors ──────────────────────────────────────────────────
namespace SearchColor {
namespace Light {
constexpr const char* MatchBg   = "#FDE68A";
constexpr const char* MatchFg   = "#92400E";
constexpr const char* CurrentBg = "#F59E0B";
constexpr const char* CurrentFg = "#FFFFFF";
}
namespace Dark {
constexpr const char* MatchBg   = "#452705";
constexpr const char* MatchFg   = "#FDE68A";
constexpr const char* CurrentBg = "#F5A623";
constexpr const char* CurrentFg = "#121212";
}
}
