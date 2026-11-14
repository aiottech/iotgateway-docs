---
sidebar_position: 2
title: Quick Start Guide
description: Step-by-step instructions for unboxing, powering, pairing, and configuring your Mini IoT Gateway (MGW).
---

# Quick Start Guide

Welcome to the **Mini IoT Gateway (MGW)** Developer Center. Every `MGW-IoT-002` gateway is fully provisioned, tested, and pre-activated at the factory, arriving ready for deployment. 

This guide will walk you through powering up your gateway for the first time, connecting to its configuration portal, setting up your local factory Wi-Fi, and verifying communication with your Host MCU.

---

## 1. What You Need Before You Begin

Before powering up your `MGW-IoT-002` module, ensure you have:
* A stable 5V DC Power Supply connected to the USB-C power port.
* A Wi-Fi enabled smartphone, tablet, or laptop (for initial configuration).
* Your local factory Wi-Fi Router SSID and WPA2 Password.

---

## 2. Powering Up & Boot Modes

When you apply power to the MGW gateway, it initializes its system storage and reads the persistent boot configuration from non-volatile flash. MGW operates in two primary user-facing boot modes:

1. **Configure Mode (`STARTUP_CONFIGURE_MODE`):** 
   * **When it triggers:** Automatically on first boot out-of-box, or when explicitly requested.
   * **Behavior:** Launches a local Wi-Fi Access Point (SoftAP) and hosts the internal web configuration dashboard, allowing you to perform initial settings.
2. **Normal Mode (`STARTUP_NORMAL_MODE`):**
   * **When it triggers:** Once initial settings are saved and the gateway is rebooted.
   * **Behavior:** Disables the SoftAP hotspot and establishes full high-speed bi-directional communication with your Host MCU (over UART or SPI) and local automation networks.

---

## 3. Step 1: Connecting via Wi-Fi SoftAP (Configuration Portal)

When powering up the gateway out-of-box, it automatically starts in **Configure Mode**:

1. Open the Wi-Fi settings on your laptop or smartphone.
2. Search for available wireless networks and connect to the SSID broadcasted by your gateway:
   * **SSID Format:** `MGW-XXXXXXXX` (where `XXXXXXXX` is your unique 8-character hardware device code derived from your base adapter MAC address, printed on the gateway label).
   * **Password:** Initially set to your device code (or `admin` depending on factory batch).
3. Once connected, open your web browser (Google Chrome, Microsoft Edge, or Safari) and navigate to the gateway's fixed management IP address:
   * **URL:** `http://192.168.4.1`
4. You will be greeted by the **MGW Admin Center** login screen. Log in using default administrator credentials:
   * **Username:** `admin`
   * **Password:** `admin`

---

## 4. Step 2: Initial Settings & Wi-Fi Pairing

Once logged into the Web Dashboard, you will land on the **Wi-Fi** configuration panel:

1. **Scan Networks:** Click the **[ Scan ]** button to populate a dropdown list of all surrounding Wi-Fi router access points. Select your local facility router SSID.
2. **Enter Password:** Input your router's WPA2 Wi-Fi password.
3. **Connect:** Click the **[ Connect to Wi-Fi ]** button. 
4. **Verify IP:** Once connected, the gateway will acquire an IP address from your router, allowing you to manage it seamlessly over your local network.

---

## 5. Step 3: Rebooting into Normal Mode

After completing your initial network settings:
1. Click the **[ Finish & Reboot ]** button on the dashboard (or execute `cmd restart` from the console).
2. The gateway will safely flush its registers and reboot into **Normal Mode**, shutting down its SoftAP and opening the high-speed Host MCU data bridge.

---

## 6. Step 4: Verifying Host MCU Communication

After rebooting into **Normal Mode**:
1. Connect your Host MCU (e.g., STM32 / NXP) to MGW over physical UART1 or SPI1.
2. Send a basic diagnostic command to verify serial communication:
   ```text
   cmd version
   ```
3. The gateway will reply with a standard Normal Mode response frame:
   ```text
   Transfer-Mode: normal
   Result-Len: 11
   Data-Len: 11
   Data-Position: 0-10
   Duration: 12 ms
   CRC32: 35b78386
   Result: success

   <actual_version_number>
   ```
