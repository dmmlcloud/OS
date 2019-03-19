#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>

pid_t p1;
pid_t p2;
pid_t p3;

GtkWidget *label_1;
GtkWidget *label_2;
GtkWidget *label_3;

char time_s[40];
char file_s[200];
char count_s[20];
int sum = 0;
int counter = 1;

FILE *fp;

void *subp1(){
    time_t t;
    struct tm *lt;
    while(counter <= 1000)
    {
        time(&t);
        lt = localtime(&t);
        sprintf(time_s,"%d/%d/%d/ %d:%d:%d", lt->tm_year+1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
        gtk_label_set_text(GTK_LABEL(label_1),time_s);
        sleep(1);
    }
    return NULL;
}

void *subp2(){
    while(counter <= 1000)
    {
        char buf[200];
        if(fgets(file_s, 201, fp) == NULL)
            rewind(fp);
        gtk_label_set_text(GTK_LABEL(label_2),file_s);
        sleep(1);
    }
    return NULL;
}

void *subp3(){
    while(counter <= 1000)
    {
        sum = sum + counter;
        sprintf(count_s,"%d", sum);
        gtk_label_set_text(GTK_LABEL(label_3),count_s);
        counter++;
        sleep(1);
    }
    return NULL;
}

void on_button_clicked(GtkWidget *button, gpointer userdata)
{
    gtk_main_quit();
}

void destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}
 
int main(int argc,char *argv[])
{
    void *status;
    fp = fopen("/etc/fstab", "r");
    pthread_t p1, p2, p3;
    //process 1
    if((p1 = fork()) == -1)
    {
        printf("Create process1 failed");
        exit(1);
    }
    
    if(p1 == 0)
    {
        GtkWidget *window_1;
        GtkWidget *button_1;
        GtkWidget *vbox_1;
        gtk_init(&argc,&argv);
        vbox_1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_set_border_width (GTK_CONTAINER (vbox_1), 100);
        window_1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        button_1 = gtk_button_new_with_label("Exit");
        gtk_window_set_title(GTK_WINDOW(window_1),"Time");
        g_signal_connect(G_OBJECT(button_1), "clicked", G_CALLBACK(on_button_clicked), NULL);
        g_signal_connect(G_OBJECT(window_1), "destroy", G_CALLBACK(destroy), NULL);
        label_1 = gtk_label_new("Hello, World");
        gtk_window_set_default_size(GTK_WINDOW(window_1), 200, 200);
        gtk_window_set_position(GTK_WINDOW(window_1), GTK_WIN_POS_CENTER);
        gtk_box_pack_start (GTK_BOX (vbox_1), label_1, FALSE, FALSE, 10);
        gtk_box_pack_start (GTK_BOX (vbox_1), button_1, FALSE, FALSE, 10);
        gtk_container_add(GTK_CONTAINER(window_1),vbox_1);
        gtk_widget_show_all(window_1);
        pthread_create(&p1, NULL, subp1, NULL);
        gtk_main();
    }
    //process 2
    else{
    if((p2 = fork()) == -1)
    {
        printf("Create process2 failed");
        exit(1);
    }
    
    if(p2 == 0)
    {
        GtkWidget *window_2;
        GtkWidget *button_2;
        GtkWidget *vbox_2;
        gtk_init(&argc,&argv);
        vbox_2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_set_border_width (GTK_CONTAINER (vbox_2), 100);
        window_2 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        button_2 = gtk_button_new_with_label("Exit");
        gtk_window_set_title(GTK_WINDOW(window_2),"Fstab");
        g_signal_connect(G_OBJECT(button_2), "clicked", G_CALLBACK(on_button_clicked), NULL);
        g_signal_connect(G_OBJECT(window_2), "destroy", G_CALLBACK(destroy), NULL);
        label_2 = gtk_label_new("Hello, World");
        gtk_window_set_default_size(GTK_WINDOW(window_2), 200, 200);
        gtk_window_set_position(GTK_WINDOW(window_2), GTK_WIN_POS_CENTER);
        gtk_box_pack_start (GTK_BOX (vbox_2), label_2, FALSE, FALSE, 10);
        gtk_box_pack_start (GTK_BOX (vbox_2), button_2, FALSE, FALSE, 10);
        gtk_container_add(GTK_CONTAINER(window_2),vbox_2);
        gtk_widget_show_all(window_2);
        pthread_create(&p2, NULL, subp2, NULL);
        gtk_main();
    }
    //process 3
    else{
    if((p3 = fork()) == -1)
    {
        printf("Create process3 failed");
        exit(1);
    }
    
    if(p3 == 0)
    {
        GtkWidget *window_3;
        GtkWidget *vbox_3;
        GtkWidget *button_3;
        gtk_init(&argc,&argv);
        vbox_3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_set_border_width (GTK_CONTAINER (vbox_3), 100);
        window_3 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        button_3 = gtk_button_new_with_label("Exit");
        gtk_window_set_title(GTK_WINDOW(window_3),"Counter");
        g_signal_connect(G_OBJECT(button_3), "clicked", G_CALLBACK(on_button_clicked), NULL);
        g_signal_connect(G_OBJECT(window_3), "destroy", G_CALLBACK(destroy), NULL);
        label_3 = gtk_label_new("Hello, World");
        gtk_window_set_default_size(GTK_WINDOW(window_3), 200, 200);
        gtk_window_set_position(GTK_WINDOW(window_3), GTK_WIN_POS_CENTER);
        gtk_box_pack_start (GTK_BOX (vbox_3), label_3, FALSE, FALSE, 10);
        gtk_box_pack_start (GTK_BOX (vbox_3), button_3, FALSE, FALSE, 10);
        gtk_container_add(GTK_CONTAINER(window_3),vbox_3);
        gtk_widget_show_all(window_3);
        pthread_create(&p3, NULL, subp3, NULL);
        gtk_main();
    }
    }
    }
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
    waitpid(p3, NULL, 0);
    fclose(fp);
    fp = NULL;
return 0;
} 
