#ifndef DETAILS_SCROLLAREA_H
#define DETAILS_SCROLLAREA_H

#include <QScrollArea>

class Node;

class Details_scrollArea : public QScrollArea
{
public:
    Details_scrollArea(QWidget *parent = nullptr);
    void setNewDetailsWidget(Node *newDisplayingNode, QWidget *newWidget);

    Node *currentDisplayingNode;
};

#endif // DETAILS_SCROLLAREA_H
