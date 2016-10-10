#include "ScrollWindow.h"
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QGraphicsSceneMouseEvent>

ScrollWindow::ScrollWindow():
    width_(500),
    height_(400),
    scrollBarFG_(new QGraphicsPixmapItem()),
    scrollBarPosition_(0),
    scrollBarBeingDragged_(false)
{
    scrollBarFG_->setParentItem(this);
    drawScrollBar_(); // draw the initial scrollbar
}

/// Adds the specified gui to the ScrollWindow.
/// The first Gui added is on the very top, each subsequent Gui added goes
/// under the previous Gui.
void ScrollWindow::add(Gui *gui)
{
    guis_.push_back(gui);
    drawScrollBar_(); // redraw the scroll bar
}

/// Sets the height of the window of the ScrollWindow.
void ScrollWindow::setHeight(double height)
{
    height_ = height;
    drawScrollBar_();
}

/// Sets the width of the window of the ScrollWindow.
void ScrollWindow::setWidth(double width)
{
    width_ = width;
    drawScrollBar_();
}

QGraphicsItem *ScrollWindow::getGraphicsItem()
{
    return this;
}

void ScrollWindow::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    scrollBarBeingDragged_ = true;
    QGraphicsPixmapItem::mousePressEvent(event);
}

void ScrollWindow::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    scrollBarBeingDragged_ = false;
}

void ScrollWindow::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (scrollBarBeingDragged_){
        // set scroll bar position
        scrollBarPosition_ = event->pos().y() / height_;
        if (scrollBarPosition_ > 1)
            scrollBarPosition_ = 1;
        drawScrollBar_();
    }
}

/// Draws the scrollbar in its current state.
void ScrollWindow::drawScrollBar_()
{
    // draw background scrol bar
    QImage img(QSize(15,height_),QImage::Format_RGB32);
    img.fill(Qt::blue);
    setPixmap(QPixmap::fromImage(img));

    // draw foreground bar
    double totalStackHeight = 0;
    for (Gui* gui:guis_){
        totalStackHeight += gui->getGraphicsItem()->boundingRect().height();
    }
    double viewFraction = height_ / totalStackHeight;
    if (viewFraction > 1)
        viewFraction = 1;
    double scrollBarHeight = viewFraction * height_;

    QImage img2(QSize(15,scrollBarHeight),QImage::Format_RGB32);
    img2.fill(Qt::red);
    scrollBarFG_->setPixmap(QPixmap::fromImage(img2));

    scrollBarFG_->setPos(0,scrollBarPosition_*height_);

    // draw necessary guis
    // approach:
    // - traverse through guis (list of guis)
    // - if it needs to be drawn, draw it
    double currentOffset = 0;
    double nextAddOffset = 0;
    double topThreshold = scrollBarFG_->pos().y() / height_;
    double botThreshold = (scrollBarFG_->pos().y() + scrollBarHeight) / height_;
    double topPosThreshold = topThreshold * totalStackHeight;
    double botPosThreshold = botThreshold * totalStackHeight;
    for (Gui* gui:guis_){
        // remove all guis
        gui->getGraphicsItem()->setVisible(false);

        // if gui is fully within top/bot box, show it
        double guiHeight = gui->getGraphicsItem()->boundingRect().height();
        if (currentOffset >= topPosThreshold && currentOffset+guiHeight <= botPosThreshold){
            gui->setParentGui(this);
            gui->setGuiPos(QPointF(15,0+nextAddOffset));
            nextAddOffset += guiHeight;
            gui->getGraphicsItem()->setVisible(true);
        }
        currentOffset += guiHeight;
    }
}
