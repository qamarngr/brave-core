/**
 * Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

package org.chromium.chrome.browser.vpn.utils;

import android.app.Activity;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.text.TextUtils;
import android.util.Pair;

import androidx.core.app.NotificationCompat;
import androidx.fragment.app.FragmentActivity;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import org.chromium.base.Log;
import org.chromium.chrome.R;
import org.chromium.chrome.browser.app.BraveActivity;
import org.chromium.chrome.browser.vpn.activities.BraveVpnPlansActivity;
import org.chromium.chrome.browser.vpn.activities.BraveVpnProfileActivity;
import org.chromium.chrome.browser.vpn.activities.BraveVpnSupportActivity;
import org.chromium.chrome.browser.vpn.fragments.BraveVpnAlwaysOnErrorDialogFragment;
import org.chromium.chrome.browser.vpn.fragments.BraveVpnConfirmDialogFragment;
import org.chromium.chrome.browser.vpn.models.BraveVpnServerRegion;
import org.chromium.chrome.browser.vpn.models.BraveVpnWireguardProfileCredentials;
import org.chromium.chrome.browser.vpn.utils.BraveVpnPrefUtils;
import org.chromium.chrome.browser.vpn.utils.BraveVpnProfileUtils;
import org.chromium.chrome.browser.vpn.wireguard.WireguardConfigUtils;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class BraveVpnUtils {
    private static final String TAG = "BraveVPN";
    public static final String SUBSCRIPTION_PARAM_TEXT = "subscription";
    public static final String IAP_ANDROID_PARAM_TEXT = "iap-android";
    public static final String VERIFY_CREDENTIALS_FAILED = "verify_credentials_failed";
    public static final int BRAVE_VPN_NOTIFICATION_ID = 36;

    public static boolean mIsServerLocationChanged;
    public static String selectedServerRegion;
    private static ProgressDialog mProgressDialog;

    public enum AlwaysOnVpnType {
        BRAVE_VPN,
        OTHER_VPN,
        NONE;
    }

    public static boolean isBraveVpnFeatureEnable() {
        if (BraveVpnPrefUtils.isBraveVpnFeatureEnabled()) {
            return true;
        }
        return false;
    }

    public static void openBraveVpnPlansActivity(Activity activity) {
        Intent braveVpnPlanIntent = new Intent(activity, BraveVpnPlansActivity.class);
        braveVpnPlanIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        activity.startActivity(braveVpnPlanIntent);
    }

    public static void openBraveVpnProfileActivity(Context context) {
        Intent braveVpnProfileIntent = new Intent(context, BraveVpnProfileActivity.class);
        braveVpnProfileIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        context.startActivity(braveVpnProfileIntent);
    }

    public static void openBraveVpnSupportActivity(Context context) {
        Intent braveVpnSupportIntent = new Intent(context, BraveVpnSupportActivity.class);
        braveVpnSupportIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        context.startActivity(braveVpnSupportIntent);
    }

    public static void showProgressDialog(Activity activity, String message) {
        mProgressDialog = ProgressDialog.show(activity, "", message, true);
    }

    public static void dismissProgressDialog() {
        if (mProgressDialog != null && mProgressDialog.isShowing()) {
            mProgressDialog.dismiss();
        }
    }

    public static String getRegionForTimeZone(String jsonTimezones, String currentTimezone) {
        // Add root element to make it real JSON, otherwise getJSONArray cannot parse it
        jsonTimezones = "{\"regions\":" + jsonTimezones + "}";
        try {
            JSONObject result = new JSONObject(jsonTimezones);
            JSONArray regions = result.getJSONArray("regions");
            for (int i = 0; i < regions.length(); i++) {
                JSONObject region = regions.getJSONObject(i);
                JSONArray timezones = region.getJSONArray("timezones");
                for (int j = 0; j < timezones.length(); j++) {
                    if (timezones.getString(j).equals(currentTimezone)) {
                        return region.getString("name");
                    }
                }
            }
        } catch (JSONException e) {
            Log.e(TAG, "BraveVpnUtils -> getRegionForTimeZone JSONException error " + e);
        }
        return "";
    }

    public static Pair<String, String> getHostnameForRegion(String jsonHostnames) {
        jsonHostnames = "{\"hostnames\":" + jsonHostnames + "}";
        try {
            JSONObject result = new JSONObject(jsonHostnames);
            JSONArray hostnames = result.getJSONArray("hostnames");
            ArrayList<JSONObject> hosts = new ArrayList<JSONObject>();
            for (int i = 0; i < hostnames.length(); i++) {
                JSONObject hostname = hostnames.getJSONObject(i);
                if (hostname.getInt("capacity-score") == 0
                        || hostname.getInt("capacity-score") == 1) {
                    hosts.add(hostname);
                }
            }

            JSONObject hostname;
            if (hosts.size() < 2) {
                hostname = hostnames.getJSONObject(new Random().nextInt(hostnames.length()));
            } else {
                hostname = hosts.get(new Random().nextInt(hosts.size()));
            }
            return new Pair<>(hostname.getString("hostname"), hostname.getString("display-name"));
        } catch (JSONException e) {
            Log.e(TAG, "BraveVpnUtils -> getHostnameForRegion JSONException error " + e);
        }
        return new Pair<String, String>("", "");
    }

    public static BraveVpnWireguardProfileCredentials getWireguardProfileCredentials(
            String jsonWireguardProfileCredentials) {
        try {
            JSONObject wireguardProfileCredentials =
                    new JSONObject(jsonWireguardProfileCredentials);
            BraveVpnWireguardProfileCredentials braveVpnWireguardProfileCredentials =
                    new BraveVpnWireguardProfileCredentials(
                            wireguardProfileCredentials.getString("api-auth-token"),
                            wireguardProfileCredentials.getString("client-id"),
                            wireguardProfileCredentials.getString("mapped-ipv4-address"),
                            wireguardProfileCredentials.getString("mapped-ipv6-address"),
                            wireguardProfileCredentials.getString("server-public-key"));
            return braveVpnWireguardProfileCredentials;
        } catch (JSONException e) {
            Log.e(TAG, "BraveVpnUtils -> getWireguardProfileCredentials JSONException error " + e);
        }
        return null;
    }

    public static Long getPurchaseExpiryDate(String json) {
        try {
            JSONObject purchase = new JSONObject(json);
            String expiryTimeInString = purchase.getString("expiryTimeMillis");
            return Long.parseLong(expiryTimeInString);
        } catch (JSONException | NumberFormatException e) {
            Log.e(TAG,
                    "BraveVpnUtils -> getPurchaseExpiryDate JSONException | NumberFormatException error "
                            + e);
        }
        return 0L;
    }

    public static List<BraveVpnServerRegion> getServerLocations(String jsonServerLocations) {
        List<BraveVpnServerRegion> vpnServerRegions = new ArrayList<>();
        if (TextUtils.isEmpty(jsonServerLocations)) {
            return vpnServerRegions;
        }
        jsonServerLocations = "{\"servers\":" + jsonServerLocations + "}";
        try {
            JSONObject result = new JSONObject(jsonServerLocations);
            JSONArray servers = result.getJSONArray("servers");
            for (int i = 0; i < servers.length(); i++) {
                JSONObject server = servers.getJSONObject(i);
                BraveVpnServerRegion vpnServerRegion =
                        new BraveVpnServerRegion(server.getString("continent"),
                                server.getString("name"), server.getString("name-pretty"));
                vpnServerRegions.add(vpnServerRegion);
            }
        } catch (JSONException e) {
            Log.e(TAG, "BraveVpnUtils -> getServerLocations JSONException error " + e);
        }
        return vpnServerRegions;
    }

    public static Notification getBraveVpnNotification(Context context) {
        NotificationCompat.Builder notificationBuilder =
                new NotificationCompat.Builder(context, BraveActivity.CHANNEL_ID);

        notificationBuilder.setSmallIcon(R.drawable.ic_vpn)
                .setAutoCancel(false)
                .setContentTitle(context.getResources().getString(R.string.brave_firewall_vpn))
                .setContentText(
                        context.getResources().getString(R.string.brave_vpn_notification_message))
                .setStyle(new NotificationCompat.BigTextStyle().bigText(
                        context.getResources().getString(R.string.brave_vpn_notification_message)))
                .setPriority(NotificationCompat.PRIORITY_DEFAULT);

        return notificationBuilder.build();
    }

    public static void cancelBraveVpnNotification(Context context) {
        NotificationManager notificationManager =
                (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.cancel(BRAVE_VPN_NOTIFICATION_ID);
    }

    public static void resetProfileConfiguration(Activity activity) {
        if (BraveVpnProfileUtils.getInstance().isBraveVPNConnected(activity)) {
            BraveVpnProfileUtils.getInstance().stopVpn(activity);
        }
        try {
            WireguardConfigUtils.deleteConfig(activity);
        } catch (Exception ex) {
            Log.e(TAG, "resetProfileConfiguration : " + ex.getMessage());
        }
        BraveVpnPrefUtils.setHostname("");
        BraveVpnPrefUtils.setHostnameDisplay("");
        BraveVpnPrefUtils.setServerRegion(BraveVpnPrefUtils.PREF_BRAVE_VPN_AUTOMATIC);
        BraveVpnPrefUtils.setResetConfiguration(true);
        dismissProgressDialog();
    }

    public static void showVpnAlwaysOnErrorDialog(Activity activity) {
        BraveVpnAlwaysOnErrorDialogFragment mBraveVpnAlwaysOnErrorDialogFragment =
                new BraveVpnAlwaysOnErrorDialogFragment();
        mBraveVpnAlwaysOnErrorDialogFragment.setCancelable(false);
        mBraveVpnAlwaysOnErrorDialogFragment.show(
                ((FragmentActivity) activity).getSupportFragmentManager(),
                "BraveVpnAlwaysOnErrorDialogFragment");
    }

    public static void showVpnConfirmDialog(Activity activity) {
        BraveVpnConfirmDialogFragment braveVpnConfirmDialogFragment =
                new BraveVpnConfirmDialogFragment();
        braveVpnConfirmDialogFragment.setCancelable(false);
        braveVpnConfirmDialogFragment.show(
                ((FragmentActivity) activity).getSupportFragmentManager(),
                "BraveVpnConfirmDialogFragment");
    }
}
