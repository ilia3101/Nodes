/*****************************************************************************
 * DashGL.com GTK GL-Area                                                    *
 * This file is in the public domain                                         *
 * Contributors: Benjamin Collins                                            *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#ifdef __linux__
#include <alloca.h>
#endif
#include <stdio.h>

#include "../../UILibrary/UILibrary.h"

#include <GL/glew.h>
#include <gtk/gtk.h>
#include <math.h>
#include <time.h>

#define NAME "My Window"


void writebmp(unsigned char * data, int width, int height, char * filename) {
    int rowbytes = width*3+(4-(width*3%4))%4, imagesize = rowbytes*height, y;
    unsigned short header[] = {0x4D42,0,0,0,0,26,0,12,0,width,height,1,24};
    *((unsigned int *)(header+1)) = 26 + imagesize-((4-(width*3%4))%4);
    FILE * file = fopen(filename, "wb");
    fwrite(header, 1, 26, file);
    for (y = 0; y < height; ++y) fwrite(data+(y*width*3), rowbytes, 1, file);
    fwrite(data, width*3, 1, file);
    fclose(file);
}


static void activate(GtkApplication* app, gpointer user_data);
static void on_realize(GtkGLArea *area);
static gboolean on_render(GtkGLArea *area, GdkGLContext *context);

GLuint program;
GLuint vao, vbo_triangle, vbo_circle;
GLint attribute_coord2d;
GLuint tex;






int MainWindowWidth = 600;
int MainWindowHeight = 600;

UIFrame_t * mainDiv;

// UIImage_t * Pic;

UIImage_t * MainPic = NULL;

double scalefac = /* 0.75 */1;

int fghjkl = 0;
int X, Y;



#include "BuildInterface.c"

GTimer * Timer;


