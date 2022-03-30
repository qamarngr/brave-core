/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import { sendWithPromise } from 'chrome://resources/js/cr.m.js';

/** @interface */
export class BraveAdblockBrowserProxy {
  getInstance() { }
  getRegionalLists() { }
  enableFilterList() { }
}

/**
* @implements {BraveAdblockBrowserProxy}
*/
export class BraveAdblockBrowserProxyImpl {
  /** @instance */
  static getInstance() {
    return instance || (instance = new BraveAdblockBrowserProxyImpl());
  }

  /** @returns {Promise} */
  getRegionalLists () {
    return sendWithPromise('brave_adblock.getRegionalLists')
  }

  enableFilterList (uuid, enabled) {
    chrome.send('brave_adblock.enableFilterList', [uuid, enabled])
  }
}

/** @type {BraveAdblockBrowserProxyImpl} */
let instance
