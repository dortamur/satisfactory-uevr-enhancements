#include "ButtonHintBarFix.h"
#include "FGButtonHintBar.h"
#include "UEVREnhancements.h"

void FButtonHintBarFix::RegisterHooks() {
  if (!WITH_EDITOR) {
    SUBSCRIBE_METHOD(UFGButtonHintBar::OnKeyHintsUpdated, [](auto &Scope, UFGButtonHintBar *FGButtonHintBar) {
      UE_LOG(UEVREnhancements, Verbose, TEXT("Skipping OnKeyHintsUpdated"));
      Scope.Cancel();
    });
    SUBSCRIBE_METHOD(UFGButtonHintBar::HandleKeyHintsChanged, [](auto &Scope, UFGButtonHintBar *FGButtonHintBar) {
      UE_LOG(UEVREnhancements, Verbose, TEXT("Skipping HandleKeyHintsChanged"));
      Scope.Cancel();
    });
    SUBSCRIBE_METHOD(UFGButtonHintBar::HandleSlateFocusChanging, [=](auto &Scope, UFGButtonHintBar *FGButtonHintBar, const FFocusEvent& FocusEvent, const FWeakWidgetPath& OldFocusedWidgetPath, const TSharedPtr<SWidget>& OldFocusedWidget, const FWidgetPath& NewFocusedWidgetPath, const TSharedPtr<SWidget>& NewFocusedWidget) {
      UE_LOG(UEVREnhancements, Verbose, TEXT("Skipping HandleSlateFocusChanging"));
      Scope.Cancel();
    });
  }
}
