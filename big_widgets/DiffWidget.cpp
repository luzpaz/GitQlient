#include "DiffWidget.h"

#include <FileDiffWidget.h>
#include <FullDiffWidget.h>

#include <QLogger.h>

#include <QStackedWidget>
#include <QMessageBox>
#include <QHBoxLayout>

using namespace QLogger;

DiffWidget::DiffWidget(const QSharedPointer<Git> git, QWidget *parent)
   : QFrame(parent)
   , centerStackedWidget(new QStackedWidget())
   , mFullDiffWidget(new FullDiffWidget(git))
   , mFileDiffWidget(new FileDiffWidget(git))
{
   centerStackedWidget->setCurrentIndex(0);
   centerStackedWidget->addWidget(mFullDiffWidget);
   centerStackedWidget->addWidget(mFileDiffWidget);
   centerStackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

   const auto layout = new QHBoxLayout();
   layout->addWidget(centerStackedWidget);

   setLayout(layout);
}

void DiffWidget::clear() const
{
   centerStackedWidget->setCurrentIndex(0);
   mFullDiffWidget->clear();
   mFileDiffWidget->clear();
}

void DiffWidget::configure(const QString &currentSha, const QString &previousSha, const QString &file)
{
   const auto fileWithModifications = mFileDiffWidget->configure(currentSha, previousSha, file);

   if (fileWithModifications)
   {
      QLog_Info(
          "UI",
          QString("Requested diff for file {%1} on between commits {%2} and {%3}").arg(file, currentSha, previousSha));

      centerStackedWidget->setCurrentIndex(2);
   }
   else
      QMessageBox::information(this, tr("No modifications"), tr("There are no content modifications for this file"));
}

void DiffWidget::reload(const QString &sha, const QString &parentSha)
{

   mFullDiffWidget->loadDiff(sha, parentSha);
}
