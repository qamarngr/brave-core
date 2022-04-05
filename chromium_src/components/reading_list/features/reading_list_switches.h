/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_COMPONENTS_READING_LIST_FEATURES_READING_LIST_SWITCHES_H_
#define BRAVE_CHROMIUM_SRC_COMPONENTS_READING_LIST_FEATURES_READING_LIST_SWITCHES_H_

#include "src/components/reading_list/features/reading_list_switches.h"

namespace reading_list {
namespace switches {

// Indicates whether or not the side panel for the reading list should be
// created in the toolbar view.
bool IsSidePanelEnabled();

}  // namespace switches
}  // namespace reading_list

#endif  // BRAVE_CHROMIUM_SRC_COMPONENTS_READING_LIST_FEATURES_READING_LIST_SWITCHES_H_
