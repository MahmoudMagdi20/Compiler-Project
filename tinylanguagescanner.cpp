#include "tinylanguagescanner.h"
#include "ui_tinylanguagescanner.h"

TinyLanguageScanner::TinyLanguageScanner(QWidget *parent): QMainWindow(parent), ui(new Ui::TinyLanguageScanner)
{
    ui->setupUi(this);
}

TinyLanguageScanner::~TinyLanguageScanner()
{
    delete ui;
}

QVector<QPair<QString, QString>> tokens;
QVector<QString> text2words;
QVector<QString> identifiers;
QVector<QString> numbers;
QString  inputText="";
QString text="";
QString text2="";
QString error_msg="";
QStack<QChar> para;
QVector<SyntaxTreeNode*> points;
QVector<QGraphicsRectItem *> rectangles;
QVector<QGraphicsEllipseItem *> ellipses;
QVector<QGraphicsTextItem *> txts;
QVector<QGraphicsLineItem *> lines;
QVector<int> distances;
int elis=0;
int rects=0;
int ls=0;
QString diff = "|" + QString::number(ls);
QVector<int> children;
bool rightInput =true;
bool error = false;
int m;
SyntaxTreeNode* root = nullptr;
SyntaxTreeNode* match(QString expectedToken);
SyntaxTreeNode* addop();
SyntaxTreeNode* mulop();
SyntaxTreeNode* term();
SyntaxTreeNode* simple_exp();
SyntaxTreeNode* exp();
SyntaxTreeNode* assign_stmt();
SyntaxTreeNode* repeat_stmt();
SyntaxTreeNode* statement();
SyntaxTreeNode* stmt_sequence();
SyntaxTreeNode* factor();
SyntaxTreeNode* comparison_op();
SyntaxTreeNode* read_stmt();
SyntaxTreeNode* write_stmt();
SyntaxTreeNode* program();
SyntaxTreeNode* if_stmt();
int sno=0;
enum class State
{
    START,
    INCOMMENT,
    INID,
    INNUM,
    INASSIGN,
    ERROR
};
bool TinyLanguageScanner::isTokenReservedWord(QString a)
{
    if (a == "if" || a == "else" || a == "then" || a == "end" || a == "repeat" || a == "until" || a == "read" || a == "write")
        return true;
    else
        return false;
}
bool TinyLanguageScanner::isLetter(QChar c)
{
    if ((c >= 65 && c <= 90) || ((c >= 97) && c <= 122))
        return true;
    else
        return false;
}
bool TinyLanguageScanner::isDigit(QChar c)
{
    if (c >= 48 && c <= 57)
        return true;
    else
        return false;
}
bool TinyLanguageScanner::isNumber(QString c)
{
    for(int i=0;i<c.length();i++)
        if(isDigit(c[i])==false)
            return false;
    return true;
}
bool TinyLanguageScanner::isSymbol(QChar c)
{
    if (c == 40 || c == 41 || c == 42 || c == 43 || c == 45 || c == 47 || c == 60 || c == 61 || c == 62 || c == 59 || c == ']' || c == ')' || c == '(')
        return true;
    return false;
}
bool TinyLanguageScanner::isOp(QString c)
{
    if(c == "*" || c == "/" || c == "+" || c == "-" || c == "<" || c == "=")
        return true;
    return false;
}
void TinyLanguageScanner::getTextFile()
{
    bool newfile=true;
    l:
    QString file_name=QFileDialog::getOpenFileName(this,"open a file ",QDir::homePath());
    QFile file(file_name);
    QFileInfo finfo(file);
    QString info=finfo.suffix();
    if(info=="")
        QMessageBox::warning(this,"","Error:no file is chosen");

    else if(info!="txt")
    {
        QMessageBox::warning(this,"","Error:Wrong file type \n please chose another one");
        goto l;
    }
    if(!file.open(QFile::ReadOnly|QFile::Text))
    {
        if( inputText!="")
        {
        ui->code->setPlainText( inputText);
        newfile=false;
        }
    }
    if(newfile)
    {
    QTextStream in(&file);
    text =in.readAll();

     inputText=text;
    ui->code->setPlainText( inputText);
    }
    file.close();
}
void TinyLanguageScanner::on_filebutton_clicked()
{
    getTextFile();
}
void TinyLanguageScanner::prepareinput()
{
    text2words.clear();
    identifiers.clear();
    numbers.clear();

    text2="";

    inputText=ui->code->toPlainText();
    if( inputText=="")
    {
        QMessageBox::warning(this,"","Error:No code found");
        rightInput=false;
    }
    else if(inputText.contains(',')==false)
    {

        QMessageBox::warning(this,"","not right input for process");
        rightInput=false;
    }
    else
    {
    int j=0;
    while(inputText[j]!=',')
    {
        text2+=inputText[j];
        j++;
    }

    for(int i=0;i<inputText.length();i++)
    {
        if(inputText[i]=='\n')
            while(inputText[i]!=','&&i<inputText.length())
            {
                text2=text2+inputText[i];
                i++;
            }
    }

    QString save=text2;
    for(int i=0;i<text2.length();i++)
        if(text2[i]=='\n')
            save[i]=',';
        else
            continue;
    text2=save;
    text2.remove(' ');
    int i=0;
    while(i<text2.length())
    {
        save="";
        while(text2[i]!=',')
        {
            save+=text2[i];
            i++;
        }
        text2words.push_back(save);

        i++;
    }
    save="";

    for(int it=0;it<text2words.size();it++)
        if(text2words[it].length()>2&&!isTokenReservedWord(text2words[it])&&!(isNumber(text2words[it])))
           identifiers.push_back(text2words[it]);
        else if((text2words[it].length()==2)&&(text2words[it]!="if"||text2words[it]!=":="))
            identifiers.push_back(text2words[it]);
        else if(text2words[it].length()==1&&isLetter(text2words[it][0])&&!isDigit(text2words[it][0])&&!isSymbol(text2words[it][0]))
            identifiers.push_back(text2words[it]);
        else if(isNumber(text2words[it]))
            numbers.push_back(text2words[it]);
    rightInput=true;
    }
}
void TinyLanguageScanner::Draw(QString l, QString s, int x, int y, int width, int height)
{
    if(l == "rect")
    {
        QGraphicsRectItem *rectangle;
        QGraphicsTextItem *txt;
        txt=scene->addText(s);
        txt->setPos(x+width/3-20, y+height/4);
        txt->setScale(2);
        rectangle=scene->addRect(x,y,width,height);
        rectangles.push_back(rectangle);

    }

    if(l == "eli")
    {
        QGraphicsEllipseItem *ellipse;
        QGraphicsTextItem *txt;
        txt=scene->addText(s);
        txt->setPos(x+width/3, y+height/4);
        txt->setScale(2);
        ellipse=scene->addEllipse(x,y,width,height);

    }


}
void TinyLanguageScanner::prepareDraw()
{
    scene->clear();
    graphicsview->setScene(scene);
    syntaxTree->setWindowState(Qt::WindowMaximized);
    QRect r =QApplication::desktop()->geometry();
    graphicsview->setGeometry(0,0,r.width(),r.height()-100);

}
void TinyLanguageScanner::syntaxTreeDraw(SyntaxTreeNode* root)
{
    if (root == nullptr)
            return;
        if(isOp(root->str())||root->children.size()==0)
            Draw("eli",root->str(),root->x,root->y,130,60);
        else
            Draw("rect",root->str(),root->x,root->y,130,60);

        for (auto child : root->children)
            syntaxTreeDraw(child);
        if(root->sibling!=nullptr)
            syntaxTreeDraw(root->sibling);
}
void TinyLanguageScanner::DrawLines()
{


    for (auto i : points)
        for(auto j:points)
        {
        if(i->x==j->x&&i->y==j->y&&i!=j)
            i->x=(j->x)-200;

        }

    for(auto i:points)
     {
        for(auto j:i->children)
        {
            qreal ab=0,ba=0,ac=0,ca=0;
            QGraphicsLineItem *line;
            ab=i->x;
            ab=ab+65;
            ba=i->y;
            ba=ba+60;
            ac=j->x;
            ac=ac+65;
            ca=j->y;
            line=scene->addLine(ab,ba,ac,ca);

        }
        if(i->sibling!=nullptr)
        {
            qreal ab=0,ba=0,ac=0,ca=0;
            QGraphicsLineItem *line;
            ab=i->x;
            ab=ab+130;
            ba=i->y;
            ba=ba+30;
            ac=i->sibling->x;
            ca=i->sibling->y;
            ca=ca+30;
            line=scene->addLine(ab,ba,ac,ca);
        }
    }
    points.clear();
}
void TinyLanguageScanner::assign_coordinates(SyntaxTreeNode* root, int x, int y)
{
    if (root == nullptr)
        return;

    root->x = x;
    root->y = y;
    points.push_back(root);
    int temp =0;

    for (auto child : root->children)
    {
        assign_coordinates(child, root->x+temp, root->y+400);
        if(child->sibling!=nullptr)
        temp+=1000;
        else
        temp += 400;
    }
    assign_coordinates(root->sibling, root->x+800, root->y);
}

