#include "ButtonHintBarFix.h"
#include "FGButtonHintBar.h"
#include "UEVREnhancements.h"

static FDelegateHandle UFGButtonHintBar_OnKeyHintsUpdated;
static FDelegateHandle UFGButtonHintBar_HandleKeyHintsChanged;
static FDelegateHandle UFGButtonHintBar_SkipSlateFocusChanged;

class UEVRFixHooks {
public:
  static void SkipOnKeyHintsUpdated(TCallScope<void(__cdecl *)(UFGButtonHintBar *)> &Scope,
                                    UFGButtonHintBar *FGButtonHintBar) {
    UE_LOG(UEVREnhancements, Verbose, TEXT("Skipping OnKeyHintsUpdated"));
    Scope.Cancel();
  }

  static void SkipKeyHintsChanged(TCallScope<void(__cdecl *)(UFGButtonHintBar *)> &Scope,
                                  UFGButtonHintBar *FGButtonHintBar) {
    UE_LOG(UEVREnhancements, Verbose, TEXT("Skipping HandleKeyHintsChanged"));
    Scope.Cancel();
  }

  static void SkipSlateFocusChanging(
      TCallScope<void(__cdecl *)(UFGButtonHintBar *, const FFocusEvent &, const FWeakWidgetPath &,
                                 const TSharedPtr<SWidget> &, const FWidgetPath &, const TSharedPtr<SWidget> &)> &Scope,
      UFGButtonHintBar *FGButtonHintBar, const FFocusEvent &FocusEvent, const FWeakWidgetPath &OldFocusedWidgetPath,
      const TSharedPtr<SWidget> &OldFocusedWidget, const FWidgetPath &NewFocusedWidgetPath,
      const TSharedPtr<SWidget> &NewFocusedWidget) {
    UE_LOG(UEVREnhancements, Verbose, TEXT("Skipping HandleSlateFocusChanging"));
    Scope.Cancel();
  }
};

void UButtonHintBarFix::RegisterUEVRFixHooks() {
  if (!WITH_EDITOR) {
    UE_LOG(UEVREnhancements, Verbose, TEXT("Adding Hooks to disable UFGButtonHintBar functions to prevent UEVR crashes..."));
    UFGButtonHintBar_OnKeyHintsUpdated =
        SUBSCRIBE_METHOD(UFGButtonHintBar::OnKeyHintsUpdated, &UEVRFixHooks::SkipOnKeyHintsUpdated);
    UFGButtonHintBar_HandleKeyHintsChanged =
        SUBSCRIBE_METHOD(UFGButtonHintBar::HandleKeyHintsChanged, &UEVRFixHooks::SkipKeyHintsChanged);
    UFGButtonHintBar_SkipSlateFocusChanged =
        SUBSCRIBE_METHOD(UFGButtonHintBar::HandleSlateFocusChanging, &UEVRFixHooks::SkipSlateFocusChanging);
  }
}

void UButtonHintBarFix::UnregisterUEVRFixHooks() {
  if (!WITH_EDITOR) {
    UE_LOG(UEVREnhancements, Verbose, TEXT("Removing UFGButtonHintBar Hooks..."));
    if (UFGButtonHintBar_OnKeyHintsUpdated.IsValid()) {
      UNSUBSCRIBE_METHOD(UFGButtonHintBar::OnKeyHintsUpdated, UFGButtonHintBar_OnKeyHintsUpdated);
    }
    if (UFGButtonHintBar_HandleKeyHintsChanged.IsValid()) {
      UNSUBSCRIBE_METHOD(UFGButtonHintBar::HandleKeyHintsChanged, UFGButtonHintBar_HandleKeyHintsChanged);
    }
    if (UFGButtonHintBar_SkipSlateFocusChanged.IsValid()) {
      UNSUBSCRIBE_METHOD(UFGButtonHintBar::HandleSlateFocusChanging, UFGButtonHintBar_SkipSlateFocusChanged);
    }
  }
}