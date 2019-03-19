#define PROC_PID_POS 1
#define PROC_NAME_POS 2
#define PROC_STATE_POS 3
#define PROC_CPU_UTIME_POS 14
#define PROC_CPU_STIME_POS 15
#define PROC_PRIORITY_POS 18
#define PROC_START_TIME 22
#define PROC_VMSIZE_POS 24

void CreatePageProcess(GtkWidget *notebook);
void ShowProcInfo(void);
void ReadProc(const char *path);
void SearchProcByPID(gpointer data);
void ProcessRun(gpointer data);
void SetEntryInfo(GtkWidget *clist, gint row, gint column, GdkEventButton *event, gpointer data);
void Sort(int item);
gint refresh_proc(gpointer data);
void SetOrder(GtkCList *clist, gint column, gpointer data);
GtkWidget *clist;
static GtkWidget *entry;
gint select_num = 1;

typedef struct proc_info{
	char name[300];
	char pid[30];
	char state[30];
	char mem[30];
	char priority[30];
	char cpu[30];
	int pd;
	int prior;
	int memory;
	int vmsize;
	float cpuratio;
	float starttime;
	float utime;
	float stime;
    float cputime;
    struct proc_info *next; 
}proc_info;

proc_info *pi = NULL;
proc_info *pn = NULL;

void CreatePageProcess(GtkWidget *notebook)
{
    GtkWidget *vbox;
    GtkWidget *scroll;
    GtkWidget *table;
    GtkWidget *button_1, *button_3;
    
    table = gtk_table_new(10, 10, TRUE);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), table, gtk_label_new("Processes Information"));
    gtk_widget_show(table);
    
    gchar *item_name[6] = {"Processname", "PID", "state", "CPU(%)", "Mem(KB)", "Priority"};
    
    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_table_attach_defaults(GTK_TABLE(table), scroll, 0, 10, 0, 9);
    gtk_widget_show(scroll);
    
    clist = gtk_clist_new_with_titles(6, item_name);
    g_signal_connect(GTK_OBJECT(clist), "select_row", GTK_SIGNAL_FUNC(SetEntryInfo), NULL);
    g_signal_connect(GTK_OBJECT(clist),"click_column",GTK_SIGNAL_FUNC(SetOrder),NULL);
    //g_signal_connect
    gtk_clist_set_column_width(GTK_CLIST(clist), 0, 200);
    gtk_clist_set_column_width(GTK_CLIST(clist), 1, 100);
    gtk_clist_set_column_width(GTK_CLIST(clist), 2, 100);
    gtk_clist_set_column_width(GTK_CLIST(clist), 3, 100);
    gtk_clist_set_column_width(GTK_CLIST(clist), 4, 100);
    gtk_clist_set_column_width(GTK_CLIST(clist), 5, 100);
    
    gtk_container_add(GTK_CONTAINER(scroll), clist);
    gtk_widget_show(clist);
    
    entry = gtk_entry_new();
    button_1 = gtk_button_new_with_label("Search by PID");
    gtk_container_set_border_width(GTK_CONTAINER(button_1), 9);
    button_3 = gtk_button_new_with_label("Refresh");
    gtk_container_set_border_width(GTK_CONTAINER(button_3), 9);
    gtk_table_attach_defaults(GTK_TABLE(table), entry, 0, 4, 9, 10);
	gtk_table_attach_defaults(GTK_TABLE(table), button_1, 4, 6, 9, 10);
	gtk_table_attach_defaults(GTK_TABLE(table), button_3, 8, 10, 9, 10);
	gtk_widget_show(entry);
	gtk_widget_show(button_1);
	gtk_widget_show(button_3);
	g_signal_connect(G_OBJECT(button_1), "clicked", G_CALLBACK(SearchProcByPID), NULL);
	g_signal_connect(G_OBJECT(button_3), "clicked", G_CALLBACK(ShowProcInfo), NULL);
	
	ShowProcInfo();
}

