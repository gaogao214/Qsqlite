#ifndef MYSQL_H
#define MYSQL_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QTime>
#include <QFuture>
#include <QtConcurrent>

QT_BEGIN_NAMESPACE
namespace Ui {
class mysql;
}
QT_END_NAMESPACE

class mysql : public QMainWindow
{
    Q_OBJECT

public:
    mysql(QWidget *parent = nullptr);
    ~mysql();

    void run();
public:
    bool connectSQL();

    void tablewidgetSQL();
private slots:
    void on_pushButtonInsert_clicked();

    void on_pushButtonSearch_clicked();

    void on_pushButtonDelete_clicked();

    void on_pushButtonSerachAll_clicked();

    void on_pushButtonUpdate_clicked();

private:
    Ui::mysql *ui;

    QSqlDatabase db;
    QSqlTableModel *model;

    bool m_loop = true;

    int m_updateStatusInterval = 5;
    int m_updateInterLockInterval = 2;
    QFuture<void> m_future;

    int count = 0;


};
#endif // MYSQL_H
