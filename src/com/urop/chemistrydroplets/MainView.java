package com.urop.chemistrydroplets;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;

import java.util.List;

import org.opencv.android.JavaCameraView;

import android.content.Context;
import android.hardware.Camera;
import android.hardware.Camera.PictureCallback;
import android.hardware.Camera.Size;
import android.util.AttributeSet;
import android.util.Log;

public class MainView extends JavaCameraView {
	
	private static final String TAG = "Urop::MainView";
	
	public MainView(Context context, AttributeSet attrs) {
		super(context, attrs);
	}
	
	
	@SuppressWarnings("deprecation")
	public void lockAutoExposure() {
		//lock auto exposure. Dynamic exposure compensation prevents image processing functions from detecting Droplets
		Camera.Parameters params = mCamera.getParameters();
		params.setExposureCompensation(params.getMinExposureCompensation());
		params.setAutoExposureLock(true);
		mCamera.setParameters(params);
	}
	
	public void setFPS() {
		Camera.Parameters params = mCamera.getParameters();
		params.setPreviewFpsRange(31000, 31000);
		mCamera.setParameters(params);
	}
	
	public void setRecordingHint() {
		Camera.Parameters params = mCamera.getParameters();
		params.setRecordingHint(true);
		mCamera.setParameters(params);
	}
	
	public void setResolution(Size resolution) {
		disconnectCamera();
		mMaxWidth = resolution.width;
		mMaxHeight = resolution.height;
		connectCamera(getWidth(), getHeight());
	}

	public List<Size> getResolutionList() {
		return mCamera.getParameters().getSupportedPreviewSizes();
		
	}
	


}
