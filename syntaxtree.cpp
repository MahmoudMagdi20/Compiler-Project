#include "syntaxtree.h"

SyntaxTree::SyntaxTree(QWidget *parent) : QMainWindow(parent) , ui(new SyntaxTree)
{
    //ui->setupUi(this);
    scene=new QGraphicsScene(this);

    ellipse=scene->addEllipse(0,0,100,50);
    text=scene->addText("read");
    text->setPos(150,125);
    rectangle=scene->addRect(100,100,100,50);
    line=scene->addLine(50,50,150,100);
}
