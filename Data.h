#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QVector>
#include <QPair>

    struct Dte {
    int Y,M,D;
};

//基本設定
struct BaseInfo {
    QString Name;            // 名稱
    QString Author;          // 作者
    QString OSITD;           // 一句話簡介
    QString ITD;             // 作品簡介
    QVector<QString> Types;  // 作品類型列表
    QVector<QString> Tags;   // 作品標籤列表
};

// 世界觀
struct WorldView {
    QPair<Dte,Dte> TRange;       // 時間範圍
    QString TimeInfo;           // 時空背景
    QString BGInfo;             // 背景要素
    QString SPInfo;             // 獨特要素
};

// 角色
struct Character {
    // 左側資料
    QString Name;             // 名稱
    QString Type;             // 人物分類(主角、配角等)
    QString OSITD;            // 一句介紹
    QString Gender;           // 性別
    Dte Birthday;             // 生日
    int Age;                  // 年齡
    QString Appearance;       // 外表
    QString Personality;      // 性格
    QString ITD;              // 介紹

    // 右側列表
    QVector<QString> PTags;         // 優點列表
    QVector<QString> CTags;         // 弱點列表
    QVector<QString> SCharacters;   // 相同面角色
    QVector<QString> OCharacters;   // 對立面角色
};

// 故事梗概
struct StoryOutline {
    QPair<int,int> CRange;    // 章節範圍
    QPair<Dte,Dte> TRange;    // 時間範圍
    QString OSITD;            // 一句簡介
};

// 正文
struct MainText {
    int Chapters;             // 章節編號
    QString Title;            // 章節標題
    QString Content;          // 正文內容
};

#endif // DATA_H
