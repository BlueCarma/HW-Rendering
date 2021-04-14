#include <QSGTexture>
#include <QQuickWindow>
#include <QSGNode>
#include <QSGSimpleTextureNode>

#include "imagepainter.h"




ImagePainter* singleton = nullptr;

ImagePainter::ImagePainter()
{
    singleton = this;
    setFlags(QQuickItem::ItemHasContents);

    QObject::connect(this, &ImagePainter::triggerUpdate, this, &ImagePainter::onTriggerUpdate, Qt::QueuedConnection);

}

ImagePainter *ImagePainter::getSingleton()
{
    return singleton;
}

QImage ImagePainter::image() const
{
    return m_image;
}

void ImagePainter::setImage(QImage& image)
{
    if (m_image == image)
        return;

    m_image = image.copy();

    //This is needed to trigger an update of updatePaintNode
//    update();
    emit triggerUpdate();
}

void ImagePainter::onTriggerUpdate()
{
    qDebug() << Q_FUNC_INFO;

    update();
}

QSGNode *ImagePainter::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *)
{
    qDebug() << Q_FUNC_INFO;
    auto node = dynamic_cast<QSGSimpleTextureNode *>(oldNode);

    if (!node) {
        node = new QSGSimpleTextureNode();
    }

    QSGTexture *texture = window()->createTextureFromImage(m_image, QQuickWindow::TextureIsOpaque);
    node->setOwnsTexture(true);
    node->setRect(boundingRect());
    node->markDirty(QSGNode::DirtyMaterial | QSGNode::DirtyGeometry);
    node->setTexture(texture);

   return node;

}
