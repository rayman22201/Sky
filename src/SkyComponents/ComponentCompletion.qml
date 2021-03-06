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

MouseArea
{
    //---------------------------------------------------------------------------------------------
    // Settings
    //---------------------------------------------------------------------------------------------

    width: parent.width

    height: st.componentCompletion_height

    hoverEnabled: true

    cursor: MouseArea.PointingHandCursor

    //---------------------------------------------------------------------------------------------
    // Event
    //---------------------------------------------------------------------------------------------

    onClicked:
    {
        currentIndex = index;

        itemClicked(index);
    }

    onDoubleClicked: itemDoubleClicked(index)

    //---------------------------------------------------------------------------------------------
    // Childs
    //---------------------------------------------------------------------------------------------

    Rectangle
    {
        id: background

        anchors.left  : parent.left
        anchors.right : parent.right
        anchors.top   : parent.top
        anchors.bottom: border.top

        gradient: Gradient
        {
            GradientStop
            {
                position: 0.0

                color:
                {
                    if      (index == currentIndex) return st.itemList_colorSelectFocusA;
                    else if (pressed)               return st.itemList_colorPressA;
                    else if (containsMouse)         return st.itemList_colorHoverA;
                    else                            return st.itemList_colorA;
                }
            }

            GradientStop
            {
                position: 1.0

                color:
                {
                    if      (index == currentIndex) return st.itemList_colorSelectFocusB;
                    else if (pressed)               return st.itemList_colorPressB;
                    else if (containsMouse)         return st.itemList_colorHoverB;
                    else                            return st.itemList_colorB;
                }
            }
        }

        TextBase
        {
            anchors.fill: parent

            leftMargin : st.dp8
            rightMargin: st.dp8

            verticalAlignment: Text.AlignVCenter

            text: title

            style: Text.Raised

            elide: Text.ElideLeft
        }
    }

    BorderHorizontal
    {
        id: border

        anchors.bottom: parent.bottom

        color: st.itemList_colorBorder
    }
}
