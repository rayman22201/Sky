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

ImageScale
{
    id: itemIcon

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int style: Sk.IconSunken

    property int styleSize: st.icon_styleSize

    //---------------------------------------------------------------------------------------------
    // Style

    property bool enableFilter: true

    property ImageColorFilter filterDefault: st.icon_filter
    property ImageColorFilter filterShadow : st.icon_filterShadow
    property ImageColorFilter filterSunken : st.icon_filterSunken

    //---------------------------------------------------------------------------------------------
    // Private

    property bool pStyleVisible: (enableFilter && style != Sk.IconNormal)

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias imageStyle: imageStyle

    //---------------------------------------------------------------------------------------------
    // Style

    property alias filter: itemIcon.filter

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    filter: (enableFilter) ? filterDefault : null

    scaling: false

    //---------------------------------------------------------------------------------------------
    // Animations
    //---------------------------------------------------------------------------------------------

    Behavior on opacity
    {
        PropertyAnimation { duration: st.duration_fast }
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    ImageScale
    {
        id: imageStyle

        anchors.fill: parent

        anchors.topMargin   : (style == Sk.IconRaised) ?  styleSize : -styleSize
        anchors.bottomMargin: (style == Sk.IconRaised) ? -styleSize :  styleSize

        sourceSize: (itemIcon.isExplicitSize) ? itemIcon.sourceSize : undefined

        sourceArea: itemIcon.sourceArea

        z: -1

        visible: pStyleVisible

        source       : (pStyleVisible) ? itemIcon.source        : ""
        sourceDefault: (pStyleVisible) ? itemIcon.sourceDefault : ""

        loadMode: itemIcon.loadMode
        fillMode: itemIcon.fillMode

        asynchronous: itemIcon.asynchronous
        cache       : itemIcon.cache

        scaling: itemIcon.scaling

        filter: (style == Sk.IconSunken) ? filterSunken
                                         : filterShadow
    }
}
