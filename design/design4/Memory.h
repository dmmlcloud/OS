void CreatePageMemory(GtkWidget *notebook);
void DrawingMem(void);
gint refresh_mem(gpointer data);
static gboolean memory_configure_event (GtkWidget *widget,GdkEventConfigure *event,gpointer data);
static gboolean memory_expose_event (GtkWidget *widget,GdkEventExpose *event,gpointer data);
void GetMemoryInfo(void);

GtkWidget *draw_memory;
GdkPixmap *memory_graph;
static gint memoryPoints[101];
static gfloat memory_rate = 0.0;
GtkWidget *InfoLabel;

typedef struct memory{
	char MemTotal[50];
	char MemFree[50];
	char Buffers[50];
	char Cached[50];
}memory_info;

memory_info memi;
gdouble memoryUseRatio;

void CreatePageMemory(GtkWidget *notebook)
{
    GtkWidget *table;
    GtkWidget *frame;
    GtkWidget *memory_frame;
    GtkWidget *MemInfo;
    
    frame = gtk_frame_new("");
    gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
    gtk_widget_show(frame);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame, gtk_label_new("Memory Information"));
    
    table = gtk_table_new(10, 10, TRUE);
    gtk_container_add(GTK_CONTAINER(frame), table);
    
    memory_frame = gtk_frame_new("Memory using curve");
    gtk_table_attach_defaults(GTK_TABLE(table), memory_frame, 1, 9, 0, 5);
    gtk_widget_show(memory_frame);
    
    MemInfo = gtk_frame_new("Memory Information");
    gtk_table_attach_defaults(GTK_TABLE(table), MemInfo, 0, 10, 5, 10);
    gtk_widget_show(MemInfo);
    
    InfoLabel = gtk_label_new("test");
    gtk_container_add(GTK_CONTAINER(MemInfo), InfoLabel);
    gtk_widget_show(InfoLabel);
    GetMemoryInfo();
    gtk_widget_show(table);
    
    
    draw_memory = gtk_drawing_area_new();
    gtk_widget_set_app_paintable(draw_memory, TRUE);
    gtk_drawing_area_size(GTK_DRAWING_AREA(draw_memory), 40, 80);
    gtk_container_add(GTK_CONTAINER(memory_frame), draw_memory);
    gtk_widget_show(draw_memory);
    
    DrawingMem();
}

static gboolean memory_configure_event (GtkWidget *widget,GdkEventConfigure *event,gpointer data)
{
    if (memory_graph)
        g_object_unref (G_OBJECT (memory_graph));
    memory_graph = gdk_pixmap_new (widget->window,
                                widget->allocation.width,
                                widget->allocation.height,-1);
    gdk_draw_rectangle (memory_graph,widget->style->white_gc,
                        TRUE,0,0,widget->allocation.width,
                        widget->allocation.height);
    return TRUE;
}

static gboolean memory_expose_event (GtkWidget *widget,GdkEventExpose *event,gpointer data)
{
    gdk_draw_drawable (widget->window,
                        widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                        memory_graph,
                        event->area.x, event->area.y,
                        event->area.x, event->area.y,
                        event->area.width, event->area.height);
    return FALSE;
}

void DrawingMem(void)
{
	int i;
	for (i = 0; i < 100; i++)
	{
		memoryPoints[i] = 999;
	}

	g_signal_connect(draw_memory, "expose_event",G_CALLBACK(memory_expose_event), NULL);
	g_signal_connect(draw_memory, "configure_event", G_CALLBACK(memory_configure_event), NULL);
	gtk_widget_show(draw_memory);
}

gint refresh_mem(gpointer data)
{
	GdkGC *gc_chart_memory = gdk_gc_new(draw_memory->window);
	GdkColor color;
	gdk_color_parse("balck",&color);
	gdk_gc_set_rgb_fg_color(gc_chart_memory, &color);
	gdk_draw_rectangle(memory_graph, gc_chart_memory, TRUE, 0, 0,draw_memory->allocation.width, draw_memory->allocation.height);
	
        int width, height, curPoint, step;
        memory_rate = memoryUseRatio;
        width = draw_memory->allocation.width;
	height = draw_memory->allocation.height;
	curPoint = (int) (memory_rate * (double) height);
	memoryPoints[99] = height - curPoint;
	int j;
	for (j = 0; j < 99; j++)
		memoryPoints[j] = memoryPoints[j + 1];
	step = width / 99;
	
	GdkGC *gc = gdk_gc_new(GDK_DRAWABLE(memory_graph));
        gdk_color_parse("red",&color);
	gdk_gc_set_rgb_fg_color(gc, &color);
	gdk_gc_set_line_attributes(gc, 1, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_MITER);
	for (j = 99; j >= 1; j--)
	{
		gdk_draw_line(GDK_DRAWABLE(memory_graph), gc, j * step, memoryPoints[j], 
		(j - 1) * step, memoryPoints[j - 1]); 
	}
	gtk_widget_queue_draw(draw_memory);

	GetMemoryInfo();
	return TRUE;
}

void GetMemoryInfo(void)
{
    char MemoryLabel[2000];
    char *info;
	GetInfo("/proc/meminfo", "MemTotal", memi.MemTotal);
	GetInfo("/proc/meminfo", "MemFree", memi.MemFree);
	GetInfo("/proc/meminfo", "Buffers", memi.Buffers);
	GetInfo("/proc/meminfo", "Cached", memi.Cached);

    strcpy(MemoryLabel, "\nMemory Total:");
	info = strtok(memi.MemTotal, "\n");
	strcat(MemoryLabel, info);
	
	strcat(MemoryLabel, "\nMemory Free: ");
	info = strtok(memi.MemFree, "\n");
	strcat(MemoryLabel, info);
	
	strcat(MemoryLabel, "\nBuffers: ");
	info = strtok(memi.Buffers, "\n");
	strcat(MemoryLabel, info);
	
	strcat(MemoryLabel, "\nCached: ");
	info = strtok(memi.Cached, "\n");
	strcat(MemoryLabel, info);
	
	gtk_label_set_text(GTK_LABEL(InfoLabel), MemoryLabel);
    
	int mem_total, mem_free;
	mem_total = atoi(memi.MemTotal);
	mem_free = atoi(memi.MemFree);
	memoryUseRatio = (double)(mem_total - mem_free) / mem_total;
	//printf("memUseRatio = %lf\n", memUseRatio);

	return;
}
