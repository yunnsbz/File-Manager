#include "TableManager.h"
#include "FileModelOperations.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
#endif

#include <QFileSystemModel>
#include <QUrl>
#include <QDesktopServices>
#include <QHeaderView>
#include <QTimer>
#include <QAbstractItemView>
#include <QModelIndex>
#include <QVector>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QStringList>
#include <QFileInfo>
#include <QVector>
#include <QWidget>
#include <QMenu>

FM_BEGIN_NAMESPACE

TableManager::TableManager(QTabWidget* tabWidget, QTableView* tableView, FileModelOperations* fileModelOp, QObject *parent)
    :
    QObject(parent),
    m_tabWidget(tabWidget),
    m_fileModelOp_(fileModelOp),
    m_tableView_(tableView)
{
    auto* fileModel = m_fileModelOp_->getFileModel();
    tableView->setModel(fileModel);
    setColumnResize();
    tableView->verticalHeader()->setDefaultSectionSize(10);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(tableView, &QTableView::customContextMenuRequested, this, &TableManager::onContextMenuRequested);
    connect(tableView, &QTableView::doubleClicked, this, &TableManager::onTableDoubleClicked);
}

void TableManager::onTableDoubleClicked(const QModelIndex &modelIndex)
{
    const auto firstColumnIndex = modelIndex.siblingAtColumn(0); // her zaman ilk sütunu al

    navigateToFolder(m_tabWidget->currentIndex(), firstColumnIndex);

    emit tableDoubleClicked(modelIndex);
}

void TableManager::onContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = m_tableView_->indexAt(pos);
    if (!index.isValid())
        return;

    QMenu contextMenu;
    contextMenu.addAction("open with", this, [this, &index](){
        call_Win32_OpenWithMenu(index);
    });
    contextMenu.addSeparator();
    contextMenu.addAction("action1", this, &TableManager::setTableToDefault);
    contextMenu.addSeparator();
    contextMenu.addAction("action2", this, &TableManager::setTableToDefault);
    contextMenu.addAction("action3", this, &TableManager::setTableToDefault);
    contextMenu.addAction("action4", this, &TableManager::setTableToDefault);
    contextMenu.exec(m_tableView_->viewport()->mapToGlobal(pos));

}

void TableManager::call_Win32_OpenWithMenu(QModelIndex index)
{
    const QString filePath = m_fileModelOp_->getFileModel()->filePath(index);
#ifdef Q_OS_WIN32
    SHELLEXECUTEINFO sei{};
    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.fMask = SEE_MASK_INVOKEIDLIST;
    sei.hwnd = nullptr;
    sei.lpVerb = L"openas";
    sei.lpFile = reinterpret_cast<LPCWSTR>(filePath.utf16());
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteEx(&sei)) {
        // Hata işleme
        const DWORD err = GetLastError();
        qDebug() << "ShellExecuteEx failed with error:" << err;
    }
#endif
}

void TableManager::setTableToDefault()
{
    auto* fileModel = m_fileModelOp_->getFileModel();
    fileModel->setRootPath("");
    const QModelIndex index = fileModel->index(fileModel->rootPath());
    m_tableView_->setRootIndex(index);
}

void TableManager::setTableContent(int tabIndex)
{
    // set table view content:
    auto index = m_fileModelOp_->getTabModelIndex(tabIndex);

    if (!index.isValid())
    {
        setTableToDefault();
        return;
    }

    m_tableView_->setRootIndex(index);
}

void TableManager::navigateToFolder(int tabIndex, QModelIndex firstColumnIndex)
{
    auto* fileModel = m_fileModelOp_->getFileModel();
    if (!fileModel->hasChildren(firstColumnIndex))
    {
        const QString filePath = fileModel->filePath(firstColumnIndex);

        // Dosya uzantısı
        const QString suffix = QFileInfo(filePath).suffix().toLower();
        const QStringList videoExts = { "mp4", "avi", "mkv", "mov" };
        const QStringList audioExts = { "mp3", "wav", "flac", "ogg" };

        if (videoExts.contains(suffix) || audioExts.contains(suffix))
        {
            openMediaWindow(filePath);
        }
        else
        {
            QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
        }
    }
    else
    {
        m_tableView_->setRootIndex(firstColumnIndex);
        m_fileModelOp_->setTabModelIndex(tabIndex,firstColumnIndex);
    }
}

void TableManager::setColumnResize()
{
    QHeaderView* header = m_tableView_->horizontalHeader();

    // Stretch ile başlangıç yerleşimi. bu işlem interactive modu kapatır. aşağıda tekrar açılacak
    header->setSectionResizeMode(QHeaderView::Stretch);
    m_tableView_->resizeColumnsToContents();

    // Tablonun güncellenmesini bekle
    QTimer::singleShot(0, this, [this]() {
        auto* header1 = m_tableView_->horizontalHeader();
        const int columnCount = m_tableView_->model()->columnCount();

        // stretch değerlerini kaydet:
        QVector<int> widths;
        for (int i = 0; i < columnCount; ++i)
        {
            widths.append(m_tableView_->columnWidth(i));
        }
        // Kullanıcının sütun genişliklerini değiştirebilmesi için Interactive moda geç (stretch değerlerini değiştirir)
        for (int i = 0; i < columnCount; ++i)
        {
            header1->setSectionResizeMode(i, QHeaderView::Interactive);
        }
        // Stretch'te belirlenmiş genişlikleri geri uygula:
        for (int i = 0; i < columnCount; ++i)
        {
            m_tableView_->setColumnWidth(i, widths[i]);
        }
    });
}

void TableManager::openMediaWindow(const QString filePath)
{
    auto* player = new QMediaPlayer;
    auto* audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);

    QFileInfo fi(filePath);
    const QString suffix = fi.suffix().toLower();

    QWidget* window = new QWidget;
    window->setAttribute(Qt::WA_DeleteOnClose); // pencere kapanınca belleği temizle

    QVBoxLayout* layout = new QVBoxLayout(window);

    if (QStringList{ "mp4", "avi", "mkv", "mov" }.contains(suffix))
    {
        auto* videoWidget = new QVideoWidget;
        layout->addWidget(videoWidget);
        player->setVideoOutput(videoWidget);
    }
    else
    {
        layout->addWidget(new QLabel("Playing audio: " + fi.fileName()));
    }

    player->setSource(QUrl::fromLocalFile(filePath));
    player->play();

    connect(window, &QVideoWidget::destroyed, player, &QMediaPlayer::stop);

    window->setWindowTitle("Playing: " + fi.fileName());
    window->resize(640, 480);
    window->show();
}

void TableManager::setRootIndex(QModelIndex modelIndex)
{
    m_tableView_->setRootIndex(modelIndex);
}

FM_END_NAMESPACE
