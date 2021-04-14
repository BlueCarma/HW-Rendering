#ifndef IMAGEPAINTER_H
#define IMAGEPAINTER_H

#include <QQuickItem>
#include <QImage>
#include <QDebug>

class ImagePainter : public QQuickItem
{
    Q_OBJECT

public:
    ImagePainter();

    static ImagePainter* getSingleton();

    QImage image() const;

public slots:

    void setImage(QImage& image);
    void onTriggerUpdate();

signals:
    void triggerUpdate();

protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

private:
    QImage m_image;
};

#endif // IMAGEPAINTER_H
