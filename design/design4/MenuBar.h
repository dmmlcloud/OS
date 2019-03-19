#ifndef __MENUBAR_H
#define __MENUBAR_H
void CreateMenuBar(GtkWidget *vbox);
void ShowAbout(void);
void ShutDown(void);

void CreateMenuBar(GtkWidget *vbox){
    GtkWidget *menu1;
    GtkWidget *menu2;
    GtkWidget *menu_bar;
    GtkWidget *HELP_MENU, *FUNC_MENU;
    GtkWidget *menuitems;
    
    menu_bar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, TRUE, 0);
    gtk_widget_show(menu_bar);
    
    menu1 = gtk_menu_new();
    menu2 = gtk_menu_new();
    
    FUNC_MENU = gtk_menu_item_new_with_label("functions");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), FUNC_MENU);
	gtk_widget_show(FUNC_MENU);
	
	HELP_MENU = gtk_menu_item_new_with_label("help");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), HELP_MENU);
	gtk_widget_show(HELP_MENU);
	
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(FUNC_MENU), menu1);
	menuitems = gtk_menu_item_new_with_label("Exit");
	g_signal_connect(G_OBJECT(menuitems), "activate", G_CALLBACK(gtk_main_quit), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu1), menuitems);
	gtk_widget_show(menuitems);
	
	menuitems = gtk_menu_item_new_with_label("Shutdown");
	g_signal_connect(G_OBJECT(menuitems), "activate", G_CALLBACK(ShutDown), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu1), menuitems);
	gtk_widget_show(menuitems);
	
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(HELP_MENU), menu2);
	menuitems = gtk_menu_item_new_with_label("About");
	g_signal_connect(G_OBJECT(menuitems), "activate", G_CALLBACK(ShowAbout), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu2), menuitems);
	gtk_widget_show(menuitems);	
}

void ShowAbout(void)
{
    GtkWidget *dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "SysMonitor for Linux");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "Aster 1.0");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "This is a simple tool for supervising computer's status.");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "(c) Ziyu Liu");
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "https://blog.csdn.net/qq_38976909");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void ShutDown(void)
{
    if(geteuid() != 0)
    {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Please root and try again");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
    else
        system("/sbin/init 0");
}
#endif
