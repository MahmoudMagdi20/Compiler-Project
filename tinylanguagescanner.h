#ifndef TINYLANGUAGESCANNER_H
#define TINYLANGUAGESCANNER_H

#include <QMainWindow>
#include <QFile>
#include<QTextStream>
#include <QFileDialog>
#include<QMessageBox>
#include<QDir>
#include <iostream>
#include <fstream>
#include <QString>
#include <sstream>
#include <QVector>
#include<QPair>
#include <QStack>
#include<QGraphicsScene>
#include<QGraphicsView>
#include<QGraphicsItem>
#include <QApplication>
#include <QDesktopWidget>
#include <algorithm>
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class TinyLanguageScanner; }
QT_END_NAMESPACE

class SyntaxTreeNode {
public:
    QString s;
    int x;
    int y;
    SyntaxTreeNode* sibling;
    vector<SyntaxTreeNode*> children;
    SyntaxTreeNode(QString s) : s(s),sibling(nullptr) {}
    void add_child (SyntaxTreeNode *s)
    {
        children.push_back(s);
    }
    void add_sibling(SyntaxTreeNode* s)
    {
        sibling = s;
    }
    QString str()
    {
        return s;
    }
};

class TinyLanguageScanner : public QMainWindow
{
    Q_OBJECT

public:
    TinyLanguageScanner(QWidget *parent = nullptr);
    ~TinyLanguageScanner();

private slots:
    void on_filebutton_clicked();
    void on_scannerButton_clicked();
    void on_parserButton_clicked();

  //void on_scannerButton_clicked();

private:
    Ui::TinyLanguageScanner *ui;
    bool isTokenReservedWord(QString a);
    bool isLetter(QChar c);
    bool isDigit(QChar c);
    bool isNumber(QString c);
    bool isSymbol(QChar c);
    bool isOp(QString c);
    void getTextFile();
    void Draw(QString type,QString text,int x,int y,int width,int height);
    void syntaxTreeDraw(SyntaxTreeNode*);
    bool scanner(QString a);
    bool parser();
    void prepareDraw();
    void assign_coordinates(SyntaxTreeNode* root, int x, int y);
    void prepareinput();
    void DrawLines();
    QMainWindow *syntaxTree=new QMainWindow;
    QGraphicsView *graphicsview=new QGraphicsView(syntaxTree);
    QVector<QGraphicsScene*>scenes;
    QGraphicsScene *scene=new QGraphicsScene(graphicsview);


};

#endif // TINYLANGUAGESCANNER_H
