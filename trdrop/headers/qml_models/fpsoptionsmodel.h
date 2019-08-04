#ifndef FPSOPTIONSMODEL_H
#define FPSOPTIONSMODEL_H

#include <QAbstractTableModel>
#include <QDebug>
#include <memory>

#include "headers/cpp_interface/fpsoptions.h"
#include "headers/cpp_interface/frameratemodel.h"
#include "headers/qml_models/resolutionsmodel.h"

//!
class FPSOptionsModel : public QAbstractListModel
{
    Q_OBJECT
//! constructors
public:
    //! TODO
    FPSOptionsModel(std::shared_ptr<FramerateModel> shared_framerate_model
                  , std::shared_ptr<QList<FPSOptions>> shared_fps_options_list
                  , std::shared_ptr<ResolutionsModel> shared_resolution_model
                  , QObject * parent = nullptr)
        : QAbstractListModel(parent)
        , _shared_framerate_model(shared_framerate_model)
        , _shared_fps_options_list(shared_fps_options_list)
        , _shared_resolution_model(shared_resolution_model)
        , _max_video_count(3)
    {
        _init_options();
        _setup_role_names();
    }
    //!
    enum FPSOptionsRoles
    {
        ColorPickNameRole            = Qt::UserRole + 30
      , ColorPickTooltipRole         = Qt::UserRole + 31
      , ColorPickValueRole           = Qt::UserRole + 32
      , PixelDifferenceNameRole      = Qt::UserRole + 33
      , PixelDifferenceTooltipRole   = Qt::UserRole + 34
      , PixelDifferenceValueRole     = Qt::UserRole + 35
      , PixelDifferenceEnabledRole   = Qt::UserRole + 36
      , DisplayedTextNameRole        = Qt::UserRole + 37
      , DisplayedTextTooltipRole     = Qt::UserRole + 38
      , DisplayedTextValueRole       = Qt::UserRole + 39
      , DisplayedTextFontRole        = Qt::UserRole + 40
      , DisplayedTextEnabledRole     = Qt::UserRole + 41
      , RecordedFramerateNameRole    = Qt::UserRole + 42
      , RecordedFramerateTooltipRole = Qt::UserRole + 43
      , RecordedFramerateValueRole   = Qt::UserRole + 44
      , FPSOptionsEnabled            = Qt::UserRole + 46
    };
//! methods
public:
    //! TODO
    Q_SIGNAL void propagateFPSOptions(const QList<FPSOptions> fpsOptionsList);
    //! TODO
    int rowCount(const QModelIndex & parent = QModelIndex()) const override
    {
        Q_UNUSED(parent)
        return _shared_fps_options_list->size();
    }
    //! a getter for the _role_names to enable the access via QML
    QHash<int, QByteArray> roleNames() const override { return _role_names; }
    //! QAbstractModel function which is called if a read in QML happens
    QVariant data(const QModelIndex & index,int role) const override
    {
        int row = index.row();
        switch (role)
        {
            case ColorPickNameRole:
                return (*_shared_fps_options_list)[row].fps_plot_color.name();
            case ColorPickTooltipRole:
                return (*_shared_fps_options_list)[row].fps_plot_color.tooltip();
            case ColorPickValueRole:
                return (*_shared_fps_options_list)[row].fps_plot_color.color();
            case PixelDifferenceNameRole:
                return (*_shared_fps_options_list)[row].pixel_difference.name();
            case PixelDifferenceTooltipRole:
                return (*_shared_fps_options_list)[row].pixel_difference.tooltip();
            case PixelDifferenceValueRole:
                return (*_shared_fps_options_list)[row].pixel_difference.value();
            case PixelDifferenceEnabledRole:
                return (*_shared_fps_options_list)[row].pixel_difference.enabled();
            case DisplayedTextNameRole:
                 return (*_shared_fps_options_list)[row].displayed_text.name();
            case DisplayedTextTooltipRole:
                 return (*_shared_fps_options_list)[row].displayed_text.tooltip();
            case DisplayedTextValueRole:
                 return (*_shared_fps_options_list)[row].displayed_text.value();
            case DisplayedTextFontRole:
                 return (*_shared_fps_options_list)[row].displayed_text.font();
            case DisplayedTextEnabledRole:
                 return (*_shared_fps_options_list)[row].displayed_text.enabled();
            case RecordedFramerateNameRole:
                 return (*_shared_fps_options_list)[row].recorded_framerate.name();
            case RecordedFramerateTooltipRole:
                 return (*_shared_fps_options_list)[row].recorded_framerate.tooltip();
            case RecordedFramerateValueRole:
                 return (*_shared_fps_options_list)[row].recorded_framerate.value();
            case FPSOptionsEnabled:
                 return (*_shared_fps_options_list)[row].enabled;
            default:
                return QVariant();
        }
    }
    //! QAbstractModel function which is called if an assignment in QML happens
    bool setData(const QModelIndex & index, const QVariant & value, int role) override
    {
        int row = index.row();
        if      (role == ColorPickNameRole)          (*_shared_fps_options_list)[row].fps_plot_color.setName(value.toString());
        else if (role == ColorPickTooltipRole)       (*_shared_fps_options_list)[row].fps_plot_color.setTooltip(value.toString());
        else if (role == ColorPickValueRole)         (*_shared_fps_options_list)[row].fps_plot_color.setColor(value.toString());
        else if (role == PixelDifferenceNameRole)    (*_shared_fps_options_list)[row].pixel_difference.setName(value.toString());
        else if (role == PixelDifferenceTooltipRole) (*_shared_fps_options_list)[row].pixel_difference.setTooltip(value.toString());
        else if (role == PixelDifferenceValueRole)   (*_shared_fps_options_list)[row].pixel_difference.setValue(value.toUInt());
        else if (role == PixelDifferenceEnabledRole) (*_shared_fps_options_list)[row].pixel_difference.setEnabled(value.toBool());
        else if (role == DisplayedTextNameRole)      (*_shared_fps_options_list)[row].displayed_text.setName(value.toString());
        else if (role == DisplayedTextTooltipRole)   (*_shared_fps_options_list)[row].displayed_text.setTooltip(value.toString());
        else if (role == DisplayedTextValueRole)     (*_shared_fps_options_list)[row].displayed_text.setValue(value.toString());
        else if (role == DisplayedTextFontRole)      (*_shared_fps_options_list)[row].displayed_text.setFont(value.value<QFont>());
        else if (role == DisplayedTextEnabledRole)   (*_shared_fps_options_list)[row].displayed_text.setEnabled(value.toBool());
        else if (role == RecordedFramerateValueRole) (*_shared_fps_options_list)[row].recorded_framerate.setValue(value.toDouble());
        else if (role == FPSOptionsEnabled)          (*_shared_fps_options_list)[row].enabled = value.toBool();
        else return false;
        QModelIndex toIndex(createIndex(rowCount() - 1, index.column()));
        emit dataChanged(index, toIndex);
        return true;
    }
    //! tells the views that the model's state has changed -> this triggers a "recompution" of the delegate
    Q_INVOKABLE void resetModel()
    {
        beginResetModel();
        endResetModel();
    }
    //! apply the pixel difference to all indices
    Q_INVOKABLE void applyPixelDifference(const QVariant & value)
    {
        QModelIndex q0 = createIndex(0, 0);
        QModelIndex q1 = createIndex(1, 0);
        QModelIndex q2 = createIndex(2, 0);
        setData(q0, value, PixelDifferenceValueRole);
        setData(q1, value, PixelDifferenceValueRole);
        setData(q2, value, PixelDifferenceValueRole);
    }
    //! apply the pixel difference to all indices
    Q_INVOKABLE void applyColor(const QVariant & value, const int & row)
    {
        QModelIndex q = createIndex(row, 0);
        setData(q, value, ColorPickValueRole);
    }
    //! TODO
    Q_INVOKABLE void revertModelToDefault()
    {
        for (quint8 id = 0; id < 3; ++id) {
            (*_shared_fps_options_list)[id].revert_to_default();
        }
        resetModel();
    }
    //! TODO
    Q_INVOKABLE void updateEnabledRows(const QList<QVariant> filePaths)
    {
        int video_count = filePaths.size();
        for(int i = 0; i < _shared_fps_options_list->size(); ++i)
        {
            const bool loaded_file = i < video_count;
            QModelIndex q = createIndex(i, 0);
            setData(q, loaded_file, FPSOptionsEnabled);
        }
    }
    //! TODO
    Q_INVOKABLE void setRecordedFramerates(const QList<double> recorded_framerates)
    {
        for (int i = 0; i < recorded_framerates.length(); ++i)
        {
            const double framerate = recorded_framerates[i];
            QModelIndex q = createIndex(i, 0);
            setData(q, framerate, RecordedFramerateValueRole);
        }
    }
//! methods
private:
    //! Set names to the role name hash container (QHash<int, QByteArray>)
    void _setup_role_names()
    {
        _role_names[ColorPickNameRole]    = "colorName";
        _role_names[ColorPickTooltipRole] = "colorTooltip";
        _role_names[ColorPickValueRole]   = "color";

        _role_names[PixelDifferenceNameRole]    = "pixelDifferenceName";
        _role_names[PixelDifferenceTooltipRole] = "pixelDifferenceTooltip";
        _role_names[PixelDifferenceValueRole]   = "pixelDifference";
        _role_names[PixelDifferenceEnabledRole] = "pixelDifferenceEnabled";

        _role_names[DisplayedTextNameRole]      = "displayedTextName";
        _role_names[DisplayedTextTooltipRole]   = "displayedTextTooltip";
        _role_names[DisplayedTextValueRole]     = "displayedText";
        _role_names[DisplayedTextFontRole]      = "displayedTextFont";
        _role_names[DisplayedTextEnabledRole]   = "displayedTextEnabled";

        _role_names[RecordedFramerateNameRole]    = "recordedFramerateName";
        _role_names[RecordedFramerateTooltipRole] = "recordedFramerateTooltip";
        _role_names[RecordedFramerateValueRole]   = "recordedFramerate";

        _role_names[FPSOptionsEnabled] = "fpsOptionsEnabled";
    }
    //! TODO
    void _init_options()
    {
        for (quint8 id = 0; id < _max_video_count; ++id) {
            _shared_fps_options_list->append(FPSOptions(id, _shared_framerate_model, _shared_resolution_model));
        }
    }

//! member
private:
    //! TODO
    std::shared_ptr<FramerateModel> _shared_framerate_model;
    //! TODO
    std::shared_ptr<QList<FPSOptions>> _shared_fps_options_list;
    //! TODO
    std::shared_ptr<ResolutionsModel> _shared_resolution_model;
    //! TODO
    quint8 _max_video_count;
    //! used by the QAbstractListModel to save the role names from QML
    QHash<int, QByteArray> _role_names;
};

#endif // FPSOPTIONSMODEL_H
