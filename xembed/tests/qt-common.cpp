#include "qt-common.h"

#include <qprocess.h>

XEmbedQtChildSite::XEmbedQtChildSite (QWidget *parent, bool active, bool isQt)
  : QXEmbed (parent)
{
  show ();
  
  QProcess *proc = new QProcess (this);

  proc->setCommunication (QProcess::Stdout);
    
  if (isQt)
    proc->addArgument ("./qt-client");
  else
    proc->addArgument ("./gtk-client");

  if (active)
    proc->addArgument (QString::number (winId ()));

  proc->start ();

  if (!active)
    {
      QString pid_str = proc->readLineStdout ();
      embed (pid_str.toLong ());
    }
}

