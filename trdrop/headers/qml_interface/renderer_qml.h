#ifndef IMAGEVIEWER_QML_H
#define IMAGEVIEWER_QML_H

#include <QtQuick/QQuickPaintedItem>
#include <QPainter>
#include <QColor>

#include <memory>
#include <cmath>
#include <iostream>
#include "headers/cpp_interface/framerateoptions.h"
#include "headers/qml_models/exportoptionsmodel.h"
#include "headers/cpp_interface/framerateplot.h"
#include "headers/cpp_interface/frametimeplot.h"

#include "headers/cpp_interface/tearmodel.h"

//! Renders all visual elements (plots / text / tears) onto the qimage
class RendererQML : public QObject
{
    Q_OBJECT

//! constructors
public:
    //! default constructor
    RendererQML(std::shared_ptr<QList<FramerateOptions>> shared_fps_options_list
              , std::shared_ptr<GeneralOptionsModel> shared_general_options_model
              , std::shared_ptr<ExportOptionsModel> shared_export_options_model
              , std::shared_ptr<FrameratePlot> shared_framerate_plot_instance
              , std::shared_ptr<FrametimePlot> shared_frametime_plot_instance
              , std::shared_ptr<TearModel> shared_tear_model
              , std::shared_ptr<ResolutionsModel> shared_resolution_model
              , QQuickItem *parent = nullptr)
        : QObject(parent)
        , _shared_fps_options_list(shared_fps_options_list)
        , _shared_general_options_model(shared_general_options_model)
        , _shared_export_options_model(shared_export_options_model)
        , _shared_framerate_plot_instance(shared_framerate_plot_instance)
        , _shared_frametime_plot_instance(shared_frametime_plot_instance)
        , _shared_tear_model(shared_tear_model)
        , _shared_resolution_model(shared_resolution_model)
        , _cached_images(30, QImage()) // currently hardcoded to 30 on startup (half of 60 of the default framerate analysis time)
    { }

//! methods
public:
    //! signal to wait for to render the full image
    Q_SIGNAL void imageReady(const QImage & image);
    //! paints all elements and emits imageReady when done
    Q_SLOT void processImage(QImage qml_image)
    {
        // if somehow the image is null, we don't terminate (should not happen)
        if (qml_image.isNull()) return;

        // convert to alpha
        if (_shared_export_options_model->export_as_overlay())
        {
            qml_image = QImage(qml_image.size(), QImage::Format_ARGB32);
            qml_image.fill(Qt::transparent);
        } else {
            qml_image = _get_next_image(qml_image).copy();
        }

        // saving the image for redraw purposes
        _qml_image = qml_image.copy();

        QPainter painter;
        painter.begin(&qml_image);

        // paint framerate dependent "widgets"
        if (_shared_general_options_model->get_enable_framerate_analysis())
        {
            _draw_fps_text(painter);
            _draw_framerate_graph(painter);
        }
        // paint frametime dependent "widgets"
        if (_shared_general_options_model->get_enable_frametime_analysis())
        {
            _draw_frametime_graph(painter);
        }
        // paint tear depedent "widgets"
        if (_shared_general_options_model->get_enable_tear_analysis())
        {
            _draw_tears(painter);
        }

        painter.end();
        emit imageReady(qml_image);
    }
    //! can be triggered if options change
    Q_SLOT void redraw()
    {
        int framerate_analysis_range = (*_shared_general_options_model).get_framerate_range();
        if (!_is_cached_image_queue_size_valid(framerate_analysis_range))
        {
            _reshape_cached_image_queue();
        }
        processImage(_qml_image);
    }
    //! can be triggered if you need to reshape the cached image queue + redraw
    Q_SLOT void forced_reshape_redraw()
    {
        _reshape_cached_image_queue();
        processImage(_qml_image);
    }

//! methods
private:
    //! paint framerate text
    void _draw_fps_text(QPainter & painter)
    {
        int video_count  = _get_video_count();
        int image_width  = _qml_image.size().width();
        int image_height = _qml_image.size().height();
        size_t frame_index = _get_current_frame_index();

        // this positioning logic is not inside the fps options as we need the index
        for(int i = 0; i < _shared_fps_options_list->size(); ++i)
        {
            const bool options_enabled = (*_shared_fps_options_list)[i].enabled;
            const bool text_enabled    = (*_shared_fps_options_list)[i].displayed_text.enabled();
            const double rel_fps_text_x_position = (*_shared_fps_options_list)[i].rel_fps_text_x_position.value();
            const double rel_fps_text_y_position = (*_shared_fps_options_list)[i].rel_fps_text_y_position.value();
            if (options_enabled && text_enabled)
            {
                int x_step = static_cast<int>(image_width / video_count);
                int y_step = static_cast<int>(image_height * rel_fps_text_y_position);
                int x = x_step * i + (x_step * rel_fps_text_x_position); // width
                int y = y_step; // height
                (*_shared_fps_options_list)[i].paint_fps_text(&painter, x, y, frame_index);
            }
        }
    }
    //! calls the underlying instance to draw the graph
    void _draw_framerate_graph(QPainter & painter)
    {
        bool enable_framerate_centering = (*_shared_general_options_model).get_enable_framerate_centering();
        bool enable_x_axis_text = (*_shared_general_options_model).get_enable_x_axis_text();
        _shared_framerate_plot_instance->draw_framerate_plot(&painter, enable_framerate_centering, enable_x_axis_text);
    }
    //! calls the underlying instance to draw the graph
    void _draw_frametime_graph(QPainter & painter)
    {
        _shared_frametime_plot_instance->draw_frametime_plot(&painter);
    }
    //! calls each tearmodel to draw each tear
    void _draw_tears(QPainter & painter)
    {
        _shared_tear_model->draw_tears(&painter);
    }
    //! returns the amount of videos loaded (slightly dirty)
    int _get_video_count()
    {
        int video_count = 0;
        for(int i = 0; i < _shared_fps_options_list->size(); ++i)
        {
            if ((*_shared_fps_options_list)[i].enabled) ++video_count;
        }
        return video_count;
    }

