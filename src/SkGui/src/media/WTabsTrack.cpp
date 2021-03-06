//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WTabsTrack.h"

#ifndef SK_NO_TABSTRACK

// Qt includes
#include <QXmlStreamWriter>

// Sk includes
#include <WControllerApplication>
#include <WControllerXml>
#include <WControllerPlaylist>
#include <WTabTrack>
#include <WAbstractThreadAction>

// 3rdparty includes
#include <qtlockedfile>

// 3rdparty namespaces
using namespace QtLP_Private;

//=================================================================================================
// WTabsTrackWrite and WTabsTrackWriteReply
//=================================================================================================

struct WTabsTrackDataTab
{
    int id;
};

class WTabsTrackWrite : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WTabsTrackWrite(WTabsTrackPrivate * data)
    {
        this->data = data;

        name    = sk->name   ();
        version = sk->version();
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    WTabsTrackPrivate * data;

    QString path;

    QString name;
    QString version;

    int currentId;

    QList<WTabsTrackDataTab> dataTabs;
};

//-------------------------------------------------------------------------------------------------

class WTabsTrackWriteReply : public WAbstractThreadReply
{
    Q_OBJECT

public:
    WTabsTrackWriteReply(WTabsTrackPrivate * data)
    {
        this->data = data;
    }

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

public: // Variables
    WTabsTrackPrivate * data;
};

//=================================================================================================
// WTabsTrackWrite
//=================================================================================================

/* virtual */ WAbstractThreadReply * WTabsTrackWrite::createReply() const
{
    return new WTabsTrackWriteReply(data);
}

/* virtual */ bool WTabsTrackWrite::run()
{
    QtLockedFile file(path);

    QTimer timer;

    timer.start(60000); // 1 minute timeout

    while (file.isLocked() && timer.isActive()) ;

    if (file.isLocked())
    {
        qWarning("WTabsTrackWrite::run: File is locked %s.", path.C_STR);

        return false;
    }

    if (file.open(QIODevice::WriteOnly) == false)
    {
        qWarning("WTabsTrackWrite::run: Failed to open file %s.", path.C_STR);

        return false;
    }

    file.lock(QtLockedFile::WriteLock);

    QXmlStreamWriter stream(&file);

    stream.setAutoFormatting(true);

    stream.writeStartDocument();

    stream.writeStartElement(name);

    stream.writeTextElement("version", version);

    stream.writeStartElement("tabsTrack");

    stream.writeTextElement("currentId", QString::number(currentId));

    stream.writeStartElement("tabs");

    foreach (const WTabsTrackDataTab & data, dataTabs)
    {
        stream.writeStartElement("tab");

        stream.writeTextElement("id", QString::number(data.id));

        stream.writeEndElement(); // tab
    }

    stream.writeEndElement(); // tabs
    stream.writeEndElement(); // tabsTrack
    stream.writeEndElement(); // name

    stream.writeEndDocument();

    file.unlock();

    qDebug("TABS SAVED");

    return true;
}

//=================================================================================================
// WTabsTrackWriteReply
//=================================================================================================

/* virtual */ void WTabsTrackWriteReply::onCompleted(bool ok)
{
    data->setSaved(ok);
}

//=================================================================================================
// WTabsTrackRead and WTabsTrackReadReply
//=================================================================================================

class WTabsTrackReadReply;

class WTabsTrackRead : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WTabsTrackRead(WTabsTrackPrivate * data)
    {
        this->data = data;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

private: // Functions
    bool load(QXmlStreamReader * stream, WTabsTrackReadReply * reply);

public: // Variables
    WTabsTrackPrivate * data;

    QString path;
};

class WTabsTrackReadReply : public WAbstractThreadReply
{
    Q_OBJECT

public:
    WTabsTrackReadReply(WTabsTrackPrivate * data)
    {
        this->data = data;
    }

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

public: // Variables
    WTabsTrackPrivate * data;

    int currentId;

