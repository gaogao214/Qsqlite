#include "mysql.h"
#include "ui_mysql.h"
#include <QTextCodec>
#include <QMessageBox>
#include <QSqlQuery>
#include <QtSql>
#include <QDebug>
#include <QSqlError>
#include <QSqlTableModel>

#define QSQLTABLEMODEL_FLAG

mysql::mysql(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::mysql)
{
    ui->setupUi(this);

    connectSQL();//连接数据库
    on_pushButtonSearch_clicked();//显示数据库到界面

}
void mysql::run()
{

    m_future = QtConcurrent::run([this] {

        while(m_loop) {
            QThread::msleep(100);


            m_updateInterLockInterval--;

            if (m_updateInterLockInterval == 0) {
                m_updateInterLockInterval = 2;

                qDebug()<<"m_updateInterLockInterval:"<<m_updateInterLockInterval<<"\n";
            }
            qDebug()<<"count:"<<count++<<" thread id:"<< QThread::currentThreadId() << QThread::currentThread();;
            m_updateStatusInterval--;
            if (m_updateStatusInterval == 0) {
                m_updateStatusInterval = 5;

                qDebug()<<"m_updateStatusInterval:"<<m_updateStatusInterval<<"\n";
            }
        }
    });
}

mysql::~mysql()
{
    delete ui;
    m_loop = false;
    // m_future.waitForFinished();
}

//连接数据库
bool mysql::connectSQL()
{
    //这条语句会打印:drivers ("QSQLITE", "QMYSQL", "QMYSQL3", "QODBC", "QODBC3", "QPSQL", "QPSQL7")本地的数据库
    qDebug() << "drivers" << QSqlDatabase::drivers();
    //======================================👇创建数据库==========================================================
    // 加载驱动：这里的驱动必须是上一句打印里面的完全一样，必须是大写，不然会报错：QSqlDatabase: QSQLITE driver not loaded
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QSqlQuery sql_query;

    db.setDatabaseName("./StudentDB.db"); // 建立链接（链接一个数据库文件）
    if(!db.open())
    {
        return false;
        qDebug()<<"Error: Failed to connect database." << db.lastError();
    }else
    {
          return true;
        qDebug() << "Succeed to connect database." ;
    }
}

//读取数据库中的数据到tablewidget
void mysql::tablewidgetSQL()
{

#ifdef QSQLTABLEMODEL_FLAG

#elif
    //使用QSqlQUery方法读取
    //============================连接数据库之前判断数据库是否已经连接=============================
    QSqlDatabase db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
        db = QSqlDatabase::database("qt_sql_default_connection");
    else
        db = QSqlDatabase::addDatabase("QSQLITE");
    //============================连接数据库之前判断数据库是否已经连接=============================

    //============================打开数据库===============================
    db.setDatabaseName("./StudentDB.db");
    db.open();
    if(!db.open())
    {
        qDebug()<<"open the file fail";
    }
    //============================打开数据库===============================
    //==========================查找数据库中的表============================
    QString tableName = "students";
    QSqlQuery query;
    query.exec(QString("select * from %1").arg(tableName));
    //==========================查找数据库中的表============================
    //设置tablewidget的表头项
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels({u8"id",u8"名字",u8"成绩",u8"班级"});
    int row = 0;
    while(query.next())
    {
        ui->tableWidget->setRowCount(row+1);
        for(int column = 0; column < query.record().count(); column++)
        {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(column).toString());
            qDebug()<<"item:"<<item->text()<<" row :"<<row<<"  cloumn:"<<column;
            ui->tableWidget->setItem(row,column,item);
        }
        row++;
    }
#endif
}

//往数据库中插入数据
void mysql::on_pushButtonInsert_clicked()
{

    if(!connectSQL())
        return;
    //========================================👆创建数据库========================================================


    QStringList names;
    names<<u8"小A"<<u8"小B"<<u8"小C"<<u8"小D"<<u8"小E"<<u8"小F"<<u8"小G"
          <<u8"小H"<<u8"小I"<<u8"小G"<<u8"小K"<<u8"小L"<<u8"小M"<<u8"小N";

    QStringList clases;
    clases<<u8"初1-1班"<<u8"初1-2班"<<u8"初1-3班"<<u8"初2-1班"
           <<u8"初2-2班"<<u8"初2-3班"<<u8"初3-1班"<<u8"初3-2班"<<u8"初3-3班";
    //========================================👇清空数据库－》创建student表========================================================
    QSqlQuery query;
    QSqlQuery sql_query;
    query.exec("DROP TABLE students");        //先清空一下表

    //创建一个students表,标题分别为id、name、score、class
    query.exec(u8"CREATE TABLE students ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "名字 VARCHAR(40) NOT NULL, "
               " 成绩 INTEGER NOT NULL, "
               "班级 VARCHAR(40) NOT NULL)");
    //创建一个students表
    //========================================👆清空数据库－》创建student表========================================================

    //========================================👇插入表=======================================================================
    query.prepare(u8"INSERT INTO students (名字, 成绩,班级) "
                  "VALUES (:name, :score, :class)");
    //为每一列标题添加绑定值

    foreach (QString name, names)       //从names表里获取每个名字
    {
        query.bindValue(":name", name);                      //向绑定值里加入名字
        query.bindValue(":score", (qrand() % 101));      //成绩
        query.bindValue(":class", clases[qrand()%clases.length()] );    //班级
        query.exec();               //加入库中
    }
    //=========================================👆插入表=======================================================================

    //========================================👇查询表=======================================================================
    //查询
    // sql_query.exec("select * from students;");
    // if(!sql_query.exec())
    // {
    //     qDebug()<<sql_query.lastError();
    // }
    // else
    // {
    //     while(sql_query.next())
    //     {
    //         int id = sql_query.value(0).toInt();
    //         QString name = sql_query.value(1).toString();
    //         int age = sql_query.value(2).toInt();

    //         qDebug()<<"ID = "<<id<<" ; NAME = "<<name <<" ; AGE = "<<age;
    //     }
    // }
    //========================================👆查询表=======================================================================

    // db.close();


}

