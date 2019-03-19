void GetInfo(char *path, char *name, char info[]);
static gboolean cpu_configure_event (GtkWidget *widget,GdkEventConfigure *event,gpointer data);
static gboolean cpu_expose_event (GtkWidget *widget,GdkEventExpose *event,gpointer data);
float getCpuUseRatio(int n);
gint refresh_cpu(gpointer data);
void CreatePageCPU(GtkWidget *notebook);
void DrawCPU(void);
void GetCPUInfo(void);
//static void GetCpuCount(char info[]);

GtkWidget *InfoLabel;
GtkWidget *draw_cpu;
GtkWidget *window;
GtkWidget *cpuRate;
GdkPixmap *cpu_graph;

static gint cpuPoints[100];
static gfloat cpu_rate = 0;
float puser = 0, ptotal = 0, pidle = 0;

void CreatePageCPU(GtkWidget *notebook)   //CPU
{
    GtkWidget *frame;
    GtkWidget *lb;
    GtkWidget *CPUframe;
    GtkWidget *CPUInfo;
    GtkWidget *table;
    lb = gtk_label_new("CPU Information");

    frame = gtk_frame_new("");
    gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
    gtk_widget_show(frame);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook),frame,lb);
    table = gtk_table_new(10, 10, TRUE);
    gtk_container_add(GTK_CONTAINER(frame), table);
    
    CPUframe = gtk_frame_new("CPU using curve");
    gtk_table_attach_defaults(GTK_TABLE(table), CPUframe, 1, 9, 0, 5);
    gtk_widget_show(CPUframe);
    
    CPUInfo = gtk_frame_new("CPU Information");
    gtk_table_attach_defaults(GTK_TABLE(table), CPUInfo, 0, 10, 5, 10);
    gtk_widget_show(CPUInfo);
    
    InfoLabel = gtk_label_new("test");
    gtk_container_add(GTK_CONTAINER(CPUInfo), InfoLabel);
    gtk_widget_show(InfoLabel);
    GetCPUInfo();
    gtk_widget_show(table);
    
    draw_cpu = gtk_drawing_area_new();
    gtk_widget_set_app_paintable(draw_cpu, TRUE);
    gtk_drawing_area_size(GTK_DRAWING_AREA(draw_cpu), 40, 80);
    gtk_container_add(GTK_CONTAINER(CPUframe), draw_cpu);
    gtk_widget_show(draw_cpu);
    
    DrawCPU();
    
}

void GetCPUInfo(void)
{
    int cpusum;
    char cpuName[60], cpuMHz[20], cacheSize[20], cpuCores[5], addrSize[50];
    char *info;
    char cpuLabel[2000];
    
    GetInfo("/proc/cpuinfo", "model name", cpuName);
    GetInfo("/proc/cpuinfo", "cpu MHz", cpuMHz);
    GetInfo("/proc/cpuinfo", "cache size", cacheSize);
    GetInfo("/proc/cpuinfo", "cpu cores", cpuCores);
    GetInfo("/proc/cpuinfo", "address sizes", addrSize);
    

    
    strcpy(cpuLabel, "\nCPU model:\n");
	info = strtok(cpuName, "\n");
	strcat(cpuLabel, info);
	
	strcat(cpuLabel, "\nCPU frequency: ");
	info = strtok(cpuMHz, "\n");
	strcat(cpuLabel, info);
	
	strcat(cpuLabel, "\nCPU cores count: ");
	info = strtok(cpuCores, "\n");
	strcat(cpuLabel, info);
	
	strcat(cpuLabel, "\n\nCache size: ");
	info = strtok(cacheSize, "\n");
	strcat(cpuLabel, info);
	
	strcat(cpuLabel, "\nCPU address size: ");
	info = strtok(addrSize, "\n");
	strcat(cpuLabel, info);
	
	gtk_label_set_text(GTK_LABEL(InfoLabel), cpuLabel);
}

void DrawCPU(void)
{
	int i;
	for (i = 0; i < 100; i++)
	{
		cpuPoints[i] = 999;
	}
	g_signal_connect(draw_cpu, "expose_event",G_CALLBACK(cpu_expose_event), NULL);
	g_signal_connect(draw_cpu, "configure_event", G_CALLBACK(cpu_configure_event), NULL);
	gtk_widget_show(draw_cpu);
}

