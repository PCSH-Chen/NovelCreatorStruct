#ifndef FILE_H
#define FILE_H

#include "Data.h"
#include <QString>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

class FileManager {
public:
    FileManager(const QString& basePath = "data") : m_basePath(basePath) {
        ensureDirectoryExists();
    }

    // === 儲存功能實作 ===
    bool SaveBaseInfo(const BaseInfo& inf) {
        QFile file(m_basePath + "/" + BASE_INFO_FILE);
        if (!file.open(QIODevice::WriteOnly)) {
            return false;
        }
        QJsonDocument doc(baseInfoToJson(inf));
        file.write(doc.toJson());
        return true;
    }

    bool SaveWorldView(const WorldView& inf) {
        QFile file(m_basePath + "/" + WORLD_VIEW_FILE);
        if (!file.open(QIODevice::WriteOnly)) {
            return false;
        }
        QJsonDocument doc(worldViewToJson(inf));
        file.write(doc.toJson());
        return true;
    }

    bool SaveCharacter(const QVector<Character>& inf) {
        QFile file(m_basePath + "/" + CHARACTERS_FILE);
        if (!file.open(QIODevice::WriteOnly)) {
            return false;
        }
        QJsonArray array;
        for (const auto& character : inf) {
            array.append(characterToJson(character));
        }
        QJsonDocument doc(array);
        file.write(doc.toJson());
        return true;
    }

    bool SaveStoryOutline(const QVector<StoryOutline>& inf) {
        QFile file(m_basePath + "/" + STORY_OUTLINE_FILE);
        if (!file.open(QIODevice::WriteOnly)) {
            return false;
        }
        QJsonArray array;
        for (const auto& outline : inf) {
            array.append(storyOutlineToJson(outline));
        }
        QJsonDocument doc(array);
        file.write(doc.toJson());
        return true;
    }
public:
    //Load
    bool LoadBaseInfo(BaseInfo& inf) {
        QFile file(m_basePath + "/" + BASE_INFO_FILE);
        if (!file.open(QIODevice::ReadOnly)) {
            return false;
        }

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isNull()) {
            return false;
        }

        QJsonObject json = doc.object();
        inf.Name = json["name"].toString();
        inf.Author = json["author"].toString();
        inf.OSITD = json["ositd"].toString();
        inf.ITD = json["itd"].toString();

        // 讀取類型列表
        inf.Types.clear();
        QJsonArray typesArray = json["types"].toArray();
        for (const auto& type : typesArray) {
            inf.Types.append(type.toString());
        }

        // 讀取標籤列表
        inf.Tags.clear();
        QJsonArray tagsArray = json["tags"].toArray();
        for (const auto& tag : tagsArray) {
            inf.Tags.append(tag.toString());
        }

