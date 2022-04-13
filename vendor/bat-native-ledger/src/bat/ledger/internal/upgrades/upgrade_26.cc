/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/upgrades/upgrade_26.h"

#include "bat/ledger/internal/upgrades/migration_job.h"

namespace ledger {

namespace {

const char kSQL[] = R"sql(
  ALTER TABLE unblinded_tokens RENAME TO unblinded_tokens_temp;

  DROP INDEX IF EXISTS unblinded_tokens_creds_id_index;

  DROP INDEX IF EXISTS unblinded_tokens_redeem_id_index;

  CREATE TABLE unblinded_tokens (
    token_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    token_value TEXT,
    public_key TEXT,
    value DOUBLE NOT NULL DEFAULT 0,
    creds_id TEXT,
    expires_at TIMESTAMP NOT NULL DEFAULT 0,
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    redeemed_at TIMESTAMP NOT NULL DEFAULT 0,
    redeem_id TEXT,
    redeem_type INTEGER NOT NULL DEFAULT 0,
    CONSTRAINT unblinded_tokens_unique
      UNIQUE (token_value, public_key)
  );

  CREATE INDEX unblinded_tokens_creds_id_index ON unblinded_tokens (creds_id);

  CREATE INDEX unblinded_tokens_redeem_id_index ON unblinded_tokens (redeem_id);

  INSERT OR IGNORE INTO unblinded_tokens (token_id, token_value, public_key,
  value, creds_id, expires_at, created_at, redeemed_at, redeem_id, redeem_type)
  SELECT token_id, token_value, public_key, value, creds_id, expires_at,
  created_at, redeemed_at, redeem_id, redeem_type FROM unblinded_tokens_temp;

  PRAGMA foreign_keys = off;
    DROP TABLE IF EXISTS unblinded_tokens_temp;
  PRAGMA foreign_keys = on;
)sql";

}  // namespace

constexpr int Upgrade26::kVersion;

void Upgrade26::Start() {
  CompleteWithFuture(context().StartJob<MigrationJob>(kVersion, kSQL));
}

}  // namespace ledger
