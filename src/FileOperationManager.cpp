#include "FileOperationManager.h"
#include "PasteFileOperation.h"


FileOperationManager::FileOperationManager(QObject *) {
    operationMap["paste"] = new PasteFileOperation();
    operationMap["move"] = new PasteFileOperation();
    operationMap["rename"] = new PasteFileOperation();
    operationMap["new_folder"] = new PasteFileOperation();

    connect(operationMap["paste"], &IFileOperation::progress, this, &FileOperationManager::onProgress);
    connect(operationMap["paste"], &IFileOperation::error, this, &FileOperationManager::onError);
    connect(operationMap["paste"], &IFileOperation::finished, this, &FileOperationManager::onFinished);
    };


void FileOperationManager::undoLast() {

}

void FileOperationManager::PasteOperation(QString dst)
{
    QVariantMap params;

    auto op = operationMap["paste"];
    if (op)
    {
        // kopyalanmış yolların tamaını addOperation için src ile birleştirip param haline getirip sıraya ekliyoruz
        for (int i = 0; i < copiedPaths.count(); ++i)
        {
            params["src"] = copiedPaths.dequeue();
            params["dst"] = dst;

            params["op"] = "paste";

            // işlem tamamlandığında işlem geçmişine ekle:
            connect(op, &IFileOperation::finished, this, [this, params](){
                operationHistory.append(params);
            });

            currentOperation = params;
            op->addOperations(params);
        }

        // tüm yapılacaklar sıraya eklendiğinde işlemi başlat
        op->start();
    }
}

void FileOperationManager::addToCut(QString src)
{
    copiedPaths.enqueue(src);

    //TODO: seçimin UI üzerinde grileşmesi gerekir.
}

void FileOperationManager::addToCopy(QString src)
{
    copiedPaths.enqueue(src);
}

void FileOperationManager::onProgress(int percent) {
    qDebug() << "Progress:" << percent << "%";
}

void FileOperationManager::onError(const QString &msg) {
    qWarning() << "Error:" << msg;
}

void FileOperationManager::onFinished(const QString &info) {
    qDebug() << "Operation Finished:" << info;
}