    QList<WTabsTrackDataTab> dataTabs;
};


//=================================================================================================
// WTabsTrackRead
//=================================================================================================

/* virtual */ WAbstractThreadReply * WTabsTrackRead::createReply() const
{
    return new WTabsTrackReadReply(data);
}

/* virtual */ bool WTabsTrackRead::run()
{
    WTabsTrackReadReply * reply = qobject_cast<WTabsTrackReadReply *> (this->reply());

    QtLockedFile file(path);

    QTimer timer;

    timer.start(60000); // 1 minute timeout

    while (file.isLocked() && timer.isActive()) ;

    if (file.isLocked())
    {
        qWarning("WTabsTrackRead::run: File is locked %s.", path.C_STR);

        return false;
    }

    if (file.open(QIODevice::ReadOnly) == false)
    {
        qWarning("WTabsTrackRead::run: Failed to open file %s.", path.C_STR);

        return false;
    }

    file.lock(QtLockedFile::ReadLock);

    QXmlStreamReader stream(&file);

    if (load(&stream, reply) == false)
    {
        qWarning("WTabsTrackRead::run: Invalid file %s.", path.C_STR);

        return false;
    }

    file.unlock();

    qDebug("TABS LOADED");

    return true;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

bool WTabsTrackRead::load(QXmlStreamReader * stream, WTabsTrackReadReply * reply)
{
    if (wControllerXml->readNextStartElement(stream, "currentId") == false) return false;

    reply->currentId = wControllerXml->readNextInt(stream);

    while (wControllerXml->readNextStartElement(stream, "tab"))
    {
        WTabsTrackDataTab data;

        if (wControllerXml->readNextStartElement(stream, "id") == false) return false;

        data.id = wControllerXml->readNextInt(stream);

        reply->dataTabs.append(data);
    }

    return true;
}

//=================================================================================================
// WTabsTrackReadReply
//=================================================================================================

/* virtual */ void WTabsTrackReadReply::onCompleted(bool ok)
{
    WTabsTrack * q = data->q_func();

    bool saveEnabled = q->saveEnabled();

    q->setSaveEnabled(false);

    if (ok)
    {
        QList<WAbstractTab *> tabs;

        foreach (const WTabsTrackDataTab & dataTab, dataTabs)
        {
            WAbstractTab * tab = data->createTab();

            tab->setId(dataTab.id);

            tab->setSaveEnabled(saveEnabled);

            tabs.append(tab);
        }

        data->loadTabs(tabs);

        if (currentId != -1)
        {
            q->setCurrentId(currentId);
        }
        else q->setCurrentTab(NULL);
    }

    q->setSaveEnabled(saveEnabled);

    data->setLoaded(ok);
}

//=================================================================================================
// WTabsTrackPrivate
//=================================================================================================

WTabsTrackPrivate::WTabsTrackPrivate(WTabsTrack * p) : WAbstractTabsPrivate(p) {}

void WTabsTrackPrivate::init()
{
    highlightedTab   = NULL;
    highlightedIndex = -1;
}

//-------------------------------------------------------------------------------------------------
// Slots
//-------------------------------------------------------------------------------------------------

void WTabsTrackPrivate::onHighlightedTabDestroyed()
{
    Q_Q(WTabsTrack); q->setHighlightedTab(NULL);
}

//=================================================================================================
// WTabsTrack
//=================================================================================================

/* explicit */ WTabsTrack::WTabsTrack(QObject * parent)
    : WAbstractTabs(new WTabsTrackPrivate(this), parent)
{
    Q_D(WTabsTrack); d->init();
}

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WTabsTrack::closeTabs()
{
    Q_D(WTabsTrack);

    setCurrentIndex(0);

    while (d->tabs.count() > 1)
    {
        deleteTab(d->tabs.last());
    }

    if (d->currentTab)
    {
        d->currentTab->toTabTrack()->clearBookmarks();
    }
}

/* Q_INVOKABLE */ void WTabsTrack::closeOtherTabs(WAbstractTab * tab)
{
    Q_D(WTabsTrack);

    setCurrentTab(tab);

    for (int i = 0; i < d->tabs.count(); i++)
    {
        WAbstractTab * abstractTab = d->tabs.at(i);

        if (abstractTab != tab)
        {
            deleteTab(abstractTab);

            i--;
        }
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WTabTrack * WTabsTrack::currentTabTrack() const
{
    Q_D(const WTabsTrack);

    if (d->currentTab)
    {
         return d->currentTab->toTabTrack();
    }
    else return NULL;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WTabTrack * WTabsTrack::tabBookmarkAt(int index) const
{
    WAbstractTab * tab = tabAt(index);

    if (tab)
    {
         return tab->toTabTrack();
    }
    else return NULL;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WTabTrack * WTabsTrack::tabFromSource(const QUrl & source) const
{
    Q_D(const WTabsTrack);

    foreach (WAbstractTab * tab, d->tabs)
    {
        WTabTrack * tabTrack = tab->toTabTrack();

        if (tabTrack->source() == source)
        {
            return tabTrack;
        }
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------
// WLocalObject reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WTabsTrack::getParentPath() const
{
    return wControllerPlaylist->pathStorageTabs();
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractTabs implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractTab * WTabsTrack::createTab(WAbstractTabs * parent) const
{
    return new WTabTrack(parent);
}

//-------------------------------------------------------------------------------------------------
// Protected WLocalObject reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractThreadAction * WTabsTrack::onSave(const QString & path)
{
    Q_D(WTabsTrack);

    WTabsTrackWrite * action = new WTabsTrackWrite(d);

    action->path = path;

    if (d->highlightedTab)
    {
         action->currentId = d->highlightedTab->id();
    }
    else action->currentId = currentId();

    foreach (WAbstractTab * tab, d->tabs)
    {
        WTabsTrackDataTab data;

        data.id = tab->id();

        action->dataTabs.append(data);
    }

    return action;
}

/* virtual */ WAbstractThreadAction * WTabsTrack::onLoad(const QString & path)
{
    Q_D(WTabsTrack);

    WTabsTrackRead * action = new WTabsTrackRead(d);

    action->path = path;

    return action;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WTabsTrack::hasFolder() const
{
    return true;
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractTabs reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WTabsTrack::updateIndex()
{
    Q_D(WTabsTrack);

    WAbstractTabs::updateIndex();

    int index = d->tabs.indexOf(d->highlightedTab);

    if (d->highlightedIndex != index)
    {
        d->highlightedIndex = index;

        emit highlightedIndexChanged();
    }
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WTabTrack * WTabsTrack::highlightedTab() const
{
    Q_D(const WTabsTrack); return d->highlightedTab;
}

void WTabsTrack::setHighlightedTab(WTabTrack * tab)
{
    Q_D(WTabsTrack);

    if (d->highlightedTab == tab || (tab && d->tabs.contains(tab) == false)) return;

    if (d->highlightedTab)
    {
        disconnect(d->highlightedTab, 0, this, 0);
    }

    d->highlightedTab   = tab;
    d->highlightedIndex = d->tabs.indexOf(tab);

    if (d->highlightedTab)
    {
        connect(d->highlightedTab, SIGNAL(destroyed()), this, SLOT(onHighlightedTabDestroyed()));
    }

    emit highlightedTabChanged  ();
    emit highlightedIndexChanged();

    save();
}

//-------------------------------------------------------------------------------------------------

int WTabsTrack::highlightedIndex() const
{
    Q_D(const WTabsTrack); return d->highlightedIndex;
}

void WTabsTrack::setHighlightedIndex(int index)
{
    WTabTrack * tabBookmark = tabBookmarkAt(index);

    if (tabBookmark || index == -1)
    {
        setHighlightedTab(tabBookmark);
    }
}

#endif // SK_NO_TABSTRACK

#include "WTabsTrack.moc"
