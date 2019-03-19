void CreatePageNet(GtkWidget *notebook);
gint ShowNetInfo(gpointer data);
void NetSortTransmit(int item);
void NetSetOrderTransmit(GtkCList *clist, gint column, gpointer data);
void NetSortReceive(int item);
void NetSetOrderReceive(GtkCList *clist, gint column, gpointer data);

typedef struct net_info{
	char port[20];
  char bytes[20];
  char packets[20];
	char errors[20];
	char drops[20];
  struct net_info *next;
}net_info;


int select_net_receive_num = 0;
int select_net_transmit_num = 0;

GtkWidget *clistnet_receive;
GtkWidget *clistnet_transmit;

net_info *ni = NULL;
net_info *nn = NULL;

net_info *ti = NULL;
net_info *tn = NULL;

void CreatePageNet(GtkWidget *notebook)
{
	GtkWidget *scroll_receive, *scroll_transmit, *table, *frame_receive, *frame_transmit;

	table = gtk_table_new(10, 10, TRUE);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), table, gtk_label_new("Network"));
	gtk_widget_show(table);

    frame_receive = gtk_frame_new("Received");
    gtk_table_attach_defaults(GTK_TABLE(table), frame_receive, 0, 10, 0, 5);
    gtk_widget_show(frame_receive);
    
    frame_transmit = gtk_frame_new("Transmit");
    gtk_table_attach_defaults(GTK_TABLE(table), frame_transmit, 0, 10, 5, 10);
    gtk_widget_show(frame_transmit);
    
	gchar *colname[5] = {"port", "bytes", "packets", "errors", "drops"};
	scroll_receive = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_receive), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_container_add(GTK_CONTAINER(frame_receive), scroll_receive);
	
	
	scroll_transmit = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_transmit), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_container_add(GTK_CONTAINER(frame_transmit), scroll_transmit);


	clistnet_receive = gtk_clist_new_with_titles(5, colname);
	gtk_clist_set_column_width(GTK_CLIST(clistnet_receive), 0, 140);
	gtk_clist_set_column_width(GTK_CLIST(clistnet_receive), 1, 140);
	gtk_clist_set_column_width(GTK_CLIST(clistnet_receive), 2, 140);
	gtk_clist_set_column_width(GTK_CLIST(clistnet_receive), 3, 140);
	gtk_clist_set_column_width(GTK_CLIST(clistnet_receive), 4, 140);
	
	clistnet_transmit = gtk_clist_new_with_titles(5, colname);
	gtk_clist_set_column_width(GTK_CLIST(clistnet_transmit), 0, 140);
	gtk_clist_set_column_width(GTK_CLIST(clistnet_transmit), 1, 140);
	gtk_clist_set_column_width(GTK_CLIST(clistnet_transmit), 2, 140);
	gtk_clist_set_column_width(GTK_CLIST(clistnet_transmit), 3, 140);
	gtk_clist_set_column_width(GTK_CLIST(clistnet_transmit), 4, 140);

    g_signal_connect(GTK_OBJECT(clistnet_receive),"click_column",GTK_SIGNAL_FUNC(NetSetOrderReceive),NULL);
    g_signal_connect(GTK_OBJECT(clistnet_transmit),"click_column",GTK_SIGNAL_FUNC(NetSetOrderTransmit),NULL);

	gtk_container_add(GTK_CONTAINER(scroll_receive), clistnet_receive);
	gtk_widget_show(clistnet_receive);
    gtk_container_add(GTK_CONTAINER(scroll_transmit), clistnet_transmit);
	gtk_widget_show(clistnet_transmit);

    gtk_widget_show(table);
    
	gtk_timeout_add(10000, ShowNetInfo, NULL);

	//commondbutton
	ShowNetInfo(NULL);
}

void NetSetOrderTransmit(GtkCList *clist, gint column, gpointer data)
{
	select_net_transmit_num = column;
	NetSortTransmit(select_net_transmit_num);
}

void NetSortTransmit(int item)
{
    net_info *p;
    net_info *f;
    switch(item)
    {
        case 0://ports
        {
            net_info *bp = ti;
            for(p = ti->next; p != NULL; p = p->next)
            {
                net_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(f->port[0] < p->port[0])
                    {
                        b->next = f->next;
                        f->next = p;
                        bp->next = f;
                        p = f;
                        f = b;
			            continue;
                    }
                    b = b->next;
                }
                bp = bp->next;
            }
	    break;
        }
        case 1://bytes
        {
            net_info *bp = ti;
            for(p = ti->next; p != NULL; p = p->next)
            {
                net_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(atoi(f->bytes) < atoi(p->bytes))
                    {
                        b->next = f->next;
                        f->next = p;
                        bp->next = f;
                        p = f;
			f = b;
			continue;
                    }
                    b = b->next;
                }
                bp = bp->next;
            }
	break;
        }
        case 2://packets
        {
            net_info *bp = ti;
            for(p = ti->next; p != NULL; p = p->next)
            {
                net_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(atoi(f->packets) < atoi(p->packets))
                    {
                        b->next = f->next;
                        f->next = p;
                        bp->next = f;
                        p = f;
			f = b;
			continue;
                    }
                    b = b->next;
                }
                bp = bp->next;
            }
	break;
        }
        case 3://errors
        {
            net_info *bp = ti;
            for(p = ti->next; p != NULL; p = p->next)
            {
                net_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(atoi(f->errors) < atoi(p->errors))
                    {
                        b->next = f->next;
                        f->next = p;
                        bp->next = f;
                        p = f;
			f = b;
			continue;
                    }
                    b = b->next;
                }
                bp = bp->next;
            }
        break;
        }
        case 4://drops
        {
            net_info *bp = ti;
            for(p = ti->next; p != NULL; p = p->next)
            {
                net_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(atoi(f->drops) < atoi(p->drops))
                    {
                        b->next = f->next;
                        f->next = p;
                        bp->next = f;
                        p = f;
			f = b;
			continue;
                    }
                    b = b->next;
                }
                bp = bp->next;
            }
        break;
        }
        default:
            break;
    }
    gtk_clist_freeze(GTK_CLIST(clistnet_transmit));
    gtk_clist_clear(GTK_CLIST(clistnet_transmit));
    net_info *s;
    for(s = ti->next; s != NULL; s = s->next)
    {
        gchar *list[5] = {s->port, s->bytes, s->packets, s->errors, s->drops};
        gtk_clist_append((GtkCList *) clistnet_transmit, list);
    }
    gtk_clist_thaw(GTK_CLIST(clistnet_transmit));
}

