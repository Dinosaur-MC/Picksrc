#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 设置窗口标题
    setWindowTitle("Picksrc - v1.0");

    // 禁用输出框的自动格式化和富文本
    ui->textEdit->setAutoFormatting(QTextEdit::AutoNone);
    ui->textEdit->setAcceptRichText(false);

    // 创建 查找结果数量控件 并显示在状态栏
    count = new QLabel(this);
    ui->statusbar->addWidget(count);
    author = new QLabel("<a href=\"https://github.com/Dinosaur-MC/Picksrc.git\">Made by [Github]Dinosaur-MC", this);
    author->setOpenExternalLinks(true);
    ui->statusbar->addPermanentWidget(author);

    // 点击 Research 按钮开始查找
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::check);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete count;
    delete author;
}


void MainWindow::check()    // 检查查找表单是否填写好
{
    // 若文件路径为空，不执行查找
    if(ui->leFile->text().isEmpty())
        return;
    // 若没有关键字也没有前边界，也不执行查找
    if(ui->leKeyword->text().isEmpty() && ui->leEdge->text().isEmpty())
        return;

    // 通过所有检查后执行查找
    search();
}

void MainWindow::search()   // 查找函数，每找到一项后保存该项内容并删除当前所有已找过的内容，再继续下一项的查找，直到找不到符合条件的项
{
    // 预处理文件路径，删除路径首尾的双引号（若有）
    QString filename = ui->leFile->text();
    if(filename[0] == '"' && filename[filename.count()-1] == '"')
    {
        filename.chop(1);
        filename.remove(0, 1);
    }
    qDebug() << "File Name:" << filename;

    // 以只读的文本模式打开文件
    QFile fp(filename);
    if(!fp.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        qDebug() << "[Error] Can not open this file.";
        ui->textEdit->setText(QTime::currentTime().toString("hh:mm:ss ") + "[Error] The file is not exist.");
        return;
    }

    // 读取文件中的所有内容
    QString str;
    str = fp.readAll();

    // 获取前、后边界
    QString edge = ui->leEdge->text();
    QString edge2 = ui->leEdge_2->text();

    // 开始查找内容
    QStringList strList = {};
    while(!str.isEmpty())
    {
        // 存在关键词时查找关键词
        if(!ui->leKeyword->text().isEmpty())
        {
            int p = str.indexOf(ui->leKeyword->text(), 0, Qt::CaseSensitive);
            qDebug() << "indexs:" << p << p + ui->leKeyword->text().count();

            if(p == -1)
                break;
            str.remove(0, p + ui->leKeyword->text().count());
        }

        //存在前边界时查找前边界
        if(!edge.isEmpty())
        {
            int ed = str.indexOf(edge, 0, Qt::CaseSensitive);
            qDebug() << "[0] edge index:" << ed;

            if(ed == -1)
                break;

            str.remove(0, ed + edge.count());
        }

        QString temp;
        int a;
        // 存在后边界时查找后边界，否者查找换行符'\n'，然后拷贝A到B之间的内容（A = 前边界、关键词，B = 后边界、换行符）
        if(!edge2.isEmpty())
        {
            int ed = str.indexOf(edge2, 0, Qt::CaseSensitive);
            for(a = 0; a < str.count() && a != ed ; a++)
            {
                if(!str[a].isNull())
                    temp.append(str[a]);
            }
        }
        else
        {
            for(a = 0; a < str.count() && str[a] != '\n' ; a++)
            {
                if(!str[a].isNull())
                    temp.append(str[a]);
            }
        }

        // 若选中Simplify复选框，则简化输出内容，即将字符串中所有的换行符、制表符和连续的空格等替换为一个空格（首尾的则直接删除）
        if(ui->cb_Simplify->isChecked())
        {
            temp = temp.simplified();
        }
        // 若选中Keep Edge复选框，则将前、后边界补充回输出内容中（若有）
        if(ui->cb_KE->isChecked())
        {
            if(!edge.isEmpty())
                temp.prepend(edge);
            if(!edge2.isEmpty())
                temp.append(edge2);
        }
        qDebug() << "TEMP: " << temp;
        // 将单项的输出结果存储到一个列表中
        strList.append(temp);
        str.remove(0, a);
    }

    qDebug() << "Searching finished";

    // 显示输出结果
    if(strList.count() == 0)
    {
        qDebug() << "Nothing found.";
        ui->textEdit->setText(QTime::currentTime().toString("hh:mm:ss ") + "[Result] Nothing found.");
        count->setText("");
    }
    else
    {
        qDebug() << "Found " << strList.count() << " result(s)";
        ui->textEdit->setTextColor(Qt::blue);
        ui->textEdit->setText("[Result]\n");
        ui->textEdit->setTextColor(Qt::black);
        for(int i = 0; i < strList.count() ; i++)
        {
            // 明暗条纹
            if(i%2)
                ui->textEdit->setTextBackgroundColor(QColor(255,255,255));
            else
                ui->textEdit->setTextBackgroundColor(QColor(240,240,240));

            ui->textEdit->append(strList[i]);
        }
        // 计数
        count->setText(QString("Result Count: ") + QString::number(strList.count()));
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 按下Esc键后退出程序
    if(event->key() == Qt::Key_Escape)
    {
        close();
    }
}

