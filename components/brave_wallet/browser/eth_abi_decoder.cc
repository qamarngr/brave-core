/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_wallet/browser/eth_abi_decoder.h"

#include <limits>
#include <map>

#include "base/strings/string_util.h"
#include "brave/components/brave_wallet/common/hex_utils.h"

// This file implements decoding rules of calldata according to the Contract
// Application Binary Interface (ABI).
//
// Method arguments are packed in chunks of 32 bytes, with types smaller than
// 32 bytes zero padded if necessary. Fixed-size types like uint256, address
// are represented within the available 32 bytes, however, dynamic types like
// bytes and arrays follow head-tail encoding. In this scheme, the data is
// packaged at the tail-end of the transaction's calldata. The arguments are
// references into the calldata where the content is.
//
// Parsing of complex cases such as composite types, nested arrays, etc., are
// not implemented. The decoder only extracts the calldata tail reference in
// such cases.
//
// Function selector (first 4 bytes) should NOT be part of the calldata being
// parsed.
//
// References:
//   https://docs.soliditylang.org/en/latest/abi-spec.html

namespace brave_wallet {

namespace {
// GetAddressFromData extracts an Ethereum address from the calldata at the
// specified index. The address type is static and 32-bytes wide, but we
// only consider the last 20 bytes, discarding the leading 12 bytes of
// 0-padded chars.
//
// In the future, addresses in Ethereum may become 32 bytes long:
// https://ethereum-magicians.org/t/increasing-address-size-from-20-to-32-bytes
//
// The parsed address value is prefixed by "0x".
bool GetAddressFromData(const std::string& input,
                        size_t offset,
                        std::string& arg) {
  arg = "0x" + input.substr(offset + 24, 40);
  return true;
}

// GetSizeFromData extracts a 32-byte wide size_t value from the calldata
// at the specified offset.
//
// Using this function to extract an integer outside the range of size_t is
// considered an error. Ideal candidates are calldata tail references, length
// of dynamic types, etc.
bool GetSizeFromData(const std::string& input, size_t offset, size_t& arg) {
  std::string padded_arg = "0x" + input.substr(offset, 64);
  uint256_t arg_uint;
  if (!brave_wallet::HexValueToUint256(padded_arg, &arg_uint))
    return false;

  // Since we use arg_uint as an array index, we need to cast the type to
  // something that can be used as an index, viz. size_t. To prevent runtime
  // errors, we make sure the value is within safe limits of size_t.
  if (arg_uint > std::numeric_limits<size_t>::max())
    return false;

  arg = static_cast<size_t>(arg_uint);
  return true;
}

// GetUint256HexFromData extracts a 32-byte wide uint256 value from the
// calldata at the specified offset.
//
// The parsed uint256 value is serialized as a hex string prefixed by "0x".
bool GetUint256HexFromData(const std::string& input,
                           size_t offset,
                           std::string& arg) {
  std::string padded_arg = "0x" + input.substr(offset, 64);
  uint256_t arg_uint;
  if (!brave_wallet::HexValueToUint256(padded_arg, &arg_uint))
    return false;

  arg = brave_wallet::Uint256ValueToHex(arg_uint);
  return true;
}

// GetBoolFromData extracts a 32-byte wide boolean value from the
// calldata at the specified offset.
//
// The parsed bool value is serialized as "true" or "false" strings.
bool GetBoolFromData(const std::string& input,
                     size_t offset,
                     std::string& arg) {
  std::string padded_arg = "0x" + input.substr(offset, 64);
  uint256_t arg_uint;
  if (!brave_wallet::HexValueToUint256(padded_arg, &arg_uint))
    return false;

  if (arg_uint == 0)
    arg = "false";
  else if (arg_uint == 1)
    arg = "true";
  else
    return false;

  return true;
}

// GetBytesHexFromData extracts a bytes value from the calldata at the
// specified offset using head-tail encoding mechanism. bytes are packed
// tightly in chunks of 32 bytes, with the first 32 bytes encoding the length,
// followed by the actual content.
//
// The parsed bytearray is serialized as a hex string prefixed by "0x".
bool GetBytesHexFromData(const std::string& input,
                         size_t offset,
                         std::string& arg) {
  size_t pointer;
  if (!GetSizeFromData(input, offset, pointer))
    return false;

  size_t bytes_len;
  if (!GetSizeFromData(input, pointer * 2, bytes_len))
    return false;

  arg = "0x" + input.substr(pointer * 2 + 64, bytes_len * 2);
  return true;
}

// GetAddressArrayFromData parses a calldata sequence to extract a dynamic
// array of addresses at the specified offset using head-tail encoding
// mechanism. The encoding is similar to bytes, with the first 32 bytes
// representing the number of elements in the array, followed by each element.
//
// The parsed data is joined together into a hex string prefixed by "0x".
bool GetAddressArrayFromData(const std::string& input,
                             size_t offset,
                             std::string& arg) {
  size_t pointer;
  if (!GetSizeFromData(input, offset, pointer))
    return false;

  size_t array_len;
  if (!GetSizeFromData(input, pointer * 2, array_len))
    return false;

  size_t array_offset = pointer * 2 + 64;
  arg = "0x";
  for (size_t i = 0; i < array_len; i++) {
    std::string value;
    if (!GetAddressFromData(input, array_offset, value))
      return false;
    arg += value.substr(2);
    array_offset += 64;
  }

  return true;
}

bool ABIDecodeInternal(const std::vector<std::string>& types,
                       const std::string& data,
                       std::vector<std::string>* tx_params,
                       std::vector<std::string>* tx_args) {
  size_t offset = 0;
  size_t calldata_tail = 0;

  for (const auto& type : types) {
    if ((data.length() - offset) < 64)
      return false;

    std::string value;
    if (type == "address") {
      if (!GetAddressFromData(data, offset, value))
        return false;
    } else if (type == "uint256") {
      if (!GetUint256HexFromData(data, offset, value))
        return false;
    } else if (type == "bool") {
      if (!GetBoolFromData(data, offset, value))
        return false;
    } else if (type == "bytes") {
      if (!GetBytesHexFromData(data, offset, value))
        return false;
    } else if (type == "address[]") {
      if (!GetAddressArrayFromData(data, offset, value))
        return false;
    } else {
      // For unknown/unsupported types, we only extract 32-bytes. In case of
      // dynamic types, this value is a calldata reference.
      value = data.substr(offset, 64);
    }

    // On encountering a dynamic type, we extract the reference to the start
    // of the tail section of the calldata.
    if ((type == "bytes" || type == "string" || base::EndsWith(type, "[]")) &&
        calldata_tail == 0) {
      size_t pointer;
      if (!GetSizeFromData(data, offset, pointer))
        return false;

      calldata_tail = pointer * 2;
    }

    offset += 64;

    if (tx_args)
      tx_args->push_back(value);

    if (tx_params)
      tx_params->push_back(type);
  }

  // Extraneous calldata, unless in the tail section, is considered invalid.
  if (offset != calldata_tail && data.substr(offset).length() > 0)
    return false;

  return true;
}

}  // namespace

bool UniswapEncodedPathDecode(const std::string& encodedPath,
                              std::vector<std::string>& path) {
  // Parse a Uniswap-encoded path and return a vector of addresses representing
  // each hop involved in the swap.
  //
  // Single-hop swap: Token1 → Token2
  // Multi-hop swap: Token1 → Token2 → WETH → Token3
  //
  // Each encoded hop contains a 3-byte pool fee, which is associated with the
  // address that follows. It is excluded from the result of this function.
  //
  // ┌──────────┬──────────┬──────────┬─────┐
  // │ address  │ pool fee │ address  │     │
  // │          │          │          │ ... │
  // │ 20 bytes │ 3 bytes  │ 20 bytes │     │
  // └──────────┴──────────┴──────────┴─────┘
  std::string data = encodedPath.substr(2);

  // Parse first hop address.
  if (data.length() < 40)
    return false;
  path.push_back("0x" + data.substr(0, 40));
  data = data.substr(40);

  while (true) {
    if (data.length() == 0)
      break;

    // Parse the pool fee, and ignore.
    if (data.length() < 6)
      return false;
    data = data.substr(6);

    // Parse next hop.
    if (data.length() < 40)
      return false;
    path.push_back("0x" + data.substr(0, 40));
    data = data.substr(40);
  }

  // Require a minimum of 2 addresses for a single-hop swap.
  if (path.size() < 2)
    return false;

  return true;
}

bool ABIDecode(const std::vector<std::string>& types,
               const std::string& data,
               std::vector<std::string>* tx_params,
               std::vector<std::string>* tx_args) {
  return ABIDecodeInternal(types, data, tx_params, tx_args);
}

}  // namespace brave_wallet
