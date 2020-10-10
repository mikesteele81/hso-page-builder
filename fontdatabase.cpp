#include "fontdatabase.h"
#include <QSettings>
#include <QFile>
#include <QSet>

const QString HYPNO_PATH = "/home/minirop/.local/share/Steam/steamapps/common/Hypnospace Outlaw/data/";

FontDatabase::FontDatabase()
{
    instance = this;
}

void FontDatabase::load(QString directory)
{
    assert(QFile(directory + "fontdata.ini").exists());
    QFile fontdata(directory + "fontdata.ini");
    if (fontdata.open(QFile::ReadOnly))
    {
        auto lines = fontdata.readAll().split('\n');
        for (auto & line : lines)
        {
            line = line.trimmed();
        }
        lines.removeAll(QByteArray());
        assert(lines.size() % 4 == 0);

        for (int i = 0; i < lines.size(); i += 4)
        {
            auto name = lines[i + 0].replace("[", "").replace("]", "").toLower();
            auto spacing = lines[i + 1].mid(8).toInt();
            auto lineheight = lines[i + 2].mid(11).toInt();
            auto charwidths = lines[i + 3].mid(11).split('^');

            QVector<char> chars;

            bool processChars = true;
            for (auto d : charwidths)
            {
                if (processChars)
                {
                    chars.clear();

                    for (auto c : d)
                    {
                        chars.append(c);
                    }
                }
                else
                {
                    auto width = d.toInt();

                    for (auto c : chars)
                    {
                        fonts[name].widths[c] = width;
                    }

                    chars.clear();
                }

                processChars = !processChars;
            }

            fonts[name].spacing = spacing;
            fonts[name].lineheight = lineheight;
        }
    }
}

FontDatabase::FontData & FontDatabase::GetFont(QString name)
{
    return instance->fonts[name];
}

QList<QString> FontDatabase::GetFonts()
{
    auto keys = instance->fonts.keys();
    QSet<QString> uniqueKeys;
    for (auto key : keys)
    {
        uniqueKeys.insert(key.left(key.size() - 2));
    }

    return uniqueKeys.values();
}

QPixmap FontDatabase::GetFontAtlas(QString name)
{
    return QPixmap(QString("%1images/fonts/%2.png").arg(HYPNO_PATH, name.toLower()));
}

int FontDatabase::FontData::getWidth(char c, int defaultWidth)
{
    if (widths.contains(c))
    {
        return widths[c] + spacing;
    }

    return defaultWidth + spacing;
}
