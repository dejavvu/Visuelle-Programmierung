#include "Details_scrollArea.h"

#include "node.h"
#include <QDebug>

Details_scrollArea::Details_scrollArea(QWidget *parent) :
    QScrollArea(parent)
{
    setWidgetResizable(true);
    currentDisplayingNode = nullptr;
}

void Details_scrollArea::setNewDetailsWidget(Node *newDisplayingNode, QWidget *newWidget){
    if(currentDisplayingNode != nullptr){
        currentDisplayingNode->detailsWidget = takeWidget();
    }
    setWidget(newWidget);
    currentDisplayingNode = newDisplayingNode;
}
