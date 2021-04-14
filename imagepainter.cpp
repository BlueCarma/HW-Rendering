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

    m_image = image;

    //This is needed to trigger an update of updatePaintNode
    update();
}

QSGNode *ImagePainter::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *)
{
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
