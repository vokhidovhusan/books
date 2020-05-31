/*
    Copyright (c) 2009-10 Qtrac Ltd. All rights reserved.

    This program or module is free software: you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version. It is provided
    for educational purposes and is distributed in the hope that it will
    be useful, but WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
    the GNU General Public License for more details.
*/

#include "crossfader.hpp"


CrossFader::CrossFader(const QString &filename, const QImage &first,
        const double &firstWeight, const QImage &last,
        const double &lastWeight, QObject *parent)
    : QThread(parent),
      m_filename(filename), m_firstWeight(firstWeight),
      m_lastWeight(lastWeight), m_stopped(false)
{
    QSize size = first.size().boundedTo(last.size());
    m_first = first.scaled(size, Qt::IgnoreAspectRatio,
                           Qt::SmoothTransformation);
    m_last = last.scaled(size, Qt::IgnoreAspectRatio,
                         Qt::SmoothTransformation);
}


void CrossFader::run()
{
    QImage image(m_first.width(), m_first.height(),
                 QImage::Format_RGB32);
    emit progress(0);

#if CROSSFADE == PIXEL
    const float onePercent = image.width() / 100.0;
    for (int x = 0; x < image.width(); ++x) { // Naive and slow!
        for (int y = 0; y < image.height(); ++y) {
            QRgb firstPixel = m_first.pixel(x, y);
            QRgb lastPixel = m_last.pixel(x, y);
            int red = qRound((qRed(firstPixel) * m_firstWeight) +
                             (qRed(lastPixel) * m_lastWeight));
            int green = qRound((qGreen(firstPixel) * m_firstWeight) +
                               (qGreen(lastPixel) * m_lastWeight));
            int blue = qRound((qBlue(firstPixel) * m_firstWeight) +
                              (qBlue(lastPixel) * m_lastWeight));
            image.setPixel(x, y, qRgb(red, green, blue));
            if ((y % 64) == 0 && m_stopped)
                return;
        }
        if (m_stopped)
            return;
        emit progress(qRound(x / onePercent));
    }
#elif CROSSFADE == SCANLINE
    const float onePercent = image.height() / 100.0;
    for (int y = 0; y < image.height(); ++y) { // Faster
        QRgb *firstPixels = reinterpret_cast<QRgb*>(
                m_first.scanLine(y));
        QRgb *lastPixels = reinterpret_cast<QRgb*>(
                m_last.scanLine(y));
        QRgb *pixels = reinterpret_cast<QRgb*>(image.scanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            QRgb firstPixel = firstPixels[x];
            QRgb lastPixel = lastPixels[x];
            int red = qRound((qRed(firstPixel) * m_firstWeight) +
                            (qRed(lastPixel) * m_lastWeight));
            int green = qRound((qGreen(firstPixel) * m_firstWeight) +
                            (qGreen(lastPixel) * m_lastWeight));
            int blue = qRound((qBlue(firstPixel) * m_firstWeight) +
                            (qBlue(lastPixel) * m_lastWeight));
            pixels[x] = qRgb(red, green, blue);
        }
        if (m_stopped)
            return;
        emit progress(qRound(y / onePercent));
    }
#elif CROSSFADE == BITS
    const int onePercent = qRound(image.width() * image.height() /
                                  100.0);
    QRgb *firstPixels = reinterpret_cast<QRgb*>(m_first.bits());
    QRgb *lastPixels = reinterpret_cast<QRgb*>(m_last.bits());
    QRgb *pixels = reinterpret_cast<QRgb*>(image.bits()); // Fastest
    for (int i = 0; i < image.width() * image.height(); ++i) {
        QRgb firstPixel = firstPixels[i];
        QRgb lastPixel = lastPixels[i];
        int red = qRound((qRed(firstPixel) * m_firstWeight) +
                         (qRed(lastPixel) * m_lastWeight));
        int green = qRound((qGreen(firstPixel) * m_firstWeight) +
                           (qGreen(lastPixel) * m_lastWeight));
        int blue = qRound((qBlue(firstPixel) * m_firstWeight) +
                           (qBlue(lastPixel) * m_lastWeight));
        pixels[i] = qRgb(red, green, blue);
        if ((i % onePercent) == 0) {
            if (m_stopped)
                return;
            emit progress(i / onePercent);
        }
    }
#endif
    emit progress(image.width());

    if (m_stopped)
        return;
    emit saving(m_filename);

    if (m_stopped)
        return;
    emit saved(image.save(m_filename), m_filename);
}
