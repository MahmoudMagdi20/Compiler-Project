#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H

#include <QMainWindow>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include<QtCore>
#include<QtGui>

class SyntaxTree : public QMainWindow
{
    Q_OBJECT
public:
    explicit SyntaxTree(QWidget *parent = nullptr);

signals:

public slots:

private:
    SyntaxTree *ui;
    QGraphicsScene *scene;

};

#endif // SYNTAXTREE_H
