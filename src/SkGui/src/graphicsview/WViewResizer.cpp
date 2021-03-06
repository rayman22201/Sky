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

#include "WViewResizer.h"

#ifndef SK_NO_VIEWRESIZER

// Sk includes
#include <WControllerView>
#include <WResizer>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WDeclarativeItem_p>

class SK_GUI_EXPORT WViewResizerPrivate : public WDeclarativeItemPrivate
{
protected:
    WViewResizerPrivate(WViewResizer * p);

    void init();

public: // Functions
    void refreshCoordinates();

public: // Variables
    WResizer * topLeft;
    WResizer * topRight;
    WResizer * bottomLeft;
    WResizer * bottomRight;
    WResizer * left;
    WResizer * right;
    WResizer * top;
    WResizer * bottom;

    int size;

protected:
    W_DECLARE_PUBLIC(WViewResizer)
};

//-------------------------------------------------------------------------------------------------

WViewResizerPrivate::WViewResizerPrivate(WViewResizer * p) : WDeclarativeItemPrivate(p) {}

void WViewResizerPrivate::init()
{
    Q_Q(WViewResizer);

    topLeft     = new WResizer(WResizer::TopLeft,     q);
    topRight    = new WResizer(WResizer::TopRight,    q);
    bottomLeft  = new WResizer(WResizer::BottomLeft,  q);
    bottomRight = new WResizer(WResizer::BottomRight, q);
    left        = new WResizer(WResizer::Left,        q);
    right       = new WResizer(WResizer::Right,       q);
    top         = new WResizer(WResizer::Top,         q);
    bottom      = new WResizer(WResizer::Bottom,      q);

    size = 4;

    refreshCoordinates();
}

//-------------------------------------------------------------------------------------------------

void WViewResizerPrivate::refreshCoordinates()
{
    Q_Q(WViewResizer);

    int width  = q->width ();
    int height = q->height();

    //---------------------------------------------------------------------------------------------
    // Position

    int size2x = size * 2;

    int sizeX = width  - size2x;
    int sizeY = height - size2x;

    topLeft    ->setPos(QPoint(0,     0));
    topRight   ->setPos(QPoint(sizeX, 0));
    bottomLeft ->setPos(QPoint(0,     sizeY));
    bottomRight->setPos(QPoint(sizeX, sizeY));

    sizeX = width  - size;
    sizeY = height - size;

    left  ->setPos(QPoint(0,      size2x));
    right ->setPos(QPoint(sizeX,  size2x));
    top   ->setPos(QPoint(size2x, 0));
    bottom->setPos(QPoint(size2x, sizeY));

    //---------------------------------------------------------------------------------------------
    // Size

    topLeft    ->setSize(QSize(size2x, size2x));
    topRight   ->setSize(QSize(size2x, size2x));
    bottomLeft ->setSize(QSize(size2x, size2x));
    bottomRight->setSize(QSize(size2x, size2x));

    int size4x = size * 4;

    sizeX = width  - size4x;
    sizeY = height - size4x;

    left  ->setSize(QSize(size,  sizeY));
    right ->setSize(QSize(size,  sizeY));
    top   ->setSize(QSize(sizeX, size));
    bottom->setSize(QSize(sizeX, size));
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WViewResizer::WViewResizer(QDeclarativeItem * parent)
    : WDeclarativeItem(new WViewResizerPrivate(this), parent)
{
    Q_D(WViewResizer); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected QDeclarativeItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WViewResizer::geometryChanged(const QRectF & newGeometry,
                                                     const QRectF & oldGeometry)
{
    Q_D(WViewResizer);

    WDeclarativeItem::geometryChanged(newGeometry, oldGeometry);

    d->refreshCoordinates();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

int WViewResizer::size() const
{
    Q_D(const WViewResizer); return d->size;
}

void WViewResizer::setSize(int size)
{
    Q_D(WViewResizer);

    if (d->size == size) return;

    d->size = size;

    d->refreshCoordinates();

    emit sizeChanged();
}

#endif // SK_NO_VIEWRESIZER
