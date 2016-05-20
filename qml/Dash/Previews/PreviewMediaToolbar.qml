/*
 * Copyright (C) 2015 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import "../../Components"

Rectangle {
    property alias shareData: sharingWidget.shareData

    implicitHeight: units.gu(6)
    color: Qt.rgba(0, 0, 0, 0.8)
    visible: sharingWidget.active

    AbstractButton {
        id: button
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
            leftMargin: units.gu(1)
        }
        height: units.gu(4)
        width: units.gu(4)
        onClicked: sharingWidget.showPeerPicker()

        Icon {
            anchors.centerIn: parent
            height: units.gu(3)
            width: units.gu(3)
            source: "image://theme/share"
        }
    }

    SharingPicker {
        id: sharingWidget
        objectName: "sharingWidget"
    }
}
