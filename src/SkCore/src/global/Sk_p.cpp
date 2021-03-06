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

#include "Sk_p.h"

//=================================================================================================
// WPrivate
//=================================================================================================
// Protected

WPrivate::WPrivate(WPrivatable * p)
{
    sk_q = p;
}

//-------------------------------------------------------------------------------------------------
// Public

/* virtual */ WPrivate::~WPrivate() {}

//=================================================================================================
// WPrivatable
//=================================================================================================
// Protected

WPrivatable::WPrivatable(WPrivate * p)
{
    sk_d = p;
}

/* virtual */ WPrivatable::~WPrivatable()
{
    delete sk_d;
}
