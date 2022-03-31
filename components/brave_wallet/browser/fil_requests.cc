/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/browser/fil_requests.h"
#include "absl/types/optional.h"
#include "base/json/json_reader.h"
#include "base/logging.h"
#include "brave/components/brave_wallet/browser/json_rpc_requests_helper.h"
#include "third_party/jsoncpp/source/include/json/value.h"
#include "third_party/jsoncpp/source/include/json/writer.h"

namespace brave_wallet {

std::string fil_getBalance(const std::string& address) {
  return GetJsonRpc1Param("Filecoin.WalletBalance", address);
}

std::string fil_getTransactionCount(const std::string& address) {
  return GetJsonRpc1Param("Filecoin.MpoolGetNonce", address);
}

std::string fil_estimateGas(const std::string& from_address,
                            const std::string& to_address,
                            const std::string& gas_premium,
                            const std::string& gas_fee_cap,
                            int64_t gas_limit,
                            uint64_t nonce,
                            const std::string& max_fee,
                            const std::string& value) {
  Json::Value params(Json::arrayValue);

  Json::Value transaction(Json::objectValue);
  transaction["To"] = to_address;
  transaction["From"] = from_address;
  transaction["Value"] = value;
  transaction["GasPremium"] = gas_premium.empty() ? "0" : gas_premium;
  transaction["GasFeeCap"] = gas_fee_cap.empty() ? "0" : gas_fee_cap;
  transaction["Method"] = 0;
  transaction["Version"] = 0;
  transaction["Params"] = "";
  transaction["GasLimit"] = gas_limit;
  transaction["Nonce"] = nonce;
  params.append(std::move(transaction));

  Json::Value fee(Json::objectValue);
  fee["MaxFee"] = max_fee.empty() ? "0" : max_fee;
  params.append(std::move(fee));

  Json::Value cids(Json::arrayValue);
  params.append(std::move(cids));

  Json::Value dictionary(Json::objectValue);
  dictionary["jsonrpc"] = "2.0";
  dictionary["method"] = "Filecoin.GasEstimateMessageGas";
  dictionary["params"] = std::move(params);
  dictionary["id"] = 1;

  Json::StreamWriterBuilder builder;
  builder["commentStyle"] = "None";
  builder["indentation"] = "";
  std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
  std::ostringstream ssb;
  writer->write(dictionary, &ssb);
  return ssb.str();
}

absl::optional<std::string> fil_sendTransaction(const std::string& signed_tx) {
  base::JSONReader::ValueWithError parsed_tx =
      base::JSONReader::ReadAndReturnValueWithError(signed_tx);
  if (!parsed_tx.value || !parsed_tx.value->is_dict()) {
    VLOG(1) << "Error parsing transaction JSON: " << parsed_tx.error_message;
    return absl::nullopt;
  }
  auto* message_value = parsed_tx.value->FindKey("message");
  auto* signature_value = parsed_tx.value->FindKey("signature");
  if (!message_value || !signature_value)
    return absl::nullopt;
  base::Value params(base::Value::Type::LIST);
  params.Append(message_value->Clone());
  params.Append(signature_value->Clone());
  // params.Append(cid.Clone());

  base::Value dictionary(base::Value::Type::DICTIONARY);
  dictionary.SetStringKey("jsonrpc", "2.0");
  dictionary.SetStringKey("method", "Filecoin.MpoolPush");
  dictionary.SetKey("params", std::move(params));
  dictionary.SetIntKey("id", 1);

  return GetJSON(dictionary);
}

}  // namespace brave_wallet
