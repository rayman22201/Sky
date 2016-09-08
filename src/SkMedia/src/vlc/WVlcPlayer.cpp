//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkMedia module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WVlcPlayer.h"

#ifndef SK_NO_VLCPLAYER

// Qt includes
#include <QCoreApplication>
#include <QMutex>

// Sk includes
#include <private/Sk_p>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int VLCPLAYER_NETWORK_CACHE = 200;

//=================================================================================================
// WVlcPlayerPrivate
//=================================================================================================

class SK_MEDIA_EXPORT WVlcPlayerPrivate : public WPrivate
{
public: // Enums
    enum EventType
    {
        EventCreate = QEvent::User,
        EventBackend,   // WVlcPlayerEventBackend
        EventSetSource, // WVlcPlayerEvent QUrl
        EventPlay,
        EventPause,
        EventStop,
        EventSeekTo,    // WVlcPlayerEvent int
        EventSetSpeed,  // WVlcPlayerEvent qreal
        EventSetVolume, // WVlcPlayerEvent int
        EventDelete
    };

public:
    WVlcPlayerPrivate(WVlcPlayer * p);

    void init(WVlcEngine * engine, QThread * thread);

public: // Functions
    QString encodeUrl(const QUrl & url) const;

public: // Static functions
    static void eventMedia(const struct libvlc_event_t * event, void * data);

    static void eventPlaying(const struct libvlc_event_t * event, void * data);
    static void eventPaused (const struct libvlc_event_t * event, void * data);
    static void eventStopped(const struct libvlc_event_t * event, void * data);

    static void eventBuffering(const struct libvlc_event_t * event, void * data);

    static void eventLengthChanged(const struct libvlc_event_t * event, void * data);
    static void eventTimeChanged  (const struct libvlc_event_t * event, void * data);

    static void eventEndReached(const struct libvlc_event_t * event, void * data);

    static void eventEncounteredError(const struct libvlc_event_t * event, void * data);

public: // Variables
    QMutex mutex;

    WVlcEngine * engine;

    libvlc_media_player_t * player;

    QObject * backend;

    QStringList options;

    bool repeat;

    int networkCache;

    QString proxyHost;
    QString proxyPassword;

protected:
    W_DECLARE_PUBLIC(WVlcPlayer)
};

//-------------------------------------------------------------------------------------------------

WVlcPlayerPrivate::WVlcPlayerPrivate(WVlcPlayer * p) : WPrivate(p) {}

void WVlcPlayerPrivate::init(WVlcEngine * engine, QThread * thread)
{
    Q_Q(WVlcPlayer);

    this->engine = engine;

    player = NULL;

    backend = NULL;

    repeat = false;

    networkCache = VLCPLAYER_NETWORK_CACHE;

    if (thread) q->moveToThread(thread);

    QCoreApplication::postEvent(q, new QEvent(static_cast<QEvent::Type>
                                              (WVlcPlayerPrivate::EventCreate)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QString WVlcPlayerPrivate::encodeUrl(const QUrl & url) const
{
    QString result = url.toString();

    return result.replace(" ", "%20");
}

//-------------------------------------------------------------------------------------------------
// Private static functions
//-------------------------------------------------------------------------------------------------

/* static */ void WVlcPlayerPrivate::eventMedia(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    unsigned int width  = 0;
    unsigned int height = 0;

    libvlc_video_get_size(d->player, 0, &width, &height);

    QCoreApplication::postEvent(d->backend, new WVlcMediaEvent(width, height));
}

/* static */ void WVlcPlayerPrivate::eventPlaying(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend,
                                new QEvent(static_cast<QEvent::Type> (WVlcPlayer::EventPlaying)));
}

/* static */ void WVlcPlayerPrivate::eventPaused(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend,
                                new QEvent(static_cast<QEvent::Type> (WVlcPlayer::EventPaused)));
}

/* static */ void WVlcPlayerPrivate::eventStopped(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend,
                                new QEvent(static_cast<QEvent::Type> (WVlcPlayer::EventStopped)));
}

//-------------------------------------------------------------------------------------------------