        return true;
    }

    bool LoadWorldView(WorldView& inf) {
        QFile file(m_basePath + "/" + WORLD_VIEW_FILE);
        if (!file.open(QIODevice::ReadOnly)) {
            return false;
        }

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isNull()) {
            return false;
        }

        QJsonObject json = doc.object();

        // 讀取時間範圍
        QJsonObject timeRange = json["timeRange"].toObject();
        inf.TRange.first = jsonToDate(timeRange["start"].toObject());
        inf.TRange.second = jsonToDate(timeRange["end"].toObject());

        inf.TimeInfo = json["timeInfo"].toString();
        inf.BGInfo = json["bgInfo"].toString();
        inf.SPInfo = json["spInfo"].toString();

        return true;
    }

    bool LoadCharacter(QVector<Character>& inf) {
        QFile file(m_basePath + "/" + CHARACTERS_FILE);
        if (!file.open(QIODevice::ReadOnly)) {
            return false;
        }

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isNull()) {
            return false;
        }

        QJsonArray array = doc.array();
        inf.clear();

        for (const auto& value : array) {
            QJsonObject charJson = value.toObject();
            Character character;

            // 讀取基本資訊
            character.Name = charJson["name"].toString();
            character.Type = charJson["type"].toString();
            character.OSITD = charJson["ositd"].toString();
            character.Gender = charJson["gender"].toString();
            character.Birthday = jsonToDate(charJson["birthday"].toObject());
            character.Age = charJson["age"].toInt();
            character.Appearance = charJson["appearance"].toString();
            character.Personality = charJson["personality"].toString();
            character.ITD = charJson["itd"].toString();

            // 讀取列表資訊
            QJsonArray pTags = charJson["pTags"].toArray();
            QJsonArray cTags = charJson["cTags"].toArray();
            QJsonArray sCharacters = charJson["sCharacters"].toArray();
            QJsonArray oCharacters = charJson["oCharacters"].toArray();

            for (const auto& tag : pTags) {
                character.PTags.append(tag.toString());
            }
            for (const auto& tag : cTags) {
                character.CTags.append(tag.toString());
            }
            for (const auto& char_ : sCharacters) {
                character.SCharacters.append(char_.toString());
            }
            for (const auto& char_ : oCharacters) {
                character.OCharacters.append(char_.toString());
            }

            inf.append(character);
        }

        return true;
    }

    bool LoadStoryOutline(QVector<StoryOutline>& inf) {
        QFile file(m_basePath + "/" + STORY_OUTLINE_FILE);
        if (!file.open(QIODevice::ReadOnly)) {
            return false;
        }

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isNull()) {
            return false;
        }

        QJsonArray array = doc.array();
        inf.clear();

        for (const auto& value : array) {
            QJsonObject outlineJson = value.toObject();
            StoryOutline outline;

            // 讀取章節範圍
            QJsonObject chapterRange = outlineJson["chapterRange"].toObject();
            outline.CRange.first = chapterRange["start"].toInt();
            outline.CRange.second = chapterRange["end"].toInt();

            // 讀取時間範圍
            QJsonObject timeRange = outlineJson["timeRange"].toObject();
            outline.TRange.first = jsonToDate(timeRange["start"].toObject());
            outline.TRange.second = jsonToDate(timeRange["end"].toObject());

            outline.OSITD = outlineJson["ositd"].toString();

            inf.append(outline);
        }

        return true;
    }

    bool LoadMainText(QVector<MainText>& inf) {
        QString chapterPath = m_basePath + "/" + MAIN_TEXT_DIR;
        QDir dir(chapterPath);
        if (!dir.exists()) {
            return false;
        }

        // 獲取所有 JSON 檔案並按名稱排序
        QStringList filters;
        filters << "chapter_*.json";
        QStringList files = dir.entryList(filters, QDir::Files, QDir::Name);

        inf.clear();
        for (const QString& file : files) {
            QFile chapterFile(chapterPath + "/" + file);
            if (!chapterFile.open(QIODevice::ReadOnly)) {
                continue;
            }

            QJsonDocument doc = QJsonDocument::fromJson(chapterFile.readAll());
            if (doc.isNull()) {
                continue;
            }

            QJsonObject json = doc.object();
            MainText text;
            text.Chapters = json["chapter"].toInt();
            text.Title = json["title"].toString();
            text.Content = json["content"].toString();

            inf.append(text);
        }

        return true;
    }

    bool SaveMainText(const QVector<MainText>& inf) {
        QString chapterPath = m_basePath + "/" + MAIN_TEXT_DIR;
        QDir dir(chapterPath);
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        // 儲存每個章節為獨立檔案
        for (const auto& text : inf) {
            QString filename = QString("%1/chapter_%2.json")
            .arg(chapterPath)
                .arg(text.Chapters, 4, 10, QChar('0'));

            QFile file(filename);
            if (!file.open(QIODevice::WriteOnly)) {
                return false;
            }
            QJsonDocument doc(mainTextToJson(text));
            file.write(doc.toJson());
        }
        return true;
    }