//查询全部数据
void mysql::on_pushButtonSearch_clicked()
{
#ifdef QSQLTABLEMODEL_FLAG

    //使用QSqlTableModel 方法
    model = new QSqlTableModel(this);
    model->setTable("students");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);


    //不显示第二列
    //model->removeColumn(1);

    //查询整张表
    model->select();

    ui->tableView->setModel(model);
#elif
     //使用QSqlQuery 方法
    QSqlQuery sql_query;
    // qDebug()<<tr("database connection name:%1").arg(db.connectionName());
    if(QSqlDatabase::contains("qt_sql_default_connection"))
    {
        db = QSqlDatabase::database("qt_sql_default_connection");
        qDebug()<<u8"存在";
        //查询
        sql_query.exec("select * from students;");

        if(!sql_query.exec())
        {
            qDebug()<<sql_query.lastError();
        }
        else
        {
            while(sql_query.next())
            {
                int id = sql_query.value(0).toInt();
                QString name = sql_query.value(1).toString();
                int age = sql_query.value(2).toInt();

                qDebug()<<"ID = "<<id<<" ; NAME = "<<name <<" ; AGE = "<<age;
            }
        }
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }

#endif
}


void mysql::on_pushButtonDelete_clicked()
{
#ifdef QSQLTABLEMODEL_FLAG
    //使用QSqlTableModel 方法
    int curRow = ui->tableView->currentIndex().row();
    model->removeRow(curRow);
    int ok = QMessageBox::warning(this,tr("删除当前行！"),
                                  tr("确定删除当前行吗"),
                                  QMessageBox::Yes,QMessageBox::No);
    if(ok == QMessageBox::No)
        model->revertAll(); //如果不删除，则撤销
    else
        model->submitAll(); //否则提交，在数据库中删除改行

#elif
     //使用QSqlQuery方法
    if(QSqlDatabase::contains("qt_sql_default_connection"))
    {
        db = QSqlDatabase::database("qt_sql_default_connection");
        // QString id = ui->tableWidget->item(m_row, 0)->text();
        // QString name = ui->tableWidget->item(m_row, 1)->text();

        QSqlQuery query(db);

        if(query.exec(QString("delete from students where  id='%1'")//删除所在行
                           .arg("1")))
        {
            qDebug("delete data success!");
        }
        else
        {
            qDebug()<<"error"<<query.lastError();
        }
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }

#endif
}

//修改数据
void mysql::on_pushButtonUpdate_clicked()
{
#ifdef QSQLTABLEMODEL_FLAG
    //使用QSqlTableModel 方法
    QString goodsName = ui->lineEditName->text();
    // QDate currentDate = QDateTime::currentDateTime().date();
    // QString goodsDate = currentDate.toString("yyyy-MM-dd");
    float goodsPrice = (ui->lineEditScore->text()).toFloat();
    int goodsNum = (ui->lineEditNumber->text()).toInt();
    float tatal = goodsPrice * goodsNum;
    QString goodsTotal = QString::number(tatal, 'f', 2);
    if(goodsName.isEmpty() || tatal <= 0)
    {
        QMessageBox::information(this,"提示","修改失败,数据为空");
        return;
    }

    int curRow = ui->tableView->currentIndex().row();
    QSqlRecord record = model->record(curRow);
    record.setValue(0,goodsName);
    // record.setValue(1,goodsDate);
    record.setValue(1,goodsPrice);
    record.setValue(2,goodsNum);
    // record.setValue(4,tatal);


    if(model->setRecord(curRow, record))
    {
        model->submitAll();
    }
#elif
    //使用QSqlQuery方法
    //更新数据，将班级名称作为限制条件进行数据更新
    // QString queryString = QString("update class set class_teacher='%1' ,student_number=%2 where class_name='%3' ")
    //                   .arg("张主任").arg(30).arg("一班");

    qDebug()<<"on_pushButtonUpdate_clicked";
    if(QSqlDatabase::contains("qt_sql_default_connection"))
    {
        db = QSqlDatabase::database("qt_sql_default_connection");
        // QString id = ui->tableWidget->item(m_row, 0)->text();
        // QString name = ui->tableWidget->item(m_row, 1)->text();

        QSqlQuery query(db);
        if(query.exec(QString("update students set name='%1' where id=%2")
                           .arg("小I").arg("2")))
        {
            qDebug("update data success");
        }
        else
        {
            qDebug()<<"error"<<query.lastError();
        }
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }

#endif
}