int main(int argc, char *argv[])
{
    Timer = g_timer_new();
    g_timer_start (Timer);
    int width = 507;
    int height = 500;

    int i = 0;

    // mainDiv = new_UIFrame();

    // UIFrame_t * MainDiv_new = new_UIFrame(UIDivType());

    mainDiv = ui_test();

    UIImage_t * pic = new_UIImage(width*scalefac+0.5, height*scalefac+0.5, Pixels_RGB_48i);


    int times = 4;
    clock_t start = clock();

    for (int i = 0; i < times; i++)
    {
        UIFrameDraw( mainDiv,
                     pic,
                     scalefac,
                     UIMakeCoordinate(0,0),
                     UIMakeRect(width,height),
                     1 );
    }

    clock_t end = clock();
    double Time = (((double)(end - start) / CLOCKS_PER_SEC)*1000.0) / times;
    printf("\n\nDrawing took %.3fms.\n\n\n", Time);

    writebmp(pic->Data,pic->Width,pic->Height, "test.BMP");
    // Preview("test.BMP");


    // PicA = new_UIImage(width, height, Pixels_RGB_24i);
    MainPic = new_UIImage(5000, 4000, Pixels_RGBA_64i);
    // Draw(MainPic);
    // MainPic = PicB;


	int status, milo = 0;
    puts("HIHIHI");

	GtkApplication * app = gtk_application_new("com.Ilia.Nodes", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}

GtkWidget * main_window;

//https://developer.gnome.org/gtk3/stable/GtkFileChooserDialog.html
static void buttonfunc (GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    // scalefac+=0.1;
    // gtk_widget_queue_draw(main_window);
    // return;

    dialog = gtk_file_chooser_dialog_new ( "Open File",
                                           main_window,
                                           action,
                                           "_Cancel",
                                           GTK_RESPONSE_CANCEL,
                                           "_Open",
                                           GTK_RESPONSE_ACCEPT,
                                           NULL );

    res = gtk_dialog_run (GTK_DIALOG (dialog));

    /* if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        filename = gtk_file_chooser_get_filename (chooser);
        // open_file (filename);
        // g_free (filename);
    } */

    // GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    // dialog = gtk_message_dialog_new (main_window,
    //                                 flags,
    //                                 /* GTK_MESSAGE_INFO
    //                                 GTK_MESSAGE_WARNING
    //                                 GTK_MESSAGE_QUESTION
    //                                 GTK_MESSAGE_ERROR
    //                                 GTK_MESSAGE_OTHER */
    //                                 GTK_MESSAGE_INFO,
    //                                 GTK_BUTTONS_YES_NO,
    //                                 "Error iflkj kjdshfsa hfkjdsf sdfkj hfkjsa f hsakjf sakjd k");
    // gtk_dialog_run (GTK_DIALOG (dialog));
    // gtk_widget_destroy (dialog);

    gtk_widget_destroy (dialog);
}

void button_press_event(GtkWidget * widget, GdkEventButton * event, gpointer userdata)
{
    int upordown = 0;

    /* Get mouse location */
    double mouse_x = event->x;
    double mouse_y = gdk_window_get_height(event->window) - event->y;

    double scale_fac = (double)gtk_widget_get_scale_factor(widget) * scalefac;
    double x_scaled = mouse_x / scalefac;
    double y_scaled = mouse_y / scalefac;

    /* If pressed or released */
    if ( event->type == GDK_BUTTON_PRESS ||
         event->type == GDK_2BUTTON_PRESS ||
         event->type == GDK_3BUTTON_PRESS )
        upordown = 1;
    else if (event->type == GDK_BUTTON_RELEASE)
        upordown = 0;
    
    if (upordown == 1)
        puts("ButtonDown");
    else 
        puts("ButtonUP");

    if (event->button == 1 || event->button == 2 || event->button == 3)
        UIFrameMouseButton(mainDiv, event->button, upordown, mouse_x/scalefac, mouse_y/scalefac, UIFrameGetRect(mainDiv, UIMakeRect(0,0)));

    if (UIFrameGetNeedsRedraw(mainDiv))
        gtk_widget_queue_draw(main_window);
}

void motion_notify_event(GtkWidget * widget, GdkEventMotion * event)
{
    int mouse_x, mouse_y;
    GdkModifierType state;

    if (event->is_hint)
        gdk_window_get_pointer(event->window, &mouse_x, &mouse_y, &state);
    else
    {
        mouse_x = event->x;
        mouse_y = event->y;
        state = event->state;
    }

    mouse_y = gdk_window_get_height(event->window) - mouse_y - 1;

    printf("mouse %i, %i\n", mouse_x, mouse_y);

    UIFrameSetMouseLocation(mainDiv, mouse_x/scalefac, mouse_y/scalefac, UIFrameGetRect(mainDiv, UIMakeRect(0,0)));

    if (UIFrameGetNeedsRedraw(mainDiv))
        gtk_widget_queue_draw(main_window);
}


GLuint textures[2];


// Shader sources
const GLchar* vertexSource = "glsl("
 "   #version 150 core"
  "  in vec2 position;"
"    in vec3 color;"
"    in vec2 texcoord;"
"    out vec3 Color;"
"    out vec2 Texcoord;"
"    void main()"
"    {"
"        Color = color;"
"        Texcoord = texcoord;"
"        gl_Position = vec4(position, 0.0, 1.0);"
"    }"
")glsl";
/* This version only shows texKitten */
/* Fragemtn shader that converts from XYZ to sRGB (or monitor space) */
// const GLchar * fragmentSource = R"glsl(
//     #version 150 core
//     in vec3 Color;
//     in vec2 Texcoord;
//     out vec4 outColor;
//     uniform sampler2D texKitten;
//     // uniform sampler2D texPuppy;
//     void main()
//     {
//         mat3 ColourMatrix = mat3(
//             3.2404542, -1.5371385, -0.4985314,
//             -0.9692660, 1.8760108, 0.0415560,
//             0.0556434, -0.2040259, 1.0572252
//         );

//         vec4 pixel = texture(texKitten, Texcoord);

//         /* Colour = first 3 channels * colour matrix */
//         vec3 Colour = pixel.rgb * ColourMatrix;

//         /* Apply gamma and premultiply alpha */
//         float alpha = pixel[3];
//         outColor = vec4(pow(Colour, vec3(1/2.2))*alpha, alpha);
//     }
// )glsl";
const GLchar* fragmentSource = "glsl("
"    #version 150 core"
"    in vec2 Texcoord;"
"    uniform sampler2D texKitten;"
"    void main()"
"    {"
"        vec4 pixel = texture(texKitten, Texcoord);"
"        gl_FragColor = pixel * pixel[3];"
"        // gl_FragColor = pixel;"
"    }"
")glsl";

// #define UseTransparentWindow

static void activate(GtkApplication *app, gpointer user_data)
{
    /* Use dark theme pls */
    g_object_set (gtk_settings_get_default (), "gtk-application-prefer-dark-theme", TRUE, NULL);


    GtkWidget *window;
    GtkWidget *gl_area;

    GtkWidget * header_bar = gtk_header_bar_new();
    gtk_header_bar_set_title(header_bar, NAME);
    gtk_header_bar_set_show_close_button (header_bar, TRUE);
    gtk_style_context_add_class(gtk_widget_get_style_context(header_bar), "DarkTitlebar");
    // gtk_header_bar_set_has_subtitle(header_bar, TRUE);
    // gtk_header_bar_set_subtitle(header_bar, "subtitle");
    // gtk_header_bar_set_decoration_layout(header_bar, "subtitle");

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW (window), NAME);
    gtk_window_set_default_size (GTK_WINDOW (window), 530, 320);
#ifdef UseTransparentWindow
    gtk_window_set_titlebar(GTK_WINDOW(window), header_bar);
#endif
    // gtk_window_set_decorated(window, FALSE);
    /* center it */
    gtk_window_set_position(window, GTK_WIN_POS_CENTER);
    // gtk_window_set_urgency_hint (window, TRUE);
    main_window = window;

    // gtk_window_set_decorated(window, FALSE);





        /* COLORCOLOR COLOUR */









    /*************************** STYLE ***************************/
    GtkCssProvider *provider = gtk_css_provider_new ();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen (display);
    gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    char * style =
    ".titlebar {"
    "    background-color: rgb(0,0,0);"
    "    background-image: none;"
    "    box-shadow: none;"
    "    border: none;"
    "}";
#ifdef UseTransparentWindow
    gtk_css_provider_load_from_path(provider,"/home/ilia/GTKTest/stylegood.css",NULL);
#endif
    // gtk_css_provider_load_from_path(provider,"/home/ilia/GTKTest/koolstyle.css",NULL);

	gl_area = gtk_gl_area_new();
#ifdef UseTransparentWindow
    gtk_gl_area_set_has_alpha(gl_area, TRUE);
#else
    gtk_gl_area_set_has_alpha(gl_area, FALSE);
#endif
	gtk_widget_set_vexpand(gl_area, TRUE);
	gtk_widget_set_hexpand(gl_area, TRUE);
	g_signal_connect (gl_area, "realize", G_CALLBACK(on_realize), NULL);
	g_signal_connect (gl_area, "render", G_CALLBACK(on_render), NULL);

    /* Add event receiving thing to the gl area */
    GtkWidget * gl_area_box = gtk_event_box_new();
    gtk_widget_set_events(gl_area_box, GDK_POINTER_MOTION_MASK); /* To recieve all mouse move events */
    gtk_container_add (gl_area_box, gl_area);
	gtk_container_add(GTK_CONTAINER(window), gl_area_box);
    // gtk_window_set_titlebar(GTK_WINDOW(window), NULL);


    g_signal_connect(gl_area_box, "motion_notify_event",
                                     motion_notify_event, NULL);
    g_signal_connect(gl_area_box, "button-press-event",
                                     button_press_event, NULL);
    g_signal_connect(gl_area_box, "button-release-event",
                                     button_press_event, NULL);


    /******************************** BUTTONS *********************************/

    GtkWidget * button = gtk_button_new_with_label ("Hello");
    GtkWidget * button2 = gtk_button_new_with_label ("world");
    GtkWidget * button3 = gtk_button_new_with_label ("🤮");
    GtkWidget * button4 = gtk_button_new_with_label ("😜 🐸 😻  🇷🇺 ");
    // GtkWidget * button3 = gtk_button_new_with_label ("<");
    // GtkWidget * button4 = gtk_button_new_with_label (">");
    // gtk_widget_set_margin_bottom(button4, 4);
    g_signal_connect (button, "clicked", G_CALLBACK (buttonfunc), NULL);


    // GtkWidget * grid_test = gtk_grid_new();
    // gtk_grid_attach(grid_test,
    //                 button3,
    //                 0,
    //                 0,
    //                 1,
    //                 1);
    // gtk_grid_attach(grid_test,
    //                 button4,
    //                 1,
    //                 0,
    //                 1,
    //                 1);
    // gtk_grid_set_column_spacing(grid_test, 1);

#define HAS_BUTTONS 0

#if HAS_BUTTONS
    gtk_header_bar_pack_start(header_bar, button);
    gtk_header_bar_pack_start(header_bar, button4);
    gtk_header_bar_pack_start(header_bar, button2);
    gtk_header_bar_pack_start(header_bar, button3);
#endif
    // gtk_header_bar_pack_start(header_bar, grid_test);


    // /* Setting title as a widget test */
    // GtkWidget * CustomTitle = gtk_button_new_with_label(NAME);
    // gtk_header_bar_set_custom_title (header_bar, CustomTitle);
    gtk_style_context_add_class(gtk_widget_get_style_context(button), "DarkButton");
    gtk_style_context_add_class(gtk_widget_get_style_context(button2), "DarkButton");
    gtk_style_context_add_class(gtk_widget_get_style_context(button3), "DarkButton");
    gtk_style_context_add_class(gtk_widget_get_style_context(button4), "DarkButton");
    // g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
    // g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
    // gtk_container_add (GTK_CONTAINER (button_box), button);


	gtk_widget_show_all(window);
}


