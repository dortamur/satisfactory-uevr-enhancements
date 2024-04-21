#include "MouseEmulation.h"
#include "Framework/Application/SlateApplication.h"

// Derived from:
//  https://forums.unrealengine.com/t/how-to-inject-simulate-mouse-clicks/25602/12
//  https://pastebin.com/QqsTYYkv
void UMouseEmulation::SimulateMouseButton(bool bLeft, bool bButtonDown, bool bWidgetsOnly, bool bWorldOnly)
{
    // Determine Mouse Button and Input Type
    FKey MouseButton = bLeft ? EKeys::LeftMouseButton : EKeys::RightMouseButton;
    EInputEvent InputEvent = bButtonDown ? EInputEvent::IE_Pressed : EInputEvent::IE_Released;

    // Get our slate application
    FSlateApplication& SlateApp = FSlateApplication::Get();

    // Create a pointer event
    FPointerEvent MouseEvent(
        0,
        SlateApp.CursorPointerIndex,
        SlateApp.GetCursorPos(),
        SlateApp.GetLastCursorPos(),
        SlateApp.GetPressedMouseButtons(),
        MouseButton,
        0,
        SlateApp.GetPlatformApplication()->GetModifierKeys()
    );

    // Mouse Button Down
    if (bButtonDown)
    {
        if (!bWorldOnly)
        {
            // Get the widget window under the mouse
            TArray<TSharedRef<SWindow>> WindowsArray;
            FWidgetPath WidgetPath = SlateApp.LocateWindowUnderMouse(SlateApp.GetCursorPos(), WindowsArray);

            for (int32 i = WidgetPath.Widgets.Num() - 1; i >= 0; --i)
            {
                const FArrangedWidget& ArrangedWidget = WidgetPath.Widgets[i];
                const TSharedRef<SWidget>& Widget = ArrangedWidget.Widget;
                TSharedPtr<SWindow> Window = SlateApp.FindWidgetWindow(Widget);
                if (Window.IsValid())
                {
                    // Found the window under the mouse
                    SlateApp.ProcessMouseButtonDownEvent(Window->GetNativeWindow(), MouseEvent);
                    return;
                }
            }
        }

        if (!bWidgetsOnly)
        {
            // World click
            TSharedPtr<FGenericWindow, ESPMode::ThreadSafe> NullWindow;
            SlateApp.ProcessMouseButtonDownEvent(NullWindow, MouseEvent);
            return;
        }
        return;
    } else {
      // Mouse Button up
      SlateApp.ProcessMouseButtonUpEvent(MouseEvent);
    }
}