/* static */ void WVlcPlayerPrivate::eventBuffering(const struct libvlc_event_t *, void * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend, new QEvent(static_cast<QEvent::Type>
                                                       (WVlcPlayer::EventBuffering)));
}

//-------------------------------------------------------------------------------------------------

/* static */ void WVlcPlayerPrivate::eventLengthChanged(const struct libvlc_event_t * event,
                                                        void                        * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    libvlc_time_t length = event->u.media_player_length_changed.new_length;

    QCoreApplication::postEvent(d->backend,
                                new WVlcPlayerEvent(WVlcPlayer::EventLengthChanged, (qint64) length));
}

/* static */ void WVlcPlayerPrivate::eventTimeChanged(const struct libvlc_event_t * event,
                                                      void                        * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    int time = event->u.media_player_time_changed.new_time;

    QCoreApplication::postEvent(d->backend,
                                new WVlcPlayerEvent(WVlcPlayer::EventTimeChanged, time));
}

//-------------------------------------------------------------------------------------------------

/* static */ void WVlcPlayerPrivate::eventEndReached(const struct libvlc_event_t *,
                                                     void                        * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend, new QEvent(static_cast<QEvent::Type>
                                                       (WVlcPlayer::EventEndReached)));
}

//-------------------------------------------------------------------------------------------------

/* static */ void WVlcPlayerPrivate::eventEncounteredError(const struct libvlc_event_t *,
                                                           void                        * data)
{
    WVlcPlayerPrivate * d = static_cast<WVlcPlayerPrivate *> (data);

    if (d->backend == NULL) return;

    QCoreApplication::postEvent(d->backend,
                                new QEvent(static_cast<QEvent::Type> (WVlcPlayer::EventError)));
}

//=================================================================================================
// WVlcPlayerEventBackend
//=================================================================================================

class WVlcPlayerEventBackend : public QEvent
{
public:
    WVlcPlayerEventBackend(QObject * backend,
                           libvlc_video_format_cb  setup,
                           libvlc_video_cleanup_cb cleanup,
                           libvlc_video_lock_cb    lock,
                           libvlc_video_unlock_cb  unlock,
                           libvlc_video_display_cb display)
        : QEvent(static_cast<QEvent::Type> (WVlcPlayerPrivate::EventBackend))
    {
        this->backend = backend;

        this->setup   = setup;
        this->cleanup = cleanup;

        this->lock    = lock;
        this->unlock  = unlock;
        this->display = display;
    }

public: // Variables
    QObject * backend;

    libvlc_video_format_cb  setup;
    libvlc_video_cleanup_cb cleanup;

    libvlc_video_lock_cb    lock;
    libvlc_video_unlock_cb  unlock;
    libvlc_video_display_cb display;
};

//=================================================================================================
// WVlcPlayerEventSource
//=================================================================================================

class WVlcPlayerEventSource : public QEvent
{
public:
    WVlcPlayerEventSource(const QUrl & media, const QUrl & audio)
        : QEvent(static_cast<QEvent::Type> (WVlcPlayerPrivate::EventSetSource))
    {
        this->media = media;
        this->audio = audio;
    }

public: // Variables
    QUrl media;
    QUrl audio;
};

//=================================================================================================
// WVlcPlayerPrivateEvent
//=================================================================================================

class WVlcPlayerPrivateEvent : public QEvent
{
public:
    WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventType type, const QVariant & value)
        : QEvent(static_cast<QEvent::Type> (type))
    {
        this->value = value;
    }

public: // Variables
    QVariant value;
};

//=================================================================================================
// WVlcPlayer
//=================================================================================================

WVlcPlayer::WVlcPlayer(WVlcEngine * engine, QThread * thread, QObject * parent)
    : QObject(parent), WPrivatable(new WVlcPlayerPrivate(this))
{
    Q_D(WVlcPlayer); d->init(engine, thread);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::setBackend(QObject * backend, libvlc_video_format_cb  setup,
                                                                 libvlc_video_cleanup_cb cleanup,
                                                                 libvlc_video_lock_cb    lock,
                                                                 libvlc_video_unlock_cb  unlock,
                                                                 libvlc_video_display_cb display)
{
    QCoreApplication::postEvent(this, new WVlcPlayerEventBackend(backend, setup, cleanup, lock,
                                                                 unlock, display));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::setSource(const QUrl & media, const QUrl & audio)
{
    QCoreApplication::postEvent(this, new WVlcPlayerEventSource(media, audio));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::play(int at)
{
    QCoreApplication::postEvent(this,
                                new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventPlay, at));
}

/* Q_INVOKABLE */ void WVlcPlayer::pause()
{
    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WVlcPlayerPrivate::EventPause)));
}

