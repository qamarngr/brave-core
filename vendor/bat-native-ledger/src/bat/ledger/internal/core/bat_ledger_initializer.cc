/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/core/bat_ledger_initializer.h"

#include <utility>

#include "bat/ledger/internal/core/bat_ledger_job.h"
#include "bat/ledger/internal/ledger_impl.h"
#include "bat/ledger/internal/upgrades/upgrade_manager.h"

namespace ledger {

namespace {

class LedgerImplInitializer : public BATLedgerContext::Object {
 public:
  static constexpr char kContextKey[] = "ledger-impl-initializer";

  Future<bool> Initialize() {
    auto* ledger = context().GetLedgerImpl();

    ledger->publisher()->SetPublisherServerListTimer();
    ledger->contribution()->SetReconcileTimer();
    ledger->promotion()->Refresh(false);
    ledger->contribution()->Initialize();
    ledger->promotion()->Initialize();
    ledger->api()->Initialize();
    ledger->recovery()->Check();

    return MakeReadyFuture(true);
  }
};

template <typename... Ts>
class InitializeJob : public BATLedgerJob<bool> {
 public:
  void Start() { StartNext<Ts..., End>(); }

 private:
  struct End {};

  template <typename T, typename... Rest>
  void StartNext() {
    context().LogVerbose(FROM_HERE) << "Initializing " << T::kContextKey;
    context().template Get<T>().Initialize().Then(
        ContinueWith(this, &InitializeJob::OnCompleted<T, Rest...>));
  }

  template <>
  void StartNext<End>() {
    context().LogVerbose(FROM_HERE) << "Initialization complete";
    Complete(true);
  }

  template <typename T, typename... Rest>
  void OnCompleted(bool success) {
    if (!success) {
      context().LogError(FROM_HERE) << "Error initializing " << T::kContextKey;
      return Complete(false);
    }
    StartNext<Rest...>();
  }
};

using InitializeAllJob = InitializeJob<UpgradeManager, LedgerImplInitializer>;

}  // namespace

const char BATLedgerInitializer::kContextKey[] = "bat-ledger-initializer";

BATLedgerInitializer::BATLedgerInitializer() = default;

BATLedgerInitializer::~BATLedgerInitializer() = default;

Future<bool> BATLedgerInitializer::Initialize() {
  if (!initialize_future_) {
    initialize_future_ = SharedFuture(context().StartJob<InitializeAllJob>());
  }

  return initialize_future_->Then(
      base::BindOnce([](const bool& success) { return success; }));
}

}  // namespace ledger
