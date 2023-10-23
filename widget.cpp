#include "widget.h"
#include "ui_widget.h"
#include<QStyle>
#include<QFileDialog>
#include<QDir>
#include <QTime>
#include<QMediaContent>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ////////////////////    Buttons style  ///////////////////
    ui->setupUi(this);
    ui->pushButtonAdd->setIcon(style()->standardIcon(QStyle::SP_DriveDVDIcon));
    ui->pushButtonPrev->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->pushButtonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->pushButtonPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->pushButtonStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->pushButtonNext->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui->pushButtonMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

    this->muted = false;


    //ui->horizontalSliderProgress->setTickPosition(QSlider::TicksBelow);

    //////////////////////    Player init //////////////////
    m_player = new QMediaPlayer(this);
    //on_horizontalSliderVolume_valueChanged(70);
    m_player->setVolume(30);
    ui->labelVolume->setText((QString("Volume: ").append(QString::number(m_player->volume()))));
    ui->horizontalSliderVolume->setValue(m_player->volume());


    connect(m_player, &QMediaPlayer::durationChanged, this, &Widget::on_duration_changed);
    connect(m_player, &QMediaPlayer::positionChanged, this, &Widget::on_position_changed);


    ///////////////////////   Playlist   ////////////////////

    m_playlist_model = new QStandardItemModel(this);
    ui->tablePlaylist->setModel(m_playlist_model);//связываем таблицу с моделью
    m_playlist_model->setHorizontalHeaderLabels(QStringList() << tr("Audio track") << tr("File path"));

    ui->tablePlaylist->hideColumn(1);//скрываем столбец адресом файла
    ui->tablePlaylist->horizontalHeader()->setStretchLastSection(true);//растягиваем отображаемый столбец на всю ширину окна
    ui->tablePlaylist->setEditTriggers((QAbstractItemView::NoEditTriggers));//запрещаем редактирование ячеек таблицы

    ui->tablePlaylist->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tablePlaylist->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

    m_playlist = new QMediaPlaylist(m_player);
    m_player->setPlaylist(m_playlist);

    connect(ui->tablePlaylist, &QTableView::doubleClicked,
            [this](const QModelIndex& index){ m_playlist->setCurrentIndex(index.row()); m_player->play();});
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged,
            [this](int index)
    {
        this->setWindowTitle("Media Player PU_211 -" + ui->labelComposition->text());
        ui->labelComposition->setText(m_playlist_model->data(m_playlist_model->index(index, 0)).toString());
        ui->tablePlaylist->selectRow(index);
    }
    );
    ////////////////////////////////////////////////////////////


    ///////////////////////   Load Playlist   ////////////////////

     /*m_playlist->load(QUrl::fromLocalFile("D:/Source/Qt/build-MediaPlayer-Desktop_Qt_5_12_12_MSVC2015_64bit-Debug/debug/playlist.m3u"), "m3u");
    for(int i=0; i < m_playlist->mediaCount(); i++)
    {
        QMediaContent content = m_playlist->media(i);
        QString url = content.canonicalUrl().url();
        QList<QStandardItem*> items;
        items.append(new QStandardItem(QDir(url).dirName()));
        items.append(new QStandardItem(url));
        m_playlist_model->appendRow(items);
    }*/
    load_playlist(DEFAULT_PLAYLIST);

    ui->pushButtonShuffle->setCheckable(true);
    ui->pushButtonLoop->setCheckable(true);
    shuffle = false;
    loop = false;
}

Widget::~Widget()
{
   //QMessageBox mb (QMessageBox::Icon::Information, QString("Buy"), QString("Buy"), QMessageBox::StandardButton::Ok, this);
   //mb.Show();
  // m_playlist->save(QUrl::fromLocalFile(DEFAULT_PLAYLIST), QString(DEFAULT_PLAYLIST).split('.').back().toStdString().c_str());

    delete this->m_playlist_model;
    delete this->m_playlist;
    delete this->m_player;
   delete ui;
}

void Widget::load_playlist(QString filename)
{
    QString format = filename.split('.').back();
    m_playlist->load(QUrl::fromLocalFile(filename), format.toStdString().c_str());
   for(int i=0; i < m_playlist->mediaCount(); i++)
   {
       QMediaContent content = m_playlist->media(i);
       QString url = content.canonicalUrl().url();
       QList<QStandardItem*> items;
       items.append(new QStandardItem(QDir(url).dirName()));
       items.append(new QStandardItem(url));
       m_playlist_model->appendRow(items);
   }
}

void Widget::save_playlist(QString filename)
{
    QString format = filename.split('.').back();
    m_playlist->save(QUrl::fromLocalFile(filename), format.toStdString().c_str());
}



