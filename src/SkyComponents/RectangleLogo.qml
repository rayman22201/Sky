//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkyComponents module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

import QtQuick 1.1
import Sky     1.0

Rectangle
{
    //---------------------------------------------------------------------------------------------
    // Properties style
    //---------------------------------------------------------------------------------------------

    property real logoRatio: st.logoRatio

    property color colorA: st.logo_colorA
    property color colorB: st.logo_colorB

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias source: image.source

    //---------------------------------------------------------------------------------------------

    property alias image: image

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    gradient: Gradient
    {
        GradientStop { position: 0.0; color: colorA }
        GradientStop { position: 1.0; color: colorB }
    }

    ImageScale
    {
        id: image

        anchors.fill: parent

        anchors.leftMargin : Math.round(parent.width / logoRatio)
        anchors.rightMargin: anchors.leftMargin

        source: st.logo

        fillMode: Image.PreserveAspectFit
    }
}
