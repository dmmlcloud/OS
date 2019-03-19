void CreatePageDisk(GtkWidget *notebook);
gint ShowDiskInfo(gpointer data);
void DiskSort(int item);
void DiskSetOrder(GtkCList *clist, gint column, gpointer data);

typedef struct disk_info{
	char name[20];
  char major[20];
  char minor[20];
	char blocks[20];
  struct disk_info *next;
}disk_info;

int select_disk_num = -1;
GtkWidget *clistdisk;

disk_info *di = NULL;
disk_info *dn = NULL;

void CreatePageDisk(GtkWidget *notebook)
{
	GtkWidget *vbox, *scroll, *table;

	table = gtk_table_new(10, 10, TRUE);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), table, gtk_label_new("Disk Information"));
	gtk_widget_show(table);


	gchar *colname[4] = {"name", "major", "minor", "blocks"};

	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	//gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll, gtk_label_new("Page3-process"));
	gtk_table_attach_defaults(GTK_TABLE(table), scroll, 0, 10, 0, 9);


	clistdisk = gtk_clist_new_with_titles(4, colname);
	gtk_clist_set_column_width(GTK_CLIST(clistdisk), 0, 180);
	gtk_clist_set_column_width(GTK_CLIST(clistdisk), 1, 180);
	gtk_clist_set_column_width(GTK_CLIST(clistdisk), 2, 180);
	gtk_clist_set_column_width(GTK_CLIST(clistdisk), 3, 180);

  g_signal_connect(GTK_OBJECT(clistdisk),"click_column",GTK_SIGNAL_FUNC(DiskSetOrder),NULL);

	gtk_container_add(GTK_CONTAINER(scroll), clistdisk);
	gtk_widget_show(clistdisk);


	gtk_timeout_add(10000, ShowDiskInfo, NULL);

	//commondbutton
	ShowDiskInfo(NULL);
}
void DiskSetOrder(GtkCList *clist, gint column, gpointer data)
{
	select_disk_num = column;
	DiskSort(select_disk_num);
}

void DiskSort(int item)
{
    disk_info *p;
    disk_info *f;
    switch(item)
    {
        case 0://Name
        {
            disk_info *bp = di;
            for(p = di->next; p != NULL; p = p->next)
            {
                disk_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(f->name[1] < p->name[1])
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
        case 1://pid
        {
            disk_info *bp = di;
            for(p = di->next; p != NULL; p = p->next)
            {
                disk_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(atoi(f->major) < atoi(p->major))
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
        case 2://ppid
        {
            disk_info *bp = di;
            for(p = di->next; p != NULL; p = p->next)
            {
                disk_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(atoi(f->minor) < atoi(p->minor))
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
        case 3://cpu
        {
            disk_info *bp = di;
            for(p = di->next; p != NULL; p = p->next)
            {
                disk_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(atoi(f->blocks) < atoi(p->blocks))
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
    gtk_clist_freeze(GTK_CLIST(clistdisk));
    gtk_clist_clear(GTK_CLIST(clistdisk));
    disk_info *s;
    for(s = di->next; s != NULL; s = s->next)
    {
        gchar *list[4] = {s->name, s->major, s->minor, s->blocks};
        gtk_clist_append((GtkCList *) clistdisk, list);
    }
    gtk_clist_thaw(GTK_CLIST(clistdisk));
}

gint ShowDiskInfo(gpointer data)
{
	FILE *fp;
  if(di != NULL)
    {
      dn = di->next;
      while(1)
      {
        free(di);
        di = dn;
        if(dn == NULL)
          break;
        dn = dn->next;
      }
    }
    di = (disk_info *)malloc(sizeof(disk_info));
    di->next = NULL;
    dn = di;

	fp = fopen("/proc/partitions","r");
    char buf[256];
	gtk_clist_clear(GTK_CLIST(clistdisk));
    fgets(buf,sizeof(buf),fp);
    fgets(buf,sizeof(buf),fp);
	while (fgets(buf,sizeof(buf),fp) != NULL)
	{
        dn->next = (disk_info *)malloc(sizeof(disk_info));
      	dn = dn->next;
      	dn->next = NULL;
        sscanf(buf,"%s %s %s %s",dn->major,dn->minor,dn->blocks,dn->name);
	}
    fclose(fp);
    DiskSort(select_disk_num);
    return TRUE;
}