static void on_realize(GtkGLArea *area)
{
	g_print("on_realize\n");
	
	gtk_gl_area_make_current(area);
	
	if (gtk_gl_area_get_error (area) != NULL) {
		fprintf(stderr, "Unknown error\n");
		return;
	}

	glewExperimental = GL_TRUE;
	glewInit();

    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLfloat vertices[] = {
    //  Position      Color             Texcoords
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
         1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
        -1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create an element array
    GLuint ebo;
    glGenBuffers(1, &ebo);

    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

    // Load textures
    glGenTextures(2, textures);

    int width, height;
    unsigned char * image;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
        float pixels[] = {
            0.0f, 1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,   0.6f, 0.2f, 0.2f, 0.6f,    0.1f, 0.5f, 0.8f, 0.9f,
            1.0f, 0.5f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 0.0f, 0.0f
        };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 3, 3, 0, GL_RGBA, GL_FLOAT, pixels);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 3, 3, 0, GL_RGBA, GL_FLOAT, pixels);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 3, 3, 0, GL_RGBA, GL_FLOAT, pixels);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 3, 3, 0, GL_RGBA, GL_FLOAT, pixels);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 3, 3, 0, GL_RGBA, GL_FLOAT, pixels2);
    glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGBA, GL_FLOAT, pixels);
    glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    char * renderer = (char *) glGetString (GL_RENDERER);
    printf("OpenGL renderer: %s\n", renderer);
    renderer = (char *) glGetString (GL_VERSION);
    printf("OpenGL version: %s\n", renderer);
}

