#include <kapp.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtimer.h>

#include <qxembed.h>

#include "qt-client.h"

XEmbedQtButtonBox::XEmbedQtButtonBox (XEmbedQtClient *client)
  : QWidget (client)
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
  QHBoxLayout *layout = new QHBoxLayout (this);

  layout->setAutoAdd (TRUE);
  
  new QLabel ("Qt", this);
  
  new QLineEdit (this);
  
  QPushButton *button;

  button = new QPushButton( "&Close", this );
  QObject::connect (button, SIGNAL(clicked()), this, SLOT(close()));

  button = new QPushButton( "&Blink", this );
  QObject::connect (button, SIGNAL(clicked()), this, SLOT(blink()));

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
XEmbedQtClient::addButtons ()
{
  XEmbedQtButtonBox *box = new XEmbedQtButtonBox (this);
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