SyntaxTreeNode* match(QString expectedToken)
{
    if (expectedToken == "Number")
    {
            if (m!= text2words.size() && find(numbers.begin(), numbers.end(), text2words[m]) != numbers.end())
            {
                    return new SyntaxTreeNode(text2words[m++]);
            }
            else
            {
                    error = true;
                    return new SyntaxTreeNode("");
            }
    }
    else if (expectedToken == "Identifier")
    {
            if (m != text2words.size() && find(identifiers.begin(), identifiers.end(), text2words[m]) != identifiers.end())
            {
                    return new SyntaxTreeNode(text2words[m++]);
            }
            else
            {
                    error = true;
                    return new SyntaxTreeNode("");
            }
    }
    else if (m!= text2words.size() && text2words[m] == expectedToken)
    {
            return new SyntaxTreeNode(text2words[m++]);
    }
    else
    {
            error = true;
            return new SyntaxTreeNode("");
    }
}
SyntaxTreeNode* addop()
{
    if (m != text2words.size() && text2words[m] == "+")
    {
        match("+");
        return new SyntaxTreeNode("+");
    }

    if (m != text2words.size() && text2words[m] == "-")
    {
        match("-");
        return new SyntaxTreeNode("-");
    }
}
SyntaxTreeNode* mulop()
{
    if (m != text2words.size() && text2words[m] == "*")
    {
        match("*");
        return new SyntaxTreeNode("*");
    }
    if (m != text2words.size() && text2words[m] == "/")
    {
        match("/");
        return new SyntaxTreeNode("/");
    }
}
SyntaxTreeNode* term()
{
    SyntaxTreeNode* temp = factor();
    SyntaxTreeNode* newtemp;
    while (m != text2words.size() && (text2words[m] == "*" || text2words[m] == "/"))
    {
        newtemp = mulop();
        newtemp->add_child(temp);
        newtemp->add_child(factor());
        temp = newtemp;
    }
    return temp;
}
SyntaxTreeNode* simple_exp()
{
    SyntaxTreeNode* temp = term();
    SyntaxTreeNode* newtemp;
    while (m != text2words.size() && (text2words[m] == "+" || text2words[m] == "-"))
    {
            newtemp = addop();
            newtemp->add_child(temp);
            newtemp->add_child(term());
            temp = newtemp;
    }
    return temp;
}
SyntaxTreeNode* exp()
{
    SyntaxTreeNode* temp = simple_exp();
    SyntaxTreeNode* newtemp;
    if (m != text2words.size() && (text2words[m] == "<" || text2words[m] == "="))
    {
            newtemp = comparison_op();
            newtemp->add_child(temp);
            newtemp->add_child(simple_exp());
            temp = newtemp;
    }
    return temp;
}
SyntaxTreeNode* assign_stmt()
{
    SyntaxTreeNode* temp = match("Identifier");
    SyntaxTreeNode* newTemp = match(":=");
    newTemp->add_child(temp);
    newTemp->add_child(exp());
    return newTemp;
}
SyntaxTreeNode* repeat_stmt()
{
    SyntaxTreeNode* temp = match("repeat");
    temp->add_child(stmt_sequence());
    match("until");
    temp->add_child(exp());
    return temp;
}
SyntaxTreeNode* read_stmt()
{
    SyntaxTreeNode* temp = match("read");
    temp->add_child(match("Identifier"));
    return temp;
}
SyntaxTreeNode* write_stmt()
{
    SyntaxTreeNode* temp = match("write");
    temp->add_child(exp());
    return temp;
}
SyntaxTreeNode* comparison_op()
{
    if (m != text2words.size() && text2words[m] == "<")
            return match("<");
    else if (m != text2words.size() && text2words[m] == "=")
            return match("=");
}
SyntaxTreeNode* factor()
{
    SyntaxTreeNode* temp;
    if (m != text2words.size() && text2words[m] == "(")
    {
            match("(");
            temp = exp();
            match(")");
            return temp;
    }
    else if (m != text2words.size() && find(numbers.begin(), numbers.end(), text2words[m]) != numbers.end())
    {
            return new SyntaxTreeNode(text2words[m++]);
    }
    else if (m != text2words.size() && find(identifiers.begin(), identifiers.end(), text2words[m]) != identifiers.end())
    {
            return new SyntaxTreeNode(text2words[m++]);
    }
    else
    {
            error = true;
            return new SyntaxTreeNode("");
    }
}
SyntaxTreeNode* if_stmt()
{
    SyntaxTreeNode* temp = match("if");
    temp->add_child(exp());
    match("then");
    temp->add_child(stmt_sequence());
    if (m != text2words.size() && text2words[m] == "end")
    {
        match("end");
    }
    else if (m != text2words.size() && text2words[m] == "else")
    {
        match("else");
        temp->add_child(stmt_sequence());
        match("end");
    }
    return temp;
}
SyntaxTreeNode* statement()
{
    if (m != text2words.size() && text2words[m] == "if")
    {
        return if_stmt();
    }
    else if (m != text2words.size() && text2words[m] == "repeat")
    {
        return repeat_stmt();
    }
    else if (m != text2words.size() && find(identifiers.begin(), identifiers.end(), text2words[m]) != identifiers.end())
    {
        return assign_stmt();
    }
    else if (m != text2words.size() && text2words[m] == "read")
    {
        return read_stmt();
    }
    else if (m != text2words.size() && text2words[m] == "write")
    {
        return write_stmt();
    }
    else
    {
        error = true;
        return new SyntaxTreeNode("");
    }
}
SyntaxTreeNode* stmt_sequence()
{
    SyntaxTreeNode* temp = statement();
    SyntaxTreeNode* temp2 = temp;
    while (m != text2words.size() && text2words[m] == ";")
    {
        match(";");
        SyntaxTreeNode* newtemp = statement();
        temp2->add_sibling(newtemp);
        temp2 = newtemp;
    }
    return temp;
}
SyntaxTreeNode* program()
{
    return stmt_sequence();
}

