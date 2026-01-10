//
// file:			main_deskdupl_test01.cpp
// path:			src/tests/main_deskdupl_test01.cpp
// created on:		2026 Jan 09
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <libdeskdupl/deskdupl.h>
#include <QGuiApplication>
#include <QString>
#include <QImage>
#include <QDir>
#include <QFileInfo>

// Define a simple struct to hold our saving state if needed
struct CallbackData {
    QDir OutputDirectory;
    int FrameCount;
};

// Callback function
void FrameCallback(void* userData, const void* a_img)
{
    const QImage* img = (const QImage*)a_img;
    if (!userData || !img) return;
    CallbackData* data = (CallbackData*)userData;

    // Save
    const QString fileName = QString("frame_%1.png").arg(data->FrameCount, 6, 10, QChar('0'));
    const QString filePath = QFileInfo(data->OutputDirectory, fileName).filePath();

    img->save(filePath, "PNG");
    data->FrameCount++;
}


int main(int a_argc, char* a_argv[])
{
    QGuiApplication app(a_argc, a_argv);

    // Prepare User Data
    CallbackData cbData;
    cbData.OutputDirectory = QFileInfo(QFileInfo(a_argv[0]).dir(),"out").filePath();
    cbData.FrameCount = 0;

    if (!cbData.OutputDirectory.exists()) {
        cbData.OutputDirectory.mkpath(".");
        if (!cbData.OutputDirectory.exists()) {
            return 1;
        }
    }

    // Register Callbacks
    int result = RegisterAndStartDesktopChangeCalbakc(&cbData, FrameCallback);
    if (result != 0)
    {
        //MessageBoxA(nullptr, "Failed to register callback", "Error", MB_OK);
        return 1;
    }

    QCoreApplication::exec();

    // Unregister (Unreachable in infinite loop but good practice)
    UnregisterDesktopChangeCalbakc();

    return 0;
}
