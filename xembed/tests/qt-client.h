#include <qframe.h>

class QHBox;

class XEmbedQtClient;

class XEmbedQtButtonBox : public QWidget
{
  Q_OBJECT

 public:
  
  XEmbedQtButtonBox (QWidget *parent, XEmbedQtClient *client);
};

class XEmbedQtClient : public QFrame
{
  Q_OBJECT

 public:

  XEmbedQtClient ();

 public slots:
   void addButtons ();

 private:
   QHBox *hbox_;
  
 private slots:
   void blink();
   void addGtkChild();
   void addQtChild();
};
