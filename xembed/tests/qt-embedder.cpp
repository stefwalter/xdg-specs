#include <kapp.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qprocess.h>
#include <qpushbutton.h>
#include <qvbox.h>

#include <qxembed.h>

#include "qt-embedder.h"

XEmbedQtEmbedder::XEmbedQtEmbedder ()
{
  // Create menu
  QPopupMenu *file_menu = new QPopupMenu (this);
  
  menuBar()->insertItem ("&File", file_menu);

  file_menu->insertItem( "&Quit", this, SLOT( maybeQuit() ), CTRL+Key_Q );

  vbox_ = new QVBox (this);

  setCentralWidget (vbox_);

  QPushButton *button;

  button = new QPushButton ("Add Active GTK+ Child", vbox_);
  QObject::connect (button, SIGNAL(clicked()), this, SLOT(addActiveGtkChild()));
  
  button = new QPushButton ("Add Passive GTK+ Child", vbox_);
  QObject::connect (button, SIGNAL(clicked()), this, SLOT(addPassiveGtkChild()));

  button = new QPushButton ("Add Active Qt Child", vbox_);
  QObject::connect (button, SIGNAL(clicked()), this, SLOT(addActiveQtChild()));
    
  button = new QPushButton ("Add Passive Qt Child", vbox_);
  QObject::connect (button, SIGNAL(clicked()), this, SLOT(addPassiveQtChild()));

  button = new QPushButton ("Remove Last Child", vbox_);
  QObject::connect (button, SIGNAL(clicked()), this, SLOT(removeChild()));
}


void
XEmbedQtEmbedder::addChild (bool active, bool isQt)
{
  QXEmbed *embedder = new QXEmbed (vbox_);
  embedder->show ();

  embedders_.append (embedder);

  QProcess *proc = new QProcess (this);

  proc->setCommunication (QProcess::Stdout);
    
  if (isQt)
    proc->addArgument ("./qt-client");
  else
    proc->addArgument ("./gtk-client");

  if (!active)
    proc->addArgument (QString::number (embedder->winId ()));

  proc->start ();

  if (active)
    {
      QString pid_str = proc->readLineStdout ();
      embedder->embed (pid_str.toLong ());
    }
}

void
XEmbedQtEmbedder::addActiveGtkChild ()
{
  addChild (TRUE, FALSE);
}

void
XEmbedQtEmbedder::addPassiveGtkChild ()
{
  addChild (TRUE, FALSE);
}

void
XEmbedQtEmbedder::addActiveQtChild ()
{
  addChild (TRUE, TRUE);
}

void
XEmbedQtEmbedder::addPassiveQtChild ()
{
  addChild (FALSE, TRUE);
}

void
XEmbedQtEmbedder::removeChild ()
{
  if (!embedders_.isEmpty ())
    {
      embedders_.getLast()->close();
      embedders_.removeLast();
    }
}

void
XEmbedQtEmbedder::maybeQuit ()
{
  QMessageBox mb( "qt-embedder", "Really Quit?",
		  QMessageBox::Information,
		  QMessageBox::Yes | QMessageBox::Default,
		  QMessageBox::Cancel | QMessageBox::Escape,
		  0, this );
  mb.setButtonText( QMessageBox::Yes, "&Quit" );
  mb.setButtonText( QMessageBox::No, "Cancel");
  
  if (mb.exec() == QMessageBox::Yes)
    {
      qApp->closeAllWindows ();
    }
}

int main( int argc, char **argv )
{
  KApplication a( argc, argv, "qt-embedder" );

  XEmbedQtEmbedder *embedder = new XEmbedQtEmbedder ();
  
  a.setMainWidget( embedder );
  embedder->show();
  
  return a.exec();
}
