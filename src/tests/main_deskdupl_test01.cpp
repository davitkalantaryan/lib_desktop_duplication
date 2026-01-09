//
// file:			main_deskdupl_test01.cpp
// path:			src/tests/main_deskdupl_test01.cpp
// created on:		2026 Jan 09
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <libdeskdupl/deskdupl.h>
#include <cinternal/signals.h>
#include <QString>
#include <QImage>
#include <QDir>

// Define a simple struct to hold our saving state if needed
struct CallbackData {
    std::string OutputDirectory;
    int FrameCount;
};

// Callback function
void FrameCallback(void* userData, const void* a_img)
{
    const QImage* img = (const QImage*)a_img;
    if (!userData || !img) return;
    CallbackData* data = (CallbackData*)userData;

    // Ensure directory exists (basic check, optimized to not do it every frame if possible in real apps, 
    // but for this test consistent with original logic)
    static bool dirChecked = false;
    QString dirPath = QString::fromStdString(data->OutputDirectory);
    if (!dirChecked)
    {
        QDir dir(dirPath);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        dirChecked = true;
    }

    // Save
    QString filename = QString("%1/frame_%2.png")
        .arg(dirPath)
        .arg(data->FrameCount, 6, 10, QChar('0'));

    img->save(filename, "PNG");
    data->FrameCount++;
}


int main(int a_argc, char* a_argv[])
{
    // Prepare User Data
    CallbackData cbData;
    cbData.OutputDirectory = "./out";
    cbData.FrameCount = 0;

    // Register Callbacks
    int result = RegisterAndStartDesktopChangeCalbakc(&cbData, FrameCallback);
    if (result != 0)
    {
        //MessageBoxA(nullptr, "Failed to register callback", "Error", MB_OK);
        return -1;
    }

    // Wait Loop as requested
    while (1)
    {
        CinternalSleepInterruptableMs(10);
        // In a real app we might look for a quit signal or key press
        // For this test, user said they will terminate via Task Manager
    }

    // Unregister (Unreachable in infinite loop but good practice)
    UnregisterDesktopChangeCalbakc();

    return 0;
}
