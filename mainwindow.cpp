#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QTimer>
#include <QDebug>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // =========================
    // 🌌 外交线控制按钮
    // =========================

    allianceBox =
        new QCheckBox("Alliance", this);

    enemyBox =
        new QCheckBox("Enemy", this);

    neutralBox =
        new QCheckBox("Neutral", this);
    connect(allianceBox, &QCheckBox::toggled,
            this,
            [=](bool checked)
            {
                showAlliance = checked;
                update();
            });

    connect(enemyBox, &QCheckBox::toggled,
            this,
            [=](bool checked)
            {
                showEnemy = checked;
                update();
            });

    connect(neutralBox, &QCheckBox::toggled,
            this,
            [=](bool checked)
            {
                showNeutral = checked;
                update();
            });

    // 默认状态
    allianceBox->setChecked(false);
    enemyBox->setChecked(false);
    neutralBox->setChecked(false);

    // =========================
    // 🌌 按钮样式
    // =========================

    allianceBox->setStyleSheet(
        "QCheckBox { color: white; }"
        "QCheckBox::indicator { width: 15px; height: 15px; }"
        );

    enemyBox->setStyleSheet(
        "QCheckBox { color: white; }"
        "QCheckBox::indicator { width: 15px; height: 15px; }"
        );

    neutralBox->setStyleSheet(
        "QCheckBox { color: white; }"
        "QCheckBox::indicator { width: 15px; height: 15px; }"
        );
    // =========================
    // ⏸️ 暂停按钮
    // =========================

    pauseButton =
        new QPushButton("Pause", this);

    pauseButton->setGeometry(width() - 140,
                             height() - 50,
                             100,
                             32);

    pauseButton->setStyleSheet(
        "QPushButton {"
        "background-color: rgba(40,40,60,180);"
        "color: white;"
        "border: 1px solid gray;"
        "border-radius: 8px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: rgba(80,80,120,200);"
        "}"
        );
    connect(pauseButton,
            &QPushButton::clicked,
            this,
            [=]()
            {
                isPaused = !isPaused;

                if (isPaused)
                {
                    timer->stop();

                    // 🌌 自动导出文明报告
                    exportCivilizationReport();

                    pauseButton->setText("Resume");
                }
                else
                {
                    timer->start(100);

                    pauseButton->setText("Pause");
                }
            });
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

        // =========================
        // 🌍 文明名称生成
        // =========================

        QString prefixes[] =
            {
                "Nova",
                "Eden",
                "Orion",
                "Titan",
                "Astra",
                "Zenith",
                "Solar",
                "Nebula"
            };

        QString suffixes[] =
            {
                "Union",
                "Empire",
                "Alliance",
                "Federation",
                "Collective",
                "Dominion"
            };

        QString prefix =
            prefixes[
                QRandomGenerator::global()->bounded(8)
        ];

        QString suffix =
            suffixes[
                QRandomGenerator::global()->bounded(6)
        ];

        c.name = prefix + " " + suffix;

        // =========================
        // 🌌 文明类型
        // =========================

        if (c.aggression > 70)
        {
            c.type = "Aggressive";
        }
        else if (c.visibility < 30)
        {
            c.type = "Hidden";
        }
        else if (c.stability > 80)
        {
            c.type = "Peaceful";
        }
        else
        {
            c.type = "Balanced";
        }
        // =========================
        // 🏛️ 政治制度生成
        // =========================

        if (c.military > 120 &&
            c.aggression > 70)
        {
            c.politics =
                "Military Dictatorship";
        }
        else if (c.technology > 7)
        {
            c.politics =
                "Technocracy";
        }
        else if (c.stability > 80 &&
                 c.aggression < 30)
        {
            c.politics =
                "Peace Republic";
        }
        else if (c.visibility < 25)
        {
            c.politics =
                "Hidden Civilization";
        }
        else if (c.energy > 130)
        {
            c.politics =
                "Resource Empire";
        }
        else if (c.stability < 30)
        {
            c.politics =
                "Chaos Tribe";
        }
        else
        {
            c.politics =
                "Balanced Federation";
        }
        // =========================
        // 🌌 文明阵营
        // =========================

        QString factions[] =
            {
                "Blue Alliance",
                "Red Empire",
                "Green Union",
                "Golden Federation"
            };

        c.faction =
            factions[
                QRandomGenerator::global()->bounded(4)
        ];

        civilizations.append(c);
        // =========================
        // 🌌 初始化外交关系
        // =========================

        for (int i = 0; i < civilizations.size(); i++)
        {
            civilizations[i].relations.resize(
                civilizations.size());

            for (int j = 0; j < civilizations.size(); j++)
            {
                if (i == j)
                {
                    civilizations[i].relations[j] = 100;
                }
                else
                {
                    civilizations[i].relations[j] =
                        QRandomGenerator::global()
                            ->bounded(-30, 31);
                }
            }
        }
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
        // 🌌 宇宙年份增长
        universeYear++;
        // 🌌 宇宙纪元系统

        // 🌌 宇宙纪元系统

        if (universeYear < 500)
        {
            currentEra = "Ancient Era";

            warModifier = 0.5f;
            techModifier = 0.7f;
            resourceModifier = 1.2f;
        }
        else if (universeYear < 1500)
        {
            currentEra = "Expansion Era";

            warModifier = 1.0f;
            techModifier = 1.2f;
            resourceModifier = 1.0f;
        }
        else if (universeYear < 3000)
        {
            currentEra = "Galactic War Era";

            warModifier = 1.8f;
            techModifier = 1.0f;
            resourceModifier = 0.8f;
        }
        else
        {
            currentEra = "Dark Age";

            warModifier = 0.7f;
            techModifier = 0.5f;
            resourceModifier = 0.6f;
        }
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
            // 🌍 资源星球自动恢复
            // =========================

            for (ResourcePlanet &p : planets)
            {
                // 小概率恢复资源
                if (QRandomGenerator::global()->bounded(4) == 0)
                {
                    p.energy += int(2 * resourceModifier);

                    // 资源上限
                    if (p.energy > 400)
                    {
                        p.energy = 400;
                    }
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
            if (QRandomGenerator::global()->bounded(3) == 0)
            {
                c.energy--;
            }

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
            // 🌱 文明自然恢复系统
            // =========================

            if (c.energy > 80 &&
                c.stability > 60)
            {
                // 小概率恢复生命
                if (QRandomGenerator::global()->bounded(8) == 0)
                {
                    c.life++;

                    // 限制生命上限
                    if (c.life > 150)
                    {
                        c.life = 150;
                    }
                }
            }

            // 🌟 科技发展
            if (c.age % int(200 / techModifier) == 0)
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
            // =========================
            // 🏛️ 政治制度效果
            // =========================

            // ⚔️ 军政府
            if (c.politics == "Military Dictatorship")
            {
                // 更容易扩张
                c.aggression += 1;

                // 军事增长更快
                if (QRandomGenerator::global()->bounded(8) == 0)
                {
                    c.military += 1;
                }

                // 稳定度缓慢下降
                if (QRandomGenerator::global()->bounded(10) == 0)
                {
                    c.stability--;
                }
            }

            // 🧠 技术治国
            else if (c.politics == "Technocracy")
            {
                // 科技发展更快
                if (QRandomGenerator::global()->bounded(20) == 0)
                {
                    c.technology++;
                }

                // 更低侵略性
                if (c.aggression > 20)
                {
                    c.aggression--;
                }
            }

            // ☮️ 和平共和国
            else if (c.politics == "Peace Republic")
            {
                // 社会更稳定
                if (QRandomGenerator::global()->bounded(5) == 0)
                {
                    c.stability++;
                }

                // 缓慢恢复生命
                if (QRandomGenerator::global()->bounded(8) == 0)
                {
                    c.life++;
                }

                // 不喜欢战争
                if (c.aggression > 10)
                {
                    c.aggression--;
                }
            }

            // 🌑 隐藏文明
            else if (c.politics == "Hidden Civilization")
            {
                // 更难被发现
                if (c.visibility > 5)
                {
                    c.visibility--;
                }

                // 科技缓慢提升
                if (QRandomGenerator::global()->bounded(30) == 0)
                {
                    c.technology++;
                }
            }
            // =========================
            // 🌌 属性限制
            // =========================

            if (c.aggression > 100) c.aggression = 100;
            if (c.aggression < 0) c.aggression = 0;

            if (c.stability > 100) c.stability = 100;
            if (c.stability < 0) c.stability = 0;

            if (c.visibility > 100) c.visibility = 100;
            if (c.visibility < 0) c.visibility = 0;

            if (c.technology > 10) c.technology = 10;

            if (c.life > 200) c.life = 200;

            if (c.military > 300) c.military = 300;
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
                    // 防止关系数组越界
                    if (j >= a.relations.size() ||
                        i >= b.relations.size())
                    {
                        continue;
                    }

                    int relation =
                        a.relations[j];
                    // =========================
                    // 🌌 外交关系自然变化
                    // =========================

                    // 同阵营更容易友好
                    if (a.faction == b.faction)
                    {
                        a.relations[j] += 1;
                        b.relations[i] += 1;
                    }
                    else
                    {
                        // 不同阵营缓慢恶化
                        if (QRandomGenerator::global()->bounded(5) == 0)
                        {
                            a.relations[j] -= 1;
                            b.relations[i] -= 1;
                        }
                    }

                    // 和平文明更容易友善
                    if (a.type == "Peaceful" ||
                        b.type == "Peaceful")
                    {
                        a.relations[j] += 1;
                        b.relations[i] += 1;
                    }

                    // 高侵略文明更容易敌对
                    if (a.aggression > 70 ||
                        b.aggression > 70)
                    {
                        a.relations[j] -= 1;
                        b.relations[i] -= 1;
                    }
                    // =========================
                    // 🌌 限制关系值范围
                    // =========================

                    if (a.relations[j] > 100)
                    {
                        a.relations[j] = 100;
                    }

                    if (a.relations[j] < -100)
                    {
                        a.relations[j] = -100;
                    }

                    if (b.relations[i] > 100)
                    {
                        b.relations[i] = 100;
                    }

                    if (b.relations[i] < -100)
                    {
                        b.relations[i] = -100;
                    }

                    if (relation > 40)
                    {
                        attackChance -= 40;
                    }
                    else if (relation < -40)
                    {
                        attackChance += 40;
                    }

                    if (attackChance < 5)
                    {
                        attackChance = 5;
                    }

                    if (attackChance > 95)
                    {
                        attackChance = 95;
                    }
                    attackChance *= warModifier;

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

                        int damage = (powerA - powerB) /25;

                        if (damage < 1) damage = 1;

                        b.life -= damage;
                        a.relations[j] -= 15;
                        b.relations[i] -= 15;

                        // =========================
                        // 🔴 创建战争激光
                        // =========================

                        Laser laser;

                        laser.start = a.pos;
                        laser.end = b.pos;

                        laser.life = 6;

                        lasers.append(laser);

                        // 🌍 战争消耗
                        a.energy -= 3;
                        a.military -= 1;
                        a.stability -= 2;

                        // 🌟 胜利收益
                        a.energy += 5;

                        // 🌌 战争日志
                        eventLogs.append(
                            QString("[Year %1] ")
                                .arg(universeYear)
                            + a.name +
                            " attacked " +
                            b.name
                            );
                    }
                    else
                    {
                        // b 获胜

                        int damage = (powerB - powerA) / 10;

                        if (damage < 5) damage = 5;

                        a.life -= damage;
                        a.relations[j] -= 15;
                        b.relations[i] -= 15;

                        Laser laser;

                        laser.start = b.pos;
                        laser.end = a.pos;

                        laser.life = 6;

                        lasers.append(laser);

                        b.energy -= 3;
                        b.military -= 1;
                        b.stability -= 2;

                        b.energy += 5;

                        eventLogs.append(
                            QString("[Year %1] ")
                                .arg(universeYear)
                            + b.name +
                            " attacked " +
                            a.name
                            );
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
                // =========================
                // 🌍 子文明名称
                // =========================

                QString prefixes[] =
                    {
                        "Nova",
                        "Eden",
                        "Orion",
                        "Titan",
                        "Astra",
                        "Zenith",
                        "Solar",
                        "Nebula"
                    };

                QString suffixes[] =
                    {
                        "Union",
                        "Empire",
                        "Alliance",
                        "Federation",
                        "Collective",
                        "Dominion"
                    };

                QString prefix =
                    prefixes[
                        QRandomGenerator::global()->bounded(8)
                ];

                QString suffix =
                    suffixes[
                        QRandomGenerator::global()->bounded(6)
                ];

                child.name = prefix + " " + suffix;
                // =========================
                // 🌌 子文明类型
                // =========================

                if (child.aggression > 70)
                {
                    child.type = "Aggressive";
                }
                else if (child.visibility < 30)
                {
                    child.type = "Hidden";
                }
                else if (child.stability > 80)
                {
                    child.type = "Peaceful";
                }
                else
                {
                    child.type = "Balanced";
                }
                // =========================
                // 🏛️ 子文明政治制度
                // =========================

                if (child.military > 120 &&
                    child.aggression > 70)
                {
                    child.politics =
                        "Military Dictatorship";
                }
                else if (child.technology > 7)
                {
                    child.politics =
                        "Technocracy";
                }
                else if (child.stability > 80 &&
                         child.aggression < 30)
                {
                    child.politics =
                        "Peace Republic";
                }
                else if (child.visibility < 25)
                {
                    child.politics =
                        "Hidden Civilization";
                }
                else if (child.energy > 130)
                {
                    child.politics =
                        "Resource Empire";
                }
                else if (child.stability < 30)
                {
                    child.politics =
                        "Chaos Tribe";
                }
                else
                {
                    child.politics =
                        "Balanced Federation";
                }
                child.faction = c.faction;   //子文明继承母文明阵营
                child.relations.resize(civilizations.size() + 1);

                for (int k = 0;
                     k < child.relations.size();
                     k++)
                {
                    child.relations[k] =
                        QRandomGenerator::global()
                            ->bounded(-20, 21);
                }

                civilizations.append(child);
                eventLogs.append(
                    QString("[Year %1] ")
                        .arg(universeYear)
                    + child.name +
                    " was born"
                    );
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
                // 🌌 文明灭亡日志
                eventLogs.append(
                    QString("[Year %1] ")
                        .arg(universeYear)
                    + civilizations[i].name +
                    " collapsed"
                    );

                civilizations.remove(i);
            }
        }
        // ⭐ 记录当前文明数量
        history.append(civilizations.size());

        // =========================
        // 🌌 宇宙自然生成文明
        // =========================

        if (civilizations.size() < 20)
        {
            if (QRandomGenerator::global()->bounded(100) < 8)
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
                c.energy = 120;
                c.age = 0;
                c.stability = 80;
                c.visibility = 20;
                c.aggression = 20;
                c.military = 40;
                // =========================
                // 🌍 文明名称生成
                // =========================

                QString prefixes[] =
                    {
                        "Nova",
                        "Eden",
                        "Orion",
                        "Titan",
                        "Astra",
                        "Zenith",
                        "Solar",
                        "Nebula"
                    };

                QString suffixes[] =
                    {
                        "Union",
                        "Empire",
                        "Alliance",
                        "Federation",
                        "Collective",
                        "Dominion"
                    };

                QString prefix =
                    prefixes[
                        QRandomGenerator::global()->bounded(8)
                ];

                QString suffix =
                    suffixes[
                        QRandomGenerator::global()->bounded(6)
                ];

                c.name = prefix + " " + suffix;

                // =========================
                // 🌌 文明类型
                // =========================

                if (c.aggression > 70)
                {
                    c.type = "Aggressive";
                }
                else if (c.visibility < 30)
                {
                    c.type = "Hidden";
                }
                else if (c.stability > 80)
                {
                    c.type = "Peaceful";
                }
                else
                {
                    c.type = "Balanced";
                }

                // =========================
                // 🏛️ 政治制度
                // =========================

                if (c.military > 120 &&
                    c.aggression > 70)
                {
                    c.politics =
                        "Military Dictatorship";
                }
                else if (c.technology > 7)
                {
                    c.politics =
                        "Technocracy";
                }
                else if (c.stability > 80 &&
                         c.aggression < 30)
                {
                    c.politics =
                        "Peace Republic";
                }
                else if (c.visibility < 25)
                {
                    c.politics =
                        "Hidden Civilization";
                }
                else if (c.energy > 130)
                {
                    c.politics =
                        "Resource Empire";
                }
                else if (c.stability < 30)
                {
                    c.politics =
                        "Chaos Tribe";
                }
                else
                {
                    c.politics =
                        "Balanced Federation";
                }

                civilizations.append(c);
            }
        }
        // ⭐ 防止无限增长（最多保存300帧）
        if (history.size() > 300)
        {
            history.remove(0);
        }
        // =========================
        // 🔴 更新激光寿命
        // =========================
        for (int i = lasers.size() - 1; i >= 0; i--)
        {
            lasers[i].life--;

            if (lasers[i].life <= 0)
            {
                lasers.remove(i);
            }
        }
        // =========================
        // 🌌 同步外交关系数组大小
        // =========================

        int civCount = civilizations.size();

        for (int i = 0; i < civCount; i++)
        {
            civilizations[i].relations.resize(civCount);
            for (int j = 0; j < civCount; j++)
            {
                if (i == j)
                {
                    civilizations[i].relations[j] = 100;
                }
                else if (civilizations[i].relations[j] == 0)
                {
                    civilizations[i].relations[j] =
                        QRandomGenerator::global()
                            ->bounded(-30, 31);
                }
            }

            for (int j = 0; j < civCount; j++)
            {
                // 自己对自己
                if (i == j)
                {
                    civilizations[i].relations[j] = 100;
                }
            }
        }
        // =========================
        // 🌌 限制日志数量
        // =========================

        while (eventLogs.size() > 12)
        {
            eventLogs.removeFirst();
        }
        update();
            });

    timer->start(100); // 每100毫秒更新一次
}

