


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

        QFileInfo destInfo(dst);
        QDir destDir = destInfo.absoluteDir();
        if (!destDir.exists()) {
            destDir.mkpath(".");
        }

        QFile file(src);
        if (!file.exists()) {
            emit error("Source does not exist: " + src);
            continue;
        }

        if (!file.copy(dst)) {
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
