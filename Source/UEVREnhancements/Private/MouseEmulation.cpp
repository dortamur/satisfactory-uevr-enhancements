#include "MouseEmulation.h"
#include "Framework/Application/SlateApplication.h"

// Derived from:
//  https://forums.unrealengine.com/t/how-to-inject-simulate-mouse-clicks/25602/12
//  https://pastebin.com/QqsTYYkv
void UMouseEmulation::SimulateMouseButton(const FKey MouseButton, bool bButtonDown, bool bWidgetsOnly, bool bWorldOnly)
{
    // Determine Mouse Button and Input Type
    // FKey MouseButton = bLeft ? EKeys::LeftMouseButton : EKeys::RightMouseButton;
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

void UMouseEmulation::SimulateMouseButtonWithModifier(const FKey MouseButton, bool bButtonDown, bool bShift, bool bCtrl, bool bAlt)
{
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
        FModifierKeysState(bShift, false, bCtrl, false, bAlt, false, false, false, false)
    );

    // Get the main game application window even if it does not have focus
    // TSharedPtr<SWindow> MainSlateWindow = SlateApp.GetActiveTopLevelWindow();
    // if (!MainSlateWindow.IsValid())
    // {
    //     TArray<TSharedRef<SWindow>> AllWindows = SlateApp.GetWindows();
    //     if (AllWindows.Num() > 0)
    //     {
    //         MainSlateWindow = AllWindows[0];
    //     }
    // }
    // TSharedPtr<FGenericWindow> NativeWindow = MainSlateWindow.IsValid() ? MainSlateWindow->GetNativeWindow() : nullptr;

    if (bButtonDown) {
        TSharedPtr<FGenericWindow, ESPMode::ThreadSafe> NullWindow;
        SlateApp.ProcessMouseButtonDownEvent(NullWindow, MouseEvent);
    }
    else {
        SlateApp.ProcessMouseButtonUpEvent(MouseEvent);
    }
}

void UMouseEmulation::SimulateMouseDoubleClick(const FKey MouseButton)
{
    // Get our slate application
    FSlateApplication& SlateApp = FSlateApplication::Get();

    // Create a pointer event
    FPointerEvent DoubleClickEvent(
        0,
        SlateApp.CursorPointerIndex,
        SlateApp.GetCursorPos(),
        SlateApp.GetLastCursorPos(),
        SlateApp.GetPressedMouseButtons(),
        MouseButton,
        0,
        SlateApp.GetPlatformApplication()->GetModifierKeys()
    );
    // World click
    TSharedPtr<FGenericWindow, ESPMode::ThreadSafe> NullWindow;
    SlateApp.ProcessMouseButtonDoubleClickEvent(NullWindow, DoubleClickEvent);
    // SlateApp.ProcessMouseButtonDownEvent(NullWindow, DoubleClickEvent);
    return;
}

void UMouseEmulation::SimulateMouseMove(const FKey MouseButton)
{
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

    // World click
    TSharedPtr<FGenericWindow, ESPMode::ThreadSafe> NullWindow;
    SlateApp.ProcessMouseMoveEvent(MouseEvent);
    return;
}

void UMouseEmulation::SimulateMouseScroll(float ScrollAmount)
{
    // Get our slate application
    FSlateApplication& SlateApp = FSlateApplication::Get();

    // Create a pointer event
    FPointerEvent MouseEvent(
        0,
        SlateApp.CursorPointerIndex,
        SlateApp.GetCursorPos(),
        SlateApp.GetLastCursorPos(),
        SlateApp.GetPressedMouseButtons(),
        EKeys::MouseScrollUp,
        ScrollAmount,
        SlateApp.GetPlatformApplication()->GetModifierKeys()
    );

    SlateApp.ProcessMouseWheelOrGestureEvent(MouseEvent, nullptr);
}

void UMouseEmulation::ScaleXYToMousePosition(float X, float Y, FVector2D& MousePosition)
{
    if (GEngine && GEngine->GameViewport) {
        FVector2D ViewportSize = GEngine->GameViewport->GetWindow()->GetSizeInScreen();
        // FVector2D ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();
        MousePosition = FVector2D((ViewportSize.X * X), (ViewportSize.Y * Y));
    }
}
