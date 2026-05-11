#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    background.load("D:/Qt_code/UniverseDemo/star.png");
    qDebug() << background.isNull();
    resize(1200, 700);    //窗口变宽
    setAutoFillBackground(false);
    setAttribute(Qt::WA_OpaquePaintEvent);//⭐粒子运动尾迹
    for(int i = 0; i < 40; i++)
    {
        int x = QRandomGenerator::global()->bounded(width());
        int y = QRandomGenerator::global()->bounded(height());

        Civilization c;
        c.pos = QPoint(x, y);
        c.speed = QRandomGenerator::global()->bounded(1, 6); // 1~5
        c.life = QRandomGenerator::global()->bounded(50, 150);// ⭐生命值初始化
        c.level = QRandomGenerator::global()->bounded(1, 4);// ⭐ 文明等级初始化
        // 🌍 文明属性初始化
        c.population = QRandomGenerator::global()->bounded(500, 5000);

        c.technology = QRandomGenerator::global()->bounded(1, 6);

        c.energy = QRandomGenerator::global()->bounded(50, 150);

        c.age = 0;

        c.stability = QRandomGenerator::global()->bounded(40, 100);

        c.visibility = QRandomGenerator::global()->bounded(10, 80);

        c.aggression = QRandomGenerator::global()->bounded(0, 100);

        c.military = QRandomGenerator::global()->bounded(20, 100);

        civilizations.append(c);
    }

    // =========================
    // 🌍 创建资源星球
    // =========================

    for (int i = 0; i < 8; i++)
    {
        ResourcePlanet p;

        p.pos = QPoint(
            QRandomGenerator::global()->bounded(width() - 300),
            QRandomGenerator::global()->bounded(height())
            );

        p.energy =
            QRandomGenerator::global()->bounded(100, 300);

        planets.append(p);
    }

    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, [=]()
            {
                // 每一帧更新文明
        for (int i = 0; i < civilizations.size(); i++)

        {
             Civilization &c = civilizations[i];

            // =========================
            // 🌍 文明AI移动系统
            // =========================

            // 默认随机移动
            int dx =
                QRandomGenerator::global()
                    ->bounded(2 * c.speed + 1)
                - c.speed;

            int dy =
                QRandomGenerator::global()
                    ->bounded(2 * c.speed + 1)
                - c.speed;

            // =========================
            // 🌌 寻找最近资源星球
            // =========================

            int nearestDist = 999999;

            QPoint targetPos = c.pos;

            for (const ResourcePlanet &p : planets)
            {
                if (p.energy <= 0) continue;

                int ddx = p.pos.x() - c.pos.x();
                int ddy = p.pos.y() - c.pos.y();

                int dist2 = ddx * ddx + ddy * ddy;

                if (dist2 < nearestDist)
                {
                    nearestDist = dist2;

                    targetPos = p.pos;
                }
            }

            // =========================
            // ⭐ 能源不足时主动靠近资源
            // =========================

            if (c.energy < 60 ||
                c.technology > 6)
            {
                if (targetPos.x() > c.pos.x())
                {
                    dx += 1;
                }
                else
                {
                    dx -= 1;
                }

                if (targetPos.y() > c.pos.y())
                {
                    dy += 1;
                }
                else
                {
                    dy -= 1;
                }
            }

            // =========================
            // 🚀 移动
            // =========================

            // =========================
            // ⚠️ 弱文明逃离强文明
            // =========================

            for (const Civilization &other : civilizations)
            {
                if (&other == &c) continue;

                if (other.life <= 0) continue;

                // 对方比自己强很多
                if (other.military > c.military + 50)
                {
                    int ddx = other.pos.x() - c.pos.x();
                    int ddy = other.pos.y() - c.pos.y();

                    int dist2 = ddx * ddx + ddy * ddy;

                    // 距离较近
                    if (dist2 < 10000)
                    {
                        // 反方向逃跑
                        if (ddx > 0)
                        {
                            dx -= 2;
                        }
                        else
                        {
                            dx += 2;
                        }

                        if (ddy > 0)
                        {
                            dy -= 2;
                        }
                        else
                        {
                            dy += 2;
                        }
                    }
                }
            }

            // =========================
            // ⚔️ 高侵略文明主动追击弱文明
            // =========================

            if (c.aggression > 70)
            {
                for (const Civilization &other : civilizations)
                {
                    if (&other == &c) continue;

                    if (other.life <= 0) continue;

                    // 对方比自己弱
                    if (other.military < c.military)
                    {
                        int ddx =
                            other.pos.x() - c.pos.x();

                        int ddy =
                            other.pos.y() - c.pos.y();

                        int dist2 =
                            ddx * ddx + ddy * ddy;

                        if (dist2 < 40000)
                        {
                            // 主动靠近
                            if (ddx > 0)
                            {
                                dx += 1;
                            }
                            else
                            {
                                dx -= 1;
                            }

                            if (ddy > 0)
                            {
                                dy += 1;
                            }
                            else
                            {
                                dy -= 1;
                            }
                        }
                    }
                }
            }

            c.pos += QPoint(dx, dy);
            // 🌍 移动消耗能源
            c.energy -= 1;

            // 🌍 缓慢获取能源
            if (QRandomGenerator::global()->bounded(5) == 0)
            {
                c.energy++;

                if (c.energy > 150)
                {
                    c.energy = 150;
                }
            }

            // 2️⃣ 边界
            int universeWidth = width() - 300; // 右侧300像素留给侧边栏

            if (c.pos.x() < 0) c.pos.setX(0);
            if (c.pos.y() < 0) c.pos.setY(0);
            if (c.pos.x() > universeWidth) c.pos.setX(universeWidth);
            if (c.pos.y() > height()) c.pos.setY(height());

            // 3️⃣ 生命衰减（必须在演化前）
             if (QRandomGenerator::global()->bounded(3) == 0)
             {
                // 🌍 能源不足会导致生命下降
                if (c.energy <= 0)
                {
                    c.life -= 2;
                }
                else
                {
                    // 正常缓慢衰老
                    if (QRandomGenerator::global()->bounded(4) == 0)
                    {
                        c.life--;
                    }
                }
             }

            // 4️⃣ 演化逻辑
            if (c.life > 100 && c.level < 5)
            {
                c.level++;
                c.life -= 30;
            }

            if (c.life < 30 && c.level > 1)
            {
                c.level--;
            }

            // 5️⃣ 死亡处理（先标记，不删除）
            if (c.life <= 0)
            {
                c.life = 0;
            }

            if (c.pos.x() < 0) c.pos.setX(0);
            if (c.pos.y() < 0) c.pos.setY(0);
            if (c.pos.x() > width()) c.pos.setX(width());
            if (c.pos.y() > height()) c.pos.setY(height());

            // 🌍 文明年龄增长
            c.age++;
            // =========================
            // 🌱 文明自然恢复
            // =========================

            if (c.energy > 80 &&
                c.stability > 60)
            {
                if (QRandomGenerator::global()->bounded(5) == 0)
                {
                    c.life++;

                    if (c.life > 150)
                    {
                        c.life = 150;
                    }
                }
            }

            // 🌟 科技发展
            if (c.age % 200 == 0)
            {
                c.technology++;

                if (c.technology > 10)
                {
                    c.technology = 10;
                }
            }

            // ⚔️ 科技促进军事实力
            if (QRandomGenerator::global()->bounded(20) == 0)
            {
                c.military += c.technology / 2;

                if (c.military > 200)
                {
                    c.military = 200;
                }
            }

            // 🌑 隐藏文明更稳定
            if (c.visibility < 30)
            {
                c.stability += 1;

                if (c.stability > 100)
                {
                    c.stability = 100;
                }
            }
        }

        for (int i = 0; i < civilizations.size(); i++)
        {
            Civilization &c = civilizations[i];  // ⭐⭐⭐ 关键：定义 c
            for (int j = i + 1; j < civilizations.size(); j++)
            {
                Civilization &a = civilizations[i];
                Civilization &b = civilizations[j];

                if (a.life <= 0 || b.life <= 0) continue;

                int dx = a.pos.x() - b.pos.x();
                int dy = a.pos.y() - b.pos.y();

                int dist2 = dx * dx + dy * dy;


                // =========================
                // 🌌 探测范围（科技越高越远）
                // =========================

                int detectRangeA =
                    40 +
                    a.technology * 12;

                int detectRangeB =
                    40 +
                    b.technology * 12;

                // 平方距离
                int detectA2 = detectRangeA * detectRangeA;
                int detectB2 = detectRangeB * detectRangeB;

                // =========================
                // 🔭 是否发现对方
                // =========================

                bool aFindsB = false;
                bool bFindsA = false;

                // 暴露度越高越容易被发现
                if (dist2 < detectA2)
                {
                    int chance =
                        b.visibility +
                        a.technology * 3;

                    if (QRandomGenerator::global()->bounded(100)
                        < chance)
                    {
                        aFindsB = true;
                    }
                }

                if (dist2 < detectB2)
                {
                    int chance =
                        a.visibility +
                        b.technology * 3;

                    if (QRandomGenerator::global()->bounded(100)
                        < chance)
                    {
                        bFindsA = true;
                    }
                }

                // =========================
                // ⚔️ 只有发现后才战争
                // =========================

                if (aFindsB || bFindsA)
                {
                    // 🌌 侵略倾向判定
                    int attackChance =
                        (a.aggression + b.aggression
                         - a.stability / 3
                         - b.stability / 3);

                    if (QRandomGenerator::global()->bounded(100)
                        < attackChance)
                    {
                    // =========================
                    // ⚔️ 文明战力计算
                    // =========================

                    int powerA =
                        a.technology * 5 +
                        a.military * 3 +
                        a.energy +
                        a.stability;

                    int powerB =
                        b.technology * 5 +
                        b.military * 3 +
                        b.energy +
                        b.stability;

                    // =========================
                    // 🌌 战争结果
                    // =========================

                    if (powerA > powerB)
                    {
                        // a 获胜

                        int damage = (powerA - powerB) /20;

                        if (damage < 2) damage = 2;

                        b.life -= damage;

                        // 🌍 战争消耗
                        a.energy -= 3;
                        a.military -= 1;
                        a.stability -= 2;

                        // 🌟 胜利收益
                        a.energy += 5;
                    }
                    else
                    {
                        // b 获胜

                        int damage = (powerB - powerA) / 10;

                        if (damage < 5) damage = 5;

                        a.life -= damage;

                        b.energy -= 3;
                        b.military -= 1;
                        b.stability -= 2;

                        b.energy += 5;
                    }
                    }
                }
            }

            if (c.life > 120 && QRandomGenerator::global()->bounded(1000) < 5)
            {
                Civilization child;

                // 🌍 在父文明附近出生
                child.pos = c.pos + QPoint(
                                QRandomGenerator::global()->bounded(21) - 10,
                                QRandomGenerator::global()->bounded(21) - 10
                                );

                child.speed = QRandomGenerator::global()->bounded(1, 6);
                child.life = 80;
                child.level = c.level;

                civilizations.append(child);
            }

            // =========================
            // 🌍 文明采集资源
            // =========================

            for (ResourcePlanet &p : planets)
            {
                int dx = c.pos.x() - p.pos.x();
                int dy = c.pos.y() - p.pos.y();

                int dist2 = dx * dx + dy * dy;

                // ⭐ 接近资源星球
                if (dist2 < 1600)
                {
                    // 🌟 获取资源
                    if (p.energy > 0)
                    {
                        c.energy += 2;

                        p.energy -= 1;

                        // 科技缓慢提升
                        if (QRandomGenerator::global()->bounded(20) == 0)
                        {
                            c.technology++;
                        }

                        // 限制科技上限
                        if (c.technology > 10)
                        {
                            c.technology = 10;
                        }
                    }
                }
            }
            // =========================
            // 🌍 资源星球恢复
            // =========================

            for (ResourcePlanet &p : planets)
            {
                if (QRandomGenerator::global()->bounded(10) == 0)
                {
                    p.energy++;

                    if (p.energy > 300)
                    {
                        p.energy = 300;
                    }
                }
            }

        }

        // ⭐⭐⭐ 关键：倒序删除死亡文明
        for (int i = civilizations.size() - 1; i >= 0; i--)
        {
            if (civilizations[i].life <= 0)
            {
                // ⭐ 如果删的是当前选中
                if (i == selectedIndex)
                {
                    selectedIndex = -1;
                }
                // ⭐ 如果删的是前面的元素，要调整索引
                else if (i < selectedIndex)
                {
                    selectedIndex--;
                }

                civilizations.remove(i);
            }
        }
        // ⭐ 记录当前文明数量
        history.append(civilizations.size());

        // =========================
        // 🌌 宇宙自然诞生文明
        // =========================

        if (civilizations.size() < 15)
        {
            if (QRandomGenerator::global()->bounded(100) < 5)
            {
                Civilization c;

                c.pos = QPoint(
                    QRandomGenerator::global()->bounded(width() - 300),
                    QRandomGenerator::global()->bounded(height())
                    );

                c.speed = 2;
                c.life = 100;
                c.level = 1;

                c.population = 1000;
                c.technology = 1;
                c.energy = 100;
                c.age = 0;
                c.stability = 70;
                c.visibility = 30;
                c.aggression = 20;
                c.military = 30;

                civilizations.append(c);
            }
        }

        // ⭐ 防止无限增长（最多保存300帧）
        if (history.size() > 300)
        {
            history.remove(0);
        }
        update();
            });

    timer->start(100); // 每100毫秒更新一次
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // 🌌 绘制星空背景
    painter.drawPixmap(rect(), background);

    // =========================
    // 🌍 绘制资源星球
    // =========================

    for (const ResourcePlanet &p : planets)
    {
        // ⭐ 资源星球颜色
        if (p.energy > 50)
        {
            painter.setBrush(QColor(0, 150, 255));
        }
        else
        {
            painter.setBrush(QColor(80, 80, 80));
        }

        // ⭐ 根据资源量决定大小
        int radius = p.energy / 40;

        if (radius < 6) radius = 6;
        if (radius > 15) radius = 15;

        painter.drawEllipse(p.pos, radius, radius);
    }

    //  // 背景拖尾
    // painter.setBrush(QColor(0, 0, 0,60)); // 黑色 + 透明度
    // painter.setPen(Qt::NoPen);
    // painter.drawRect(rect());

    // 🌌 绘制右侧信息栏背景
    painter.setBrush(QColor(20, 20, 30, 220));
    painter.setPen(Qt::NoPen);
    painter.drawRect(width() - 300, 0, 300, height());

    //绘制侧边栏标题
    painter.setPen(Qt::white);

    QFont titleFont;
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    painter.setFont(titleFont);

    painter.drawText(width() - 280, 40,
                     "Civilization Monitor");

    //显示实时文明数量
    QFont normalFont;
    normalFont.setPointSize(10);
    painter.setFont(normalFont);

    QString countText = QString("Civilizations: %1")
                            .arg(civilizations.size());

    painter.drawText(width() - 280, 90, countText);

    // 📈 在侧边栏绘制实时文明数量折线图
    int graphX = width() - 280;
    int graphY = 250;
    int graphHeight = 120;

    // 边框
    painter.setPen(Qt::gray);
    painter.drawRect(graphX, graphY - graphHeight,
                     240, graphHeight);

    // 折线
    painter.setPen(Qt::yellow);

    for (int i = 1; i < history.size(); i++)
    {
        int x1 = graphX + (i - 1);
        int y1 = graphY - history[i - 1] * 2;

        int x2 = graphX + i;
        int y2 = graphY - history[i] * 2;

        painter.drawLine(x1, y1, x2, y2);
    }

    painter.setPen(Qt::white);
    painter.drawText(graphX, graphY + 20,
                     "Population History");

    // 画文明
    for (int i = 0; i < civilizations.size(); i++)
    {
        const Civilization &c = civilizations[i];

        if (c.life <= 0) continue;

        // 🌍 根据能源决定亮度
        int alpha =
            c.energy +
            c.visibility +
            20;

        if (alpha > 255) alpha = 255;
        if (alpha < 50) alpha = 50;

        // 🌈 根据等级设置颜色
        switch (c.level)
        {
        case 1:
            painter.setBrush(QColor(150, 150, 150, alpha));
            break;

        case 2:
            painter.setBrush(QColor(0, 255, 0, alpha));
            break;

        case 3:
            painter.setBrush(QColor(255, 0, 0, alpha));
            break;

        default:
            painter.setBrush(QColor(255, 255, 255, alpha));
        }

        // 🔵 根据生命值决定大小
        int radius = c.life / 12;
        if (radius < 4) radius = 4;
        if (radius > 12) radius = 12;

        painter.drawEllipse(c.pos, radius, radius);
    }

    // 点击显示信息
    if (selectedIndex != -1 && selectedIndex < civilizations.size())
    {
        const Civilization &c = civilizations[selectedIndex];

        painter.setPen(Qt::white);

        QFont infoFont;
        infoFont.setPointSize(10);
        painter.setFont(infoFont);

        int infoX = width() - 280;
        int infoY = 340;

        // =========================
        // 🌌 标题
        // =========================

        painter.drawText(infoX, infoY,
                         "Selected Civilization");

        // =========================
        // 🌍 基础信息
        // =========================

        painter.drawText(infoX, infoY + 40,
                         QString("Population: %1")
                             .arg(c.population));

        painter.drawText(infoX, infoY + 70,
                         QString("Technology: %1")
                             .arg(c.technology));

        painter.drawText(infoX, infoY + 100,
                         QString("Energy: %1")
                             .arg(c.energy));

        painter.drawText(infoX, infoY + 130,
                         QString("Age: %1")
                             .arg(c.age));

        // =========================
        // ⚖️ 社会属性
        // =========================

        painter.drawText(infoX, infoY + 180,
                         QString("Stability: %1")
                             .arg(c.stability));

        painter.drawText(infoX, infoY + 210,
                         QString("Visibility: %1")
                             .arg(c.visibility));

        painter.drawText(infoX, infoY + 240,
                         QString("Aggression: %1")
                             .arg(c.aggression));

        // =========================
        // ⚔️ 军事属性
        // =========================

        painter.drawText(infoX, infoY + 290,
                         QString("Military: %1")
                             .arg(c.military));

        painter.drawText(infoX, infoY + 320,
                         QString("Life: %1")
                             .arg(c.life));

        painter.drawText(infoX, infoY + 350,
                         QString("Speed: %1")
                             .arg(c.speed));

        painter.drawText(infoX, infoY + 380,
                         QString("Level: %1")
                             .arg(c.level));
    }


}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint clickPos = event->pos();

    selectedIndex = -1;  // 先清空选择

    for (int i = 0; i < civilizations.size(); i++)
    {
        Civilization &c = civilizations[i];

        int dx = c.pos.x() - clickPos.x();
        int dy = c.pos.y() - clickPos.y();

        int dist2 = dx * dx + dy * dy;

        // ⭐ 点击范围（20像素）
        if (dist2 <225)
        {
            selectedIndex = i;
            break;
        }
    }

    update();  // 触发重绘
}