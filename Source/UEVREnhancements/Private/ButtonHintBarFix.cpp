#include "ButtonHintBarFix.h"
#include "FGButtonHintBar.h"
#include "UEVREnhancements.h"

void FButtonHintBarFix::RegisterHooks() {
  if (!WITH_EDITOR) {
    SUBSCRIBE_METHOD(UFGButtonHintBar::OnKeyHintsUpdated, [](auto &Scope, UFGButtonHintBar *FGButtonHintBar) {
      UE_LOG(UEVREnhancements_Log, Warning, TEXT("Skipping OnKeyHintsUpdated"));
      Scope.Cancel();
    });
    SUBSCRIBE_METHOD(UFGButtonHintBar::HandleKeyHintsChanged, [](auto &Scope, UFGButtonHintBar *FGButtonHintBar) {
      UE_LOG(UEVREnhancements_Log, Warning, TEXT("Skipping HandleKeyHintsChanged"));
      Scope.Cancel();
    });
  }
}
