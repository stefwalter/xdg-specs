#include <qxembed.h>

class XEmbedQtChildSite : public QXEmbed
{
  Q_OBJECT

 public:

  XEmbedQtChildSite (QWidget *parent, bool active, bool isQt);
};
