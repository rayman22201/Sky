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

BaseButtonPiano
{
    id: buttonPianoFull

    //---------------------------------------------------------------------------------------------
    // Properties
    //---------------------------------------------------------------------------------------------

    property int padding: st.buttonPiano_padding

    property int paddingLeft : padding
    property int paddingRight: padding

    property int spacing: st.buttonPiano_spacing

    property int minimumWidth: -1
    property int maximumWidth: -1

    //---------------------------------------------------------------------------------------------
    // Private

    property int pIconWidth: Math.max(itemIcon.width, background.height)

    //---------------------------------------------------------------------------------------------
    // Aliases
    //---------------------------------------------------------------------------------------------

    property alias isIconDefault: itemIcon.isSourceDefault

    property alias icon       : itemIcon.source
    property alias iconDefault: itemIcon.sourceDefault

    property alias iconWidth : itemIcon.width
    property alias iconHeight: itemIcon.height

    property alias iconSourceSize: itemIcon.sourceSize
    property alias iconSourceArea: itemIcon.sourceArea

    property alias iconLoadMode: itemIcon.loadMode
    property alias iconFillMode: itemIcon.fillMode

    property alias iconAsynchronous: itemIcon.asynchronous
    property alias iconCache       : itemIcon.cache

    property alias iconScaling: itemIcon.scaling

    property alias iconStyle: itemIcon.style

    property alias text: itemText.text
    property alias font: itemText.font

    //---------------------------------------------------------------------------------------------

    property alias itemIcon: itemIcon
    property alias itemText: itemText

    //---------------------------------------------------------------------------------------------
    // Style

    property alias enableFilter: itemIcon.enableFilter

    property alias filterIcon      : itemIcon.filterDefault
    property alias filterIconShadow: itemIcon.filterShadow
    property alias filterIconSunken: itemIcon.filterSunken

    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: getPreferredWidth()

    height: st.buttonPiano_height + borderSizeHeight

    //---------------------------------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------------------------------

    function getPreferredWidth()
    {
        var size;

        if (iconWidth)
        {
            if (text)
            {
                size = Math.max(iconWidth, height - borderSizeHeight) + sk.textWidth(font, text)
                       +
                       borderSizeWidth + paddingRight + spacing;
            }
            else size = Math.max(iconWidth, height - borderSizeHeight) + borderSizeWidth;
        }
        else if (text)
        {
            size = sk.textWidth(font, text) + borderSizeWidth + paddingLeft + paddingRight;
        }

        if (minimumWidth != -1)
        {
            size = Math.max(minimumWidth, size);
        }

        if (maximumWidth != -1)
        {
            size = Math.min(size, maximumWidth);
        }

        return size;
    }

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Icon
    {
        id: itemIcon

        anchors.left: parent.left

        anchors.leftMargin: Math.round((pIconWidth - width) / 2)

        anchors.verticalCenter: parent.verticalCenter

        sourceSize.height: (scaling) ? -1 : parent.height

        opacity: (buttonPianoFull.enabled) ? 1.0 : st.icon_opacityDisable

        style: (checked) ? Sk.IconRaised
                         : Sk.IconSunken

        filterDefault: st.buttonPiano_filterIcon
        filterShadow : st.buttonPiano_filterIconShadow
        filterSunken : st.buttonPiano_filterIconSunken
    }

    TextBase
    {
        id: itemText

        anchors.fill: parent

        leftMargin: (iconWidth) ? pIconWidth + spacing
                                : paddingLeft

        rightMargin: paddingRight

        verticalAlignment: Text.AlignVCenter

        opacity: (buttonPianoFull.enabled) ? 1.0 : st.text_opacityDisable

        style: (checked) ? Text.Raised
                         : Text.Sunken

        Behavior on opacity
        {
            PropertyAnimation { duration: st.duration_fast }
        }
    }
}
