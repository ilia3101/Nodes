#include "../../UILibrary/UILibrary.h"

double randoMm()
{
    return pow((rand()%3)/2.0, 1.5)/2.0 + 0.5;
}

// typedef struct {
//     int number;
// } user_data_test_t;

int num12121212 = 0;

void button_user_func(UIFrame_t * Button)
{
    printf("\n\n\n\n\n\n\n\n\n\nBUTTON pressed: %s\n\n\n\n\n\n\n\n\n\n\n\n\n\n", UIFrameGetID(Button));
}

UIFrame_t * ui_test()
{
    UIFrame_t * main_div = new_UIFrame(UIDivType(), "Main");

    UIFrameSetXCoordinateAbsolute(main_div, 0, 400, 0);
    UIFrameSetYCoordinateAbsolute(main_div, 0, 400, 0);

    // UIDivSetBackgroundColour(main_div, UIMakeColour(1.0, 0.3, 0.3, 0.4));
    UIDivSetBackgroundColour(main_div, UIDefaultColourBackgroundB);
    UIDivSetBorderColour(main_div, UIMakeColour(0.8, 0.2, 0.2, 1.0));


    UIFrame_t * a = new_UIFrame(UIDivType(), "A");
    // UIFrameSetXCoordinateAbsolute(a, 146, 333, 0);
    UIFrameSetXCoordinateRelative(a,10,10);
    UIFrameSetYCoordinateAbsolute(a, 70, 20, 0);
    UIDivSetBackgroundColour(a, UIMakeColour(0.3, 0.65, 1.0, 0.3));
    UIDivSetBorderColour(a, UIMakeColour(0.16, 0.5, 0.8, 1.0));
    UIDivSetBorderThickness(a, 3.3);


    UIFrame_t * b = new_UIFrame(UIDivType(), "B");
    // UIDivSetBackgroundColour(b, UIMakeColour(0.33, 0.56, 0.33, 1.0));
    UIDivSetBackgroundColour(b, UIMakeColour(0.4, 0.7, 0.4, 1.0));
    // UIDivSetBorderColour(b, UIMakeColour(0.3, 0.7, 0.3, 1.0));
    UIDivSetBorderColour(b, UIMakeColour(0.0, 0.0, 0.0, 1.0));
    UIFrameSetXCoordinateAbsolute(b,100,130,1);
    UIFrameSetYCoordinateRelative(b,10,10);
    UIDivSetBorderThickness(b, 2);


    UIFrame_t * buttons[5];
    
    int spacing = 133;
    int buttonwidth = 125;
    int numbuttons = 12;
    for (int i = 0; i < 5; ++i)
    {
        char button_id[20];
        sprintf(button_id, "button_%i", i+1);
        char button_text[20];
        sprintf(button_text, "button %i", i+1);
        if (i == 3)
        {
            puts("HIHI");
            buttons[i] = new_UIFrame(UILabelType(), button_id);
            UILabelSetText(buttons[i], "fsdfs");
        }
        else if (i == 4)
        {
            puts("Slider");
            buttons[i] = new_UIFrame(UISliderType(), button_id);
        }
        else {
            buttons[i] = new_UIFrame(UIButtonType(), button_id);
            // UIButtonSetBorderThickness(buttons[i], 1);
            // UIButtonSetBackgroundMouseOverColour(buttons[i], UIMakeColour(randoMm(),randoMm(),randoMm(),1.0));
            // UIButtonSetBackgroundColour(buttons[i], UIMakeColour(0.9,0.9,0.9,1));
            // UIButtonSetBackgroundMouseOverColour(buttons[i], UIMakeColour(0.95,0.95,0.95,1.0));
            // UIButtonSetBackgroundMouseDownColour(buttons[i], UIMakeColour(0.95,0.95,0.95,1.0));
            UIButtonSetText(buttons[i],button_text);
            UIButtonSetClickFunction(buttons[i], button_user_func);
        }
        UIFrameSetXCoordinateAbsolute(buttons[i],33+i*spacing,buttonwidth,0);
        UIFrameSetYCoordinateAbsolute(buttons[i],16,37,1);
        FrameAddSubframe(main_div, buttons[i]);
    }


    // UIFrame_t * subdiv1 = new_UIFrame(UIDivType(), "Subdiv");
    // UIFrameSetXCoordinateRelative(subdiv1, 12, 12);
    // UIFrameSetYCoordinateRelative(subdiv1, 12, 12);

    // FrameAddSubframe(main_div, subdiv1);


    // FrameAddSubframe(main_div, b);
    // FrameAddSubframe(main_div, a);

    return main_div;
}















// UIImage_t * Buffer_0;
// UIImage_t * Buffer_1;

// int current_buffer = 0;

// void UITest_init()
// {
//     Buffer_0 = new_UIImage(10, 10, Pixels_RGBA_64i);
//     Buffer_1 = new_UIImage(10, 10, Pixels_RGBA_64i);
// }

// void UITest_Pthread(void * MainFrame)
// {
//     while (1 < 2)
//     {
//         if (UIFrameGetNeedsRedraw(MainFrame))
//         {
//             UIImage_t * DrawImg = (current_buffer)?Buffer_0:Buffer_1;
//             UIImageChange(DrawImg, MainWindowWidth, MainWindowWidth, Pixels_RGBA_64i);
//             double SF = 1.5;
//             UIFrameDraw( MainFrame,
//                          DrawImg,
//                          SF, /* ScaleFac */
//                          UIMakeCoordinate(0,0),
//                          UIMakeRect(MainWindowWidth/SF,MainWindowHeight/SF),
//                          1 );
//             return;
//         }
//         else
//         {
//             usleep(16000);
//         }
//     }
// }

// UIImage_t * GetUIImage()
// {
//     UIImage_t * Pic = (current_buffer)?Buffer_0:Buffer_1;
//     current_buffer = 1 - current_buffer;
//     return Pic;
// }