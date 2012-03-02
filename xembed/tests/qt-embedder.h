#include <qmainwindow.h>
#include <qptrlist.h>

class QVBox;

class XEmbedQtEmbedder : public QMainWindow
{
  Q_OBJECT

 public:

  XEmbedQtEmbedder ();

 private:
  QVBox *vbox_;
  QPtrList<QWidget> embedders_;

  void addChild (bool active, bool isQt);

 private slots:
 
  void addActiveGtkChild ();
  void addPassiveGtkChild ();
  void addActiveQtChild ();
  void addPassiveQtChild ();
  void removeChild ();
  void maybeQuit ();
};
