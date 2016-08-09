//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WABSTRACTBACKEND_H
#define WABSTRACTBACKEND_H

// Qt includes
#include <QObject>
#include <QUrl>
#include <QImage>

// Sk includes
#include <Sk>

#ifndef SK_NO_ABSTRACTBACKEND

// Forward declarations
class WAbstractBackendPrivate;
class QGraphicsItem;
class QPainter;
class QStyleOptionGraphicsItem;

class SK_GUI_EXPORT WAbstractBackend : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(State)
    Q_ENUMS(StateLoad)
    Q_ENUMS(Quality)
    Q_ENUMS(FillMode)

    Q_PROPERTY(QGraphicsItem * parentItem READ parentItem WRITE setParentItem
               NOTIFY parentItemChanged)

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(State     state     READ state     NOTIFY stateChanged)
    Q_PROPERTY(StateLoad stateLoad READ stateLoad NOTIFY stateLoadChanged)

    Q_PROPERTY(bool hasStarted READ hasStarted NOTIFY startedChanged)
    Q_PROPERTY(bool hasEnded   READ hasEnded   NOTIFY endedChanged)

    Q_PROPERTY(bool isLoading READ isLoading NOTIFY stateLoadChanged)

    Q_PROPERTY(bool isStarting  READ isStarting  NOTIFY stateLoadChanged)
    Q_PROPERTY(bool isResuming  READ isResuming  NOTIFY stateLoadChanged)
    Q_PROPERTY(bool isBuffering READ isBuffering NOTIFY stateLoadChanged)

    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY stateChanged)
    Q_PROPERTY(bool isPaused  READ isPaused  NOTIFY stateChanged)
    Q_PROPERTY(bool isStopped READ isStopped NOTIFY stateChanged)

    Q_PROPERTY(int currentTime READ currentTime NOTIFY currentTimeChanged)
    Q_PROPERTY(int duration    READ duration    NOTIFY durationChanged)

    Q_PROPERTY(qreal speed READ speed WRITE setSpeed NOTIFY speedChanged)

    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)

    Q_PROPERTY(bool repeat READ repeat WRITE setRepeat NOTIFY repeatChanged)

    Q_PROPERTY(Quality quality READ quality WRITE setQuality NOTIFY qualityChanged)

    Q_PROPERTY(Quality qualityActive READ qualityActive NOTIFY qualityActiveChanged)

    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)

public:
    enum State
    {
        StateStopped,
        StatePlaying,
        StatePaused
    };

    enum StateLoad
    {
        StateLoadDefault,
        StateLoadStarting,
        StateLoadResuming,
        StateLoadBuffering
    };

    enum Quality
    {
        QualityInvalid,
        QualityMinimum,
        QualityLow,
        QualityMedium,
        QualityHigh,
        QualityUltra,
        QualityMaximum
    };

    enum FillMode
    {
        Stretch,
        PreserveAspectFit,
        PreserveAspectCrop
    };

public:
    WAbstractBackend();
protected:
    WAbstractBackend(WAbstractBackendPrivate * p);

    /* virtual */ ~WAbstractBackend();

public: // Interface
    Q_INVOKABLE void loadSource(const QUrl & url, int duration = -1, int currentTime = -1);

    Q_INVOKABLE void play  ();
    Q_INVOKABLE void replay();

    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop ();
    Q_INVOKABLE void clear();

    Q_INVOKABLE void seekTo(int msec);

    Q_INVOKABLE const QSizeF & getSize() const;
    Q_INVOKABLE void           setSize(const QSizeF & size);

    Q_INVOKABLE void drawFrame(QPainter * painter, const QStyleOptionGraphicsItem * option);

    Q_INVOKABLE void   updateFrame();
    Q_INVOKABLE QImage getFrame   () const;

    Q_INVOKABLE bool deleteBackend();

protected: // Functions
    void setState    (State     state);
    void setStateLoad(StateLoad stateLoad);

    void setEnded(bool ended);

    void setCurrentTime(int msec);
    void setDuration   (int msec);

    void setQualityActive(Quality quality);

    void deleteNow();

protected: // Abstract functions
    virtual bool backendSetSource(const QUrl & url) = 0;

    virtual bool backendPlay  () = 0;
    virtual bool backendReplay() = 0;

    virtual bool backendPause() = 0;
    virtual bool backendStop () = 0;

    virtual void backendSetVolume(int percent) = 0;

    virtual bool backendDelete() = 0;

protected: // Virtual functions
    virtual void backendSeekTo(int msec); /* {} */

    virtual void backendSetSpeed(qreal speed); /* {} */

    virtual void backendSetRepeat(bool repeat); /* {} */

    virtual void backendSetQuality(Quality quality); /* {} */

    virtual void backendSetFillMode(FillMode fillMode); /* {} */

    virtual void backendSetSize(const QSizeF & size); /* {} */

    virtual void backendDrawFrame(QPainter                       * painter,
                                  const QStyleOptionGraphicsItem * option); /* {} */

    virtual void   backendUpdateFrame();       /* {} */
    virtual QImage backendGetFrame   () const; /* {} */

signals:
    void ended();

    void parentItemChanged();

    void sourceChanged();

    void stateChanged    ();
    void stateLoadChanged();

    void startedChanged();

    void endedChanged();

    void playingChanged();
    void pausedChanged ();

    void currentTimeChanged();
    void durationChanged   ();

    void speedChanged();

    void volumeChanged();

    void repeatChanged();

    void qualityChanged      ();
    void qualityActiveChanged();

    void fillModeChanged();

public: // Properties
    QGraphicsItem * parentItem() const;
    void            setParentItem(QGraphicsItem * parent);

    QUrl source() const;
    void setSource(const QUrl & url);

    State     state    () const;
    StateLoad stateLoad() const;

    bool hasStarted() const;
    bool hasEnded  () const;

    bool isLoading() const;

    bool isStarting () const;
    bool isResuming () const;
    bool isBuffering() const;

    bool isPlaying() const;
    bool isPaused () const;
    bool isStopped() const;

    int currentTime() const;
    int duration   () const;

    qreal speed() const;
    void  setSpeed(qreal speed);

    int  volume() const;
    void setVolume(int percent);

    bool repeat() const;
    void setRepeat(bool repeat);

    Quality quality() const;
    void    setQuality(Quality quality);

    Quality qualityActive() const;

    FillMode fillMode() const;
    void     setFillMode(FillMode fillMode);

private:
    W_DECLARE_PRIVATE(WAbstractBackend)

    friend class WAbstractHook;
    friend class WAbstractHookPrivate;
};

#endif // SK_NO_ABSTRACTBACKEND
#endif // WABSTRACTBACKEND_H
