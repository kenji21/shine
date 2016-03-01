#include "keystore.h"


KeyStore::KeyStore(QObject *parent): QObject(parent),
#if QT_VERSION < 0x050000
        m_settings(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/shine/shine.conf", QSettings::IniFormat)
#else      
        m_settings(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first() + "/shine/shine.conf", QSettings::IniFormat)
#endif
{
}

QString KeyStore::apiKey() const
{
    qDebug() << "reading from" << m_settings.fileName();
    return m_settings.value("apiKey").toString();
}

void KeyStore::setApiKey(const QString &apiKey)
{
    m_settings.setValue("apiKey", apiKey);
    emit apiKeyChanged();
}
