//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WTHREADACTIONS_H
#define WTHREADACTIONS_H

// Qt includes
#include <QThread>

// Sk includes
#include <Sk>

#ifndef SK_NO_THREADACTIONS

// Forward declarations
class WThreadActionsPrivate;
class WAbstractThreadAction;
class WAbstractThreadReply;

class SK_CORE_EXPORT WThreadActions : public QObject, public WPrivatable
{
    Q_OBJECT

public:
    explicit WThreadActions(QObject * parent = NULL);

public: // Interface
    WAbstractThreadReply * pushAction(WAbstractThreadAction * action);

private:
    W_DECLARE_PRIVATE(WThreadActions)

    friend class WAbstractThreadAction;
    friend class WAbstractThreadActionPrivate;
};

#endif // SK_NO_THREADACTIONS
#endif // WTHREADACTIONS_H
