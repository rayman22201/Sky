//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkBackend module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WBackendTmdb.h"

#ifndef SK_NO_BACKENDTMDB

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>
#include <WControllerPlaylist>

//-------------------------------------------------------------------------------------------------
// Static variables

static const QString BACKENDTMDB_MATCH      = "[,.:\\-_(){}\\[\\]]";
static const QString BACKENDTMDB_MATCH_SHOW = "[\\-_(){}\\[\\]]";

static const int BACKENDTMDB_YEAR = 1800;

static const int BACKENDTMDB_HASH_MAX = 1000;

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WBackendNet_p>

class SK_BACKEND_EXPORT WBackendTmdbPrivate : public WBackendNetPrivate
{
public: // Enums
    enum Type
    {
        Default,
        Movie,
        Show
    };

public:
    WBackendTmdbPrivate(WBackendTmdb * p);

    void init();

public: // Functions
    void applyQuery(WBackendNetQuery * query, const QString & label, const QString & q) const;

    QString extractShow(const QString & label) const;

    bool match(const QStringList & listA, const QStringList & listB) const;

    Type getType(const QString & data) const;

    QStringList getList     (const QString & data) const;
    QStringList getListClear(const QString & data) const;

public: // Variables
    QList<QString>          shows;
    QHash<QString, QString> hash;

protected:
    W_DECLARE_PUBLIC(WBackendTmdb)
};

//-------------------------------------------------------------------------------------------------

WBackendTmdbPrivate::WBackendTmdbPrivate(WBackendTmdb * p) : WBackendNetPrivate(p) {}

void WBackendTmdbPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBackendTmdbPrivate::applyQuery(WBackendNetQuery * query, const QString & label,
                                                               const QString & q) const
{
    QStringList list = getListClear(q.toLower());

    QString title;

    for (int i = 0; i < list.count(); i++)
    {
        QString string = list.at(i);

        Type type = getType(string);

        if (type == Movie)
        {
            int index = i + 1;

            if (index < list.count())
            {
                QString data = list.at(index);

                if (data.length() == 4 && data.toInt() >= BACKENDTMDB_YEAR)
                {
                    title.append(string);

                    string = data;
                }
                else
                {
                    if (title.isEmpty()) return;

                    title.chop(1);
                }
            }
            else
            {
                if (title.isEmpty()) return;

                title.chop(1);
            }

            QUrl url("https://www.themoviedb.org/search");

#ifdef QT_4
            url.addQueryItem("query", title + " y:" + string);

            url.addQueryItem("language", "en");
#else
            QUrlQuery urlQuery(url);

            urlQuery.addQueryItem("query", title + " y:" + string);

            urlQuery.addQueryItem("language", "en");

            url.setQuery(urlQuery);
#endif

            query->url = url;

            query->data = title;

            return;
        }
        else if (type == Show)
        {
            int index = string.indexOf('e');

            int season = string.mid(1, index - 1).toInt();

            if (season < 1) return;

            int episode = string.mid(index + 1).toInt();

            if (episode < 1) return;

            QString show = extractShow(label);

            QString source = hash.value(show);

            if (source.isEmpty())
            {
                QVariantList variants;

                variants.append(show);
                variants.append(season);
                variants.append(episode);

                QUrl url("https://www.themoviedb.org/search/tv");

#ifdef QT_4
                url.addQueryItem("query", show);

                url.addQueryItem("language", "en");
#else
                QUrlQuery urlQuery(url);

                urlQuery.addQueryItem("query", show);

                urlQuery.addQueryItem("language", "en");

                url.setQuery(urlQuery);
#endif

                query->url = url;

                query->id   = 2;
                query->data = variants;
            }
            else
            {
                query->url = source + QString::number(season) + "?language=en";

                query->id   = 3;
                query->data = episode;
            }

            return;
        }
        else if (string.endsWith('p'))
        {
            QString test = string;

            test.chop(1);

            if (test.toInt()) break;
        }

        title.append(string + ' ');
    }

    if (title.isEmpty()) return;

    title.chop(1);

    QUrl url("https://www.themoviedb.org/search");

#ifdef QT_4
    url.addQueryItem("query", title);

    url.addQueryItem("language", "en");
#else
    QUrlQuery urlQuery(url);

    urlQuery.addQueryItem("query", title);

    urlQuery.addQueryItem("language", "en");

    url.setQuery(urlQuery);
#endif

    query->url = url;

    query->data = title;
}

//-------------------------------------------------------------------------------------------------

