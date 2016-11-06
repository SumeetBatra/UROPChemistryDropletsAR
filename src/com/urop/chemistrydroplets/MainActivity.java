package com.urop.chemistrydroplets;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Point;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.SurfaceView;
import android.view.WindowManager;


import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;

public class MainActivity extends Activity implements CvCameraViewListener2{
	
	private Mat mRgba;
	private Mat mRgb;
	private Mat grayFrame;
	private Mat grayBinary;
	private Mat processedFrame;
	private boolean locked = false;

	
	
	private MainView mOpenCvCameraView;
	
	//instantiate opencvcameraview and load native libraries
	private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
		@Override
		public void onManagerConnected(int status) {
			switch(status) {
			case LoaderCallbackInterface.SUCCESS:
			{
				mOpenCvCameraView.setMaxFrameSize(640, 480);
				mOpenCvCameraView.enableView();
				System.loadLibrary("opencvnative");
				break;
			}
			default:
			{
				super.onManagerConnected(status);
			}
			}
		}
	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		mOpenCvCameraView = (MainView) findViewById(R.id.MainActivityCameraView);
		mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
		mOpenCvCameraView.setCvCameraViewListener(this);
		
//		BlueToothModule btModule = new BlueToothModule();
//		setUpBluetooth();
		
	}
	
	private void setUpBluetooth() {
		
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		int idx = 0;
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
	
	@Override
	public boolean onTouchEvent(MotionEvent event) {
		double x = event.getX();
		double y = event.getY();
		
		switch(event.getAction()) {
		//if user taps the screen, get its x/y position 
			case MotionEvent.ACTION_UP:
			{
				Display display = getWindowManager().getDefaultDisplay();
				Point size = new Point();
				display.getSize(size);
				double displayWidth = size.x;
				double displayHeight = size.y;
				
				double scaledX = (x / displayWidth) * 640;
				double scaledY = (y / displayHeight) * 480;
				//pass the x/y scaled position to native code 
				GetTouchedPoint(scaledX, scaledY);
			}
			case MotionEvent.ACTION_MOVE:
			{
				
			}
		}
		
		return false;
	}
	
	public void onResume() {
		super.onResume();
		OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_1_0, this, mLoaderCallback);
	}
	
	public void onDestroy() {
		super.onDestroy();
		if (mOpenCvCameraView != null) {
			mOpenCvCameraView.disableView();
		}
	}

	@Override
	public void onCameraViewStarted(int width, int height) {
		//initialize frame matrices
		mRgba = new Mat();
		mRgb = new Mat();
		grayFrame = new Mat();
		grayBinary = new Mat();
		processedFrame = new Mat();
	}

	@Override
	public void onCameraViewStopped() {
		mRgba.release();
		mRgb.release();
		grayFrame.release();
		grayBinary.release();
		processedFrame.release();
	}
	


	

	@Override
	public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
		if(!locked) {
			//first time setup that requires the first frame to be processed in order to lock auto exposure and display FPS.
			mOpenCvCameraView.setFPS();
			mOpenCvCameraView.setRecordingHint();
			mOpenCvCameraView.lockAutoExposure();
			locked = true;
		}
		//convert input frame into format that native code can process. Darken the frame to make the Droplets easier to detect
		mRgba = inputFrame.rgba();
		Imgproc.cvtColor(mRgba, mRgb, Imgproc.COLOR_RGBA2RGB);
		
		mRgb.convertTo(processedFrame, -1, 1, -150);
		Imgproc.cvtColor(processedFrame, grayFrame, Imgproc.COLOR_RGB2GRAY);
		//isolate the Droplets' illumination value 
		Core.inRange(grayFrame, new Scalar(105), new Scalar(106), grayBinary);
		TrackDroplets(processedFrame.nativeObj, grayBinary.nativeObj, mRgb.nativeObj);
		
		
		return mRgb;
	}
	
	public native void TrackDroplets(long matAddrDarkRgba, long matAddrGrayBin, long matAddrRgba);
	public native void GetTouchedPoint(double x, double y);
}