void Widget::on_pushButtonAdd_clicked()
{
//    QString file = QFileDialog::getOpenFileName(this,
//                                                tr("Open file"),
//                                                QString("C:\\Users\\Галима\\Desktop\\Music"),
//                                                tr("Audio files (*.mp3 *.flac);; mp-3 (*.mp3);;Flac (*.flac)")
//                                                );
//    ui->labelComposition->setText(file);
//    m_player->setMedia(QUrl::fromLocalFile(file));
//    m_player->play();

//    this->setWindowTitle("Media Player PU_211 -" +  file.split('/').back());
    QStringList files = QFileDialog::getOpenFileNames(
                         this,
                         tr("Open files"),
                         QString("E:\\Users\\Galima\\Desktop\\Music"),
                         tr("Audio files (*.mp3 *.flac);; mp-3 (*.mp3);; Flac (*.flac);; Playlist (*.m3u);; CUE (*.cue)")
                );
    QString format = files.back().split('.').back();
    if(format == "m3u")
    {
        load_playlist(files.back());
        return;
    }
    for(QString filesPath: files)
    {
        //1) Создаем строку:
        //Каждая строка таблицы 'tablePlayList' - это списокс стандартных вхождений
        QList<QStandardItem*> items;
        items.append(new QStandardItem(QDir(filesPath).dirName()));
        items.append(new QStandardItem(filesPath));
        m_playlist_model->appendRow(items);
        m_playlist->addMedia(QUrl(filesPath));

    }

}

void Widget::on_horizontalSliderVolume_valueChanged(int value)

{
    ui->labelVolume->setText(QString("Volume: ").append(QString::number(value)));
    m_player->setVolume(value);
}


void Widget::on_pushButtonPlay_clicked()
{
    m_player->play();

}


void Widget::on_pushButtonPause_clicked()
{
    m_player->pause();
}


void Widget::on_pushButtonStop_clicked()
{
    m_player->stop();
}

void Widget::on_position_changed(qint64 position)
{
    ui->horizontalSliderProgress->setValue(position);
    QTime qt_position = QTime::fromMSecsSinceStartOfDay(position);
    ui->labelPlayed->setText(QString("Played: ").append(qt_position.toString("mm:ss")));
}

void Widget::on_duration_changed(qint64 duration)
{
    ui->horizontalSliderProgress->setMaximum(duration);
    QTime qt_duration = QTime::fromMSecsSinceStartOfDay(duration);
    ui->labelDuration->setText(QString("Duration: ").append(qt_duration.toString("mm:ss")));

}

void Widget::on_horizontalSliderProgress_valueChanged(qint64 position)
{
    m_player->setPosition(position);
}

void Widget::on_pushButtonMute_clicked()
{
    muted = !muted;
    m_player->setMuted(muted);
    ui->pushButtonMute->setIcon(style()->standardIcon(muted ? QStyle::SP_MediaVolumeMuted : QStyle::SP_MediaVolume));
}


void Widget::on_pushButtonNext_clicked()
{
    m_playlist->next();
}


void Widget::on_pushButtonPrev_clicked()
{
    m_playlist->previous();
}


void Widget::on_pushButtonCLR_clicked()
{
    m_playlist->clear();
    m_playlist_model->clear();
    ui->labelComposition->setText("Erased");
}


void Widget::on_pushButtonRem_clicked()
{
// QModelIndexList list = ui->tablePlaylist->selec;
//  for(QModelIndex i: list)
//  {
//      m_playlist->removeMedia(i.row());
//  }

      QItemSelectionModel* selection = ui->tablePlaylist->selectionModel();
      QModelIndexList rows = selection->selectedRows();
      for(QModelIndexList::iterator it = rows.begin(); it !=rows.end(); ++it)
      {
         if(m_playlist->removeMedia(it->row()));
          m_playlist_model->removeRows(it->row(), 1);
      }

      //while(!rows.isEmpty())
      //{
          //m_playlist_model->removeRows(rows.last().row(), 1);
         // rows.removeLast();
      //}

      }

void Widget::on_horizontalSliderProgress_sliderMoved(int position)
{
    m_player->setPosition(position);
}

/*void Widget::on_horizontalSliderVolume_sliderMoved(int position)
{
    m_player->setVolume(position);
}*/

void Widget::on_pushButtonShuffle_clicked()
{
     //m_playlist->shuffle();
    if(!shuffle)
    {
        shuffle = true;
        m_playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::Random);
        ui->pushButtonShuffle->setChecked(shuffle);
        ui->pushButtonShuffle->setBackgroundRole(QPalette::Dark);
        ui->pushButtonShuffle->setForegroundRole(QPalette::Light);
    }
    else
    {
        shuffle = false;
        m_playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::Sequential);
        ui->pushButtonShuffle->setChecked(shuffle);
        ui->pushButtonShuffle->setBackgroundRole(QPalette::Light);
        ui->pushButtonShuffle->setForegroundRole(QPalette::Dark);
    }
}

void Widget::on_pushButtonLoop_clicked()
{
    loop = !loop;
    m_playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::Sequential);
    if(loop)m_playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::Loop);
    if(shuffle)m_playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::Random);
    //m_player->setPlaylist(m_playlist);
    //m_player->play();

}

