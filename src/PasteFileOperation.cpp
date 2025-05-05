


#include "PasteFileOperation.h"
#include <qdir.h>

// params: src, dst, op (all three are QString)
void PasteFileOperation::start()
{
    int count = requestQueue.count();
    int finishedCount = 0;
    while (!requestQueue.isEmpty()) {
        queueMutex.lock();
        QVariantMap req = requestQueue.dequeue();
        queueMutex.unlock();

        QString src = req["src"].toString();
        QString dst = req["dst"].toString();

        QFileInfo srcInfo(src);
        //QFileInfo destInfo(dst);

        QDir destDir(dst);
        if (!destDir.exists()) {
            destDir.mkpath(".");
        }

        QFile file(src);
        if (!file.exists()) {
            emit error("Source does not exist: " + src);
            continue;
        }
        QString fullDstPath = destDir.filePath(srcInfo.fileName());

        // if path has the same file/folder it should create a duplicate with "-copy" postfix
        if(fullDstPath == src)
        {
            //find dot
            int lastDotPos = fullDstPath.lastIndexOf('.');

            QString extention = "";
            QString name = fullDstPath;

            // if dot exist seperate name and the extention:
            if (lastDotPos != -1) {
                name = fullDstPath.left(lastDotPos);
                extention = fullDstPath.mid(lastDotPos);
            }
            fullDstPath = name + "-copy" + extention;
            qDebug() << fullDstPath;
        }

        // if a different path has the same file
        //todo

        if (!file.copy(fullDstPath)) {
            emit error("Copy failed: " + src + "      dst: " + dst);
        } else {
            finishedCount++;
            int percent = static_cast<int>((double)finishedCount / count * 100);
            emit progress(percent);
            emit finished("Copied: " + src);
        }
    }
}

void PasteFileOperation::addOperations(QVariantMap params)
{
    AbstractFileOp::addOperations(params);

    // ek birşey yapılması gerekirse...
}

void PasteFileOperation::undo(QVariantMap )
{

}

void PasteFileOperation::cancel()
{

}
