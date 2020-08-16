#include "IFetcher.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>

#include <QLogger.h>

using namespace QLogger;

namespace Jenkins
{
IFetcher::IFetcher(const Config &config, QObject *parent)
   : QObject(parent)
   , mConfig(config)
{
}

void IFetcher::get(const QString &urlStr, int port, bool customUrl)
{
   const auto apiUrl = urlStr.endsWith("api/json") || customUrl ? urlStr : urlStr + "api/json";

   QUrl url(apiUrl);
   url.setPort(port);

   QNetworkRequest request(url);

   if (!mConfig.user.isEmpty() && !mConfig.token.isEmpty())
      request.setRawHeader(QByteArray("Authorization"),
                           QString("Basic %1:%2").arg(mConfig.user, mConfig.token).toLocal8Bit().toBase64());

   const auto reply = mConfig.accessManager->get(request);
   connect(reply, &QNetworkReply::finished, this, &IFetcher::processReply);
}

void IFetcher::processReply()
{
   const auto reply = qobject_cast<QNetworkReply *>(sender());
   const auto data = reply->readAll();

   if (data.isEmpty())
      QLog_Warning("Jenkins", QString("Reply from {%1} is empty.").arg(reply->url().toString()));

   const auto json = QJsonDocument::fromJson(data);

   if (json.isNull())
   {
      QLog_Error("Jenkins", QString("Data from {%1} is not a valid JSON").arg(reply->url().toString()));
      QLog_Trace("Jenkins", QString("Data received:\n%1").arg(QString::fromUtf8(data)));
      return;
   }

   processData(json);
}
}