void NetSetOrderReceive(GtkCList *clist, gint column, gpointer data)
{
	select_net_receive_num = column;
	NetSortReceive(select_net_receive_num);
}

void NetSortReceive(int item)
{
    net_info *p;
    net_info *f;
    switch(item)
    {
        case 0://ports
        {
            net_info *bp = ni;
            for(p = ni->next; p != NULL; p = p->next)
            {
                net_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(f->port[0] < p->port[0])
                    {
                        b->next = f->next;
                        f->next = p;
                        bp->next = f;
                        p = f;
                        f = b;
			            continue;
                    }
                    b = b->next;
                }
                bp = bp->next;
            }
	    break;
        }
        case 1://bytes
        {
            net_info *bp = ni;
            for(p = ni->next; p != NULL; p = p->next)
            {
                net_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(atoi(f->bytes) < atoi(p->bytes))
                    {
                        b->next = f->next;
                        f->next = p;
                        bp->next = f;
                        p = f;
			f = b;
			continue;
                    }
                    b = b->next;
                }
                bp = bp->next;
            }
	break;
        }
        case 2://packets
        {
            net_info *bp = ni;
            for(p = ni->next; p != NULL; p = p->next)
            {
                net_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(atoi(f->packets) < atoi(p->packets))
                    {
                        b->next = f->next;
                        f->next = p;
                        bp->next = f;
                        p = f;
			f = b;
			continue;
                    }
                    b = b->next;
                }
                bp = bp->next;
            }
	break;
        }
        case 3://errors
        {
            net_info *bp = ni;
            for(p = ni->next; p != NULL; p = p->next)
            {
                net_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(atoi(f->errors) < atoi(p->errors))
                    {
                        b->next = f->next;
                        f->next = p;
                        bp->next = f;
                        p = f;
			f = b;
			continue;
                    }
                    b = b->next;
                }
                bp = bp->next;
            }
        break;
        }
        case 4://drops
        {
            net_info *bp = ni;
            for(p = ni->next; p != NULL; p = p->next)
            {
                net_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(atoi(f->drops) < atoi(p->drops))
                    {
                        b->next = f->next;
                        f->next = p;
                        bp->next = f;
                        p = f;
			f = b;
			continue;
                    }
                    b = b->next;
                }
                bp = bp->next;
            }
        break;
        }
        default:
            break;
    }
    gtk_clist_freeze(GTK_CLIST(clistnet_receive));
    gtk_clist_clear(GTK_CLIST(clistnet_receive));
    net_info *s;
    for(s = ni->next; s != NULL; s = s->next)
    {
        gchar *list[5] = {s->port, s->bytes, s->packets, s->errors, s->drops};
        gtk_clist_append((GtkCList *) clistnet_receive, list);
    }
    gtk_clist_thaw(GTK_CLIST(clistnet_receive));
}

gint ShowNetInfo(gpointer data)
{
	FILE *fp;
  if(ni != NULL)
    {
      nn = ni->next;
      while(1)
      {
        free(ni);
        ni = nn;
        if(nn == NULL)
          break;
        nn = nn->next;
      }
    }
    ni = (net_info *)malloc(sizeof(net_info));
    ni->next = NULL;
    nn = ni;

  if(ti != NULL)
    {
      tn = ti->next;
      while(1)
      {
        free(ti);
        ti = tn;
        if(tn == NULL)
          break;
        tn = tn->next;
      }
    }
    ti = (net_info *)malloc(sizeof(net_info));
    ti->next = NULL;
    tn = ti;

	fp = fopen("/proc/net/dev","r");
    char buf[1024];
    char buf_1[50];
    char buf_2[50];
    char buf_3[50];
    char buf_4[50];
	gtk_clist_clear(GTK_CLIST(clistnet_receive));
	gtk_clist_clear(GTK_CLIST(clistnet_transmit));
    fgets(buf,sizeof(buf),fp);
    fgets(buf,sizeof(buf),fp);
	while (fgets(buf,sizeof(buf),fp) != NULL)
	{
        nn->next = (net_info *)malloc(sizeof(net_info));
      	nn = nn->next;
      	nn->next = NULL;
      	tn->next = (net_info *)malloc(sizeof(net_info));
      	tn = tn->next;
      	tn->next = NULL;
        sscanf(buf,"%s %s %s %s %s %s %s %s %s %s %s %s %s", nn->port, nn->bytes, nn->packets, nn->errors, nn->drops, buf_1, buf_2, buf_3, buf_4, tn->bytes, tn->packets, tn->errors, tn->drops);
        nn->port[sizeof(nn->port) - 1] = '\0';
        strcpy(tn->port, nn->port);
	}
    fclose(fp);
    NetSortReceive(select_net_receive_num);
    NetSortTransmit(select_net_transmit_num);
    return TRUE;
}
