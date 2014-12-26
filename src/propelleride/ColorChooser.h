#pragma once

/*
 * This source file is part of EasyPaint.
 *
 * Copyright (c) 2012 LSPaint <https://github.com/Gr1N/LSPaint>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <QLabel>

QT_BEGIN_NAMESPACE
class QColor;
class QPixmap;
class QPainter;
class QMouseEvent;
class QColorDialog;
QT_END_NAMESPACE

/**
 * @brief Widget for selecting color.
 *
 */
class ColorChooser : public QLabel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     *
     * @param r Red
     * @param g Green
     * @param b Blue
     * @param parent Pointer for parent.
     */
    explicit ColorChooser(int newkey,
                          QString hex,
                          QWidget *parent = 0);
    ~ColorChooser();
    
private:
    QColor *mCurrentColor;
    QPixmap *mPixmapColor;
    QPainter *mPainterColor;
    int key;

public slots:
    /**
     * @brief Slot for set color to widget.
     *
     * @param color Color to set.
     */
    void setColor(const QColor &color);

signals:
    /**
     * @brief Signal for sending choosen color
     *
     * @param Color to send
     */
    void sendColor(int, const QColor &);

protected:
    void mousePressEvent(QMouseEvent *event);
    
};