void SetOrder(GtkCList *clist, gint column, gpointer data)
{
    select_num = column;
    Sort(select_num);
    return;
}
void Sort(int item)
{
    proc_info *p;
    proc_info *f;
    switch(item)
    {
        case 0://Name
        {
            proc_info *bp = pi;
            for(p = pi->next; p != NULL; p = p->next)
            {
                proc_info *b = p;
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
            proc_info *bp = pi;
            for(p = pi->next; p != NULL; p = p->next)
            {
                proc_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(f->pd < p->pd)
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
        case 2://state
        {
            proc_info *bp = pi;
            for(p = pi->next; p != NULL; p = p->next)
            {
                proc_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(f->state[0] < p->state[0])
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
            proc_info *bp = pi;
            for(p = pi->next; p != NULL; p = p->next)
            {
                proc_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(f->cpuratio < p->cpuratio)
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
        case 4://mem
        {
            proc_info *bp = pi;
            for(p = pi->next; p != NULL; p = p->next)
            {
                proc_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(f->memory< p->memory)
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
        case 5://prior
        {
            proc_info *bp = pi;
            for(p = pi->next; p != NULL; p = p->next)
            {
                proc_info *b = p;
                for(f = p->next; f != NULL; f = f->next)
                {
                    if(f->prior < p->prior)
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
    gtk_clist_freeze(GTK_CLIST(clist));
    gtk_clist_clear(GTK_CLIST(clist));
    proc_info *s;
    for(s = pi->next; s != NULL; s = s->next)
    {
        gchar *list[6] = {s->name, s->pid, s->state, s->cpu, s->mem, s->priority};
        gtk_clist_append((GtkCList *) clist, list);
    }
    gtk_clist_thaw(GTK_CLIST(clist));
}

gint refresh_proc(gpointer data)
{
	ShowProcInfo();
}

void ShowProcInfo(void)
{
    DIR *dir;
    struct dirent *dic;
    dir = opendir("/proc");
    
    if(pi != NULL)
	{
		pn = pi->next;
		while(1)
		{
			free(pi);
			pi = pn;
			if(pn == NULL)
				break;
			pn = pn->next;
		}
	}
	gtk_clist_clear(GTK_CLIST(clist));
	pi = (proc_info *)malloc(sizeof(proc_info));
        pi->next = NULL;
        pn = pi;
    while((dic = readdir(dir)) != NULL)
    {
        if((dic->d_name[0] > '0') && (dic->d_name[0] <= '9'))
        {
            ReadProc(dic->d_name);
        }
    }
    Sort(select_num);
    closedir(dir);
	return;
}

void ReadProc(const char *path)
{
    FILE *fp = NULL;
    char file[500];
    char buff[1000];
    int pd;
    char name[300];
    char state[50];
    float utime;
    float stime;
    int prior;
    float starttime;
    int vmsize;
    
    
    sprintf(file, "/proc/%s/stat", path);
    if((fp = fopen(file, "r")) == NULL)
    {
        printf("open %s failed\n", file);
        return;
    }
    int i;
    char buf[30];
    for(i = 1; i <= PROC_VMSIZE_POS; i++)
    {
        switch(i){
            case PROC_PID_POS:
            {
                fscanf(fp, "%d", &pd);
                break;
            }
            case PROC_NAME_POS:
            {
                fscanf(fp, "%s", name);
                break;
            } 
            case PROC_STATE_POS:
            {
                fscanf(fp, "%s", state);
                break;
            }
            case PROC_CPU_UTIME_POS:
            {
                fscanf(fp, "%f", &utime);
                break;
            }
            case PROC_CPU_STIME_POS:
            {
                fscanf(fp, "%f", &stime);
                break;
            }
            case PROC_PRIORITY_POS:
            {
                fscanf(fp, "%d", &prior);
                break;
            }
            case PROC_START_TIME:
            {
                fscanf(fp, "%f", &starttime);
                break;
            }
            case PROC_VMSIZE_POS:
            {
                fscanf(fp, "%d", &vmsize);
                break;
            }
            default:
		fscanf(fp,"%s",buf);
                break;
        }
    }
    fclose(fp);
    
    if(prior <= 0)
    {
	pn->next = (proc_info *)malloc(sizeof(proc_info));
        pn = pn->next;
        pn->next = NULL;
	pn->pd = pd;
    	strcpy(pn->name, name);
    	strcpy(pn->state,state);
    	pn->stime = stime;
   	 pn->utime = utime;
   	 pn->prior = prior;
   	 pn->starttime = starttime;
   	 pn->vmsize = vmsize;
    }
    
    
    sprintf(pn->pid,"%d", pn->pd);
    sprintf(pn->priority,"%d", pn->prior);
    //memory
    pn->memory = (pn->vmsize * sysconf(_SC_PAGE_SIZE) / 1024);
    sprintf(pn->mem,"%d", pn->memory);
    
    //cpuratio
    pn->cputime = pn->utime + pn->stime;
    float hz = sysconf(_SC_CLK_TCK);
    
    float runtime;
    if((fp = fopen("/proc/uptime", "r")) == NULL)
    {
        printf("open %s failed\n", file);
        return;
    }
    fscanf(fp, "%f", &runtime);
    fclose(fp);
    
    float seconds = runtime - (pn->starttime / hz);
    float pcpu = ((pn->cputime * 1000ULL) / hz) / seconds;
    pn->cpuratio = pcpu;
    sprintf(pn->cpu, "%f", pcpu);
    
    //print
    /*gchar *list[6] = {pn->name, pn->pid, pn->state, pn->cpu, pn->mem, pn->priority};
    gtk_clist_append((GtkCList *) clist, list);*/
}

void SearchProcByPID(gpointer data)
{
	const gchar *pid;
	pid = gtk_entry_get_text(GTK_ENTRY(entry));

	int i;
	FILE *fp = NULL;
	char file[500];
	char buf[9][256];
	char msg[300];

	sprintf(file, "/proc/%s/status", pid);
	if(!(fp = fopen(file, "r")))
	{
		strcpy(msg, "pid is wrong\nplease input the right id of process!\n");
		GtkWidget *dialog ;
		dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, msg, "");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		return;
	}
	for(i = 0; i < 9; i++)
	{
		fgets(buf[i], sizeof(buf[i]), fp);
	}
	strcpy(msg, buf[0]);
	for(i = 1; i < 9; i++)
		strcat(msg, buf[i]);


	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, msg, "");
	gtk_window_set_title(GTK_WINDOW(dialog), pid);
	GtkResponseType result = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	if (result == GTK_RESPONSE_YES || result == GTK_RESPONSE_APPLY)
	{
		//Kill the process
		gchar cmd_kill[50];
		strcpy(cmd_kill, "kill ");
		strcat(cmd_kill, pid);
		system(cmd_kill);
		ShowProcInfo();
	}

	return;
}

void ProcessRun(gpointer data)
{
	const gchar *name;
	name = gtk_entry_get_text(GTK_ENTRY(entry));
	int id;
	printf("%s", name);
	if((id = fork()) == 0)
	{
		system(name);
		return;
	}
	return;
}

void SetEntryInfo(GtkWidget *clist, gint row, gint column, GdkEventButton *event, gpointer data)
{
	gchar *pid;
	gtk_clist_get_text(GTK_CLIST(clist), row, 1, &pid);
	gtk_entry_set_text(GTK_ENTRY(entry), pid);
}
