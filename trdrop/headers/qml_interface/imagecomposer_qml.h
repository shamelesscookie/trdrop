#ifndef IMAGECOMPOSER_QML_H
#define IMAGECOMPOSER_QML_H

#include <QObject>
#include <QPainter>
#include <QImage>
#include <QDebug>
#include <memory>
#include "opencv2/opencv.hpp"
#include "headers/qml_models/resolutionsmodel.h"

//! composes the QList<QImage> into a single QImage
class ImageComposerQML : public QObject {

    Q_OBJECT

//! constructors
public:
    //! default constructor, the default size is dependent on the first row of the resolutionModel
    ImageComposerQML(std::shared_ptr<ResolutionsModel> shared_resolution_model
                   , QObject * parent = nullptr)
        : QObject(parent)
        , _shared_resolution_model(shared_resolution_model)
    { }

//! methods
public:
    //! signal to wait for to render the full image
    Q_SIGNAL void imageReady(const QImage & image);
    //! side by side only right now
    Q_SLOT void processImages(const QList<QImage> & _qml_image_list)
    {
        QSize _size = _shared_resolution_model->get_active_value().size();

        if (_qml_image_list.size() == 3)
        {
            const QImage scaled_image_01 = _qml_image_list[0].scaledToWidth(_size.width());
            const QImage scaled_image_02 = _qml_image_list[1].scaledToWidth(_size.width());
            const QImage scaled_image_03 = _qml_image_list[2].scaledToWidth(_size.width());

            const int video_count = 3;
            const QImage centered_image_01 = _get_center_from_image(scaled_image_01, video_count);
            const QImage centered_image_02 = _get_center_from_image(scaled_image_02, video_count);
            const QImage centered_image_03 = _get_center_from_image(scaled_image_03, video_count);

            _qml_image = QImage(_size.width(), _size.height(), QImage::Format_RGB888);
            _qml_image.fill(QColor(0, 0, 0));
            QPainter painter;
            painter.begin(&_qml_image);
            const int video_01_x = 0;
            const int video_02_x = static_cast<int>(_size.width() / video_count);
            const int video_03_x = static_cast<int>(_size.width() / video_count) * 2;
            painter.drawImage(video_01_x, 0, centered_image_01);
            painter.drawImage(video_02_x, 0, centered_image_02);
            painter.drawImage(video_03_x, 0, centered_image_03);
            painter.end();
        }
        if (_qml_image_list.size() == 2)
        {
            const QImage scaled_image_01 = _qml_image_list[0].scaledToWidth(_size.width());
            const QImage scaled_image_02 = _qml_image_list[1].scaledToWidth(_size.width());

            const int video_count = 2;
            const QImage centered_image_01 = _get_center_from_image(scaled_image_01, video_count);
            const QImage centered_image_02 = _get_center_from_image(scaled_image_02, video_count);

            _qml_image = QImage(_size.width(), _size.height(), QImage::Format_RGB888);
            _qml_image.fill(QColor(0, 0, 0));
            QPainter painter;
            painter.begin(&_qml_image);
            const int video_01_x = 0;
            const int video_02_x = static_cast<int>(_size.width() / video_count);
            painter.drawImage(video_01_x, 0, centered_image_01);
            painter.drawImage(video_02_x, 0, centered_image_02);
            painter.end();
        }
        if (_qml_image_list.size() == 1)
        {
            _qml_image = _qml_image_list[0].scaledToWidth(_size.width());
        }
        emit imageReady(_qml_image);
    }
    //! TODO
    Q_SLOT void updateComposition()
    {
        QSize size = _shared_resolution_model->get_active_value().size();
        _qml_image = _qml_image.scaledToWidth(size.width()).copy();
        emit imageReady(_qml_image);
    }
//! methods
private:
    //! TODO
    QImage _get_center_from_image(const QImage & image, quint8 video_count)
    {
        QSize _size = _shared_resolution_model->get_active_value().size();
        const int single_video_width  = static_cast<int>(_size.width()  / static_cast<double>(video_count));
        const int single_video_height = _size.height();

        const int center_x = static_cast<int>(image.width()  / 2.0);
        const int start_x  = center_x - static_cast<int>(image.width() / (2 * static_cast<double>(video_count)));
        const int start_y  = 0;

        return image.copy(start_x, start_y, single_video_width, single_video_height);
    }

//! member
private:
    //! TODO
    QImage _qml_image;
    //! TODO
    std::shared_ptr<ResolutionsModel> _shared_resolution_model;
};


#endif // IMAGECOMPOSER_QML_H