    //! default image to display until the framerate plot reaches the state to show the image (center)
    QImage _get_default_image() const
    {
        QSize size;
        // kind of scuffed to code it in here, but as the resolution is only set in the export options it doesnt make sense to include them here
        if (_qml_image.isNull())
        {
            size = QSize(960, 540);
        }
        else
        {
            size = _qml_image.size();
        }
        QImage default_image(size, QImage::Format_RGB888);
        QColor black(0, 0, 0);
        default_image.fill(black);
        return default_image;
    }
    //! returns the next frame which should be at the "center" of the FPS graph
    //! the next frame which is "popped from the front" is the next one
    //! logic is based on the assumption that `_cached_images.size` == framerate anaylsis range / 2 from general options
    QImage _get_next_image(const QImage & original_next_frame)
    {
        bool enable_framerate_centering = (*_shared_general_options_model).get_enable_framerate_centering();
        // save the image in case the framerate centering is enabled mid-run
        _cached_images.push_back(original_next_frame);
        QImage next_image = _cached_images.front().copy();
        _cached_images.pop_front();
        // if its disabled, return the incoming image
        if (!enable_framerate_centering)
        {
            return original_next_frame;
        }
        // if its enabled, check if the cache_image deque already had "real" images saved (happens at the start of an export where only dummy images are stored)
        if (next_image.isNull())
        {
            QImage filler_image(original_next_frame.size(), QImage::Format_RGB888);
            QColor black(0, 0, 0);
            filler_image.fill(black);
            return filler_image;
        }
        // it we have a resized image incoming, resize the stored image
        if (next_image.size() != original_next_frame.size())
        {
            next_image = next_image.scaledToHeight(original_next_frame.size().height());
        }
        return next_image;
    }
    //! reshape the image queue to fit the half of the framerate plot
    void _reshape_cached_image_queue()
    {
        int new_container_size = std::round((*_shared_general_options_model).get_framerate_range() / 2);
        QImage default_image = _get_default_image();
        _cached_images.resize(0); // remove previously added frames
        _cached_images.resize(new_container_size, default_image);
    }
    //! queue size has to be half the length of the framerate analysis range to show the "current" image when the plot reaches the center
    bool _is_cached_image_queue_size_valid(const int framerate_analysis_range) const
    {
        const unsigned long long half_range = framerate_analysis_range / 2;
        return _cached_images.size() == half_range;
    }
    //! calculates the "current" frame index. current means the value which is at the center of the plot if the option is enabled in general settings
    size_t _get_current_frame_index() const
    {
        if ((*_shared_general_options_model).get_enable_framerate_centering())
        {
            const int framerate_range = (*_shared_general_options_model).get_framerate_range();
            const size_t half_range = std::round(framerate_range / 2);
            return half_range;
        }
        else {
            // if centering is not set, the most current value is at the front of the framerate_history deque
            return 0;
        }
    }

//! member
private:
    //! cached image to draw onto
    QImage _qml_image;
    //! used to get te video_count
    std::shared_ptr<QList<FramerateOptions>> _shared_fps_options_list;
    //! used to check what needs to be drawn
    std::shared_ptr<GeneralOptionsModel> _shared_general_options_model;
    //! used to check if overlay only is exported
    std::shared_ptr<ExportOptionsModel> _shared_export_options_model;
    //! used draw the framerate plot
    std::shared_ptr<FrameratePlot> _shared_framerate_plot_instance;
    //! used to draw frametime plot
    std::shared_ptr<FrametimePlot> _shared_frametime_plot_instance;
    //! used to draw the tears
    std::shared_ptr<TearModel> _shared_tear_model;
    //! stores the current resolution
    std::shared_ptr<ResolutionsModel> _shared_resolution_model;
    //! Test - caching images
    std::deque<QImage> _cached_images;
};

#endif // IMAGEVIEWER_QML_H
