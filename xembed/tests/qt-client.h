#include <qmainwindow.h>

class XEmbedQtClient;

class XEmbedQtButtonBox : public QWidget
{
  Q_OBJECT

 public:
  
  XEmbedQtButtonBox (XEmbedQtClient *client);
};

class XEmbedQtClient : public QWidget
{
  Q_OBJECT

 public:

  XEmbedQtClient ();

 public slots:
   void addButtons ();

 private slots:
   void blink();
};