/* Q_INVOKABLE */ void WVlcPlayer::stop()
{
    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WVlcPlayerPrivate::EventStop)));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::seekTo(int msec)
{
    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventSeekTo,
                                                                 msec));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::setSpeed(qreal speed)
{
    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventSetSpeed,
                                                                 speed));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::setVolume(int percent)
{
    QCoreApplication::postEvent(this, new WVlcPlayerPrivateEvent(WVlcPlayerPrivate::EventSetVolume,
                                                                 percent));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::setProxy(const QString & host,
                                            int             port, const QString & password)
{
    Q_D(WVlcPlayer);

    d->mutex.lock();

    d->proxyHost     = host + ':' + QString::number(port);
    d->proxyPassword = password;

    d->mutex.unlock();
}

/* Q_INVOKABLE */ void WVlcPlayer::clearProxy()
{
    Q_D(WVlcPlayer);

    d->mutex.lock();

    d->proxyHost     = QString();
    d->proxyPassword = QString();

    d->mutex.unlock();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WVlcPlayer::deletePlayer()
{
    QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>
                                                 (WVlcPlayerPrivate::EventDelete)),
                                Qt::HighEventPriority * 100);
}

//-------------------------------------------------------------------------------------------------
// Events
//-------------------------------------------------------------------------------------------------

