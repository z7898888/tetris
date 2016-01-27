#ifndef ITEMFACTORY_H
#define ITEMFACTORY_H

class GraphicsItem;

class ItemFactory
{
    static GraphicsItem* item[7];
public:
    ItemFactory();
    static GraphicsItem* getGrapicsItem();
    static GraphicsItem* createGraphicsItem();
};

#endif // ITEMFACTORY_H
