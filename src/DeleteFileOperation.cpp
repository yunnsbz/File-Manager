#include "DeleteFileOperation.h"

#include <QDir>
#include <QFileInfo>

#include <utility>

FM_BEGIN_NAMESPACE

DeleteFileOperation::DeleteFileOperation(QList<QString> files, QObject* parent)
    :
    IFileOperation(parent),
    m_op_files_(std::move(files))
{

}

void DeleteFileOperation::start()
{
    for (int i = 0; i < m_op_files_.size(); ++i)
    {
        const auto& path = m_op_files_[i];

        const QFileInfo info(path);

        if (info.isDir())
        {
            QDir dir(path);

            if ( ! dir.removeRecursively())
            {
                emit error(QString("Could not delete FOLDER: %1").arg(path));

                continue;
            }
        }
        else if ( ! QFile::remove(path))
        {
            emit error(QString("Could not delete FILE: %1").arg(path));

            continue;
        }

        const int percent = (i + 1) * 100 / m_op_files_.size();
        emit progress(percent);
    }

    emit finished("deleted files.");
}

FM_END_NAMESPACE