bool WVlcPlayer::event(QEvent * event)
{
    Q_D(WVlcPlayer);

    QEvent::Type type = event->type();

    if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventCreate))
    {
        d->player = libvlc_media_player_new(d->engine->instance());

        // FIXME: Applying the player default volume.
        libvlc_audio_set_volume(d->player, 100);

        libvlc_event_attach(libvlc_media_player_event_manager(d->player),
                            libvlc_MediaPlayerPlaying, d->eventPlaying, d);

        libvlc_event_attach(libvlc_media_player_event_manager(d->player),
                            libvlc_MediaPlayerPaused, d->eventPaused, d);

        libvlc_event_attach(libvlc_media_player_event_manager(d->player),
                            libvlc_MediaPlayerStopped, d->eventStopped, d);

        libvlc_event_attach(libvlc_media_player_event_manager(d->player),
                            libvlc_MediaPlayerBuffering, d->eventBuffering, d);

        libvlc_event_attach(libvlc_media_player_event_manager(d->player),
                            libvlc_MediaPlayerLengthChanged, d->eventLengthChanged, d);

        libvlc_event_attach(libvlc_media_player_event_manager(d->player),
                            libvlc_MediaPlayerTimeChanged, d->eventTimeChanged, d);

        libvlc_event_attach(libvlc_media_player_event_manager(d->player),
                            libvlc_MediaPlayerEndReached, d->eventEndReached, d);

        libvlc_event_attach(libvlc_media_player_event_manager(d->player),
                            libvlc_MediaPlayerEncounteredError, d->eventEncounteredError, d);

        return true;
    }
    else if (d->player == NULL)
    {
        return QObject::event(event);
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventBackend))
    {
        WVlcPlayerEventBackend * eventBackend = static_cast<WVlcPlayerEventBackend *> (event);

        if (d->backend)
        {
            qWarning("WVlcPlayer::event: Player already has a backend.");

            return true;
        }

        Q_ASSERT(eventBackend->backend);

        d->backend = eventBackend->backend;

        libvlc_video_set_format_callbacks(d->player, eventBackend->setup, eventBackend->cleanup);

        libvlc_video_set_callbacks(d->player, eventBackend->lock, eventBackend->unlock,
                                   eventBackend->display, eventBackend->backend);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventSetSource))
    {
        WVlcPlayerEventSource * eventSource = static_cast<WVlcPlayerEventSource *> (event);

        libvlc_media_t * media = libvlc_media_new_location(d->engine->instance(),
                                                           d->encodeUrl(eventSource->media).C_STR);

        bool repeat;

        QString proxy;
        QString proxyPassword;

        d->mutex.lock();

        repeat = d->repeat;

        QString cache("network-caching=" + QString::number(d->networkCache));

        if (d->proxyHost.isEmpty() == false)
        {
            proxy         = "http-proxy="     + d->proxyHost;
            proxyPassword = "http-proxy-pwd=" + d->proxyPassword;
        }

        d->mutex.unlock();

        const QUrl & audio = eventSource->audio;

        if (audio.isEmpty() == false)
        {
            QString input = "input-slave=" + d->encodeUrl(audio);

            libvlc_media_add_option(media, input.C_STR);
        }

        if (repeat)
        {
            libvlc_media_add_option(media, "input-repeat=-1");
        }

        libvlc_media_add_option(media, cache.C_STR);

        if (proxy.isNull() == false)
        {
            libvlc_media_add_option(media, proxy        .C_STR);
            libvlc_media_add_option(media, proxyPassword.C_STR);
        }

        foreach (const QString & option, d->options)
        {
            libvlc_media_add_option(media, option.C_STR);
        }

        libvlc_event_attach(libvlc_media_event_manager(media),
                            libvlc_MediaParsedChanged, d->eventMedia, d);

        libvlc_media_player_set_media(d->player, media);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventPlay))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        int at = eventPlayer->value.toInt();

        libvlc_media_player_play(d->player);

        if (at) libvlc_media_player_set_time(d->player, at);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventPause))
    {
        libvlc_media_player_set_pause(d->player, true);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventStop))
    {
        libvlc_media_player_stop(d->player);

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventSeekTo))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        libvlc_media_player_set_time(d->player, eventPlayer->value.toInt());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventSetSpeed))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        libvlc_media_player_set_rate(d->player, eventPlayer->value.toFloat());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventSetVolume))
    {
        WVlcPlayerPrivateEvent * eventPlayer = static_cast<WVlcPlayerPrivateEvent *> (event);

        libvlc_audio_set_volume(d->player, eventPlayer->value.toInt());

        return true;
    }
    else if (type == static_cast<QEvent::Type> (WVlcPlayerPrivate::EventDelete))
    {
        libvlc_media_player_stop(d->player);

        libvlc_media_player_release(d->player);

        d->player = NULL;

        if (d->backend)
        {
            QCoreApplication::postEvent(d->backend, new QEvent(static_cast<QEvent::Type>
                                                               (WVlcPlayer::EventDelete)),
                                        Qt::HighEventPriority * 100);
        }

        return true;
    }
    else return QObject::event(event);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QStringList WVlcPlayer::options()
{
    Q_D(WVlcPlayer);

    const QMutexLocker locker(&d->mutex);

    return d->options;
}

void WVlcPlayer::setOptions(const QStringList & options)
{
    Q_D(WVlcPlayer);

    QMutexLocker locker(&d->mutex);

    if (d->options == options) return;

    d->options = options;

    locker.unlock();

    emit optionsChanged();
}

//-------------------------------------------------------------------------------------------------

bool WVlcPlayer::repeat()
{
    Q_D(WVlcPlayer);

    const QMutexLocker locker(&d->mutex);

    return d->repeat;
}

void WVlcPlayer::setRepeat(bool repeat)
{
    Q_D(WVlcPlayer);

    QMutexLocker locker(&d->mutex);

    if (d->repeat == repeat) return;

    d->repeat = repeat;

    locker.unlock();

    emit repeatChanged();
}

//-------------------------------------------------------------------------------------------------

int WVlcPlayer::networkCache()
{
    Q_D(WVlcPlayer);

    QMutexLocker locker(&d->mutex);

    return d->networkCache;
}

void WVlcPlayer::setNetworkCache(int msec)
{
    Q_D(WVlcPlayer);

    QMutexLocker locker(&d->mutex);

    if (d->networkCache == msec) return;

    d->networkCache = msec;

    locker.unlock();

    emit networkCacheChanged();
}

#endif // SK_NO_VLCPLAYER