bool TinyLanguageScanner::scanner(QString stext)
{
    tokens.clear();

    QString token = "";
    QString code=stext+" ";

    State state = State::START;
    for (int i = 0; i < code.length(); ++i)
    {
        switch (state)
        {
        case State::START:
            token = "";
            if (code[i] == '{')
            {
                state = State::INCOMMENT;
            }
            else if (isLetter(code[i]))
            {
                state = State::INID;
                token += code[i];
            }
            else if (isDigit(code[i]))
            {
                state = State::INNUM;
                token += code[i];
            }
            else if (code[i] == ':')
            {
                state = State::INASSIGN;
                token += code[i];
            }
            else if (code[i] == '+')
            {
                state = State::START;
                token += code[i];

                tokens.push_back(QPair<QString, QString>(token, "PLUS"));
            }
            else if (code[i] == '-')
            {
                state = State::START;
                token += code[i];
                tokens.push_back(QPair<QString, QString>(token, "MINUS"));

            }
            else if (code[i] == '*')
            {
                state = State::START;
                token += code[i];
                tokens.push_back(QPair<QString, QString>(token, "MULT"));

            }
            else if (code[i] == '/')
            {
                state = State::START;
                token += code[i];
                tokens.push_back(QPair<QString, QString>(token, "DIV"));

            }
            else if (code[i] == '=')
            {
                state = State::START;
                token += code[i];
                tokens.push_back(QPair<QString, QString>(token, "EQUAL"));

            }
            else if (code[i] == '<')
            {
                state = State::START;
                token += code[i];
                tokens.push_back(QPair<QString, QString>(token, "LESSTHAN"));

            }
            else if (code[i] == '>')
            {
                state = State::START;
                token += code[i];
                tokens.push_back(QPair<QString, QString>("Symbol: "+token, "greater than"));
            }

            else if (code[i] == ';')
            {
                state = State::START;
                token += code[i];
                tokens.push_back(QPair<QString, QString>(token, "SEMICOLON"));
            }
            else if(code[i]=='(')
            {
                para.push_back(code[i]);
                state = State::START;
                token += code[i];
                tokens.push_back(QPair<QString, QString>(token, "OPENBRACKET"));
            }
            else if(code[i]==')')
            {
                if (!para.empty())
                {
                    QChar c = para.back();
                    if (c == '(')
                    {
                        para.pop_back();
                        state = State::START;
                        token += code[i];
                        tokens.push_back(QPair<QString, QString>(token, "CLOSEDBRACKET"));
                    }
                    else
                    {
                        state = State::ERROR;
                        error_msg = "Parenthesis not closed.";
                    }
                }
                else
                {
                    state = State::ERROR;
                    error_msg = "Parenthesis not opened.";
                }
            }
            else if (code[i] == ' ' || code[i] == '\n');
            else
            {
                state = State::ERROR;
                error_msg = "Invalid Expression.";
            }
            break;

        case State::INCOMMENT:
            if (code[i] == '}')
                state = State::START;
            else if (code[i] == '{')
            {
                state = State::ERROR;
                error_msg = "Comments cannot be nested.";
            }
            break;

        case State::INID:
            if (isLetter(code[i]))
            {
                state = State::INID;
                token += code[i];
            }
            else if (isSymbol(code[i]) || code[i] == ' ' || code[i] == ':' || code[i] == '\n'||code[i]=='['||code[i]==']')
            {

                state = State::START;
                if (isTokenReservedWord(token))
                {
                    tokens.push_back(QPair<QString, QString>(token,token.toUpper()));
                }
                else
                {
                    tokens.push_back(QPair<QString, QString>(token,"IDENTIFIER"));
                }
                i--;

            }
            else
            {
                state = State::ERROR;
                error_msg = "Indefntifiers cannot have digits or special characters";
            }
            break;

        case State::INNUM:
            if (isDigit(code[i]))
            {
                state = State::INNUM;
                token += code[i];
            }
            else if (isSymbol(code[i]) || code[i] == ' ' || code[i] == ':' || code[i] == '\n')
            {
                state = State::START;
                tokens.push_back(QPair<QString, QString>(token,"NUMBER "));

                i--;
            }
            else
            {
                state = State::ERROR;
                error_msg = "Numbers cannot have letters or special characters";
            }
            break;

        case State::INASSIGN:
            if (code[i] == '=')
            {
                token += code[i];
                state = State::START;
                tokens.push_back(QPair<QString, QString>(token, "ASSIGN"));
            }
            else
            {
                state = State::ERROR;
                error_msg = "Invalid Token.";
            }
            break;
        case State::ERROR:
            return false;

        }
    }

    if (!para.empty())
    {
        while (!para.empty())
        {
            para.pop_back();
        }
        error_msg = "Brackets dont match.";
        return false;
    }

    if (state == State::INCOMMENT)
    {
        error_msg = "Closing curly bracket not found.";
        return false;
    }

    else
        return true;
}
bool TinyLanguageScanner::parser()
{
    prepareinput();
    prepareDraw();
    m=0;
    root = program();
    root->x = -300;
    root->y = -300;
    assign_coordinates(root, root->x, root->y);
    DrawLines();
    syntaxTreeDraw(root);
    text2words.clear();
    return rightInput&&!error;

}
void TinyLanguageScanner::on_scannerButton_clicked()
{
     inputText=ui->code->toPlainText();

    bool scan=scanner( inputText);
    if( inputText=="")
    {
        QMessageBox::warning(this,"","Error:No code found");

    }
    else
    {
     text="";
    if(scan==true)
    {
        for (auto i = 0; i < tokens.size(); i++)
        {
            text += tokens[i].first + " , " + tokens[i].second + "\n";
        }
        ui->code->setPlainText(text);

    }
    else
    {
        text=error_msg;
        ui->code->setPlainText(text);

    }
    }
}
void TinyLanguageScanner::on_parserButton_clicked()
{
    error=false;

    if(parser()==true)
    {

        QMessageBox::warning(this,"","vaild Grammar");
        syntaxTree->show();
    }
    else
        if(rightInput==true)
        QMessageBox::warning(this,"","Invaild Grammar");


}
