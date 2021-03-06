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

#ifndef WDECLARATIVEANIMATED_H
#define WDECLARATIVEANIMATED_H

// Sk includes
#include <WDeclarativeItem>

#ifndef SK_NO_DECLARATIVEANIMATED

class WDeclarativeAnimatedPrivate;

class SK_GUI_EXPORT WDeclarativeAnimated : public WDeclarativeItem
{
    Q_OBJECT

    Q_ENUMS(StepDirection)
    Q_ENUMS(StepMode)

    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)

    Q_PROPERTY(StepDirection stepDirection READ stepDirection WRITE setStepDirection
               NOTIFY stepDirectionChanged)

    Q_PROPERTY(bool isBackward READ isBackward NOTIFY stepDirectionChanged)
    Q_PROPERTY(bool isForward  READ isForward  NOTIFY stepDirectionChanged)

    Q_PROPERTY(StepMode stepMode READ stepMode WRITE setStepMode NOTIFY stepModeChanged)

    Q_PROPERTY(int steps READ stepCount WRITE setStepCount NOTIFY stepCountChanged)
    Q_PROPERTY(int step  READ step      WRITE setStep      NOTIFY stepChanged)

    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)

    Q_PROPERTY(int loops READ loopCount WRITE setLoopCount NOTIFY loopCountChanged)
    Q_PROPERTY(int loop  READ loop                         NOTIFY loopChanged)

public: // Enums
    enum StepDirection { StepBackward, StepForward };

    enum StepMode { StepAuto, StepManual };

public:
    explicit WDeclarativeAnimated(QDeclarativeItem * parent = NULL);
protected:
    WDeclarativeAnimated(WDeclarativeAnimatedPrivate * p, QDeclarativeItem * parent = NULL);

public: // Interface
    Q_INVOKABLE void start  ();
    Q_INVOKABLE void restart();

    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop ();

    Q_INVOKABLE void stepBackward();
    Q_INVOKABLE void stepForward ();

public: // QDeclarativeItem reimplementation
    /* virtual */ void componentComplete();

protected: // QGraphicsItem reimplementation
    /* virtual */ QVariant itemChange(GraphicsItemChange change, const QVariant & value);

signals:
    void runningChanged();

    void stepDirectionChanged();
    void stepModeChanged     ();

    void stepCountChanged();
    void stepChanged     ();

    void intervalChanged();

    void loopCountChanged();
    void loopChanged     ();

public: // Properties
    bool running() const;
    void setRunning(bool running);

    StepDirection stepDirection() const;
    void          setStepDirection(StepDirection direction);

    bool isBackward() const;
    bool isForward () const;

    StepMode stepMode() const;
    void     setStepMode(StepMode mode);

    int  stepCount() const;
    void setStepCount(int count);

    int  step() const;
    void setStep(int step);

    int  interval() const;
    void setInterval(int interval);

    int  loopCount() const;
    void setLoopCount(int count);

    int loop() const;

private:
    W_DECLARE_PRIVATE(WDeclarativeAnimated)

    Q_PRIVATE_SLOT(d_func(), void onFinished())
};

QML_DECLARE_TYPE(WDeclarativeAnimated)

#include <private/WDeclarativeAnimated_p>

#endif // SK_NO_DECLARATIVEANIMATED
#endif // WDECLARATIVEANIMATED_H
