#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QVector>
#include <QPoint>
#include <QTimer>
#include <QPixmap>
#include <QCheckBox>
#include <QPushButton>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


  class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWindow *ui;
    struct Civilization
    {
        QPoint pos;

        // 基础属性
        int speed;
        int life;
        int level;

        // 🌍 新文明属性
        int population;     // 人口
        int technology;     // 科技等级
        int energy;         // 能源
        int age;            // 文明年龄
        int stability;      // 社会稳定度
        int visibility;     // 暴露度
        int aggression;     // 扩张欲 / 侵略性
        int military;       // 军事实力
        // 🌍 文明名称
        QString name;

        // 🌌 文明类型
        QString type;
        // 🌍 政治制度
        QString politics;
        QString faction;   // 阵营
        QVector<int> relations;   // 与其它文明关系值
    };

    struct ResourcePlanet
    {
        QPoint pos;

        int energy;      // 资源储量
    };

    QVector<Civilization> civilizations;
    QVector<int> history;   // ⭐ 文明数量历史
    QVector<ResourcePlanet> planets;
    int selectedIndex = -1;  // ⭐ 当前选中的文明
    int universeYear = 0;
    QString currentEra = "Ancient Era";
    // 🌌 纪元修正值
    float warModifier = 1.0f;
    float techModifier = 1.0f;
    float resourceModifier = 1.0f;
    QTimer *timer;
    QPixmap background;
    // 🌌 外交线控制按钮
    QCheckBox *allianceBox;
    QCheckBox *enemyBox;
    QCheckBox *neutralBox;
    // ⏸️ 暂停按钮
    QPushButton *pauseButton;

    // 🌌 是否暂停
    bool isPaused = false;
    bool showAlliance = true;
    bool showEnemy = true;
    bool showNeutral = false;

    bool focusSelectedRelations = true;
    // =========================
    // ⚔️ 战争激光
    // =========================

    struct Laser
    {
        QPoint start;
        QPoint end;

        int life;   // 激光剩余时间
    };

    QVector<Laser> lasers;
    // =========================
    // 🌌 文明事件日志
    // =========================

    QStringList eventLogs;
    // 🌌 聚焦模式（只显示选中文明关系）
    bool focusMode = false;
    // 🌌 导出文明记录
    void exportCivilizationReport();
};
#endif // MAINWINDOW_H