MainWindow::~MainWindow()
{
    delete ui;
}
// =========================
// 🌌 导出文明报告
// =========================
void MainWindow::exportCivilizationReport()
{
    // 文件名（带时间）
    QString fileName =
        QString("D:/Qt_code/UniverseDemo/reports/civilization_report_%1.txt")
            .arg(QDateTime::currentDateTime()
                     .toString("yyyyMMdd_hhmmss"));

    QFile file(fileName);
    // 打开文件
    if (!file.open(QIODevice::WriteOnly |
                   QIODevice::Text))
    {
        return;
    }

    QTextStream out(&file);
    // =========================
    // 🌌 标题
    // =========================
    out << "==============================\n";
    out << " UNIVERSE CIVILIZATION REPORT \n";
    out << "==============================\n\n";

    out << "Generated Time: "
        << QDateTime::currentDateTime()
               .toString("yyyy-MM-dd hh:mm:ss")
        << "\n\n";
    // =========================
    // 🌍 文明统计
    // =========================
    out << "========== CIVILIZATIONS =========="
        << "\n\n";

    for (int i = 0;
         i < civilizations.size();
         i++)
    {
        const Civilization &c = civilizations[i];

        out << "Civilization #"
            << i + 1
            << "\n";

        out << "Name: "
            << c.name
            << "\n";

        out << "Type: "
            << c.type
            << "\n";

        out << "Politics: "
            << c.politics
            << "\n";

        out << "Faction: "
            << c.faction
            << "\n";

        out << "Population: "
            << c.population
            << "\n";

        out << "Technology: "
            << c.technology
            << "\n";

        out << "Energy: "
            << c.energy
            << "\n";

        out << "Military: "
            << c.military
            << "\n";

        out << "Stability: "
            << c.stability
            << "\n";

        out << "Aggression: "
            << c.aggression
            << "\n";

        out << "Life: "
            << c.life
            << "\n";

        out << "Level: "
            << c.level
            << "\n";

        out << "--------------------------------\n";
    }
    // =========================
    // 🌌 事件日志
    // =========================
    out << "\n========== EVENT LOG =========="
        << "\n\n";

    for (const QString &log : eventLogs)
    {
        out << log << "\n";
    }
    // =========================
    // 🌍 总结信息
    // =========================
    out << "\n========== SUMMARY =========="
        << "\n\n";

    out << "Total Civilizations: "
        << civilizations.size()
        << "\n";

    out << "Total Events: "
        << eventLogs.size()
        << "\n";

    file.close();
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

    // =========================
    // 🌌 外交线控制提示
    // =========================

    QFont smallFont;
    smallFont.setPointSize(9);

    painter.setFont(smallFont);

    painter.setPen(Qt::gray);
    //显示实时文明数量
    QFont normalFont;
    normalFont.setPointSize(10);
    painter.setFont(normalFont);

    QString countText = QString("Civilizations: %1")
                            .arg(civilizations.size());

    painter.drawText(width() - 280, 115, countText);

    QString yearText =
        QString("Universe Year: %1")
            .arg(universeYear);
    QString eraText =
        QString("Era: %1")
            .arg(currentEra);

    painter.drawText(
        width() - 280,
        165,
        eraText
        );

    painter.drawText(
        width() - 280,
        140,
        yearText
        );

    // 📈 在侧边栏绘制实时文明数量折线图
    int graphX = width() - 280;
    int graphY = 300;
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

    // =========================
    // 🔴 绘制战争激光
    // =========================

    for (const Laser &laser : lasers)
    {
        QColor laserColor(255, 0, 0, 180);

        QPen pen(laserColor);

        pen.setWidth(2);

        painter.setPen(pen);

        painter.drawLine(laser.start, laser.end);
    }
    // =========================
    // 🌌 先绘制外交线
    // =========================

    for (int i = 0; i < civilizations.size(); i++)
    {
        const Civilization &c = civilizations[i];

        if (c.life <= 0) continue;

        for (int j = i + 1;
             j < civilizations.size();
             j++)
        {
            const Civilization &other =
                civilizations[j];

            if (other.life <= 0) continue;

            // 🌌 聚焦模式
            if (focusMode)
            {
                // 只显示与选中文明相关的线
                if (i != selectedIndex &&
                    j != selectedIndex)
                {
                    continue;
                }
            }

            if (j >= c.relations.size())
            {
                continue;
            }

            int relation =
                c.relations[j];

            // 🔵 联盟
            if (allianceBox->isChecked() &&
                relation > 40)
            {
                painter.setPen(
                    QPen(
                        QColor(80, 180, 255, 120),
                        2
                        )
                    );

                painter.drawLine(
                    c.pos,
                    other.pos
                    );
            }

            // 🔴 敌对
            else if (enemyBox->isChecked() &&
                     relation < -40)
            {
                painter.setPen(
                    QPen(
                        QColor(255, 60, 60, 120),
                        2
                        )
                    );

                painter.drawLine(
                    c.pos,
                    other.pos
                    );
            }

            // 🟡 中立
            else if (neutralBox->isChecked())
            {
                painter.setPen(
                    QPen(
                        QColor(255, 220, 80, 60),
                        1
                        )
                    );

                painter.drawLine(
                    c.pos,
                    other.pos
                    );
            }
        }
    }

    // 画文明
    for (int i = 0; i < civilizations.size(); i++)
    {
        const Civilization &c = civilizations[i];

        if (c.life <= 0) continue;
        // =========================
        // 🌌 聚焦模式
        // =========================

        if (focusMode)
        {
            // 不是被选中文明
            // 也不是与它有关联的文明
            if (i != selectedIndex)
            {
                bool related = false;

                int relation =
                    civilizations[selectedIndex].relations[i];

                if (relation != 0)
                {
                    related = true;
                }

                if (!related)
                {
                    continue;
                }
            }
        }

        // 🌍 根据能源决定亮度
        int alpha =
            c.energy +
            c.visibility +
            20;

        if (alpha > 255) alpha = 255;
        if (alpha < 50) alpha = 50;

        // =========================
        // 🌌 根据阵营决定颜色
        // =========================

        if (c.faction == "Blue Alliance")
        {
            painter.setBrush(QColor(80, 160, 255, alpha));
        }
        else if (c.faction == "Red Empire")
        {
            painter.setBrush(QColor(255, 80, 80, alpha));
        }
        else if (c.faction == "Green Union")
        {
            painter.setBrush(QColor(80, 255, 120, alpha));
        }
        else if (c.faction == "Golden Federation")
        {
            painter.setBrush(QColor(255, 220, 80, alpha));
        }
        else
        {
            painter.setBrush(QColor(200, 200, 200, alpha));
        }

        // 🔵 根据生命值决定大小
        int radius = c.life / 12;
        if (radius < 4) radius = 4;
        if (radius > 12) radius = 12;

        // =========================
        // ✨ 文明光晕
        // =========================

        QColor glowColor;

        // 根据等级决定光晕颜色
        switch (c.level)
        {
        case 1:
            glowColor = QColor(180, 180, 180, 40);
            break;

        case 2:
            glowColor = QColor(0, 255, 0, 50);
            break;

        case 3:
            glowColor = QColor(255, 0, 0, 60);
            break;

        case 4:
            glowColor = QColor(180, 0, 255, 70);
            break;

        case 5:
            glowColor = QColor(255, 215, 0, 80);
            break;

        default:
            glowColor = QColor(255, 255, 255, 40);
        }

        // 光晕半径
        int glowRadius = radius + c.level * 4;

        // 无边框
        painter.setPen(Qt::NoPen);

        // 绘制光晕
        painter.setBrush(glowColor);
        // =========================
        // 🌌 文明外交连线
        // =========================


        // =========================
        // 🌌 文明疆域范围
        // =========================

        // 科技 + 人口决定疆域
        int territoryRadius =
            40 +
            c.technology * 8 +
            c.population / 500;

        // 防止太大
        if (territoryRadius > 180)
        {
            territoryRadius = 180;
        }

        // =========================
        // 🌈 阵营颜色疆域
        // =========================

        QColor territoryColor;
        int territoryAlpha = c.energy / 3;

        if (territoryAlpha < 20)
        {
            territoryAlpha = 20;
        }

        if (territoryAlpha > 120)
        {
            territoryAlpha = 120;
        }

        if (c.faction == "Blue Alliance")
        {
            territoryColor =
                QColor(80, 160, 255, territoryAlpha);
        }
        else if (c.faction == "Red Empire")
        {
            territoryColor =
                QColor(255, 80, 80,territoryAlpha);
        }
        else if (c.faction == "Green Union")
        {
            territoryColor =
                QColor(80, 255, 120, territoryAlpha);
        }
        else if (c.faction == "Golden Federation")
        {
            territoryColor =
                QColor(255, 220, 80, territoryAlpha);
        }
        else
        {
            territoryColor =
                QColor(200, 200, 200, 30);
        }

        // 绘制疆域
        painter.setBrush(territoryColor);
        painter.setPen(Qt::NoPen);

        painter.drawEllipse(
            c.pos,
            territoryRadius,
            territoryRadius
            );
        painter.setPen(Qt::NoPen);

        painter.drawEllipse(c.pos, glowRadius, glowRadius);

        painter.drawEllipse(c.pos, radius, radius);
        // =========================
        // 🌌 文明名称标签
        // =========================

        if (c.level >= 3 ||
            i == selectedIndex)
        {
            painter.setPen(Qt::white);

            QFont nameFont;
            nameFont.setPointSize(8);

            painter.setFont(nameFont);

            // 名称
            painter.drawText(
                c.pos.x() - 35,
                c.pos.y() - radius - 10,
                c.name
                );

            // 等级
            painter.drawText(
                c.pos.x() - 15,
                c.pos.y() - radius - 25,
                QString("Lv.%1")
                    .arg(c.level)
                );
        }
    }
    // =========================
    // 🌌 事件日志显示
    // =========================

    painter.setPen(Qt::white);

    QFont logFont;
    logFont.setPointSize(8);

    painter.setFont(logFont);

    int logX = width() - 280;
    int logY = 350;

    // 🌌 日志背景框
    painter.setBrush(QColor(10, 10, 20, 180));
    painter.setPen(QColor(80, 80, 100));

    painter.drawRect(
        width() - 290,
        330,
        270,
        180
        );

    painter.setPen(Qt::white);

    painter.drawText(logX, logY,
                     "[ EVENT LOG ]");

    // 最多显示10条
    int start =
        qMax(0, eventLogs.size() - 10);

    for (int i = start;
         i < eventLogs.size();
         i++)
    {
        int line =
            i - start;

        QString text = eventLogs[i];

        if (text.length() > 32)
        {
            text = text.left(32) + "...";
        }

        painter.drawText(
            logX,
            logY + 20 + line * 15,
            text
            );
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
        int infoY = 530;

        // =========================
        // 🌌 标题
        // =========================

        painter.drawText(infoX, infoY,
                         "Selected Civilization");

        painter.drawText(infoX, infoY + 30,
                         QString("Name: %1")
                             .arg(c.name));

        painter.drawText(infoX, infoY + 50,
                         QString("Type: %1")
                             .arg(c.type));

        // =========================
        // 🌍 基础信息
        // =========================
        painter.drawText(infoX, infoY + 70,
                         QString("Politics: %1")
                             .arg(c.politics));
        painter.drawText(infoX, infoY + 90,
                         QString("Faction: %1")
                             .arg(c.faction));
        // =========================
        // 🌌 外交关系统计
        // =========================

        int allyCount = 0;
        int enemyCount = 0;
        int neutralCount = 0;

        for (int i = 0; i < c.relations.size(); i++)
        {
            // 跳过自己
            if (i == selectedIndex)
            {
                continue;
            }

            int relation = c.relations[i];

            if (relation > 40)
            {
                allyCount++;
            }
            else if (relation < -40)
            {
                enemyCount++;
            }
            else
            {
                neutralCount++;
            }
        }

        // 🌟 显示统计

        painter.drawText(infoX, infoY + 120,
                         QString("Allies: %1")
                             .arg(allyCount));

        painter.drawText(infoX, infoY + 140,
                         QString("Enemies: %1")
                             .arg(enemyCount));

        painter.drawText(infoX, infoY + 160,
                         QString("Neutral: %1")
                             .arg(neutralCount));

        painter.drawText(infoX, infoY + 180,
                         QString("Population: %1")
                             .arg(c.population));

        painter.drawText(infoX, infoY + 200,
                         QString("Technology: %1")
                             .arg(c.technology));

        painter.drawText(infoX, infoY + 220,
                         QString("Energy: %1")
                             .arg(c.energy));

        painter.drawText(infoX, infoY + 240,
                         QString("Age: %1")
                             .arg(c.age));

        // =========================
        // ⚖️ 社会属性
        // =========================

        painter.drawText(infoX, infoY + 270,
                         QString("Stability: %1")
                             .arg(c.stability));

        painter.drawText(infoX, infoY + 290,
                         QString("Visibility: %1")
                             .arg(c.visibility));

        painter.drawText(infoX, infoY + 310,
                         QString("Aggression: %1")
                             .arg(c.aggression));

        // =========================
        // ⚔️ 军事属性
        // =========================

        painter.drawText(infoX, infoY + 340,
                         QString("Military: %1")
                             .arg(c.military));

        painter.drawText(infoX, infoY + 360,
                         QString("Life: %1")
                             .arg(c.life));

        painter.drawText(infoX, infoY + 380,
                         QString("Speed: %1")
                             .arg(c.speed));

        painter.drawText(infoX, infoY + 400,
                         QString("Level: %1")
                             .arg(c.level));
    }


}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint clickPos = event->pos();

    selectedIndex = -1;  // 先清空选择
    // 🌌 默认关闭聚焦
    focusMode = false;

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
            // 🌌 开启聚焦模式
            focusMode = true;
            break;
        }
    }

    update();  // 触发重绘
}
void MainWindow::resizeEvent(QResizeEvent *event)
{
    allianceBox->setGeometry(width() - 280, 70, 80, 25);

    enemyBox->setGeometry(width() - 190, 70, 80, 25);

    neutralBox->setGeometry(width() - 100, 70, 80, 25);
    pauseButton->setGeometry(width() - 140,
                             height() - 50,
                             100,
                             32);

    QMainWindow::resizeEvent(event);
}