import QtQuick 2.12
import QtQml.Models 2.1
import QtQuick.Window 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.15

import "../utils.js" as Utils

Window {
    id: exportWindow
    title: "Export"
    visible: true
    width: 700
    minimumHeight: 410
    flags: if (Qt.platform.os == "linux") { return Qt.SubWindow } else { return Qt.Dialog }
    Material.theme: Material.Dark
    Material.accent: Material.DeepPurple
    Pane {
        width: parent.width
        height: parent.height

        ListView {
            id: exportOptionsView
            anchors { fill: parent; margins: 10 }
            Layout.minimumHeight: 600
            Layout.minimumWidth: 600
            model: DelegateModel {
                id: exportOptionsVisual
                model: exportOptionsModel
                delegate: exportOptionsDelegate
            }
        }
        Component {
            id: exportOptionsDelegate
            GridLayout {
                columns: 3
                rows: 7

                Label {
                    Layout.leftMargin: 54
                    text: model.exportDirectoryName
                }
                Rectangle {
                    border {
                        color: "#8066b0"
                        width: 1
                    }
                    Layout.fillWidth: parent
                    height: 25
                    width: 300
                    Layout.leftMargin: 20
                    Layout.rightMargin: 20
                    color: "transparent"
                    TextInput {
                        id: exportDirectoryLabel
                        anchors.fill: parent
                        leftPadding: 5
                        topPadding: 2
                        bottomPadding: 2
                        text: model.exportDirectoryValue
                        clip: true
                        font.pointSize: 12
                        color: "#FFFFFF"
                        onEditingFinished: {
                            model.exportDirectoryValue = text;
                        }
                    }
                }
                Button {
                    text: "Browse"
                    action: Action {
                        onTriggered: browseDirDialog.open()
                    }
                }
                FileDialog {
                    id: browseDirDialog
                    title: "Please choose a directory"
                    visible: false
                    folder: shortcuts.movies
                    selectFolder: true
                    onAccepted: {
                        var qtFilePath = browseDirDialog.fileUrl.toString();
                        var realFilePath = Utils.urlToPath(browseDirDialog.fileUrl.toString())
                        model.exportDirectoryValue = realFilePath;
                    }
                }

                Label {
                    id: exportAsImageSequenceSwitch
                    text: model.imagesequencePrefixName
                }
                Rectangle {
                    border {
                        color: "#8066b0"
                        width: 1
                    }
                    Layout.fillWidth: parent
                    height: 25
                    width: 300
                    Layout.leftMargin: 20
                    Layout.rightMargin: 20
                    color: "transparent"
                    TextInput {
                        id: exportAsImageSequenceSwitchName
                        anchors.fill: parent
                        leftPadding: 5
                        topPadding: 2
                        bottomPadding: 2
                        enabled: model.imagesequencePrefixEnabled
                        text: model.imagesequencePrefixValue
                        clip: true
                        font.pointSize: 12
                        color: enabled ? "#FFFFFF" : "#b0b0b0";
                        onEditingFinished: {
                            model.imagesequencePrefixValue = text;
                        }
                        Component.onCompleted: {
                            text = model.imagesequencePrefixValue
                        }
                    }
                }
                ComboBox {
                    id: imageFormatComboBox
                    Layout.fillWidth: true
                    textRole: "imageFormatName"
                    model: imageFormatModel
                    //enabled: model.imagesequencePrefixEnabled
                    onActivated: {
                        imageFormatModel.setActiveValueAt(currentIndex);
                    }
                    Component.onCompleted: {
                        currentIndex = imageFormatModel.getActiveValueIndex();
                    }
                }

                Switch {
                    id: exportAsCSVSwitch
                    text: model.exportCSVName
                    checked: model.exportCSVValue
                    action: Action {
                        onTriggered: {
                            model.exportCSVValue = !model.exportCSVValue;
                            //exportAsCSVSwitchName.enabled = model.exportCSVValue;
                        }
                    }
                    ToolTip.text: model.exportCSVTooltip
                    ToolTip.delay: 500
                    ToolTip.visible: hovered
                }
                Rectangle {
                    border {
                        color: "#8066b0"
                        width: 1
                    }
                    Layout.fillWidth: parent
                    height: 25
                    width: 300
                    Layout.leftMargin: 20
                    Layout.rightMargin: 20
                    color: "transparent"
                    TextInput {
                        id: exportAsCSVSwitchName
                        anchors.fill: parent
                        leftPadding: 5
                        topPadding: 2
                        bottomPadding: 2
                        enabled: model.exportCSVValue
                        text: model.csvFileNameValue
                        clip: true

                        font.pointSize: 12
                        color: enabled ? "#FFFFFF" : "#b0b0b0";
                        onEditingFinished: {
                            model.csvFileNameValue = text;
                        }
                        Component.onCompleted: {
                            text = model.csvFileNameValue
                        }
                    }
                }
                Label {
                    Layout.columnSpan: 1
                }

                Switch {
                    id: enableLivePreviewSwitch
                    text: model.enableLivePreviewName
                    checked: model.enableLivePreviewValue
                    action: Action {
                        onTriggered: {
                            model.enableLivePreviewValue = !model.enableLivePreviewValue;
                        }
                    }
                    ToolTip.text: model.enableLivePreviewTooltip
                    ToolTip.delay: 500
                    ToolTip.visible: hovered
                }
                Label {
                    Layout.columnSpan: 2
                }

                Switch {
                    id: exportAsOverlaySwitch
                    text: model.exportAsOverlayName
                    checked: model.exportAsOverlayValue
                    action: Action {
                        onTriggered: {
                            model.exportAsOverlayValue = !model.exportAsOverlayValue;
                            if (model.exportAsOverlayValue)
                            {
                                // 0 - JPEG, 1 - PNG
                                imageFormatModel.setActiveValueAt(1);
                                imageFormatComboBox.currentIndex = 1;
                            }

                            imagecomposer.updateComposition();
                        }
                    }
                    ToolTip.text: model.exportAsOverlayTooltip
                    ToolTip.delay: 500
                    ToolTip.visible: hovered

                }
                Label {
                    Layout.columnSpan: 2
                }


                Label {
                    Layout.leftMargin: 54
                    text: "Resolution:"
                }
                ComboBox {
                    Layout.leftMargin: 20
                    Layout.rightMargin: 20
                    Layout.fillWidth: true
                    model: resolutionsModel
                    textRole: "resolutionName"
                    onActivated: {
                        resolutionsModel.setActiveValueAt(currentIndex);
                        imagecomposer.updateComposition();
                    }
                    Component.onCompleted: {
                        currentIndex = resolutionsModel.getActiveValueIndex();
                    }
                }
                Label { }

                ProgressBar {
                    id: exportProgressBar
                    Layout.columnSpan: 3
                    Layout.fillWidth: true
                    value: 0.0
                }
                Button {
                    id: exportButton
                    Layout.columnSpan: 3
                    Layout.fillWidth: true
                    enabled: model.exportButtonEnabled
                    text: "Export"
                    action: exportButtonAction
                }
                Action {
                    id: exportButtonAction
                    shortcut: "space"
                    onTriggered: {
                        if (exporter.isExporting())
                        {
                            exportButton.text = "Export";
                            exportProgressBar.value = 0.0;
                            exporter.stopExporting();
                        } else {
                            if (videocapturelist.getOpenVideosCount() > 0)
                            {
                                exporter.startExporting()
                            }
                        }
                    }
                }

                Connections {
                    target: videocapturelist
                    function onFramesReady() {
                        exportProgressBar.value = videocapturelist.getShortestVideoProgress();
                        exportButton.text       = Utils.round(videocapturelist.getShortestVideoProgress() * 100, 1) + "% (Space to pause)";
                    }
                    function onFinishedProcessing() {
                        exportButton.text = "Export";
                        exportProgressBar.value = 0.0;
                        frameprocessing.resetState(videocapturelist.getUnsignedRecordedFramerates());
                    }
                }

            }
        }
     }
}
