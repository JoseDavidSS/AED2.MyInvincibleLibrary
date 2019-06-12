#include "menu.h"
#include "ui_menu.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>


Menu::Menu(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::Menu)
{
    // Initalize default parameters
    ui->setupUi(this);
    setAcceptDrops(true);
    ui->mainLogo->setGeometry(QRect(450, 100, 200, 200));
    ui->welcomeLabel->setVisible(false);
    ui->copyBackground->setVisible(false);
    ui->dropIcon->setVisible(false);
    ui->dropLabel->setVisible(false);
    imageList = new QList<Image*>();

    // Set scene
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 850, 450);
    ui->imageView->setScene(scene);

    // Bring initial elements to front
    ui->invincibleLabel->raise();
    ui->enterButton->raise();
    ui->mainLogo->raise();

    // Button hover watchers
    ButtonHoverWatcher* addWatcher = new ButtonHoverWatcher(this, ":/main/buttons/buttons/addButton.png",":/main/buttons/buttons/addButton_hovered.png");
    ButtonHoverWatcher* enterWatcher = new ButtonHoverWatcher(this, ":/main/buttons/buttons/enterButton.png",":/main/buttons/buttons/enterButton_hovered.png");
    ui->addButton->installEventFilter(addWatcher);
    ui->enterButton->installEventFilter(enterWatcher);

    // Animation timers
    deletionTimer = new QTimer(this);
    deletionTimer->setInterval(1000);
    connect(deletionTimer, SIGNAL(timeout()), this, SLOT(deleteBackground()));
}

Menu::~Menu()
{
    delete ui;
}

//! Deletes background. Executed by timer when entering app.
void Menu::deleteBackground() {
    ui->invincibleLabel->lower();
    deletionTimer->stop();
    hasEntered = true;

    // Fade in welcome label
    QGraphicsOpacityEffect *fade = new QGraphicsOpacityEffect(this);
    ui->welcomeLabel->setVisible(true);
    ui->welcomeLabel->setGraphicsEffect(fade);
    QPropertyAnimation *fadeOut = new QPropertyAnimation(fade,"opacity");
    fadeOut->setDuration(1000);
    fadeOut->setStartValue(0);
    fadeOut->setEndValue(1);
    fadeOut->start(QPropertyAnimation::DeleteWhenStopped);
}

//! Executes while dragging file to window
void Menu::dragEnterEvent(QDragEnterEvent* e) {
    if(hasEntered) {
        if (e->mimeData()->hasUrls()) {
            e->acceptProposedAction();
        }
        ui->copyBackground->raise();
        ui->dropIcon->raise();
        ui->dropLabel->raise();
        ui->copyBackground->setVisible(true);
        ui->dropIcon->setVisible(true);
        ui->dropLabel->setVisible(true);
    }
}

//! Executes while leaving drag area of window
void Menu::dragLeaveEvent(QDragLeaveEvent* e) {
    if (hasEntered) {
        ui->copyBackground->setVisible(false);
        ui->dropIcon->setVisible(false);
        ui->dropLabel->setVisible(false);
    }
}


//! Executes when dropping file in window
void Menu::dropEvent(QDropEvent* e) {
    if (hasEntered) {
        // Define accepted image types
        QStringList accepted_types;
        accepted_types << "jpeg" << "jpg" << "png" << "heif" << "bmp";
        foreach(const QUrl & url, e->mimeData()->urls()) {
            QString fileName = url.toLocalFile();
            QFileInfo info(fileName);
            if (info.exists()) {
                if (accepted_types.contains(info.suffix().trimmed(), Qt::CaseInsensitive))
                    qDebug() << fileName;
            }
        }

        // Reset drop background elements
        ui->copyBackground->setVisible(false);
        ui->dropIcon->setVisible(false);
        ui->dropLabel->setVisible(false);
        ui->copyBackground->lower();
        ui->dropIcon->lower();
        ui->dropLabel->lower();
    }
}

//! Initializes empty image grid
void Menu::initializeGrid() {
    int gridColumns = 12;
    int gridRows = 9;
    int x = 20;
    int y = 20;
    int id = 0;

    for (int i = 0; i < gridRows; i++) {
        for (int i = 0; i < gridColumns; i++) {
            Image* image = new Image();
            image->setRect(x, y, 53, 40);
            image->setBrush(QBrush(Qt::red));
            image->setID(id);
            scene->addItem(image);
            x += 80;
            imageList->append(image);
            id++;
        }
        x = 20;
        y += 60;
    }
}

void Menu::on_addButton_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select Image"), "/home/jana", tr("Image Files (*.jpeg *.jpg *.png *.heif *.bmp)"));
    qDebug() << fileName;
}

void Menu::on_enterButton_clicked() {
    // Fade out window
    QGraphicsOpacityEffect *fade = new QGraphicsOpacityEffect(this);
    ui->invincibleLabel->setGraphicsEffect(fade);
    QPropertyAnimation *fadeOut = new QPropertyAnimation(fade,"opacity");
    fadeOut->setDuration(1000);
    fadeOut->setStartValue(1);
    fadeOut->setEndValue(0);
    fadeOut->start(QPropertyAnimation::DeleteWhenStopped);

    // Move logo to top left
    QLabel* logo = ui->mainLogo;
    QPropertyAnimation *moveLogo = new QPropertyAnimation(logo, "geometry");
    moveLogo->setDuration(1000);
    moveLogo->setStartValue(QRect(380, 100, 200, 200));
    moveLogo->setEndValue(QRect(20, 20, 100, 100));
    moveLogo->start();

    // Delete background and button
    deletionTimer->start();
    ui->enterButton->deleteLater();

    // Initalize image grid
    initializeGrid();
}

string Menu::imageToByteArray() {
    std::ifstream ifs("/home/jose/Downloads/hqdefault.jpg", std::ios::in | std::ios::binary);
    std::ostringstream oss;

    int len;
    char buf[1024];
    while ((len = ifs.readsome(buf, 1024)) > 0){
        oss.write(buf, len);
    }

    std::string data = oss.str();
    cout << data;

    return data;

}
