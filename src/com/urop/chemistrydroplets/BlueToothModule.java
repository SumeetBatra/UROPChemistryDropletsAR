package com.urop.chemistrydroplets;

import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;

/* work in progress */

public class BlueToothModule {
	private BluetoothAdapter btAdapter = null;
	private BluetoothSocket btSocket = null;
	private OutputStream outStream = null;
	
	private static final int REQUEST_CONNECT_DEVICE_SECURE = 1;
	private static final int REQUEST_ENABLE_BT = 3;
	
	//SPP UUID
	private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
	
	private static String address = "21:15:03:19:08:66";
	
	public BlueToothModule() {
		btAdapter = BluetoothAdapter.getDefaultAdapter();
		
		checkBTState();
		
		Set<BluetoothDevice> pairedDevices = btAdapter.getBondedDevices();
	}

	private void checkBTState() {
		// Check for Bluetooth support and then check to make sure it is turned on

	    // Emulator doesn't support Bluetooth and will return null
		if(btAdapter == null) {
		
		}
	}
}
