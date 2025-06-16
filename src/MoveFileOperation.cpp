#include "MoveFileOperation.h"

#include <QDir>
#include <QFileInfo>

#include <utility>

MoveFileOperation::MoveFileOperation(QSet<QString> SourcePaths, QString destinationPath, bool shouldRemove, QObject* parent)
    :
    IFileOperation(parent),
    m_op_files_src(std::move(SourcePaths)),
    m_target_dir_(destinationPath),
    m_op_should_remove_(shouldRemove)
{

}

void MoveFileOperation::start()
{
    // dosyalar kaynaktan kaldırılacaksa (kesme işlemi)
    if(m_op_should_remove_){
        CutFilesOperation();
    }
    else{
        CopyFilesOperation();
    }
}


void MoveFileOperation::CutFilesOperation()
{
    QList<QString> fileList = m_op_files_src.values(); // QSet → QList (sıralı)
    const int totalFiles = fileList.size();

    for (int i = 0; i < totalFiles; ++i)
    {
        const QString& srcPath = fileList[i];
        QFileInfo const srcInfo(srcPath);
        QString const destPath = QDir(m_target_dir_).filePath(srcInfo.fileName());

        bool moveSuccess = false;

        if (srcInfo.isDir())
        {
            // Klasörleri kopyala (recursive)
            QDir const sourceDir(srcPath);
            QDir const destDir;
            if (!destDir.mkpath(destPath)) {
                //TODO(yunnsbz): check if there is a name conflict then ask user if they want to override, rename or skip this one
                emit error(QString("Could not create target folder: %1").arg(destPath));
                continue;
            }

            moveSuccess = copyDirectoryRecursively(srcPath, destPath);
            if (moveSuccess) {
                QDir dirToDelete(srcPath);
                moveSuccess = dirToDelete.removeRecursively();
            }
        }
        else
        {
            if (QFile::copy(srcPath, destPath)) {
                moveSuccess = QFile::remove(srcPath); // kaynak dosyayı sil
            }
            //TODO(yunnsbz): check if there is a name conflict then ask user if they want to override, rename or skip this one
        }

        if (!moveSuccess) {
            emit error(QString("Could not move: %1").arg(srcPath));
            continue;
        }

        int percent = (i + 1) * 100 / totalFiles;
        emit progress(percent);
    }

    emit finished("moved (cut) files.");
}

void MoveFileOperation::CopyFilesOperation()
{
    QList<QString> fileList = m_op_files_src.values(); // QSet → QList (sıralı)
    const int totalFiles = fileList.size();

    for (int i = 0; i < totalFiles; ++i)
    {
        const QString& srcPath = fileList[i];
        QFileInfo const srcInfo(srcPath);
        QString const destPath = QDir(m_target_dir_).filePath(srcInfo.fileName());

        bool moveSuccess = false;

        if (srcInfo.isDir())
        {
            // Klasörleri kopyala (recursive)
            QDir const sourceDir(srcPath);
            QDir const destDir;
            if (!destDir.mkpath(destPath)) {
                //TODO(yunnsbz): check if there is a name conflict then ask user if they want to override, rename or skip this one
                emit error(QString("Could not create target folder: %1").arg(destPath));
                continue;
            }

            moveSuccess = copyDirectoryRecursively(srcPath, destPath);
        }
        else
        {
            moveSuccess = QFile::copy(srcPath, destPath);
            //TODO(yunnsbz): check if there is a name conflict then ask user if they want to override, rename or skip this one
        }

        if (!moveSuccess) {
            emit error(QString("Could not copy: %1").arg(srcPath));
            continue;
        }

        const int percent = (i + 1) * 100 / totalFiles;
        emit progress(percent);
    }

    emit finished("move (copy) files.");
}

bool MoveFileOperation::copyDirectoryRecursively(const QString &srcPath, const QString &destPath)
{
    QDir const srcDir(srcPath);
    if (!srcDir.exists())
    {
        return false;
    }

    QDir const destDir(destPath);
    if (!destDir.exists())
    {
        destDir.mkpath(".");
    }

    QFileInfoList const entries = srcDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
    for (const QFileInfo &entry : entries)
    {
        QString const srcFilePath = entry.absoluteFilePath();
        QString const destFilePath = destDir.filePath(entry.fileName());

        if (entry.isDir())
        {
            if (!copyDirectoryRecursively(srcFilePath, destFilePath))
            {
                return false;
            }
        }
        else
        {
            if (!QFile::copy(srcFilePath, destFilePath))
            {
                //TODO(yunnsbz): check if there is a name conflict then ask user if they want to override, rename or skip this one
                // if this is not resolving the conflict:
                return false;
            }
        }
    }

    return true;
}
