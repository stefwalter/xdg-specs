#include <kapp.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtimer.h>

#include <qxembed.h>

#include "qt-client.h"
#include "qt-common.h"

XEmbedQtButtonBox::XEmbedQtButtonBox (QWidget *parent, XEmbedQtClient *client)
  : QWidget (parent)
{
  QHBoxLayout *layout = new QHBoxLayout (this);
  layout->setAutoAdd (TRUE);
  
  QPushButton *button;

  button = new QPushButton( "&Add", this );
  QObject::connect (button, SIGNAL(clicked()), client, SLOT(addButtons()));
  
  button = new QPushButton( "&Remove", this );
  QObject::connect (button, SIGNAL(clicked()), this, SLOT(close()));
}


XEmbedQtClient::XEmbedQtClient ()
{
  setFrameStyle (QFrame::Box | QFrame::Sunken);
  
  QVBoxLayout *vlayout = new QVBoxLayout (this);

  vlayout->setAutoAdd (TRUE);
  vlayout->setMargin (5);
  
  hbox_ = new QHBox (this);

  new QLabel ("Qt", hbox_);
  
  new QLineEdit (hbox_);
  
  QPushButton *button;

  button = new QPushButton( "&Close", hbox_ );
  QObject::connect (button, SIGNAL(clicked()), this, SLOT(close()));

  button = new QPushButton( "&Blink", hbox_ );
  QObject::connect (button, SIGNAL(clicked()), this, SLOT(blink()));

  button = new QPushButton( "Add &GTK+", hbox_ );
  QObject::connect (button, SIGNAL(clicked()), this, SLOT(addGtkChild()));

  button = new QPushButton( "Add &Qt", hbox_ );
  QObject::connect (button, SIGNAL(clicked()), this, SLOT(addQtChild()));

  addButtons();
}

void
XEmbedQtClient::blink()
{
  hide ();

  QTimer *timer = new QTimer ( this );
  QObject::connect (timer, SIGNAL(timeout()), this, SLOT(show()));
  timer->start (1000, TRUE);
}

void
XEmbedQtClient::addGtkChild()
{
  new XEmbedQtChildSite (this, FALSE, FALSE);
}

void
XEmbedQtClient::addQtChild()
{
  new XEmbedQtChildSite (this, FALSE, TRUE);
}

void
XEmbedQtClient::addButtons ()
{
  XEmbedQtButtonBox *box = new XEmbedQtButtonBox (hbox_, this);
  box->show();
}

int main( int argc, char **argv )
{
  KApplication a( argc, argv, "qt-client" );
  WId embedder_xid, client_xid;
  XEmbedQtClient *client;
  
  if (argc != 1 && argc != 2)
    {
      fprintf (stderr, "Usage: qt-client [WINDOW_ID]\n");
      exit (1);
    }
  
  QXEmbed::initialize ();
  
  if (argc == 2)
    {
      embedder_xid = strtol (argv[1], NULL, 0);
      if (embedder_xid == 0)
	{
	  fprintf (stderr, "Invalid window id '%s'\n", argv[1]);
	  exit (1);
	}
      
      client = new XEmbedQtClient ();
      QXEmbed::embedClientIntoWindow (client, embedder_xid);
    }
  else
    {
      client = new XEmbedQtClient ();
      printf ("%d\n", client->winId());
      fflush (stdout);
    }
  
  a.setMainWidget( client );
   
  return a.exec();
}