QString WBackendTmdbPrivate::extractShow(const QString & label) const
{
    QStringList list;

    int index = label.indexOf(QRegExp(BACKENDTMDB_MATCH_SHOW));

    if (index == -1)
    {
         list = getListClear(label.toLower());
    }
    else list = getListClear(label.mid(0, index).toLower());

    QString show;

    for (int i = 0; i < list.count(); i++)
    {
        QString string = list.at(i);

        if (string.startsWith('s'))
        {
            int index  = 1;
            int length = string.length();

            while (index < length && string.at(index).isDigit())
            {
                index++;
            }

            if (index == length && show.isEmpty() == false)
            {
                show.chop(1);

                return show;
            }

            if (string.at(index) == 'e')
            {
                index++;

                while (index < length && string.at(index).isDigit())
                {
                    index++;

                    if (index == length && show.isEmpty() == false)
                    {
                        show.chop(1);

                        return show;
                    }
                }
            }

            index = i + 1;

            if (index != list.count() && list.at(index).toInt() && show.isEmpty() == false)
            {
                show.chop(1);

                return show;
            }
        }

        show.append(string + ' ');
    }

    if (show.isEmpty() == false)
    {
        show.chop(1);
    }

    return show;
}

//-------------------------------------------------------------------------------------------------

bool WBackendTmdbPrivate::match(const QStringList & listA, const QStringList & listB) const
{
    int from = 0;

    foreach (const QString & string, listA)
    {
        from = listB.indexOf(string, from);

        if (from == -1)
        {
            from = 0;

            foreach (const QString & string, listB)
            {
                from = listA.indexOf(string, from);

                if (from == -1) return false;
            }

            return true;
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

QStringList WBackendTmdbPrivate::getList(const QString & data) const
{
    QString result = data;

    result = result.replace(QRegExp(BACKENDTMDB_MATCH), " ");

    result = result.simplified();

    return result.split(' ');
}

QStringList WBackendTmdbPrivate::getListClear(const QString & data) const
{
    QStringList list = getList(data);

    while (list.count() && list.first().toInt())
    {
        list.removeFirst();
    }

    return list;
}

//-------------------------------------------------------------------------------------------------

WBackendTmdbPrivate::Type WBackendTmdbPrivate::getType(const QString & data) const
{
    if (data.length() == 4)
    {
        if (data.toInt() >= BACKENDTMDB_YEAR)
        {
            return Movie;
        }
    }
    else if (data.startsWith('s'))
    {
        int index  = 1;
        int length = data.length();

        while (index < length && data.at(index).isDigit())
        {
            index++;
        }

        if (index == length || data.at(index) != 'e')
        {
            return Default;
        }

        index++;

        while (index < length && data.at(index).isDigit())
        {
            index++;
        }

        if (index == length)
        {
            return Show;
        }
    }

    return Default;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendTmdb::WBackendTmdb() : WBackendNet(new WBackendTmdbPrivate(this))
{
    Q_D(WBackendTmdb); d->init();
}

//-------------------------------------------------------------------------------------------------
// WBackendNet implementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendTmdb::getId() const
{
    return "tmdb";
}

/* Q_INVOKABLE virtual */ QString WBackendTmdb::getTitle() const
{
    return "TMDb";
}

//-------------------------------------------------------------------------------------------------
// WBackendNet reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendTmdb::isSearchCover() const
{
    return true;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendTmdb::checkValidUrl(const QUrl & url) const
{
    QString source = WControllerNetwork::removeUrlPrefix(url);

    return source.startsWith("themoviedb.org");
}

/* Q_INVOKABLE virtual */ bool WBackendTmdb::checkCover(const QString &, const QString & q) const
{
    QString extension = WControllerNetwork::extractUrlExtension(q);

    return WControllerPlaylist::extensionIsVideo(extension);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendTmdb::createQuery(const QString & method,
                                           const QString & label, const QString & q) const
{
    WBackendNetQuery query;

    if (method == "cover")
    {
        Q_D(const WBackendTmdb);

        d->applyQuery(&query, label, q);
    }

    return query;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetTrack WBackendTmdb::extractTrack(const QByteArray       & data,
                                            const WBackendNetQuery & query) const
{
    WBackendNetTrack reply;

    QString content = Sk::readUtf8(data);

    if (query.id == 1) // Movie
    {
        int index = content.indexOf("<div class=\"image_content\">");

        if (index == -1) return reply;

        QString cover = WControllerNetwork::extractAttribute(content, "href", index);

        reply.track.setCover(cover);
    }
    else if (query.id == 2) // Show
    {
        QStringList list = Sk::slices(content, "<div class=\"image_content\">",
                                               "<p class=\"overview\">");

        if (list.isEmpty()) return reply;

        Q_D(const WBackendTmdb);

        QString string = list.first();

        QVariantList variants = query.data.toList();

        QString title = WControllerNetwork::extractAttributeUtf8(string, "title").toLower();

        QStringList listA = d->getList(variants.first().toString());
        QStringList listB = d->getList(title);

        if (d->match(listA, listB) == false && list.length() != 1) return reply;

        QString source = WControllerNetwork::extractAttribute(string, "href");

        source = WControllerNetwork::removeUrlExtension(source);

        WBackendNetQuery * nextQuery = &(reply.nextQuery);

        nextQuery->url = "https://www.themoviedb.org" + source
                         +
                         "/season/" + variants.at(1).toString() + "?language=en";

        nextQuery->id   = 3;
        nextQuery->data = variants.last();
    }

    return reply;
}

#endif // SK_NO_BACKENDTMDB
