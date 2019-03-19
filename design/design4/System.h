#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/utsname.h>


void CreatePageSystem(GtkWidget *notebook);
void GetSysTime(char *runtime);
gint UpdateSystime(gpointer data);

void CreatePageSystem(GtkWidget *notebook)
{
    int rs;
    char starttime[100];
    GetSysTime(starttime);
    struct utsname u_name;
    rs = uname(&u_name);
    if(rs < 0)
    {
        printf("uname() failed\n");
        exit(1);
    }
    
    char sys_info[1000];
    strcpy(sys_info, "sysname:                         ");
	strcat(sys_info, u_name.sysname);
	strcat(sys_info, "\nnodename:                     ");
	strcat(sys_info, u_name.nodename);
	strcat(sys_info, "\nrelease:                            ");
	strcat(sys_info, u_name.release);
	strcat(sys_info, "\nversion:                            ");
	strcat(sys_info, u_name.version);
	strcat(sys_info, "\nmachine:                          ");
	strcat(sys_info, u_name.machine);
	
    GtkWidget *label;
    GtkWidget *frame;
    GtkWidget *vbox;
    GtkWidget *timelabel;
    
    frame = gtk_frame_new("");
    gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
    gtk_widget_show(frame);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame, gtk_label_new("System Information"));
    
    vbox = gtk_vbox_new(FALSE, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    gtk_widget_show(vbox);
    
    label = gtk_label_new(sys_info);
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);
    gtk_widget_show(label);
    
    timelabel = gtk_label_new(starttime);
	gtk_box_pack_start(GTK_BOX(vbox), timelabel, TRUE, TRUE, 0);
	gtk_widget_show(timelabel);

	gtk_timeout_add(1000, UpdateSystime, timelabel);
}

void GetSysTime(char *starttime)
{
	char buf[50], *ch;
	time_t nowtime;
	struct tm *p;
	int i_time, i_runtime,i_stime;
	
	time(&nowtime);
	p = localtime(&nowtime);
	i_time = p->tm_hour*3600 + p->tm_min*60+p->tm_sec;

	int fd = open("/proc/uptime", O_RDONLY);
	read(fd, buf, sizeof(buf));
	close(fd);
	ch = strtok(buf, " ");
	i_runtime = atoi(ch);

	i_stime = i_time - i_runtime; 
	int hour, min, sec;
	char temp[2];
	hour = i_stime / 3600;
	min = (i_stime - 3600 * hour) / 60;
	sec = i_stime - 3600*hour - 60*min;

	strcpy(starttime, "System start from : ");
	sprintf(temp, "%02d", hour);
	strcat(starttime, temp);
	strcat(starttime, ":");
	sprintf(temp, "%02d", min);
	strcat(starttime, temp);
	strcat(starttime, ":");
	sprintf(temp, "%02d", sec);
	strcat(starttime, temp);
	strcat(starttime, "\n");

	hour = i_runtime / 3600;
	min = (i_runtime - 3600 * hour) / 60;
	sec = i_runtime - 3600*hour - 60*min;
	strcat(starttime, "System runing for : ");
	sprintf(temp, "%02d", hour);
	strcat(starttime, temp);
	strcat(starttime, ":");
	sprintf(temp, "%02d", min);
	strcat(starttime, temp);
	strcat(starttime, ":");
	sprintf(temp, "%02d", sec);
	strcat(starttime, temp);
	strcat(starttime, "\n\0");

	//printf("%s\n", starttime);
}

gint UpdateSystime(gpointer data)
{
	char systime[100];
	GetSysTime(systime);
	gtk_label_set_text(GTK_LABEL(data), systime);
	return TRUE;
}