private:
    // === JSON 轉換實作 ===
    QJsonObject dateToJson(const Dte& date) {
        QJsonObject json;
        json["year"] = date.Y;
        json["month"] = date.M;
        json["day"] = date.D;
        return json;
    }

    Dte jsonToDate(const QJsonObject& json) {
        Dte date;
        date.Y = json["year"].toInt();
        date.M = json["month"].toInt();
        date.D = json["day"].toInt();
        return date;
    }

    QJsonObject baseInfoToJson(const BaseInfo& inf) {
        QJsonObject json;
        json["name"] = inf.Name;
        json["author"] = inf.Author;
        json["ositd"] = inf.OSITD;
        json["itd"] = inf.ITD;

        QJsonArray typesArray;
        for (const auto& type : inf.Types) {
            typesArray.append(type);
        }
        json["types"] = typesArray;

        QJsonArray tagsArray;
        for (const auto& tag : inf.Tags) {
            tagsArray.append(tag);
        }
        json["tags"] = tagsArray;

        return json;
    }

    QJsonObject worldViewToJson(const WorldView& inf) {
        QJsonObject json;

        // 時間範圍
        QJsonObject timeRange;
        timeRange["start"] = dateToJson(inf.TRange.first);
        timeRange["end"] = dateToJson(inf.TRange.second);
        json["timeRange"] = timeRange;

        json["timeInfo"] = inf.TimeInfo;
        json["bgInfo"] = inf.BGInfo;
        json["spInfo"] = inf.SPInfo;

        return json;
    }

    QJsonObject characterToJson(const Character& character) {
        QJsonObject json;

        // 基本資訊
        json["name"] = character.Name;
        json["type"] = character.Type;
        json["ositd"] = character.OSITD;
        json["gender"] = character.Gender;
        json["birthday"] = dateToJson(character.Birthday);
        json["age"] = character.Age;
        json["appearance"] = character.Appearance;
        json["personality"] = character.Personality;
        json["itd"] = character.ITD;

        // 列表資訊
        QJsonArray pTags, cTags, sCharacters, oCharacters;

        for (const auto& tag : character.PTags) pTags.append(tag);
        for (const auto& tag : character.CTags) cTags.append(tag);
        for (const auto& char_ : character.SCharacters) sCharacters.append(char_);
        for (const auto& char_ : character.OCharacters) oCharacters.append(char_);

        json["pTags"] = pTags;
        json["cTags"] = cTags;
        json["sCharacters"] = sCharacters;
        json["oCharacters"] = oCharacters;

        return json;
    }

    QJsonObject storyOutlineToJson(const StoryOutline& outline) {
        QJsonObject json;

        // 章節範圍
        QJsonObject chapterRange;
        chapterRange["start"] = outline.CRange.first;
        chapterRange["end"] = outline.CRange.second;
        json["chapterRange"] = chapterRange;

        // 時間範圍
        QJsonObject timeRange;
        timeRange["start"] = dateToJson(outline.TRange.first);
        timeRange["end"] = dateToJson(outline.TRange.second);
        json["timeRange"] = timeRange;

        json["ositd"] = outline.OSITD;

        return json;
    }

    QJsonObject mainTextToJson(const MainText& text) {
        QJsonObject json;
        json["chapter"] = text.Chapters;
        json["title"] = text.Title;
        json["content"] = text.Content;
        return json;
    }

    // === 目錄管理 ===
    bool ensureDirectoryExists() {
        QDir dir(m_basePath);
        if (!dir.exists()) {
            if (!dir.mkpath(".")) {
                return false;
            }
        }
        // 確保章節目錄存在
        QString chapterPath = m_basePath + "/" + MAIN_TEXT_DIR;
        QDir chapterDir(chapterPath);
        if (!chapterDir.exists()) {
            if (!chapterDir.mkpath(".")) {
                return false;
            }
        }
        return true;
    }

    // === 檔案路徑 ===
    QString m_basePath;
    const QString BASE_INFO_FILE = "base_info.json";
    const QString WORLD_VIEW_FILE = "world_view.json";
    const QString CHARACTERS_FILE = "characters.json";
    const QString STORY_OUTLINE_FILE = "story_outline.json";
    const QString MAIN_TEXT_DIR = "chapters";
};

#endif // FILE_H