void GetInfo(char *path, char *name, char info[])
{
    int fd = open(path, O_RDONLY);
    int i = 0;
    if(fd < 0)
    {
        printf("Open file failed\n");
        exit(1);
    }
    char buf[3000];
    read(fd, buf, sizeof(buf));
    close(fd);

    char *p = NULL;
    p = strstr(buf, name);
    while(*p != ':')
        p++;
    p++;
    while(*p == ' ')
        p++;
    while(*p != '\n')
    {
        info[i] = *p;
        i++;
        p++;
    }
    info[i] = '\n';
    info[i+1] = '\0';
}


static gboolean cpu_configure_event (GtkWidget *widget,GdkEventConfigure *event,gpointer data)
{
    if (cpu_graph)
        g_object_unref (G_OBJECT (cpu_graph));
    cpu_graph = gdk_pixmap_new (widget->window,
                                widget->allocation.width,
                                widget->allocation.height,-1);
    gdk_draw_rectangle (cpu_graph,widget->style->white_gc,
                        TRUE,0,0,widget->allocation.width,
                        widget->allocation.height);
    return TRUE;
}

static gboolean cpu_expose_event (GtkWidget *widget,GdkEventExpose *event,gpointer data)
{
    gdk_draw_drawable (widget->window,
                        widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                        cpu_graph,
                        event->area.x, event->area.y,
                        event->area.x, event->area.y,
                        event->area.width, event->area.height);
    return FALSE;
}

gint refresh_cpu(gpointer data)
{
    GdkGC *gc_chart_cpu = gdk_gc_new(draw_cpu->window);
    GdkColor color;
    color.red = 0xFFFF;
    color.green = 0xFFFF;
    color.blue = 0xFFFF;
    gdk_gc_set_rgb_fg_color(gc_chart_cpu, &color);
    
    int width, height, curPoint, step;
    cpu_rate = getCpuUseRatio(1) / 100;
    gdk_draw_rectangle(cpu_graph, gc_chart_cpu, TRUE, 0, 0, draw_cpu->allocation.width, draw_cpu->allocation.height);
    width = draw_cpu->allocation.width;
    height = draw_cpu->allocation.height;
    curPoint = (int) (cpu_rate * (double)height);
    cpuPoints[99] = height - curPoint;
    int i;
    for (i = 0; i < 99; i++)
		cpuPoints[i] = cpuPoints[i + 1];
	step = width / 99;
	GdkGC *gc = gdk_gc_new(GDK_DRAWABLE(cpu_graph));
	gdk_color_parse("black", &color);
	gdk_gc_set_foreground(gc, &color);
	gdk_gc_set_line_attributes(gc, 1, GDK_LINE_SOLID, GDK_CAP_ROUND,GDK_JOIN_MITER);
	for (i = 99; i >= 1; i--)
		gdk_draw_line(GDK_DRAWABLE(cpu_graph), gc, i * step, cpuPoints[i], (i - 1) * step, cpuPoints[i - 1]); 
	gtk_widget_queue_draw(draw_cpu);

	GetCPUInfo();
	
	return TRUE;
}

float getCpuUseRatio(int n)
{
	FILE *fp;
	char buffer[1024];
	size_t buf;
	float useRatio;
	float sysRatio;
	float iowaitRatio;
	float total = 0;
    
	float user = 0, nice = 0, sys = 0, idle = 0, iowait = 0, irq = 0, softirq = 0;
	fp = fopen("/proc/stat", "r");
	if(fp == NULL)
	{
		printf("open failed");
		exit(1);
	}
	buf = fread(buffer, 1, sizeof(buffer), fp);
	fclose(fp);
	
	if (buf <= 0)
		return 0;
		
	buffer[buf] == '\0';
	sscanf(buffer, "cpu %f %f %f %f %f %f %f", &user, &nice, &sys, &idle, &iowait, &irq, &softirq);
	if (idle <= 0)
		idle = 0;
	
	total = user + nice + sys + idle + iowait + irq + softirq;
    useRatio = 100 * (1 - (idle - pidle) / (total - ptotal));
	sysRatio = 100 * sys / user;
	iowaitRatio = 100 * iowait / total;

	if (useRatio > 100)
        useRatio = 100;
	if (useRatio < 0)
        useRatio = 0;
	ptotal = total;
	puser = user;
	pidle = idle;
	cpu_rate = useRatio;
	if(n == 1)
		return useRatio;
	else if(n == 2)
		return sysRatio;
	else if(n == 3)
		return iowaitRatio;
}
