typedef struct _Child Child;
typedef struct _Socket Socket;

struct _Socket
{
  GtkWidget *box;
  GtkWidget *frame;
  GtkWidget *socket;

  Child *child;
};


void wait_for_children (void);

Socket *socket_new         (void);
void socket_start_child (Socket   *socket,
			 gboolean  active,
			 gboolean  qt);



