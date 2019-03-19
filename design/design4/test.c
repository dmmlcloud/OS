#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "MenuBar.h"
#include "CPU.h"
#include "Memory.h"
#include "Process.h"
#include "System.h"
#include "Partition.h"
#include "network.h"

void CreateNotebook(GtkWidget *vbox);
void CreateStatusBar(GtkWidget *vbox);
gint refresh_statusbar(gpointer data);
GtkWidget *statusbar;

int main(int argc,char *argv[ ])
{
	GtkWidget *window;
	GtkWidget *main_vbox;

	/**/
	gtk_init(&argc,&argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "System monitor");
	gtk_widget_set_size_request(window,800,600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	gtk_widget_show(window);
	
	main_vbox = gtk_vbox_new(FALSE, 10);
	gtk_container_set_border_width(GTK_CONTAINER(main_vbox), 5);
	gtk_container_add(GTK_CONTAINER(window), main_vbox);
	gtk_widget_show(main_vbox);
	
	CreateMenuBar(main_vbox);
	CreateNotebook(main_vbox);
	CreateStatusBar(main_vbox);
	
	gtk_timeout_add(500, refresh_cpu, NULL);
	gtk_timeout_add(500, refresh_mem, NULL);
	gtk_timeout_add(10000, refresh_proc, NULL);
	gtk_widget_show_all(window);
	gtk_main();
}


void CreateNotebook(GtkWidget *vbox)
{
	GtkWidget *notebook;	
	
	notebook = gtk_notebook_new();
	gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
	gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
	gtk_widget_show(notebook);
	
	
	CreatePageCPU(notebook);
	CreatePageSystem(notebook);
	CreatePageMemory(notebook);
	CreatePageProcess(notebook);
	CreatePageDisk(notebook);
	CreatePageNet(notebook);
	
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
}

void CreateStatusBar(GtkWidget *vbox)
{
    gint context_id;
    statusbar = gtk_statusbar_new();
    gtk_box_pack_start(GTK_BOX(vbox), statusbar, TRUE, TRUE, 0);
    gtk_widget_show(statusbar);
    context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "Timestatus");
    gtk_timeout_add(1000, refresh_statusbar, GINT_TO_POINTER(context_id));
    refresh_statusbar(GINT_TO_POINTER(context_id));
}

gint refresh_statusbar(gpointer data)
{
	time_t t;
	t=time(NULL);
	char msg[1000], temp[100];
	char *buf, *p;
	buf = ctime(&t);
	p = strtok(buf, "\n");

	strcpy(msg, buf);
	sprintf(temp, "                     CPU used: %2.2lf%%", cpu_rate * 100);
	strcat(msg, temp);
	sprintf(temp, "                     Memory used: %2.2lf%%", memory_rate * 100);
	strcat(msg, temp);

	gtk_statusbar_push(GTK_STATUSBAR(statusbar), GPOINTER_TO_INT(data), msg);
}