static gboolean on_render(GtkGLArea *area, GdkGLContext *context)
{
	g_print("on_render\n");


    // set texture's sizes
    // glBindTexture(GL_TEXTURE_2D, textures[0]);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, NULL);
    // glBindTexture(GL_TEXTURE_2D, textures[1]);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, NULL);
	
    // Clear the screen to black
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);


    /* UILIBRARY */

    int old_width = MainWindowWidth, old_height = MainWindowHeight, resize = 0;

    gtk_window_get_size(main_window, &MainWindowWidth, &MainWindowHeight);
    double scale_factor_gtk = (double)gtk_widget_get_scale_factor(main_window);
    MainWindowWidth*=scale_factor_gtk;
    MainWindowHeight*=scale_factor_gtk;

    if (MainWindowHeight != old_height || MainWindowWidth != old_width)
    {
        resize = 1;
        puts("RESIZED");
    }

    // printf("MainWindowHeight %i, old_height %i, MainWindowWidth %i, old_width %i\n", MainWindowHeight, old_height, MainWindowWidth, old_width);


    printf("scale factor: %f\n", scale_factor_gtk);
    double draw_scale_fac = scalefac*scale_factor_gtk;

    clock_t start = clock(), diff;

    if (resize) {
        MainPic = UIImageChange(MainPic, MainWindowWidth, MainWindowHeight, Pixels_RGBA_64i);

        /* Set coordinates of the top level frame, which must be absolute */
        UIFrameSetXCoordinateAbsolute(mainDiv, 0, MainWindowWidth/draw_scale_fac, 0);
        UIFrameSetYCoordinateAbsolute(mainDiv, 0, MainWindowHeight/draw_scale_fac, 0);
    }

    UIImage_t * DrawPic = MainPic;

    if (UIFrameGetNeedsRedraw(mainDiv))
    {
        UIImageFillColour(DrawPic, UIMakeColour(0.0, 0.0, 0.0, 0.0));
        // Draw(MainPic);

        /* Has the draw image been cleared */
        int cleared = 0;
        if (resize) cleared = 1;

        UIFrameDraw( mainDiv,
                     DrawPic,
                     draw_scale_fac,
                     UIMakeCoordinate(0,0),
                     UIMakeRect(MainWindowWidth/draw_scale_fac,MainWindowHeight/draw_scale_fac),
                     cleared );
    }
    diff = clock() - start;
    double UIFrameDraw_time = ((double)diff) * (1000.0 / CLOCKS_PER_SEC);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    start = clock();
    char * texfunc;
    if (resize) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, MainWindowWidth, MainWindowHeight, 0, GL_RGBA, GL_UNSIGNED_SHORT, DrawPic->Data);
        texfunc = "glTexImage2D";
    } else {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, MainWindowWidth,  MainWindowHeight, GL_RGBA, GL_UNSIGNED_SHORT, DrawPic->Data);
        texfunc = "glTexSubImage2D";
    }
    diff = clock() - start;
    double texfunc_time = ((double)diff) * (1000.0 / CLOCKS_PER_SEC);


    start = clock();
    // Draw a rectangle from the 2 triangles using 6 indices
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // glFinish();
    diff = clock() - start;
    double glDrawElements_time = ((double)diff) * (1000.0 / CLOCKS_PER_SEC);

    printf("times - UIFrameDraw: %.2fms, ", UIFrameDraw_time);
    printf("%s: %.2fms, ", texfunc, texfunc_time);
    printf("glDrawElements: %.2fms\n", glDrawElements_time);
    glFlush();



    g_timer_stop(Timer);

    double Time = g_timer_elapsed(Timer, NULL);

    printf("Tim since last frame: %.2lf milliseconds, resize: %i\n\n\n", Time*1000, resize);

    g_timer_start(Timer);

	return TRUE;

}

// rm a.out; gcc `pkg-config --cflags gtk+-3.0` main.c -lm -lGL -lGLEW -lGLU `pkg-config --libs gtk+-3.0`; ./a.out





/*

rm *.o; rm a.out; clear; clear; gcc -O3 -c `pkg-config --cflags gtk+-3.0` test/GTKTest.c; gcc -O3 -c UIGraphics.c UIFrame.c Types/UISlider.c graphics/ui_text.c Types/UILabel.c Types/UIButton.c Types/UIDiv.c; gcc GTKTest.o UIFrame.o ui_text.o UIGraphics.o UIButton.o UILabel.o UIDiv.o UISlider.o  -lm -lGL -lGLEW -lGLU `pkg-config --libs gtk+-3.0`; ./a.out
*/